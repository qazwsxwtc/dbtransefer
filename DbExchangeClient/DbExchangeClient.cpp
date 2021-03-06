// DbExchangeClient.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "ProtocolPrase.h"
#include "CNeTcpConnectClient.h"
#include "CLibeventClient.h"


void Usage()
{
	//printf()
}

int main(int argc, char* argv[])
{

	std::string strIp = "127.0.0.1";
	int iPort = 15555;
	for (int i = 1; i < argc; i++)
	{
		std::string strParam = argv[i-1];
		if (strParam == "-H" || strParam == "H" || strParam == "-h" || strParam == "h")
		{
			strIp = argv[i];
		}
		else if (strParam == "-P" || strParam == "P" || strParam == "-p" || strParam == "p")
		{
			iPort = atoi(argv[i]);
		}
		
	}
	if (strIp.length() > 0 && iPort > 0)
	{
		printf("server ip=%s, port=%d\n", strIp.c_str(), iPort);
		printf("begin run\n");
	}
	else
	{
		return 0;
	}
	
	
	
	/* = 15555;
	char szIp[64] = { 0 };
	printf("please server ip:");
	scanf_s("%s", szIp, 64);
	strIp = szIp;*/

	
	/*if (CNeTcpConnectClient::GetInstance()->CreateTcpSocket(strIp, iPort))
	{
		CNeTcpConnectClient::GetInstance()->WaitForRecvDataLoop();
	}*/

	CLibeventClient::StartClient(strIp, iPort);
	


	//CDbConnect *pDbConnect = new CDbConnect();

	//pDbConnect->Connect((SQLCHAR*)("SQL32BIT"), (SQLCHAR*)("qaz"), (SQLCHAR*)("123"));


	//std::list<std::list<std::string> > lst2Data;
	//int iColNum = 2;
	//pDbConnect->ExecDirect((SQLCHAR*)"select * from Table_ee", iColNum, lst2Data);


	return 0;
}

