#pragma once
#include <string>
#include <memory>
#include <list>

#define HeadLen	12

struct PacketHead
{
	std::string  strheadTitle;
	int          iClientNo;
	int			 iClientType;  //0, CV; 1CD
	int			 iFuncNo;
	int			 iBodyLen;
};

enum enumDataType
{
	sqlserverenum = 1,
	mysqlenum = 2,
	oracleenum = 3,
};
struct ConnectDBInfo
{
	std::string strSqlUsername;
	std::string strSqlPwd;
	std::string strDbname;
	enumDataType	eDbType;
};


class _declspec(dllexport) CProtocolPrase
{
public:
	CProtocolPrase();
	~CProtocolPrase();

	static int AllcoCLientNum();

	//��ȡ��ͷ��Ϣ
	static void GetHeadInfo(const std::string &strData ,std::shared_ptr<PacketHead> &packHead, bool bNeedHead=true);

	//��ȡ��¼��Ϣ
	static void GetLogOnInfo(const std::string &strData, std::string &userinfo, std::string &pwd);

	//�����¼��Ϣ
	static void PackLogOnInfo(const std::string &usetname, const std::string &pwd, std::string &strbody);

	//��ȡ���ݿ�������Ϣ
	static void GetConnectDbInfo(const std::string &strData, ConnectDBInfo &oConnectDBInfo);
	//��ȡ���ݿ����ӽ����Ϣ
	static bool GetConnectDbReturnInfo(const std::string &strData);
	
	//��ȡ��ѯ������Ϣ
	static void GetSelectCmd(const std::string &strData, std::string &strSqlCmd, int &iCol);
	static void GetSelectReturnCmd(const std::string &strData, std::string &strSqlData);

	//����ɹ�ʧ����Ϣ
	static void GetReturnResult(bool bSuccess, const std::string &errstr, std::string &stbody);

	//����ַ�����Ϣ
	static void GetReturnResult(const std::string &strDb, std::string &strbody);

	//�������Ϣ
	static void GetRerurnString(int iClientNo, int iClientType,int iFuncNo, const std::string &strBody, std::string &strPacket);

	//
	static void PacketDbData(const std::list<std::list<std::string> > &vec2data, int iCols, const std::string &errstr, std::string  &dataStr);

	static void PacketDbUnConn(const std::string &errstr, std::string &strBody);
private:
	static int m_clinetNo;
};

