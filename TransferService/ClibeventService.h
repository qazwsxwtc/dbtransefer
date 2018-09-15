#pragma once



#include <WinSock2.h>


#include <string>

#include "libevent/event2/bufferevent.h"
#include "libevent/event2/buffer.h"
#include "libevent/event2/listener.h"
#include "libevent/event2/util.h"
#include "libevent/event2/event.h"

using namespace std;




class CReadMaxData
{
public:
	CReadMaxData()
	{
		m_ifd = 0;
		m_iwaitlen = 0;
		m_itotal = 0;
	}
	~CReadMaxData()
	{

	}

public:
	int m_ifd;
	int m_iwaitlen;
	int m_itotal;
	std::string  m_RecvData;

};

class ClibeventService
{
public:
	ClibeventService();
	~ClibeventService();


	static ClibeventService* GetLibevent();

	bool StartLibevent(string localip, USHORT uport);

	static void signal_cb(evutil_socket_t sig, short events, void *user_data);

	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
		struct sockaddr *sa, int socklen, void *user_data);

	static void cnn_readcb(bufferevent *bev, void *user_data);
	static void conn_writecb(struct bufferevent *bev, void *user_data);
	static void conn_eventcb(struct bufferevent *bev, short events, void *user_data);


	static void RemoveConnect(struct bufferevent *bev, evutil_socket_t fd);

	static bool SendMsg(struct bufferevent *be,const string &data, int len);



private:
	
	struct event_base*			m_pEventBase;
	struct evconnlistener*		m_pListener;
	struct event*				m_pSignalEvent;

	struct sockaddr_in			m_sockIn;
	SOCKET						m_sock;
	string                      m_localIP;
	

	static ClibeventService*    m_plibevent;

	
};

