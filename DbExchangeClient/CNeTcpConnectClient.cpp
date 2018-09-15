#include "stdafx.h"
#include "CNeTcpConnectClient.h"
#include <WS2tcpip.h>
#include "ProtocolPrase.h"

std::shared_ptr<CNeTcpConnectClient> CNeTcpConnectClient::m_pTcpConnectClient = nullptr;

void CNeTcpConnectClient::SendConnectResult(int iRet, int iCLientNo, const std::string &errstr)
{
	std::string strBody;
	std::string strPacket;
	CProtocolPrase::GetReturnResult(iRet == 0 ? true : false, errstr, strBody);
	CProtocolPrase::GetRerurnString(iCLientNo, CD, ConnectSql, strBody, strPacket);

	printf("ConnectDbInfo %s \n", iRet == 0 ? "sucess" : "failed");
	send(m_sock, strPacket.c_str(), strPacket.length(), 0);
}

CNeTcpConnectClient::CNeTcpConnectClient()
{
	m_pDbConnect = nullptr;
}


CNeTcpConnectClient::~CNeTcpConnectClient()
{
}

std::shared_ptr<CNeTcpConnectClient> CNeTcpConnectClient::GetInstance()
{
	if (nullptr == m_pTcpConnectClient)
	{
		m_pTcpConnectClient = std::shared_ptr<CNeTcpConnectClient>(new CNeTcpConnectClient());
	}
	return m_pTcpConnectClient;
}

bool CNeTcpConnectClient::CreateTcpSocket(const std::string & strIp, int iPort)
{
	bool bflag = true;
	WORD wVersionRe;
	WSADATA wsaData;
	int err;

	wVersionRe = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRe, &wsaData);
	if (0 != err)
	{
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return false;
	}
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_sock)
	{
		return false;
	}

	SOCKADDR_IN addrSrv;
	inet_pton(AF_INET, strIp.c_str(), &addrSrv.sin_addr);
	//.S_un.S_addr = inet_addr(strIp.c_str());
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(iPort);


	int fd = connect(m_sock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	if (fd == -1)
	{
		int iError =GetLastError();
		printf("errno :%d", iError);
		return false;
	}

	puts("connect sucess£¡");
	return true;
}

int CNeTcpConnectClient::WaitForRecvDataLoop()
{
	while (true)
	{
		if (m_sock == 0)
		{
			Sleep(3);
			return 1;
		}

		timeval tv = { 10, 0 };
		FD_ZERO(&m_fdRead);
		FD_ZERO(&m_fdWrite);
		FD_SET(m_sock, &m_fdRead);
		FD_SET(m_sock, &m_fdWrite);

		int nrr = select(0, &m_fdRead, &m_fdWrite, NULL, &tv);
		if (nrr < 0)
		{
			//select ´íÎó 
			puts("select error!");
			return 1;
		}
		else if (nrr == 0)
		{
			//³¬Ê±
			puts("timeout!");
			return 1;
		}

		if (FD_ISSET(m_sock, &m_fdRead))
		{
			int pacsie = 1024;
			bool bflag = true;
			char pRecv[1024] = {0};

			int nerr = recv(m_sock, pRecv, pacsie - 1, 0);
			if (nerr == SOCKET_ERROR || nerr == 0)
			{
				if (nerr == -1)
				{
					CloseTcpSocket();
					exit(1);
					break;
				}
				
			}
			else
			{
				std::string strRev = pRecv;
				printf("recv size =%d \n", strRev.size());
				if (5 == strRev.length() && "login" == strRev)
				{
					std::string strBody;
					std::string strPacket;
					CProtocolPrase::PackLogOnInfo("admin", "admin", strBody);
					CProtocolPrase::GetRerurnString(0, CD, LogIn, strBody, strPacket);
				
					int irea = send(m_sock, strPacket.c_str(), strPacket.size(), 0);
					printf("run success %d - %d", irea, strPacket.size());
					//send(m_sock, "rrr");
				}
				else if(nerr > 12 && pRecv[0] == 'W' && pRecv[1] == 'J')
				{
					strRev = "";
					for (int i = 0; i < nerr; i++)
					{
						strRev += pRecv[i];
					}

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
							CloseTcpSocket();
							abort();
						}
					}
					else if (packHeadPtr->iFuncNo == ConnectSql)
					{
						if (nullptr == m_pDbConnect)
						{
							m_pDbConnect = std::shared_ptr<CDbConnect>(new CDbConnect);
						}
						ConnectDBInfo ocConnectDBInfo;
						CProtocolPrase::GetConnectDbInfo(strRev, ocConnectDBInfo);
						
						if (ocConnectDBInfo.strDbname.empty() || ocConnectDBInfo.strSqlUsername.empty() || ocConnectDBInfo.strSqlPwd.empty())
						{
							//printf("ConnectDbInfo error \n");
							SendConnectResult(-1, packHeadPtr->iClientNo, "Incomplete login information");
						}
						else
						{
							std::string errorStr;
							int iRet = m_pDbConnect->Connect((SQLCHAR*)ocConnectDBInfo.strDbname.c_str(), 
								(SQLCHAR*)ocConnectDBInfo.strSqlUsername.c_str(),
								(SQLCHAR*)ocConnectDBInfo.strSqlPwd.c_str(), errorStr);
							SendConnectResult(iRet, packHeadPtr->iClientNo, errorStr);
						}

					}
					else if (packHeadPtr->iFuncNo == ExcSelectCmd)
					{
						std::string sqlSelect;
						int iCols = 0;
						CProtocolPrase::GetSelectCmd(strRev, sqlSelect, iCols);
						std::string errStr;
						if (nullptr != m_pDbConnect)
						{
							std::list<std::list<std::string> > vec2data;
							
							int iRet = m_pDbConnect->ExecDirect((SQLCHAR*)sqlSelect.c_str(), iCols, vec2data, errStr);

							if (0 == iRet)
							{
								std::string dataStr;
								CProtocolPrase::PacketDbData(vec2data, iCols, "",  dataStr);
								std::string strPacket;
								CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo, CD, ExcSelectCmd, 
									dataStr, strPacket);
								send(m_sock, strPacket.c_str(), strPacket.length(), 0);
								continue;
								
							}
							
						}
						else
						{
							errStr = "No connection objects, please connect first";
						}
						
						std::string strBody;
						std::list<std::list<std::string> > vec2data;
						CProtocolPrase::PacketDbData(vec2data, iCols, errStr, strBody);
						std::string strPacket;
						CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo, CD, ExcSelectCmd, strBody, strPacket);
						send(m_sock, strPacket.c_str(), strPacket.length(), 0);
						
					}
				}
			}

			/*if (m_ptrMemPool->IsValidPointer(pRecv))
				m_ptrMemPool->FreeMemory(pRecv, pacsie);*/
		}

		if (FD_ISSET(m_sock, &m_fdWrite))
		{

		}
	}

	return 1;
}

void CNeTcpConnectClient::CloseTcpSocket()
{
	if (m_sock > 0)
	{
		closesocket(m_sock);
	}
}
