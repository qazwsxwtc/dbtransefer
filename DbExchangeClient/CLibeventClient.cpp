#include "stdafx.h"
#include "CLibeventClient.h"
#include "ProtocolPrase.h"
#include <thread>

#define EVENT_BASE_SOCKET 0


static std::shared_ptr<CDbConnect> m_pDbConnect = std::shared_ptr<CDbConnect>(new CDbConnect);;
static void set_tcp_no_delay(evutil_socket_t fd)

{

	int one = 1;

	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,

		(char*)&one, sizeof one);

	

}



static void timeoutcb(evutil_socket_t fd, short what, void *arg)

{

	struct event_base *base = (struct event_base *)arg;

	printf("timeout\n");



	event_base_loopexit(base, NULL);

}

static void SendConnectResult(struct bufferevent *bev, int iRet, int iCLientNo, const std::string &errstr)
{
	std::string strBody;
	std::string strPacket;
	CProtocolPrase::GetReturnResult(iRet == 0 ? true : false, errstr, strBody);
	CProtocolPrase::GetRerurnString(iCLientNo, CD, ConnectSql, strBody, strPacket);

	printf("ConnectDbInfo %s \n", iRet == 0 ? "sucess" : "failed");
	bufferevent_write(bev, strPacket.c_str(), strPacket.length());
}

static void readcb(struct bufferevent *bev, void *ctx)

{

	/* This callback is invoked when there is data to read on bev. */

	struct evbuffer *input = bufferevent_get_input(bev);

	int lenSIze = evbuffer_get_length(input);
	char *buf = new char[lenSIze];
	try
	{
		int n = 0;
		std::string strRev;
		while (n = bufferevent_read(bev, buf, lenSIze-1), n > 0)
		{
			for (size_t i = 0; i < n; i++)
			{
				strRev += buf[i];
			}
		}

	
		if (5 == strRev.length() && "login" == strRev)
		{
			
			std::string strBody;
			std::string strPacket;
			CProtocolPrase::PackLogOnInfo("admin", "admin", strBody);
			CProtocolPrase::GetRerurnString(0, CD, LogIn, strBody, strPacket);
			bufferevent_write(bev, strPacket.c_str(), strPacket.length());
			printf("run success %d",  strRev.size());
			//send(m_sock, "rrr");
		}
		else if (strRev.length() > 12 && strRev[0] == 'W' && strRev[1] == 'J')
		{
	
			std::shared_ptr<PacketHead> packHeadPtr = nullptr;

			CProtocolPrase::GetHeadInfo(strRev, packHeadPtr);
			if (packHeadPtr->iFuncNo == LogIn)
			{
				if (CProtocolPrase::GetConnectDbReturnInfo(strRev))
				{
					printf("logon sucess \n");
				}
				else
				{
					printf("logon error \n");
					
				}
			}
			else if (packHeadPtr->iFuncNo == ConnectSql)
			{
				
				ConnectDBInfo ocConnectDBInfo;
				CProtocolPrase::GetConnectDbInfo(strRev, ocConnectDBInfo);

				if (ocConnectDBInfo.strDbname.empty() || ocConnectDBInfo.strSqlUsername.empty() || ocConnectDBInfo.strSqlPwd.empty())
				{
					//printf("ConnectDbInfo error \n");
					SendConnectResult(bev ,-1, packHeadPtr->iClientNo, "Incomplete login information");
				}
				else
				{
					std::string errorStr;
					int iRet = m_pDbConnect->Connect((SQLCHAR*)ocConnectDBInfo.strDbname.c_str(),
						(SQLCHAR*)ocConnectDBInfo.strSqlUsername.c_str(),
						(SQLCHAR*)ocConnectDBInfo.strSqlPwd.c_str(), errorStr);
					SendConnectResult(bev, iRet, packHeadPtr->iClientNo, errorStr);
				}

			}
			else if (packHeadPtr->iFuncNo == ExcSelectCmd)
			{
				std::string sqlSelect;
				int iCols = 0;
				CProtocolPrase::GetSelectCmd(strRev, sqlSelect, iCols);
				std::string errStr;
				
					std::list<std::list<std::string> > vec2data;

					int iRet = m_pDbConnect->ExecDirect((SQLCHAR*)sqlSelect.c_str(), iCols, vec2data, errStr);

					if (0 == iRet)
					{
						std::string dataStr;
						CProtocolPrase::PacketDbData(vec2data, iCols, "", dataStr);
						std::string strPacket;
						
						CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo, CD, ExcSelectCmd,
							dataStr, strPacket);
						
						
						/*int iToltal = 0;
						int iCount = 0;
						while (iToltal < strPacket.length())
						{
							int iReceiv = strPacket.length() - iToltal;
							if (iReceiv >  3000 )
							{
								bufferevent_write(bev, strPacket.c_str()+ iCount *3000, 3000);
								iToltal += 3000;
							}
							else
							{
								bufferevent_write(bev, strPacket.c_str() + iToltal, iReceiv);
								iToltal += iReceiv;
							}
							iCount++;
						}*/
						bufferevent_write(bev, strPacket.c_str(), strPacket.length());
						
						printf("send db data size = %d:\n", strPacket.length());
						return;

					}

				

				std::string strBody;

				CProtocolPrase::PacketDbData(vec2data, iCols, errStr, strBody);
				std::string strPacket;
				CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo, CD, ExcSelectCmd, strBody, strPacket);
				bufferevent_write(bev, strPacket.c_str(), strPacket.length());

			}
		}
	}
	catch (...)
	{

	}
	delete[] buf;
	

	

	



	/* Copy all the data from the input buffer to the output buffer. */

	
	
	
	//evbuffer_free(output);
	//evbuffer_add_buffer(output, input);

}




static void eventcb(struct bufferevent *bev, short events, void *ptr)

{

	if (events & BEV_EVENT_CONNECTED) {

		evutil_socket_t fd = bufferevent_getfd(bev);

		set_tcp_no_delay(fd);

	}
	else if (events & BEV_EVENT_ERROR) {

		printf("NOT Connected\n");

	}

}

static void LogOn(struct bufferevent* bev)
{
	std::string strBody;
	std::string strPacket;
	CProtocolPrase::PackLogOnInfo("admin", "admin", strBody);
	CProtocolPrase::GetRerurnString(0, 1, 1, strBody, strPacket);
	//bufferevent_write(bev, strPacket.c_str(), strPacket.length());
	//send(CLibeventClient::sockfd, strPacket.c_str(), strPacket.length()-1, 0);
	bufferevent_write(bev, strPacket.c_str(), strPacket.length());
}



CLibeventClient::CLibeventClient()
{
}


CLibeventClient::~CLibeventClient()
{
}

bool CLibeventClient::StartClient(const std::string & strIP, int port)
{
	//1.初始化wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}

	
	

	//std::string strBody;
	//std::string strPacket;
	//CProtocolPrase::PackLogOnInfo("admin", "admin", strBody);
	//CProtocolPrase::GetRerurnString(0, 1, 1, strBody, strPacket);
	////bufferevent_write(bev, strPacket.c_str(), strPacket.length());
	//send(sockfd, strPacket.c_str(), strPacket.length() - 1, 0);

	struct event_base *base;

	//struct bufferevent **bevs;

	struct sockaddr_in sin;

	//struct event *evtimeout;

	//struct timeval timeout;

	base = event_base_new();

	if (!base) {

		puts("Couldn't open event base");

		return false;

	}
	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;

	inet_pton(AF_INET, strIP.c_str(), &sin.sin_addr);
	//sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */

	sin.sin_port = htons(port);

	struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);



	bufferevent_setcb(bev, readcb, NULL, eventcb, NULL);

	bufferevent_enable(bev, EV_READ | EV_WRITE);

	/*evbuffer_add(bufferevent_get_output(bev), message, block_size);*/



	if (bufferevent_socket_connect(bev,

		(struct sockaddr *)&sin, sizeof(sin)) < 0) {

		/* Error starting connection */

		bufferevent_free(bev);

		puts("error connect");

		return false;

	}

	

	event_base_dispatch(base);

	return true;
}



/*事件处理回调函数*/
void CLibeventClient::event_cb(struct bufferevent* bev, short events, void* ptr)
{
	if (events & BEV_EVENT_CONNECTED)//连接建立成功
	{
		printf("connected to server successed!\n");

		//std::thread oThrad(LogOn, bev);
		

	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("connect error happened!\n");
	}
}

void CLibeventClient::read_cb(struct bufferevent *bev, void *ctx)
{
	char msg[32] = { 0 };
	struct evbuffer* buf = bufferevent_get_input(bev);
	evbuffer_copyout(buf, (void*)msg, 32);
	if (msg)
		LogOn(bev);
	printf("read_cb() function read %s\r\n", msg);
}

void CLibeventClient::conn_writecb(bufferevent * bev, void * user_data)
{
}

int CLibeventClient::tcp_connect_server(const char* server_ip, int port)
{
	struct sockaddr_in server_addr;
	int status = -1;
	int sockfd;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip);

	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	/*if (0 == status)
	{
		errno = EINVAL;
		return -1;
	}*/

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return sockfd;


	status = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (status == -1)
	{
		closesocket(sockfd);
		return -1;
	}

	evutil_make_socket_nonblocking(sockfd);

	return sockfd;
}

void CLibeventClient::cmd_msg_cb(int fd, short events, void *arg)
{
	char msg[1024];
	int ret = recv(fd, msg, sizeof(msg),0);
	if (ret < 0)
	{
		perror("read failed");
		exit(1);
	}

	struct bufferevent* bev = (struct bufferevent*)arg;

	msg[ret] = '\0';
	//把终端消息发给服务器段
	//bufferevent_write(bev, msg, ret);

	printf("send message %s\r\n", msg);
}

