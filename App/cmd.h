#pragma once
#include <string>
#include <json/json.h>
#include "utils.h"

#define Version "V1.0"
#define MAIN_TYPE 0 //0��ʾ"��"
#define SUB_TYPE 1 //1��ʾ"��
enum AITYPE
{
	PLAYNONE = 0, //�޸�Ԥ
	PLAYMAN = 1,
	PLAYAI = 2,

};
struct PicUploadRequest
{
	int SeqNum = 0;
	std::string MainPic;
	std::string SubPic;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "PictureUpload";
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["Main"] = MainPic;
		body["Sub"] = SubPic;
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};
struct RegisterRequest
{
	int SeqNum = 0;
	int bMainType = MAIN_TYPE;
	std::string DeviceId;
	std::string LocalIp = "10.8.115.12";
	int LocalPort = 0;
	std::string Token;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "Register";
		root["SeqNum"] = SeqNum;
		root["Token"] = Token;
		Json::Value body;
		body["Version"] = Version;
		body["MainSub"] = MAIN_TYPE;
		body["DeviceId"] = DeviceId.c_str();
		body["LocalIp"] = LocalIp.c_str();
		body["LocalPort"] = LocalPort;
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}

};

struct RegisterResponse
{
	int SeqNum = 0;
	int Respcode = 0;
	std::string sVersion;
	std::string RespMsg;//���ݻ���������������������ö���ͬ��
	std::string MachineCode;//��ʾ�ڿͻ���ʱ"������"����ʾ6λ���֣���λ�����Ƿ����������ݺ�̨������ɵ�4λ���֣�ȷ�����ظ���ͬʱ�������֣�Ϊ�˷���ͻ���ͨʱ���õ��Ķ�Ӧ
	int PlayTypeEnbale;//ģ���淨,0��ʾ�رգ�1��ʾ�������˹����ɷ���˹����������������ʱ��
	int DeleteProgramEnable;//����������ɾ������0��ʾ��ɾ����1��ʾִ��ɾ��������־�����ɾ������ʱ����ʾ֪ͨ��
	std::string Token;  //���ƣ��ɷ����������������·����ͻ��˽��յ����������д��ء�
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		Respcode = root["Respcode"].asInt();
		sVersion = root["Version"].asCString();
		RespMsg = root["RespMsg"].asCString();
		if (Respcode == 0)
		{
			MachineCode = root["MachineCode"].asCString();
			if (!root["Token"].isNull())
			{
				Token = root["Token"].asCString();
			}		
		}	
		PlayTypeEnbale = root["PlayTypeEnbale"].asInt();
		DeleteProgramEnable = root["DeleteProgramEnable"].asInt();
		return true;
		
	}

};

struct KeepAliveRequest
{
	int SeqNum = 0;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "KeepLive";
		root["SeqNum"] = SeqNum;
		root["Body"] = Json::objectValue;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
	bool parse(std::string str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		return true;
	}

};

struct KeepAliveResponse
{
	int SeqNum = 0;
	int Respcode = 0;
	std::string RespMsg;
	std::string Token;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "KeepLive";
		root["SeqNum"] = SeqNum;
		
		Json::Value body;
		body["Respcode"] = Respcode;
		body["RespMsg"] = RespMsg.c_str();
		root["Body"] = body;
		
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		Respcode = root["Respcode"].asInt();
		if (!root["Token"].isNull())
		{
			Token = root["Token"].asCString();
		}
		return true;
	}

};
 
struct MethodUpdateRequest
{
	int SeqNum = 0;
	std::string DataSerial;
	int MethodLength = 11;
	int TableId = -1;
	int AIPlayEnable = 1;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "MethodUpdate";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["DataSerial"] = DataSerial.c_str();//���ţ�����Ϊ��0-99��������ʾ����̨��Ҫͬʱ����
		body["MethodLength"] = MethodLength;
		body["TableId"] = TableId;
		body["AIPlayEnable"] = AIPlayEnable;
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}

};
/*
"MethodLength": 11,              //�㷨���ȣ�����������һ���Ǵ�������
"PlayRule" : {
	"StartPlayIndex": 1,		//������ţ���Ŵ�0��ʼ��1��ʾ������1��
		"MatchedStopIndex" : 50   	//�ֺţ����к�ֹͣ
},
"ResetRule" : {
		"DetonateLength": 11,       //�������д������ȣ�Ĭ���롱MethodLength����ͬ
			"UseMatchReset" : true,     	//�б긴λ�����������ڴ���һ�κ�λ��
			"UseExplosionReset" : true,  //������λ
			"ResetIndex" : 0           	//�㷨��λ��������0��ʼ����ǰ���㷨
	},
		"AIRule": {
			"Enabled": false,			//�Ƿ�����AIPlayEnable�򷨣�true��ʾ���ã�false��ʾ������
				"AIPlayDic" : {				//AIƥ������ֵ䣬"000"��ʾ��ǰ���ǰ��λ�Ľ�����У�0��ʾ�У�1��ʾׯ
				"000": 0,
					"111" : 1,
					"1010" : 1,
					"0101" : 0,
					"11001" : 1,
					"00110" : 0,
					"01100" : 1,
					"10011" : 0,
					"011011" : 0,
					"100100" : 1,
					"001001" : 0,
					"110110" : 1,
					"010010" : 0,
					"101101" : 1
			},
				"UseRuleLengths": [3, 4, 5, 6],	//Ҫƥ��AI�����ֵ�ĳ��ȣ�����������
					"UnmatchClose" : true,			//�Ƿ����á�������ֹͣ��,
					"UseTimesNumber" : 3				//��һ�����������ڣ�����AIʹ�ô����������ϡ�UnmatchClose�����ȣ�������û�д��ֶ�ʱ��Ĭ��Ϊ1
		}
		*/
struct  MethodRule
{
	int MethodLength = 11;//�㷨���ȣ�����������һ���Ǵ�������
	int StartPlayIndex = 1;//������ţ���Ŵ�0��ʼ��1��ʾ������1��
	int MatchedStopIndex = 50;//�ֺţ����к�ֹͣ
	int DetonateLength = 11;//�������д������ȣ�Ĭ���롱MethodLength����ͬ
	bool UseMatchReset = true;//�б긴λ�����������ڴ���һ�κ�λ��
	bool UseExplosionReset = true;//������λ
	int ResetIndex = 0;           	//�㷨��λ��������0��ʼ����ǰ���㷨
	bool Enabled = false;
	std::map<std::string, int> AIPlayDic;
	int minRuleLen = -1;
	int maxRuleLen = -1;
	bool UnmatchClose = true; //�Ƿ����á�������ֹͣ��,
	int UseTimesNumber = 3;////��һ�����������ڣ�����AIʹ�ô����������ϡ�UnmatchClose�����ȣ�������û�д��ֶ�ʱ��Ĭ��Ϊ1
	void Parse(Json::Value JMethodRule)
	{
		if (JMethodRule["MethodLength"].isInt())
		{
			MethodLength = JMethodRule["MethodLength"].asInt();
		}
		if (JMethodRule["PlayRule"]["StartPlayIndex"].isInt())
		{
			StartPlayIndex = JMethodRule["PlayRule"]["StartPlayIndex"].asInt();
		}
		if (JMethodRule["PlayRule"]["MatchedStopIndex"].isInt())
		{
			MatchedStopIndex = JMethodRule["PlayRule"]["MatchedStopIndex"].asInt();
		}
		if (JMethodRule["ResetRule"]["DetonateLength"].isInt())
		{
			DetonateLength = JMethodRule["ResetRule"]["DetonateLength"].asInt();
		}
		if (JMethodRule["ResetRule"]["UseMatchReset"].isBool())
		{
			UseMatchReset = JMethodRule["ResetRule"]["DetonateLength"].asBool();
		}
		if (JMethodRule["ResetRule"]["UseExplosionReset"].isBool())
		{
			UseExplosionReset = JMethodRule["ResetRule"]["UseExplosionReset"].asBool();
		}
		if (JMethodRule["ResetRule"]["ResetIndex"].isInt())
		{
			ResetIndex = JMethodRule["ResetRule"]["ResetIndex"].asInt();
		}
		if (JMethodRule["AIRule"]["Enabled"].isBool())
		{
			Enabled = JMethodRule["AIRule"]["Enabled"].asBool();
		}
		if (JMethodRule["AIRule"]["UnmatchClose"].isBool())
		{
			UnmatchClose = JMethodRule["AIRule"]["UnmatchClose"].asBool();
		}
		if (JMethodRule["AIRule"]["UseTimesNumber"].isInt())
		{
			UseTimesNumber = JMethodRule["AIRule"]["UseTimesNumber"].asInt();
		}
		Json::Value JAIPlayDic = JMethodRule["AIRule"]["AIPlayDic"];
		{
			std::string name;
			Json::Value value;
			Json::Value::Members members;
			members = JAIPlayDic.getMemberNames();   // ��ȡ����key��ֵ
			for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++)   // ����ÿ��key
			{
				name = *iterMember;
				int namelen = name.length();
				AIPlayDic[name] = JAIPlayDic[name].asInt();
				if (minRuleLen == -1)
				{
					minRuleLen = namelen;
				}
				else
				{
					if (namelen < minRuleLen)
					{
						minRuleLen = namelen;
					}
				}
				if (maxRuleLen == -1)
				{
					maxRuleLen = namelen;
				}
				else
				{
					if (namelen > maxRuleLen)
					{
						maxRuleLen = namelen;
					}
				}
			}
			
		}

	}
};
struct MethodUpdateResponse
{
	int SeqNum = 0;
	std::string RespMsg;
	int Respcode = 0;//��0����ʾ�ɹ�����1����ʾʧ�ܣ��������˳����ط�3�Ρ�
	
	int TableId = -1;
	std::string DataSerial;
	int MethodSerial = -1;
	MethodRule mehtodRule;
	std::vector<std::string> methodlist;
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		RespMsg = root["RespMsg"].asCString();
		Respcode = root["Respcode"].asInt();
		if (Respcode == 0)
		{
			
			Json::Value TableBody = root["Body"];
			Json::Value JMethodRule = TableBody["MethodRule"];
			mehtodRule.Parse(JMethodRule);
			TableId = TableBody["TableId"].asInt();
			DataSerial  = TableBody["DataSerial"].asCString();
			MethodSerial = TableBody["MethodSerial"].asInt();
			Json::Value JMethod = TableBody["Method"];
			for (int i = 0;i < JMethod.size();i++)
			{
				methodlist.push_back(JMethod[i].asCString());
			}
		}
		return true;
	}

};

struct BoardInfoRequest
{
	int SeqNum = -1;
	std::string AccountName;////��ҳ����ȡ���˻�
	int AccountMoney = -1; // ��ҳ����ȡ���˻����
	int StopLoss = 0; //ֹ�𣨡������ͻ����������룬���ӡ��ͻ��˽���ʾ���ݣ�
	int StopProfit = 0;//ֹӯ���������ͻ����������룬���ӡ��ͻ��˽���ʾ���ݣ�
	int TableId = -1;
	std::string Method;
	int SetNumber = -1;//ѥ�ţ�����ʶ���"-"ǰ�����֣����99 ��
	int SubNumber = -1;//�ֺţ�����ʶ���"-"������֣����99
	int PlayType = 0;//�淨���ͣ���ʵΪ��0����ģ��Ϊ��1����ģ��򷨽��������ע��ʱ��ѡ��ȡ����ť����ʵ��ѡ��ȷ��  
	int BetType = -1;//ѹע����˵������Ӧ���֣��У�0����ׯ��1�����ͣ�2����ׯ�ԣ�3�����жԣ�4����ʵ������У�ֻ�¡�0����1��
	std::vector<int> BetResult;//����ƵĽ�����硰[0]��[034]�����3�����֣�
	int CoinsBet = 0;////Ѻע��Ĭ������Ϊ��100��300��800��1700��3600��7500��1550��32000��66000.136000�����̶ֹ����ʽ�򷨣�����һ����ͳ����㷨�򷨡�
	int CoinsBetSub = 0;
	int Profit = 0;//�������棬ӯΪ������Ϊ��;����¡�ׯ(1)����ӯ����ע���*0.95�����ʱ����ע���*1*1������¡���(0)����
			   //ӯ����ע���*1�����ʱ����ע���*1*1��"��"��ʱ�򲻹�ѹׯ�����У���ȫ���˻ء�
	int EndTime = 0;//���ֽ���ʱ��
	std::string time;
	int BetResultType; //��ע���
	int methodIndex = 0;
	int MainSub = MAIN_TYPE;
	int Random = 1;
	int Batch = 0;
	std::string DataSerial = "";
	bool bReport = false;
	int AIPlay = PLAYNONE;
	std::string strAi;
	int DataSources = 1;
	BoardInfoRequest()
	{

	}
	BoardInfoRequest(const BoardInfoRequest& b)
	{
		SeqNum = b.SeqNum;
		AccountName = b.AccountName;
		std::string ;////��ҳ����ȡ���˻�
		AccountMoney = b.AccountMoney;
		StopLoss = b.StopLoss;
		StopProfit = b.StopProfit;
		TableId = b.TableId;
		Method = b.Method;
		SetNumber = b.SetNumber;
		SubNumber = b.SubNumber;
		PlayType = b.PlayType;
		BetType = b.BetType;
		BetResult = b.BetResult;
		CoinsBet = b.CoinsBet;
		CoinsBetSub = b.CoinsBetSub;
		EndTime = b.EndTime;
		time = b.time;
		BetResultType = b.BetResultType;
		methodIndex = b.methodIndex;
		MainSub = b.MainSub;
		Random = b.Random;
		Batch = b.Batch;
		DataSerial = b.DataSerial;
		bReport = b.bReport;
		AIPlay = b.AIPlay;
		strAi = b.strAi;
		DataSources = b.DataSources;
		Profit = b.Profit;
	}

	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "BoardInfo";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["AccountName"] = AccountName.c_str();
		body["AccountMoney"] = AccountMoney;
		body["StopLoss"] = StopLoss;
		body["StopProfit"] = StopProfit;
		body["TableId"] = TableId;
		body["Method"] = Method.c_str();
		body["SetNumber"] = SetNumber;
		body["SubNumber"] = SubNumber;
		body["PlayType"] = PlayType;
		body["BetType"] = BetType;
		Json::Value JBetResult;
		for (int i = 0;i < BetResult.size();i++)
		{
			JBetResult.append(BetResult[i]);
		}
		body["BetResult"] = JBetResult;
		body["CoinsBet"] = CoinsBet;
		body["Profit"] = Profit;
		body["EndTime"] = EndTime;
		body["methodIndex"] = methodIndex;
		body["MainSub"] = MainSub;
		body["Random"] = Random;
		body["MethodSerial"] = Batch;
// 		char buf[1024];
// 		sprintf(buf,"%s-%s-%s")
		body["DataSerial"] = DataSerial;
		body["AIPlay"] = AIPlay;
		body["DataSources"] = DataSources;  //��ָ���ż�ѥ�ţ�"0"��ʾʶ��"1"��ʾ��ҳ���ƣ�"2"��ʾ����������
		body["AIType"] = strAi.c_str();
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};
struct DataSerialCheckRequest
{
	int SeqNum = -1;
	int TableId = -1;
	int CycleNumber = 1;
	int SetNumber = -1;
	int SubNumber = 1;
	std::string Date;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "DataSerialCheck";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["TableId"] = TableId;
		body["CycleNumber"] = CycleNumber;
		body["SetNumber"] = SetNumber;	
		body["SubNumber"] = SubNumber;
		body["Date"] = Date.c_str();
		root["Body"] = body;
		return root.toStyledString();
	}
};
struct  LastSerial
{
	int MinTime = 0;
	int MaxTime = 0;
	std::string SerialCode;
	int SetNumber = 0;
	int TableNumber = 0;
	int DateNumber = 0;
};
struct DataSerialCheckResponse
{
	std::string DataSerial;
	int TableId = -1;
	int SeqNum = 0;
	std::string RespMsg;
	int Respcode = 0;//��0����ʾ�ɹ�����1����ʾʧ�ܣ��������˳����ط�3�Ρ�
	std::string lastDataSerial;
	int lastSetNumber = -1;
	int lastSubNumber = -1;
	int lastEndTime = -1;
	std::vector<LastSerial> LastSerials;
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		Json::Value Jlast;
		Json::Value JLastSerialList;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		RespMsg = root["RespMsg"].asCString();
		Respcode = root["Respcode"].asInt();
		TableId = root["TableId"].asInt();
		if(root["DataSerial"].isString())
			DataSerial = root["DataSerial"].asCString();
		if (!root["body"].isNull())
		{
			Jlast = root["body"]["LastSerial"];
		}
		else
		{
			Jlast = root["LastSerial"];
		}
		if (!Jlast.isNull())
		{
			lastDataSerial = Jlast["DataSerial"].asCString();
			lastSetNumber = Jlast["SetNumber"].asInt();
			lastSubNumber = Jlast["SubNumber"].asInt();
			lastEndTime = Jlast["EndTime"].asInt();
		}
		JLastSerialList = root["SerialList"];
		if (!JLastSerialList.isNull())
		{
			for (int i = 0;i < JLastSerialList.size();i++)
			{
				LastSerial s;
				s.MinTime = JLastSerialList[i]["MinTime"].asInt();
				s.MaxTime = JLastSerialList[i]["MaxTime"].asInt();
				s.SerialCode = JLastSerialList[i]["SerialCode"].asCString();
				s.SetNumber = JLastSerialList[i]["SetNumber"].asInt();
				s.TableNumber = JLastSerialList[i]["TableNumber"].asInt();
				s.DateNumber = JLastSerialList[i]["DateNumber"].asInt();
				if (s.TableNumber == TableId)
				{
					LastSerials.push_back(s);
				}
			}
		}
	
		return true;
	}
	
};


struct MethodTestRequest
{
	int SeqNum = -1;
	int TableId = -1;;
	int SetNumber = -1;
	std::string DataSerial;
	std::string SubNumberList;
	std::string MethodSerial;
	int MethodLength = 0;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "MethodTest";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["TableId"] = TableId;
		body["SetNumber"] = SetNumber;
		body["DataSerial"] = DataSerial;
		body["SubNumberList"] = SubNumberList;
		body["MethodSerial"] = MethodSerial;
		body["MethodLength"] = MethodLength;
		Json::Value JAIPlayEnable;
		JAIPlayEnable.append(1);
		body["AIPlayEnable"] = JAIPlayEnable;
		body["DataSources"] = 1;  //��ָ���ż�ѥ�ţ�"0"��ʾʶ��"1"��ʾ��ҳ���ƣ�"2"��ʾ����������
		root["Body"] = body;
		return root.toStyledString();
	}
};




struct rbgbuf
{
	rbgbuf()
	{

	}
	~rbgbuf()
	{
		if (buf)
		{
			delete[] buf;
		}
	}
	uint8_t*  buf = NULL;
};





























struct SubBetsRequest
{
	int SetNumber = -1;//ѥ�ţ�����ʶ���"-"ǰ�����֣����99 ��
	int SubNumber = -1;//�ֺţ�����ʶ���"-"������֣����99
	int BetType;//ѹע����˵������Ӧ���֣��У�0����ׯ��1�����ͣ�2����ׯ�ԣ�3�����жԣ�4����ʵ������У�ֻ�¡�0����1��
	int SeqNum = -1;
	int TableId = -1;
	int CoinsBet = 0;////Ѻע��Ĭ������Ϊ��100��300��800��1700��3600��7500��1550��32000��66000.136000�����̶ֹ����ʽ�򷨣�����һ����ͳ����㷨�򷨡�
	std::string Method;
	int methodindex  = 0;
	
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "SubBetsRequest";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["TableId"] = TableId;
		body["Method"] = Method.c_str();
		body["SetNumber"] = SetNumber;
		body["SubNumber"] = SubNumber;
		body["BetType"] = BetType;
		body["methodindex"] = methodindex;
		
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		TableId = root["TableId"].asInt();
		SetNumber = root["SetNumber"].asInt();
		SubNumber = root["SubNumber"].asInt();
		BetType = root["BetType"].asInt();
		TableId = root["TableId"].asInt();
		CoinsBet = root["CoinsBet"].asInt();
		methodindex = root["methodindex"].asInt();
		Method = root["Method"].asCString();
		return true;
	}
};







struct SubBetsResponse
{
	int SetNumber = -1;//ѥ�ţ�����ʶ���"-"ǰ�����֣����99 ��
	int SubNumber = -1;//�ֺţ�����ʶ���"-"������֣����99
	int BetType;//ѹע����˵������Ӧ���֣��У�0����ׯ��1�����ͣ�2����ׯ�ԣ�3�����жԣ�4����ʵ������У�ֻ�¡�0����1��
	int SeqNum = -1;
	int TableId = -1;
	int CoinsBet = 0;////Ѻע��Ĭ������Ϊ��100��300��800��1700��3600��7500��1550��32000��66000.136000�����̶ֹ����ʽ�򷨣�����һ����ͳ����㷨�򷨡�
	std::string Method;
	int methodindex = 0;
	int Profit = 0;//�������棬ӯΪ������Ϊ��;����¡�ׯ(1)����ӯ����ע���*0.95�����ʱ����ע���*1*1������¡���(0)����
				   //ӯ����ע���*1�����ʱ����ע���*1*1��"��"��ʱ�򲻹�ѹׯ�����У���ȫ���˻ء�
	std::vector<int> BetResult;//����ƵĽ�����硰[0]��[034]�����3�����֣�
	
	void copy(SubBetsRequest& request)
	{
		SetNumber = request.SetNumber;
		SubNumber = request.SubNumber;
		BetType = request.BetType;
		SeqNum = request.SeqNum;
		TableId = request.TableId;
		CoinsBet = request.CoinsBet;
		Method = request.Method;
		methodindex = request.methodindex;
	}
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "SubBetsResponse";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["TableId"] = TableId;
		body["Method"] = Method.c_str();
		body["SetNumber"] = SetNumber;
		body["SubNumber"] = SubNumber;
		body["BetType"] = BetType;
		body["methodindex"] = methodindex;
		body["Profit"] = Profit;
		Json::Value JBetResult;
		for (int i = 0;i < BetResult.size();i++)
		{
			JBetResult.append(BetResult[i]);
		}
		body["BetResult"] = JBetResult;
		
		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		SeqNum = root["SeqNum"].asInt();
		TableId = root["TableId"].asInt();
		SetNumber = root["SetNumber"].asInt();
		SubNumber = root["SubNumber"].asInt();
		BetType = root["BetType"].asInt();
		TableId = root["TableId"].asInt();
		CoinsBet = root["CoinsBet"].asInt();
		methodindex = root["Methodindex"].asInt();
		Profit = root["Profit"].asInt();
		Method = root["Method"].asCString();

		Json::Value JBetResult = root["BetResult"];
		for (int i = 0;i < JBetResult.size();i++)
		{
			BetResult.push_back(JBetResult[i].asInt());
		}
		return true;
	}
};
struct DataSourceRequest
{
	int SeqNum;
	int TableId = -1;
	std::string DataSerial;//Ҫ��ȡ���������κ�
	int CurSubName = -1;//�ͻ��˵�ǰҪ��ľֺ�
	std::string SubNumbers;//Ҫ��ȡ���ݵľֺţ����Զ������Ӣ�Ķ��ŷָ�����Ϊ�ձ�ʾȡС�ڵ�ǰ�ֺŵ����о�����
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "DataSource";//�㷨����
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["DataSerial"] = DataSerial.c_str();
		body["SubNumbers"] = SubNumbers.c_str();
		body["CurSubName"] = CurSubName;
		body["TableId"] = TableId;

		root["Body"] = body;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};

struct DataSourceResponse
{
	std::string DataSerial;
	int TableId = -1;
	std::vector<BoardInfoRequest> requests;
	bool parse(const char* str)
	{
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(str, root, false))
		{
			return false;
		}
		Json::Value JBody = root["Body"];
		if (JBody.isNull())
		{
			return true;
		}
		TableId = JBody["TableId"].asInt();
		DataSerial = JBody["DataSerial"].asCString();
		Json::Value JSubNumbers = JBody["SubNumbers"];
		for (int i = 0;i<JSubNumbers.size();i++)
		{
			BoardInfoRequest request;
			request.TableId = TableId;
			request.SetNumber = GetSetNumber(DataSerial);
			request.SubNumber = JSubNumbers[i]["Number"].asInt();
			request.BetResultType = JSubNumbers[i]["Value"].asInt();
			request.EndTime = JSubNumbers[i]["Time"].asInt();
			request.AIPlay = 9;
			request.DataSerial = DataSerial;
			request.DataSources = 2;
			request.BetResult.push_back(request.BetResultType);
			requests.push_back(request);
		}
		return true;
	}
};
struct ClickCancleInfo
{
	int TabldId = -1;
	int nSetNumber = -1;
	int nSubNumber = -1;
	int x = -1;
	int y = -1;
	int cancletime = -1;
	bool bclick = true;
};