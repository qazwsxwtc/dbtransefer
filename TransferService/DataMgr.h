#pragma once
#include <memory>
#include <map>
#include <list>
#include <mutex>
#include <atomic> 
#include "Common.h"



class CDataMgr
{
public:
	CDataMgr();
	~CDataMgr();

	static std::shared_ptr<CDataMgr> GetCDataMgr();

	void AddShowClientInfo(int ifd, const std::shared_ptr<SocketConnectInfo> &oSocketInf);
	bool GetShowClientInfo(int ifd, std::shared_ptr<SocketConnectInfo> &oSocketInf);

	std::list <std::shared_ptr<SocketConnectInfo>> GetAllCConnect();

	void RemoveShowClientInfo(int ifd);

	void AddReadDBClient(const std::shared_ptr<SocketConnectInfo> &oSocketInf);
	bool GetReadDBClient(std::shared_ptr<SocketConnectInfo> &oSocketInf);
	bool RemoveReadDBClient(int ifd);

private:
	static std::shared_ptr<CDataMgr>  m_pCDataMgr;

	mutex m_mutex;
	std::map<int, std::shared_ptr<SocketConnectInfo>> m_ClientInfo; //clientNo, 
	std::shared_ptr<SocketConnectInfo>  m_ReadDataClient;
};

