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
	//���������������ݿ�
	int Connect(SQLCHAR* pchServerName, SQLCHAR* pchUserName, SQLCHAR* pchPassword, std::string &errorstr);
	//�Ͽ����ݿ����ӣ��ͷž��
	int DisConnect();
	//ִ��SQL��������
	int ExecDirect(SQLCHAR* strSQL, int iColNum, std::list<std::list<std::string> > &vec2data, std::string &errorstr);
	//int CreateTable();
	//int AddRecord(int IndexID, char* chName, int Age);
	//int Query(int IndexID);//Ŀǰʵ�ֲ�ѯָ��ID�ļ�¼���������Ϊ0����ȫ����ѯ

private:
	SQLHENV henv;  //�������
	SQLHDBC hdbc;  //���ݿ����Ӿ��
	SQLHSTMT hstmt; //SQL�����
};

