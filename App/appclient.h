#pragma once
#include "tcpclient.h"
#include "json/json.h"
#include "cmd.h"
#include "tcpserver.h"
#include "serverwork.h"
#include <list>
#define SDKTEST 1
#define STARTCOUNT 1
#define RowCount 9
enum PLAYMONEYTYPE
{
	BOTHBACK = 0,//index 和 money 都回退
	MONEYBACK = 1,//只回退money
	RANDOMBACK = 2,//随机回退


};

enum PLAYTYPE
{
	PLAYHOME = 0,
	PLAYRADOM = 1,
	PLAYJINGYAN = 2,
};
struct  AppConifg
{
	int n = 7;
	std::string Chip;
	std::vector<int> m_nChipList;
	int methodlength = 11;
	int m_SplitMoney = 1000;
	std::map<int, std::string> m_Methodlist;
	int nPlayType = PLAYHOME;
	int nBackType = BOTHBACK;
	std::map<std::string, std::string> AIPalyRules;
	bool bEnableAiPlay = true;
	bool bEnableAIPlayLocal = true;
	AppConifg()
	{

	}
	std::string ToJson()
	{
		Json::Value root;
		root["n"] = n;
		root["Chip"] = Chip.c_str();
		root["methodlength"] = methodlength;
		root["SplitMoney"] = m_SplitMoney;
		return root.toStyledString();

	}
	void Parse(std::string str)
	{
		Json::Reader reader;
		Json::Value value;

		if (reader.parse(str.c_str(), value))
		{
			if (!value["n"].isNull())
			{
				n = value["n"].asInt();
			}
			if (!value["Chip"].isNull())
			{
				Chip = value["Chip"].asCString();
			}
			if (!value["methodlength"].isNull())
			{
				methodlength = value["methodlength"].asInt();
			}
			if (!value["SplitMoney"].isNull())
			{
				m_SplitMoney = value["SplitMoney"].asInt();
			}

		}

	}
	void ParseAiPaly(std::string str)
	{
		Json::Reader reader;
		Json::Value root;

		if (reader.parse(str.c_str(), root))
		{
			Json::Value arrayrules = root["rules"];
			for (int i = 0;i < arrayrules.size();i++)
			{
				AIPalyRules[arrayrules[i]["name"].asCString()] = arrayrules[i]["result"].asCString();
			}

		}

	}
	std::string ToJsonMethod()
	{
		Json::Value root;
		for (int i = 0;i < m_Methodlist.size();i++)
		{
			Json::Value methodarray;
			methodarray["TableId"] = i + 1;
			methodarray["Method"] = m_Methodlist[i + 1];
			root.append(methodarray);
		}
		return root.toStyledString();

	}
	void ParseMethod(std::string str)
	{
		Json::Reader reader;
		Json::Value value;

		if (reader.parse(str.c_str(), value))
		{
			for (int i = 0;i < value.size();i++)
			{
				int nTableId = value[i]["TableId"].asInt();
				std::string str = value[i]["Method"].asCString();
				if (str.length() == methodlength)
				{
					m_Methodlist[nTableId] = value[i]["Method"].asCString();
				}

			}
		}

	}
};
class AppClient :public ClientEvent
{
public:
	AppClient()
	{
		//  		char buf[22];
		//  		memset(buf, 0, 22);
		// 		for (int j = 0;j<10;j++)
		// 		{
		// 			for (int i = 0;i < 11;i++)
		// 			{
		// 				if (rand() % 2 == 1)
		// 				{
		// 					buf[i] = '1';
		// 				}
		// 				else
		// 				{
		// 					buf[i] = '0';
		// 				}
		// 			}
		// 			std::string str = buf;
		// 			for (int j = 1;j<10;j++)
		// 			{				
		// 				m_methodlist[j].push_back(str);
		// 			}
		// 		}

	}
	~AppClient()
	{

	}
	void Init();
	std::string GetRandMethod(int ncount = 11)
	{
		srand((unsigned)time(NULL) + GetTickCount64()+ ncount);
		char buf[50];
		memset(buf, 0, 50);
		for (int i = 0;i < ncount;i++)
		{
			for (int j = 0;j< rand()%1000+200;j++)
			{
				rand();
				rand();
				rand();
				rand();
			}
			int value = rand() % 4;
			switch (value)
			{
			case 0:
				buf[i] = '0';
				break;
			case 1:
				buf[i] = '1';
				break;
			case 2:
				buf[i] = '2';
				break;
			case 3:
				buf[i] = '3';
				break;
			default:
				buf[i] = '0';
				break;
			}		
		}
		std::string str = buf;
		return str; 

	}
	std::string GetJINGYANGMethod(int nTableId);
	virtual void OnConnected();
	virtual void OnDisconnect();
	virtual void OnMessage(char* pdata, int datalen);
	virtual void OnTimer();
	void Report(BoardInfoRequest& requsest);
	bool IsMain()
	{
		return m_bMainSub == MAIN_TYPE;
	}
	std::weak_ptr<CServerWork> GetServerWork()
	{
		serverwork_wptr;
	}
	void GetDataSerial(int nTableId, int nSetNumber, int nSubNumber);
	void UpdateMehtod()
	{
// 		MethodUpdateRequest request;
// 		request.SeqNum = m_Seq++;
// 		m_client.Send(request.ToStr());
// 		Sleep(2000);
	}
	void UpdateMehtod(std::string DataSerial, int MethodLength, int nTableId);
	std::string GetNewMethod(int TableId);
	std::vector<std::string> GetMethod(int TableId)
	{
		std::vector<std::string> methodlist;
		m_mutex.lock();

		if (m_methodlist[TableId].size() > 0)
		{
			methodlist = m_methodlist[TableId];
		}
		m_mutex.unlock();
		return methodlist;
	}
	bool IsConnect()
	{
		return  m_bRegister;
		return m_client.IsConnect();
	}
	void SendMethodTest(std::list <BoardInfoRequest>& Boardlist);
	void SendPictureUpload();
	bool IsMehtodHome();
	bool IsMehtodRandom();
	bool IsMehtodJingYang();
	void GetHistoryData(std::string sDataSerial, int nTableId, int CurSubName, std::vector<int> SubNumbers);
	void GetHistoryData(std::string sDataSerial, int nTableId, int CurSubName);
	std::string GetDataSerialStr(int nTableId);
private:
	int m_Seq = time(NULL);
	CTcpClient m_client;
	std::string m_localIp = "";
	int m_serverPort = 0;
	bool m_bMainSub = MAIN_TYPE;

public:
	std::string MachineCode;
	//sub info
	int nSubLastTIme = 0;
	std::string SubDeviceId;
	std::string SubMachineCode;
	std::string SubLastTimeOut;
	std::weak_ptr<CServerWork> serverwork_wptr;
	std::map<int, std::vector<std::string>> m_methodlist;
	std::map<int, int> m_TableMethodSerialList;
	std::map<int, std::string> m_TableDataSerial;
	std::map<int, MethodRule> m_TableRules;
	std::mutex m_mutex;
	std::string m_Token;
	bool m_bPermissions = true;
	bool m_bRegister = false;
	int m_nSendPicTime = 0;
	int m_nChangeMethodIndex = 0;
	//bool m_bUserRand = false;
	std::string m_ip;
	int m_nPort = 5107;
	bool m_bUseProxy = false;
	std::string m_ProxyIp;
	int m_nProxyPort = 1080;
	std::vector<std::string> m_chipList;
	bool m_bEnbaleChip = false;
	int m_n = 0;
	CRect m_rect;
	bool bEnableJY = false;
	int m_nRecvTime = 0;
};
extern AppClient* GetAppClient();