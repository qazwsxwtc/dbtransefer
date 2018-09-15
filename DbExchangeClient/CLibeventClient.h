#pragma once
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <WinSock2.h>
#include <string>
#include "DbConnect.h"

class CLibeventClient
{
public:
	CLibeventClient();
	~CLibeventClient();

	static bool StartClient(const std::string &strIP, int port);

	/*事件处理回调函数*/
	static void event_cb(struct bufferevent* bev, short events, void* ptr);

	static void read_cb(struct bufferevent *bev, void *ctx);
	static void conn_writecb(struct bufferevent *bev, void *user_data);

	static int tcp_connect_server(const char* server_ip, int port);

	static void cmd_msg_cb(int fd, short events, void *arg);


	static struct event_base* m_base;
	
	static struct bufferevent *m_bev;

	static int sockfd;


};

