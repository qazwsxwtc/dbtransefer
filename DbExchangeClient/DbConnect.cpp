#include "stdafx.h"
#include "DbConnect.h"


CDbConnect::CDbConnect()
{
}


CDbConnect::~CDbConnect()
{
}


//���������������ݿ�
int CDbConnect::Connect(SQLCHAR* pchServerName, SQLCHAR* pchUserName, SQLCHAR* pchPassword, std::string &errorstr)
{

	SQLRETURN retcode;
	//UCHAR sql1[79] = "select No from Student where Name='jim'";

	//���뻷�����
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "���뻷�����ʧ��! ::" << retcode << endl;
		errorstr = "���뻷�����ʧ��!";
		return -1;
	}

	//���û�������
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "���û�������ʧ��! ::" << retcode << endl;
		errorstr = "Set environment attribute failed!";
		return -1;
	}

	//�������ݿ����Ӿ��
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "�������ݿ����Ӿ��ʧ��! ::" << retcode << endl;
		errorstr = "Failed to apply database connection handle!";
		return -1;
	}

	//�������ݿ�                   
	retcode = SQLConnect(hdbc, (SQLCHAR*)pchServerName, SQL_NTS, (SQLCHAR*)pchUserName,
		SQL_NTS, (SQLCHAR*)pchPassword, SQL_NTS);

	/*SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	SQLTCHAR * srv1 = (SQLTCHAR * )"Provider =SQLOLEDB.1; Password =123; Persist Security Info =True; User ID =wtc; Initial Catalog =TransferDa; Data Source =127.0.0.1;";
	SQLTCHAR * srv = (SQLTCHAR *)"DRIVER={SQL Server};SERVER=SQLEXPRESS;DATABASE=TransferDa;UID=wtc;PWD=123;";
	retcode = SQLDriverConnect(hdbc, NULL , srv1, strlen((char *)srv1), OutConnStr,255,& OutConnStrLen,SQL_DRIVER_PROMPT);
*/
	//retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR *)c, SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);

	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		//int rr = SQLGetDiagRec(,);
		cout << "�������ݿ�ʧ��! ::" << retcode << endl;
		errorstr = "Failed to connect to database!";
		return -1;
	}
	cout << "�������ݿ�ɹ���" << endl;
	errorstr = "Sucess to connect to database!";
	return 0;
}

//�Ͽ����ݿ����ӣ��ͷž��
int CDbConnect::DisConnect()
{
	//�Ͽ����ݿ�����
	SQLDisconnect(hdbc);

	//�ͷ����ݿ����Ӿ��
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);

	//�ͷŻ������
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return 0;
}

//ִ��SQL��������
int CDbConnect::ExecDirect(SQLCHAR* strSQL, int iColNum, std::list<std::list<std::string> > &lst2data, std::string &errorstr)
{
	SQLRETURN retcode;
	SQLHSTMT hstmt; //SQL�����
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "����SQL�����ʧ�ܣ�" << endl;
		errorstr = "Failed to apply SQL statement handle!";
		return -1;
	}

	retcode = SQLExecDirect(hstmt, (SQLCHAR*)strSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "ִ��SQL���ʧ�ܣ�" << endl;
		errorstr = "Failed to execute SQL statement!";
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return -1;
	}

	unsigned  char LData[1024] = {0};


	SQLLEN lenOut[20] = { SQL_NTS };
	////SQLINTEGER lenOut2 = SQL_NTS;
	//
	for (int i = 0; i < iColNum; i++)
	{
		SQLBindCol(hstmt, i+1, SQL_C_CHAR, &(LData[i*32]) , 32, &lenOut[i]);
	}
	
	//SQLBindCol(hstmt, 2, SQL_C_CHAR, L2, sizeof(L2), &lenOut1);
	int ret = SQLFetch(hstmt);
	int iLine = 0;
	int iCols = 0;
	while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		iCols = 0;
		std::list<std::string> lstStr;
		for (int i = 0; i < iColNum; i++)
		{
			iCols++;
			std::string str = (char*)(&(LData[i * 32]));
			lstStr.push_back(str);
		}
		lst2data.push_back(lstStr);
		iLine++;
		

		memset(LData, 0, 1024);
		/*printf("%d\t%s\t%s\n", i, L1, L2);*/
		ret = SQLFetch(hstmt);
		
	}

	printf("��ȡ ����%d,  ����:%d\n", iLine, iCols);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return 0;

}
//
////����һ������Ϊcustomer�ı���������������Լ������
//int CDbConnect::CreateTable()
//{
//	//int ret = 0;
//	////���֮ǰ������������Ȱѱ�ɾ��
//	//char* strSql = "if exists (select 1  from  sysobjects  where  id = object_id('customer')  and   type = 'U')  drop table customer  ";
//	//ret = ExecDirect(strSql);
//
//	////��������Ϊcustomer�ı����������IndexID�������ļ��Լ�������ǲ���Ϊ�ա�
//	//strSql = "CREATE TABLE customer (IndexID int NOT NULL, Name char(50), Age int, PRIMARY KEY(IndexID))";
//	//ret = ExecDirect(strSql);
//	//if (ret == -1)
//	//{
//	//	cout << "Create Table is failed!" << endl;
//	//	return -1;
//	//}
//	//else
//	//{
//	//	cout << "Create Table is success!" << endl;
//	//	return 0;
//	//}
//	return 0;
////}
//
////��Ӽ�¼����ӵļ�¼������ַ���Ҫ�ر�ע��
//int CDbConnect::AddRecord(int IndexID, char* chName, int Age)
//{
//	//int ret = 0;
//
//	////���customer����Ӽ�¼,����Ҫ�ر�ע�⣬SQL����ַ��������ǵ����š�
//	//char strSql[100] = { 0 };
//	////sprintf(strSql, "INSERT INTO customer (IndexID , Name , Age ) VALUES(%d, '%s', %d)", IndexID, chName, Age);
//	//ret = ExecDirect(strSql);
//	//if (ret == -1)
//	//{
//	//	cout << "add record is failed!" << endl;
//	//	return -1;
//	//}
//	return 0;
//
////}
//
////Ŀǰʵ�ֲ�ѯָ��ID�ļ�¼���������Ϊ0����ȫ����ѯ
//int CDbConnect::Query(int IndexID)
//{
//	//int ret = 0;
//	//char strSql[100] = { 0 };
//	//SQLRETURN retcode;
//	//SQLINTEGER iIndexID, iAge;
//	//SQLCHAR szName[20];
//	//SQLINTEGER cbIndexID, cbName, cbAge;
//
//
//	//if (IndexID == 0)
//	//{
//
//	//	strcpy(strSql, "SELECT * FROM customer");
//
//	//	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
//	//	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
//	//	{
//	//		cout << "����SQL�����ʧ�ܣ�" << endl;
//	//		return -1;
//	//	}
//
//	//	retcode = SQLExecDirect(hstmt, (SQLCHAR*)strSql, SQL_NTS);
//	//	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
//	//	{
//	//		cout << "ִ��SQL���ʧ�ܣ�" << endl;
//	//		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
//	//		return -1;
//	//	}
//
//
//	//	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
//	//	{
//	//		SQLGetData(hstmt, 1, SQL_C_LONG, &iIndexID, 0, &cbIndexID);
//	//		SQLGetData(hstmt, 2, SQL_C_CHAR, szName, 20, &cbName);
//	//		SQLGetData(hstmt, 3, SQL_C_LONG, &iAge, 0, &cbAge);
//
//	//		printf("IndexID=%d , Name=%s,Age = %d \n", iIndexID, szName, iAge);
//	//	}
//	//	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
//	//	return 0;
//	//}
//	//else
//	//{
//	//	return 0;
//
//	//}
//	return 0;
//}