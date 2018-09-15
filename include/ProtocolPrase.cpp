
//#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "ProtocolPrase.h"
#include "json/json.h"
using namespace Json;


int CProtocolPrase::m_clinetNo = 0;

CProtocolPrase::CProtocolPrase()
{
	
}


CProtocolPrase::~CProtocolPrase()
{
}

int CProtocolPrase::AllcoCLientNum()
{
	return ++m_clinetNo;
}


void CProtocolPrase::GetHeadInfo(const std::string &strData, std::shared_ptr<PacketHead> &packHead, bool bNeedHead /*= true*/)
{
	if (strData.length()< HeadLen) return;

	packHead = std::make_shared<PacketHead>(*(new PacketHead));
	packHead->strheadTitle = strData.substr(0, 2);
	//memcpy(&(packHead->iClientNo), &(strData[2]), sizeof(int));

	unsigned int i24data = (unsigned char)strData[2] << 24;
	unsigned int i16data = (unsigned char)strData[3] << 16;
	unsigned int i8data = (unsigned char)strData[4] << 8;
	unsigned int i0data = (unsigned char)strData[5];
	packHead->iClientNo = i24data + i16data + i8data + i0data;
	
	packHead->iClientType = (unsigned char)strData[6];  //0, CV; 1CD
	packHead->iFuncNo = (unsigned char)strData[7];
	//memcpy(&(packHead->iBodyLen), &(strData[8]), sizeof(int));
	unsigned int i24body = (unsigned char)strData[8] << 24;
	unsigned int i16body = (unsigned char)strData[9] << 16;
	unsigned int i8body = (unsigned char)strData[10] << 8;
	unsigned int i0body = (unsigned char)strData[11];
	packHead->iBodyLen = i24body + i16body + i8body + i0body;
	if (bNeedHead && strData.length() != packHead->iBodyLen + HeadLen)
	{
		printf("error len packsize=%d, needlen =%d", strData.length(), packHead->iBodyLen + HeadLen);
		packHead.reset();
	}

}

void CProtocolPrase::GetLogOnInfo(const std::string & strData, std::string & userinfo, std::string & pwd)
{
	try
	{
		Json::CharReaderBuilder bReader;
		Json::CharReader* reader(bReader.newCharReader());
		Json::Value root;
		JSONCPP_STRING errs;
		if (reader->parse((char*)(strData.c_str() + HeadLen),
			(char*)(strData.c_str() + strData.length()), &root, &errs))
		{
			userinfo = root["username"].asString();
			pwd = root["pwd"].asString();

		}
	}
	catch (...)
	{

	}
	
}

void CProtocolPrase::PackLogOnInfo(const std::string & usetname, const std::string & pwd, std::string & strbody)
{
	Json::Value root;
	
	Json::StreamWriterBuilder builder;

	//builder.settings_["result"] = bSuccess;

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;
	
	root["username"] = usetname;
	root["pwd"] = pwd;

	writer->write(root, &os);
	strbody = os.str();


}

void CProtocolPrase::GetConnectDbInfo(const std::string & strData, ConnectDBInfo & oConnectDBInfo)
{
	try
	{
		Json::CharReaderBuilder bReader;
		Json::CharReader* reader(bReader.newCharReader());
		Json::Value root;
		JSONCPP_STRING errs;
		if (reader->parse((char*)(strData.c_str() + HeadLen),
			(char*)(strData.c_str() + strData.length()), &root, &errs))
		{
			oConnectDBInfo.strSqlUsername = root["username"].asString();
			oConnectDBInfo.strSqlPwd = root["pwd"].asString();
			oConnectDBInfo.strDbname = root["dbname"].asString();
			oConnectDBInfo.eDbType = (enumDataType)root["dbtype"].asInt();

		}
	}
	catch (...)
	{

	}
}

bool CProtocolPrase::GetConnectDbReturnInfo(const std::string &strData)
{
	Json::CharReaderBuilder bReader;
	Json::CharReader* reader(bReader.newCharReader());
	Json::Value root;
	JSONCPP_STRING errs;
	//std::string strBody = strData.substr(HeadLen, strData.length());
	if (reader->parse((char*)(strData.c_str() + HeadLen),
		(char*)(strData.c_str() + strData.length()), &root, &errs))
	{
		return root["result"].asBool();
	}
	return false;
}

void CProtocolPrase::GetSelectCmd(const std::string & strData, std::string & strSqlCmd, int &iCol)
{
	try
	{
		Json::CharReaderBuilder bReader;
		Json::CharReader* reader(bReader.newCharReader());
		Json::Value root;
		JSONCPP_STRING errs;
		if (reader->parse((char*)(strData.c_str() + HeadLen),
			(char*)(strData.c_str() + strData.length()), &root, &errs))
		{
			strSqlCmd = root["sqlcmd"].asString();

			iCol = root["count"].asInt();

			/*for (int i = 0; i < (int)arrVal.size(); i++)
			{
				std::string name = arrVal[i]["name"].asString();
				lstName.push_back(name);
			}*/

			
		}
	}
	catch (...)
	{

	}
}

void CProtocolPrase::GetSelectReturnCmd(const std::string & strData, std::string & strSqlData)
{
}

void CProtocolPrase::GetReturnResult(bool bSuccess, const std::string &errstr, std::string & strdata)
{
	Json::Value root;
	/*itemdata["uid"] = 1;

	Json::Value lstjson;
	for (int i = 0; i < 4; i++)
	{
		Json::Value arr;
		arr["port"] = 1 + i;
		lstjson.append(arr);
	}
	itemdata["portlist"] = lstjson;*/


	/*Json::FastWriter writer;
	std::string pas = writer.write(itemdata);*/



	Json::StreamWriterBuilder builder;

	//builder.settings_["result"] = bSuccess;

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;
	root["result"] = bSuccess;
	root["error"] = errstr;
	writer->write(root, &os);
	strdata = os.str();

}

void CProtocolPrase::GetReturnResult(const std::string & strDb, std::string & strdata)
{
	Json::Value root;
	/*itemdata["uid"] = 1;

	Json::Value lstjson;
	for (int i = 0; i < 4; i++)
	{
	Json::Value arr;
	arr["port"] = 1 + i;
	lstjson.append(arr);
	}
	itemdata["portlist"] = lstjson;*/


	/*Json::FastWriter writer;
	std::string pas = writer.write(itemdata);*/



	Json::StreamWriterBuilder builder;

	//builder.settings_["data"] = strDb;

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;
	root["data"] = strDb;

	writer->write(root, &os);
	strdata = os.str();
}

void CProtocolPrase::GetRerurnString(int iClientNo, int iClientType, int iFuncNo, const std::string & strBody, std::string & strPacket)
{
	char szHead[HeadLen] = { 0 };
	unsigned int iLen = (unsigned int)strBody.length();
	szHead[0] = 'W';
	szHead[1] = 'J';

	szHead[2] = ((iClientNo & 0xff0000) >> 24);
	szHead[3] = (iClientNo & 0xff0000) >> 16;
	szHead[4] = (iClientNo & 0xff00) >> 8;
	szHead[5] = (iClientNo & 0xff);
	////memcpy(szHead + 2, &iClientNo, sizeof(int));
	szHead[6] = iClientType;
	szHead[7] = iFuncNo;
	////memcpy(szHead + 8, &iLen, sizeof(int));
	unsigned int iss = iLen & 0xff000000;
	szHead[8] = (iss) >> 24;
	szHead[9] = (iLen & 0xff0000) >> 16;
	szHead[10] = (iLen & 0xff00) >> 8;
	szHead[11] = (iLen & 0xff);

	for (int i = 0; i < HeadLen; i++)
	{
		strPacket += szHead[i];
	}
	strPacket += strBody;
}

void CProtocolPrase::PacketDbData(const std::list<std::list<std::string> > &vec2data, int iCols, const std::string &errstr, std::string &dataStr)
{
	Json::Value root;
	Json::Value Linejson;
	for (auto listline = vec2data.begin(); listline != vec2data.end(); listline++)
	{
		

			Json::Value Coljson;
			std::vector<std::string> vecName;
			
			int i = 0;
			for (auto col = listline->begin(); col != listline->end(); col++)
			{
				char szCol[64] = { 0 };
				sprintf_s(szCol, "%d", i);
				std::string coldName = szCol;
				Coljson[coldName] = *col;
				i++;
			}
			Linejson.append(Coljson);
		
		
		
	}
	root["data"] = Linejson;
	root["error"] = errstr;
	Json::StreamWriterBuilder builder;

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;

	writer->write(root, &os);
	dataStr = os.str();
}

void CProtocolPrase::PacketDbUnConn(const std::string & errstr, std::string & strBody)
{
	Json::Value root;

	Json::StreamWriterBuilder builder;

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;
	root["error"] = errstr;

	writer->write(root, &os);
	strBody = os.str();
}

