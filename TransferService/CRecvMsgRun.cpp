#include "CRecvMsgRun.h"
#include "DataMgr.h"
#include "libevent/event2/bufferevent.h"
#include "libevent/event2/buffer.h"
#include "libevent/event2/listener.h"
#include "libevent/event2/util.h"
#include "libevent/event2/event.h"
#include "ClibeventService.h"

CRecvMsgRun::CRecvMsgRun()
{
}


CRecvMsgRun::~CRecvMsgRun()
{
}

bool CRecvMsgRun::RunCVLogin(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	std::string strUserId;
	std::string strPwd;
	CProtocolPrase::GetLogOnInfo(str, strUserId, strPwd);

	return true;

}

bool CRecvMsgRun::RunCVConnectSql(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	std::shared_ptr<SocketConnectInfo> pSocketInf = nullptr;
	CDataMgr::GetCDataMgr()->GetReadDBClient(pSocketInf);

	if (nullptr != pSocketInf)
	{//db client Transfer
		std::string strPacket;
		CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo,
			CV, ConnectSql, (char*)(str.c_str() + HeadLen), strPacket);
		ClibeventService::SendMsg(pSocketInf->pbufev, strPacket, strPacket.length());

		return true;
	}

	return false;
}

bool CRecvMsgRun::RunCVExcSelectCmd(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	std::shared_ptr<SocketConnectInfo> pSocketInf = nullptr;
	CDataMgr::GetCDataMgr()->GetReadDBClient(pSocketInf);
	if (nullptr != pSocketInf)
	{//db client Transfer
		std::string strPacket;
		CProtocolPrase::GetRerurnString(packHeadPtr->iClientNo,
			CV, ExcSelectCmd, (char*)(str.c_str() + HeadLen), strPacket);
		ClibeventService::SendMsg(pSocketInf->pbufev, strPacket, strPacket.length());

		return true;
	}

	return false;
}

bool CRecvMsgRun::RunCDLogin(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	std::string strUserId;
	std::string strPwd;
	CProtocolPrase::GetLogOnInfo(str, strUserId, strPwd);

	return true;
}


bool CRecvMsgRun::RunCDConnectSql(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	if (nullptr != packHeadPtr)
	{
		std::shared_ptr<SocketConnectInfo> pSocketConnectInfo = nullptr;
		CDataMgr::GetCDataMgr()->GetShowClientInfo(packHeadPtr->iClientNo, pSocketConnectInfo);
		if (nullptr != pSocketConnectInfo) 
		{ 
			ClibeventService::SendMsg(pSocketConnectInfo->pbufev, str, str.length());
			return true;
		}
	}		
	return false;
}

bool CRecvMsgRun::RunCDExcSelectCmd(const std::shared_ptr<PacketHead>& packHeadPtr, const std::string & str)
{
	if (nullptr != packHeadPtr)
	{
		std::shared_ptr<SocketConnectInfo> pSocketConnectInfo = nullptr;
		CDataMgr::GetCDataMgr()->GetShowClientInfo(packHeadPtr->iClientNo, pSocketConnectInfo);
		if (nullptr != pSocketConnectInfo)
		{
			printf("RunCDExcSelectCmd ok =%d,\n ", str.length());
			ClibeventService::SendMsg(pSocketConnectInfo->pbufev, str, str.length());
			return true;
		}
	}
	return true;
}

bool CRecvMsgRun::RunCDConnectDBOut(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &strData)
{
	if (nullptr != packHeadPtr)
	{
		std::list <std::shared_ptr<SocketConnectInfo>> lstConn = CDataMgr::GetCDataMgr()->GetAllCConnect();
		for (auto item = lstConn.begin(); item != lstConn.end(); item++)
		{
			const std::shared_ptr<SocketConnectInfo> &pSocketConnectInfo = *item;
			if (nullptr != pSocketConnectInfo)
			{
				ClibeventService::SendMsg(pSocketConnectInfo->pbufev, strData, strData.length());
			
			}
		}
		
	}
	return true;
}
