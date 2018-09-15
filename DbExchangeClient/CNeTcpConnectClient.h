#pragma once
#include <memory>
#include <WinSock2.h>
#include <string>
#include "DbConnect.h"



#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")

class CNeTcpConnectClient
{
public:
	CNeTcpConnectClient();
	~CNeTcpConnectClient();

	static std::shared_ptr<CNeTcpConnectClient> GetInstance();

	bool CreateTcpSocket(const std::string &strIp, int iPort);

	int WaitForRecvDataLoop();

	void CloseTcpSocket();

private:

	static std::shared_ptr<CNeTcpConnectClient> m_pTcpConnectClient;

	SOCKET    m_sock;
	fd_set		m_fdRead;
	fd_set		m_fdWrite;

	std::shared_ptr<CDbConnect> m_pDbConnect;
	void SendConnectResult(int iRet, int iCLientNo, const std::string &errstr);
};

