#pragma once
#include "Common.h"
#include "ProtocolPrase.h"

class CRecvMsgRun
{
public:
	CRecvMsgRun();
	~CRecvMsgRun();

	static bool RunCVLogin(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);
	static bool RunCVConnectSql(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);
	static bool RunCVExcSelectCmd(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);

	static bool RunCDLogin(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);
	static bool RunCDConnectSql(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);
	static bool RunCDExcSelectCmd(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &str);

	static bool RunCDConnectDBOut(const std::shared_ptr<PacketHead> &packHeadPtr, const std::string &strData);
};

