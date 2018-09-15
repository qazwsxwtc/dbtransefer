#pragma once
#include <string>
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"

using namespace std;

enum enumClientType
{
	CV = 0,
	CD = 1
};


enum enumFunctionNo
{
	LogIn = 0,
	ConnectSql = 1,
	ExcSelectCmd = 2,
	ConnectDBOut = 3,
};


struct SocketConnectInfo
{
	struct event_base* pbase;
	struct bufferevent* pbufev;
	evutil_socket_t  fd;
	struct sockaddr saddr;
	unsigned int userid;
	enumClientType eClientType;
	int	iClientNo;
	std::string       username;
	
	SocketConnectInfo& operator =(const SocketConnectInfo& src)
	{
		this->pbase = src.pbase;
		this->pbufev = src.pbufev;
		this->fd = src.fd;
		this->saddr = src.saddr;
		this->userid = src.userid;
		this->username = src.username;
		this->eClientType = src.eClientType;
		this->iClientNo = src.iClientNo;
		return *this;
	}
} ;