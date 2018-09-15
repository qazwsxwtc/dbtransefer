#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

#include "ClibeventService.h"


bool GetLocalIP(char* ip)
{
	struct addrinfo hints;
	struct addrinfo *res, *cur;
	struct sockaddr_in *addr;
	char ipbuf[16];

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; /* Allow IPv4 */
	hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
	hints.ai_protocol = 0; /* Any protocol */
	hints.ai_socktype = SOCK_STREAM;

	//1.初始化wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.获取主机名
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}

	ret = getaddrinfo(hostname, NULL, &hints, &res);
	if (ret == -1) {
		perror("getaddrinfo");
		exit(1);
	}

	for (cur = res; cur != NULL; cur = cur->ai_next) {
		addr = (struct sockaddr_in *)cur->ai_addr;
		printf("%s\n", inet_ntop(AF_INET,
			&addr->sin_addr, ipbuf, 16));
	}
	freeaddrinfo(res);
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char szip[64] = { 0 };
	bool bok = GetLocalIP(szip);

	ClibeventService * pClibeventService = new ClibeventService();
	if (nullptr != pClibeventService)
	{
		printf("local ip: %s port: 15555\n", szip);
		pClibeventService->StartLibevent("0.0.0.0", 15555);
	}
	
	return 1;
}