#include "DataMgr.h"
#include <atomic>

std::shared_ptr<CDataMgr>  CDataMgr::m_pCDataMgr = nullptr;


//std::shared_ptr<SocketConnectInfo>  CDataMgr::m_ReadDataClient = nullptr;
//
//std::atomic_flag  CDataMgr::m_lock = ATOMIC_FLAG_INIT;


CDataMgr::CDataMgr() 
{

}


CDataMgr::~CDataMgr()
{

}

std::shared_ptr<CDataMgr> CDataMgr::GetCDataMgr()
{
	if (nullptr == m_pCDataMgr)
	{
		m_pCDataMgr = std::shared_ptr<CDataMgr>(new CDataMgr());
	}
	return m_pCDataMgr;
}

void CDataMgr::AddShowClientInfo(int ifd, const std::shared_ptr<SocketConnectInfo> & oSocketInf)
{

	m_mutex.lock();
	try
	{
		if (m_ClientInfo.find(ifd) != m_ClientInfo.end())
		{
			
		}
		else
		{
			m_ClientInfo.insert(std::make_pair(ifd, oSocketInf));
		}
	}
	catch (...)
	{

	}

	m_mutex.unlock();
	
}

bool CDataMgr::GetShowClientInfo(int ifd, std::shared_ptr<SocketConnectInfo>& oSocketInf)
{
	bool bOk = false;
	m_mutex.lock();
	try
	{
		if (m_ClientInfo.find(ifd) != m_ClientInfo.end())
		{
			oSocketInf = m_ClientInfo[ifd];
			bOk = true;
		}
	}
	catch (...)
	{

	}

	m_mutex.unlock();
	return bOk;
}

std::list<std::shared_ptr<SocketConnectInfo>> CDataMgr::GetAllCConnect()
{
	std::list<std::shared_ptr<SocketConnectInfo>> listConn;
	m_mutex.lock();
	try
	{
		for (auto item = m_ClientInfo.begin(); item != m_ClientInfo.end(); item++)
		{
			listConn.push_back(item->second);
		}
	}
	catch (...)
	{

	}

	m_mutex.unlock();
	return listConn;
}

void CDataMgr::RemoveShowClientInfo(int ifd)
{
	m_mutex.lock();
	try
	{
		if (m_ClientInfo.find(ifd) != m_ClientInfo.end())
		{
			m_ClientInfo.erase(ifd);
		}
	}
	catch (...)
	{

	}

	m_mutex.unlock();
}

void CDataMgr::AddReadDBClient(const std::shared_ptr<SocketConnectInfo> & oSocketInf)
{
	m_mutex.lock();
	try
	{
		m_ReadDataClient = oSocketInf;
	}
	catch (...)
	{

	}

	m_mutex.unlock();
}

bool CDataMgr::GetReadDBClient(std::shared_ptr<SocketConnectInfo>& oSocketInf)
{
	oSocketInf = m_ReadDataClient;
	return true;
}

bool CDataMgr::RemoveReadDBClient(int ifd)
{
	bool bRet = false;
	m_mutex.lock();
	try
	{
		if (nullptr != m_ReadDataClient && ifd == m_ReadDataClient->fd)
		{
			m_ReadDataClient.reset();
			m_ReadDataClient = nullptr;
			bRet = true;
		}
		
	}
	catch (...)
	{

	}

	m_mutex.unlock();
	return bRet;
}
