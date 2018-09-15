#include "ClibeventService.h"
#include "Common.h"
#include <signal.h>
#include "libevent/event2/bufferevent.h"
#include "libevent/event2/buffer.h"
#include "libevent/event2/listener.h"
#include "libevent/event2/util.h"
#include "libevent/event2/event.h"
#include "ProtocolPrase.h"
#include "DataMgr.h"
#include "CRecvMsgRun.h"
#include "CharacterHelper.h"
#include "base64.h"
#include <codecvt>
#include <map>


std::map<int, std::shared_ptr<CReadMaxData>>  g_mapWaitRecv ;
std::mutex  m_Mute;

std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

std::string string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

#define PACKETRECEIVLEN  10240; 

ClibeventService*   ClibeventService::m_plibevent = nullptr;

ClibeventService::ClibeventService():
	m_pEventBase(NULL),
	m_pListener(NULL),
	m_pSignalEvent(NULL),
	m_sock(-1)
{
}


ClibeventService::~ClibeventService()
{
	/*if(-1 != m_sock)*/
	
}

ClibeventService* ClibeventService::GetLibevent()
{
	if (m_plibevent == nullptr)
	{
		m_plibevent = new ClibeventService();
	}
	return m_plibevent;
}

bool ClibeventService::StartLibevent(string localip, USHORT uport)
{

	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);


	m_pEventBase = event_base_new();
	if (NULL == m_pEventBase)
	{
		printf("eventbase error \n");
		return false;
	}
	m_localIP = localip;
	memset(&m_sockIn, 0, sizeof(m_sockIn));
	m_sockIn.sin_family = AF_INET;
	m_sockIn.sin_addr.s_addr = htonl(INADDR_ANY);
	m_sockIn.sin_port = htons(uport);

	int nRet = -1;

	m_pListener = evconnlistener_new_bind(m_pEventBase, listener_cb, (void*)m_pEventBase,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 10,
		(struct sockaddr*)&m_sockIn, sizeof(m_sockIn));
	

	if (NULL == m_pListener)
	{
		fprintf(stderr, "Could not create a listener!\n");
		return false;
	}






	char str[INET_ADDRSTRLEN];

	//char *ptr = inet_ntop(AF_INET, &m_sockIn.sin_addr, str, sizeof(str));      // 代替 ptr = inet_ntoa(foo.sin_addr)


	printf("IP:%s:%d\n", inet_ntop(AF_INET, &m_sockIn.sin_addr, str, sizeof(str)), uport);

	m_pSignalEvent = evsignal_new(m_pEventBase, SIGINT, signal_cb, (void*)m_pEventBase);
	if (NULL == m_pSignalEvent || event_add(m_pSignalEvent, NULL)<0)
	{
		return false;
	}

	event_base_dispatch(m_pEventBase);

	evconnlistener_free(m_pListener);
	event_free(m_pSignalEvent);
	event_base_free(m_pEventBase);
	return true;
}

void ClibeventService::signal_cb(evutil_socket_t sig, short events, void * user_data)
{
	printf("signal_cb run");
}

void ClibeventService::listener_cb(evconnlistener * listener, evutil_socket_t fd, sockaddr * sa, int socklen, void * user_data)
{
	struct event_base* pbase = (struct event_base*)user_data;
	struct bufferevent* pbufev = NULL;

	pbufev = bufferevent_socket_new(pbase, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == pbufev)
	{
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(pbase);
		return;
	}
	sockaddr_in sin;
	memcpy(&sin, sa, sizeof(sin));
	char str[INET_ADDRSTRLEN] = {0};
	printf("fd=%u,ip=%s:%u, connect success \n ", fd, 
		inet_ntop(AF_INET, &sin.sin_addr, str, sizeof(str)), sin.sin_port);

	int nRecvBufLen = 128 * 1024; //设置为K

	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));
	bufferevent_setcb(pbufev, cnn_readcb, conn_writecb, conn_eventcb, NULL);

	bufferevent_enable(pbufev, EV_READ | EV_PERSIST);
	bufferevent_enable(pbufev, EV_WRITE);

	//CDataInfoManage* pdatamage = CDataInfoManage::GetDataInfoManage();
	//SocketConnectInfo sockinfo;
	//sockinfo.fd = fd;
	//sockinfo.pbase = pbase;
	//sockinfo.pbufev = pbufev;
	//sockinfo.sa = *sa;
	//sockinfo.userid = 0;
	//sockinfo.username = "";
	///*sockinfo.roomid = 0;*/
	//pdatamage->AddSocketInfo(fd, &sockinfo);

	std::shared_ptr<SocketConnectInfo> pSocketConnectInfo(new SocketConnectInfo);
	pSocketConnectInfo->fd = fd;
	pSocketConnectInfo->pbase = pbase;
	pSocketConnectInfo->pbufev = pbufev;
	pSocketConnectInfo->saddr = *sa;
	pSocketConnectInfo->userid = 0;
	pSocketConnectInfo->username = "";
	CDataMgr::GetCDataMgr()->AddShowClientInfo(fd, pSocketConnectInfo);

	SendMsg(pbufev, "login", 5);
}


class dataSz
{
public:
	dataSz() :szline(nullptr)
	{
		int packesize = PACKETRECEIVLEN;
		szline = new char[packesize];
	}
	~dataSz()
	{
		if (nullptr != szline)
			delete[] szline;
	}


	char* szline;
};



void ClibeventService::cnn_readcb(bufferevent * bev, void * user_data)
{
	int packesize = PACKETRECEIVLEN;
	dataSz oDataSize;
	std::string strData;
	memset(oDataSize.szline, 0, packesize);
	evutil_socket_t fd = bufferevent_getfd(bev);
	size_t n = 0;
	//std::wstring wString;
	std::string Recv;
	while (n = bufferevent_read(bev, oDataSize.szline, packesize - 1), n > 0)
	{
		for (size_t i = 0; i < n; i++)
		{
			strData += oDataSize.szline[i];
		}
		memset(oDataSize.szline, 0, packesize);
	}

	if (strData[0] == 'W' &&  strData[1] == 'J')
	{
		std::shared_ptr<PacketHead> packHeadPtr = nullptr;
		CProtocolPrase::GetHeadInfo(strData, packHeadPtr, false);
		if (nullptr != packHeadPtr && packHeadPtr->iBodyLen + 12 > strData.size())
		{
			m_Mute.lock();
			try
			{
				std::shared_ptr<CReadMaxData> oCReadMaxData =  std::shared_ptr<CReadMaxData>(new CReadMaxData());
				oCReadMaxData->m_ifd = fd;
				oCReadMaxData->m_itotal = packHeadPtr->iBodyLen + 12;
				oCReadMaxData->m_iwaitlen = oCReadMaxData->m_itotal;
				if (g_mapWaitRecv.find(fd) != g_mapWaitRecv.end())
				{
					g_mapWaitRecv[fd] = oCReadMaxData;
				}
				else
				{
					g_mapWaitRecv.insert(std::make_pair(fd, oCReadMaxData));
				}
				
			}
			catch (...)
			{

			}
			m_Mute.unlock();

		}
	}
	bool bReturn = false;
	m_Mute.lock();
	do
	{

		try
		{
			auto mapitem = g_mapWaitRecv.find(fd);
			if (mapitem != g_mapWaitRecv.end())
			{
				mapitem->second->m_RecvData += strData;
				mapitem->second->m_iwaitlen -= strData.length();
				if (mapitem->second->m_iwaitlen <= 0)
				{
					strData = mapitem->second->m_RecvData;
					g_mapWaitRecv.erase(fd);
				}
				else
				{
					bReturn = true;
				}
			}
		}
		catch (...)
		{

		}
	} while (0);
	
	m_Mute.unlock();
	if (bReturn)
	{
		return;
	}

	//strData = UTF8_To_string(Recv);

	/*str_ansi * pchar = CAPECharacterHelper::GetANSIFromUTF8((str_utf8 *)strData.c_str());
	strData = pchar;
	delete[] pchar;
	Base64Decode(pchar, strData.c_str(), strData.length());*/

	/*std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
		converter(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));
	strData = converter.to_bytes(wString);
	int hh = strData.length();
	int wih = wString.length();*/
	//std::wstring wstr = converter.from_bytes(narrow_str);

	/*std::codecvt_utf8*/

	

	if (strData[0] == 'W' &&  strData[1] == 'J')
	{
		std::shared_ptr<PacketHead> packHeadPtr = nullptr;

		CProtocolPrase::GetHeadInfo(strData, packHeadPtr);

		if (packHeadPtr != nullptr)
		{
			
			if (packHeadPtr->iClientType == enumClientType::CV)
			{
				packHeadPtr->iClientNo = fd;
				if (LogIn == packHeadPtr->iFuncNo)
				{
					printf("CV-LogIn: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCVLogin(packHeadPtr, strData);
					std::string strPacket;
					std::string strBody;
					CProtocolPrase::GetReturnResult(bOk, bOk ? "" : "User name or password error ", strBody);
					CProtocolPrase::GetRerurnString(fd,
						CV, LogIn, strBody, strPacket);

					SendMsg(bev, strPacket, strPacket.length());

				}
				else if (ConnectSql == packHeadPtr->iFuncNo)
				{
					printf("CV-ConnectSql: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCVConnectSql(packHeadPtr, strData);
					if(!bOk)
					{
						std::string strPacket;
						std::string strBody;
						CProtocolPrase::GetReturnResult(bOk, "Packet parse error ", strBody);
						CProtocolPrase::GetRerurnString(fd,
							CV, ConnectSql, strBody, strPacket);
						SendMsg(bev, strPacket, strPacket.length());
					}
					
				}
				else if (ExcSelectCmd == packHeadPtr->iFuncNo)
				{
					printf("CV-ExcSelectCmd: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCVExcSelectCmd(packHeadPtr, strData);
					if (!bOk)
					{
						std::string strPacket;
						CProtocolPrase::GetRerurnString(fd,
							CV, ExcSelectCmd, "", strPacket);
						SendMsg(bev, strPacket, strPacket.length());
					}
				}
			}
			else if (packHeadPtr->iClientType == enumClientType::CD)
			{
				if (LogIn == packHeadPtr->iFuncNo)
				{
					printf("CD-LogIn: fd = %d, datasize = %d\n", fd, strData.length());
					CRecvMsgRun::RunCDLogin(packHeadPtr, strData);
					bool bOk = CRecvMsgRun::RunCVLogin(packHeadPtr, strData);
					std::string strPacket;
					std::string strBody;
					CProtocolPrase::GetReturnResult(bOk, bOk ? "" : "User name or password error ", strBody);
					CProtocolPrase::GetRerurnString(0,
						CD, LogIn, strBody, strPacket);
					std::shared_ptr<SocketConnectInfo> pSocketInf;
					CDataMgr::GetCDataMgr()->GetShowClientInfo(fd, pSocketInf);
					CDataMgr::GetCDataMgr()->AddReadDBClient(pSocketInf);
					CDataMgr::GetCDataMgr()->RemoveShowClientInfo(fd);

					SendMsg(bev, strPacket, strPacket.length());
				}
				else if (ConnectSql == packHeadPtr->iFuncNo)
				{
					printf("CD-ConnectSql: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCDConnectSql(packHeadPtr, strData);
				}
				else if (ExcSelectCmd == packHeadPtr->iFuncNo)
				{
					printf("CD-ExcSelectCmd: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCDExcSelectCmd(packHeadPtr, strData);
				}
				else if (ConnectDBOut == packHeadPtr->iFuncNo)
				{
					printf("CD-ConnectDBOut: fd = %d, datasize = %d\n", fd, strData.length());
					bool bOk = CRecvMsgRun::RunCDConnectDBOut(packHeadPtr, strData);
				}
			}
		}

		else
		{
			printf("error packet:%d, fd = %d\n", strData.size(), fd);
		}


	}
	else
	{
		SendMsg(bev, strData, strData.length());
		printf("errordata: fd = %d, datasize = %d\n", fd, strData.length());
	}
	
	

}

void ClibeventService::conn_writecb(bufferevent * bev, void * user_data)
{
}

void ClibeventService::conn_eventcb(bufferevent * bev, short events, void * user_data)
{
	evutil_socket_t fd = bufferevent_getfd(bev);
	if (events & BEV_EVENT_EOF)
	{
		printf("fd=%u, connection closed\n", fd);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("fd=%u, some other error\n", fd);
	}
	else if (events & BEV_EVENT_TIMEOUT)
	{
		printf("fd=%u, Time out \n", fd);
	}
	//CDataInfoManage* pdatamange = CDataInfoManage::GetDataInfoManage();
	///*pdatamange->RemoveSocketInfo(fd);*/
	CDataMgr::GetCDataMgr()->RemoveShowClientInfo(fd);
	bool bRe = CDataMgr::GetCDataMgr()->RemoveReadDBClient(fd);
	if (bRe)
	{
		std::string strBody;
		std::string strPacket;
		CProtocolPrase::PacketDbUnConn("CD client UnConnect", strBody);
		CProtocolPrase::GetRerurnString(0, 0, ConnectDBOut, strBody, strPacket);
		std::list <std::shared_ptr<SocketConnectInfo>> lstConn = CDataMgr::GetCDataMgr()->GetAllCConnect();
		for (auto item = lstConn.begin(); item != lstConn.end(); item++)
		{
			const std::shared_ptr<SocketConnectInfo> &pSocketConnectInfo = *item;
			if (nullptr != pSocketConnectInfo)
			{
				SendMsg(pSocketConnectInfo->pbufev, strPacket, strPacket.length());

			}
		}
	}
	RemoveConnect(bev, fd);
	
	m_Mute.lock();
	try
	{
		if (g_mapWaitRecv.find(fd) != g_mapWaitRecv.end())
		{
			g_mapWaitRecv.erase(fd);
		}
	}
	catch (...)
	{

	}
	m_Mute.unlock();

	
}


void ClibeventService::RemoveConnect(bufferevent * bev, evutil_socket_t fd)
{
	bufferevent_free(bev);
	
}

bool ClibeventService::SendMsg(bufferevent * be, const string &data, int len)
{
	/*std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
		converter(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));

	std::wstring wstr = converter.from_bytes(data.c_str(), data.c_str()+len);*/
	/*std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
	std::string strUtf8 = string_To_UTF8(data);*/
	bufferevent_write(be, data.c_str(), data.length());

	return false;
}
