#pragma once
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include <iostream>
#include <vector>
#include <list>

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

class CDbConnect
{
public:
	CDbConnect();
	~CDbConnect();




public:
	//申请句柄，连接数据库
	int Connect(SQLCHAR* pchServerName, SQLCHAR* pchUserName, SQLCHAR* pchPassword, std::string &errorstr);
	//断开数据库连接，释放句柄
	int DisConnect();
	//执行SQL语句的命令
	int ExecDirect(SQLCHAR* strSQL, int iColNum, std::list<std::list<std::string> > &vec2data, std::string &errorstr);
	//int CreateTable();
	//int AddRecord(int IndexID, char* chName, int Age);
	//int Query(int IndexID);//目前实现查询指定ID的记录，如果输入为0，则全部查询

private:
	SQLHENV henv;  //环境句柄
	SQLHDBC hdbc;  //数据库连接句柄
	SQLHSTMT hstmt; //SQL语句句柄
};

