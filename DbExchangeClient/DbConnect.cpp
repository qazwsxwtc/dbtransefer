#include "stdafx.h"
#include "DbConnect.h"


CDbConnect::CDbConnect()
{
}


CDbConnect::~CDbConnect()
{
}


//申请句柄，连接数据库
int CDbConnect::Connect(SQLCHAR* pchServerName, SQLCHAR* pchUserName, SQLCHAR* pchPassword, std::string &errorstr)
{

	SQLRETURN retcode;
	//UCHAR sql1[79] = "select No from Student where Name='jim'";

	//申请环境句柄
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "申请环境句柄失败! ::" << retcode << endl;
		errorstr = "申请环境句柄失败!";
		return -1;
	}

	//设置环境属性
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "设置环境属性失败! ::" << retcode << endl;
		errorstr = "Set environment attribute failed!";
		return -1;
	}

	//申请数据库连接句柄
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "申请数据库连接句柄失败! ::" << retcode << endl;
		errorstr = "Failed to apply database connection handle!";
		return -1;
	}

	//连接数据库                   
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
		cout << "连接数据库失败! ::" << retcode << endl;
		errorstr = "Failed to connect to database!";
		return -1;
	}
	cout << "连接数据库成功！" << endl;
	errorstr = "Sucess to connect to database!";
	return 0;
}

//断开数据库连接，释放句柄
int CDbConnect::DisConnect()
{
	//断开数据库连接
	SQLDisconnect(hdbc);

	//释放数据库连接句柄
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);

	//释放环境句柄
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return 0;
}

//执行SQL语句的命令
int CDbConnect::ExecDirect(SQLCHAR* strSQL, int iColNum, std::list<std::list<std::string> > &lst2data, std::string &errorstr)
{
	SQLRETURN retcode;
	SQLHSTMT hstmt; //SQL语句句柄
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "申请SQL语句句柄失败！" << endl;
		errorstr = "Failed to apply SQL statement handle!";
		return -1;
	}

	retcode = SQLExecDirect(hstmt, (SQLCHAR*)strSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		cout << "执行SQL语句失败！" << endl;
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

	printf("读取 行数%d,  列数:%d\n", iLine, iCols);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return 0;

}
//
////创建一个名称为customer的表，设有主键和主键约束条件
//int CDbConnect::CreateTable()
//{
//	//int ret = 0;
//	////如果之前存在这个表，则先把表删除
//	//char* strSql = "if exists (select 1  from  sysobjects  where  id = object_id('customer')  and   type = 'U')  drop table customer  ";
//	//ret = ExecDirect(strSql);
//
//	////创建名称为customer的表，表的主键是IndexID，主键的检查约束条件是不能为空。
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
////添加记录，添加的记录如果是字符串要特别注意
//int CDbConnect::AddRecord(int IndexID, char* chName, int Age)
//{
//	//int ret = 0;
//
//	////向表customer中添加记录,这里要特别注意，SQL添加字符串必须是单引号。
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
////目前实现查询指定ID的记录，如果输入为0，则全部查询
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
//	//		cout << "申请SQL语句句柄失败！" << endl;
//	//		return -1;
//	//	}
//
//	//	retcode = SQLExecDirect(hstmt, (SQLCHAR*)strSql, SQL_NTS);
//	//	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
//	//	{
//	//		cout << "执行SQL语句失败！" << endl;
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