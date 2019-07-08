#pragma once
#include <string>
#include <json/json.h>
#include "utils.h"

#define Version "V1.0"
#define MAIN_TYPE 0 //0表示"主"
#define SUB_TYPE 1 //1表示"从
enum AITYPE
{
	PLAYNONE = 0, //无干预
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
	std::string RespMsg;//内容会依服务器上面的条件配置而不同。
	std::string MachineCode;//显示在客户端时"机器码"，显示6位数字，这位数字是服务器端依据后台随机生成的4位数字，确保不重复。同时就是数字，为了方便客户沟通时，用到的对应
	int PlayTypeEnbale;//模拟玩法,0表示关闭，1表示开启。此功能由服务端管理。软件重新启动的时候
	int DeleteProgramEnable;//后续开发，删除程序，0表示不删除，1表示执行删除程序及日志命令，此删除程序时不提示通知，
	std::string Token;  //令牌，由服务器端主动生成下发，客户端将收到后，在心跳中带回。
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
		root["Type"] = "MethodUpdate";//算法更新
		root["SeqNum"] = SeqNum;
		Json::Value body;
		body["DataSerial"] = DataSerial.c_str();//桌号，数字为：0-99，两个表示有两台需要同时更新
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
"MethodLength": 11,              //算法长度，理论上它不一定是触爆长度
"PlayRule" : {
	"StartPlayIndex": 1,		//开打序号，序号从0开始，1表示跳过第1局
		"MatchedStopIndex" : 50   	//局号，打中后停止
},
"ResetRule" : {
		"DetonateLength": 11,       //连续不中触爆长度，默认与”MethodLength“相同
			"UseMatchReset" : true,     	//中标复位，触爆长度内打中一次后复位。
			"UseExplosionReset" : true,  //触爆复位
			"ResetIndex" : 0           	//算法复位索引，从0开始，当前的算法
	},
		"AIRule": {
			"Enabled": false,			//是否启用AIPlayEnable打法，true表示启用，false表示不启用
				"AIPlayDic" : {				//AI匹配规则字典，"000"表示当前结果前三位的结果排列；0表示闲，1表示庄
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
				"UseRuleLengths": [3, 4, 5, 6],	//要匹配AI规则字典的长度，按升序排列
					"UnmatchClose" : true,			//是否启用“不中则停止”,
					"UseTimesNumber" : 3				//在一个触爆长度内，限制AI使用次数。规则上”UnmatchClose“优先，程序中没有此字段时，默认为1
		}
		*/
struct  MethodRule
{
	int MethodLength = 11;//算法长度，理论上它不一定是触爆长度
	int StartPlayIndex = 1;//开打序号，序号从0开始，1表示跳过第1局
	int MatchedStopIndex = 50;//局号，打中后停止
	int DetonateLength = 11;//连续不中触爆长度，默认与”MethodLength“相同
	bool UseMatchReset = true;//中标复位，触爆长度内打中一次后复位。
	bool UseExplosionReset = true;//触爆复位
	int ResetIndex = 0;           	//算法复位索引，从0开始，当前的算法
	bool Enabled = false;
	std::map<std::string, int> AIPlayDic;
	int minRuleLen = -1;
	int maxRuleLen = -1;
	bool UnmatchClose = true; //是否启用“不中则停止”,
	int UseTimesNumber = 3;////在一个触爆长度内，限制AI使用次数。规则上”UnmatchClose“优先，程序中没有此字段时，默认为1
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
			members = JAIPlayDic.getMemberNames();   // 获取所有key的值
			for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++)   // 遍历每个key
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
	int Respcode = 0;//“0”表示成功；“1”表示失败，服务器端尝试重发3次。
	
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
	std::string AccountName;////网页上提取的账户
	int AccountMoney = -1; // 网页上提取的账户金额
	int StopLoss = 0; //止损（“主”客户端允许输入，“从”客户端仅显示数据）
	int StopProfit = 0;//止盈（“主”客户端允许输入，“从”客户端仅显示数据）
	int TableId = -1;
	std::string Method;
	int SetNumber = -1;//靴号，区域识别后"-"前的数字，最大99 ，
	int SubNumber = -1;//局号，区域识别后"-"后的数字，最大99
	int PlayType = 0;//玩法类型，真实为“0”，模拟为“1”，模拟打法仅在最后下注的时候，选择取消按钮，真实打法选择确认  
	int BetType = -1;//压注类型说明及对应数字：闲（0）、庄（1）、和（2）、庄对（3）、闲对（4），实际软件中，只下“0”或“1”
	std::vector<int> BetResult;//最后开牌的结果，如“[0]或[034]”最多3个数字，
	int CoinsBet = 0;////押注金额。默认依次为：100，300，800，1700，3600，7500，1550，32000，66000.136000。这种固定金额式打法，还有一种是统筹计算法打法。
	int CoinsBetSub = 0;
	int Profit = 0;//本局收益，盈为正，亏为负;如果下“庄(1)”，盈是下注金额*0.95，输的时候下注金额*1*1；如果下“闲(0)”，
			   //盈是下注金额*1，输的时候下注金额*1*1；"和"的时候不管压庄还是闲，均全额退回。
	int EndTime = 0;//本局结束时间
	std::string time;
	int BetResultType; //下注结果
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
		std::string ;////网页上提取的账户
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
		root["Type"] = "BoardInfo";//算法更新
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
		body["DataSources"] = DataSources;  //仅指桌号及靴号，"0"表示识别；"1"表示网页复制；"2"表示其它，另定义
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
		root["Type"] = "DataSerialCheck";//算法更新
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
	int Respcode = 0;//“0”表示成功；“1”表示失败，服务器端尝试重发3次。
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
		root["Type"] = "MethodTest";//算法更新
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
		body["DataSources"] = 1;  //仅指桌号及靴号，"0"表示识别；"1"表示网页复制；"2"表示其它，另定义
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
	int SetNumber = -1;//靴号，区域识别后"-"前的数字，最大99 ，
	int SubNumber = -1;//局号，区域识别后"-"后的数字，最大99
	int BetType;//压注类型说明及对应数字：闲（0）、庄（1）、和（2）、庄对（3）、闲对（4），实际软件中，只下“0”或“1”
	int SeqNum = -1;
	int TableId = -1;
	int CoinsBet = 0;////押注金额。默认依次为：100，300，800，1700，3600，7500，1550，32000，66000.136000。这种固定金额式打法，还有一种是统筹计算法打法。
	std::string Method;
	int methodindex  = 0;
	
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "SubBetsRequest";//算法更新
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
	int SetNumber = -1;//靴号，区域识别后"-"前的数字，最大99 ，
	int SubNumber = -1;//局号，区域识别后"-"后的数字，最大99
	int BetType;//压注类型说明及对应数字：闲（0）、庄（1）、和（2）、庄对（3）、闲对（4），实际软件中，只下“0”或“1”
	int SeqNum = -1;
	int TableId = -1;
	int CoinsBet = 0;////押注金额。默认依次为：100，300，800，1700，3600，7500，1550，32000，66000.136000。这种固定金额式打法，还有一种是统筹计算法打法。
	std::string Method;
	int methodindex = 0;
	int Profit = 0;//本局收益，盈为正，亏为负;如果下“庄(1)”，盈是下注金额*0.95，输的时候下注金额*1*1；如果下“闲(0)”，
				   //盈是下注金额*1，输的时候下注金额*1*1；"和"的时候不管压庄还是闲，均全额退回。
	std::vector<int> BetResult;//最后开牌的结果，如“[0]或[034]”最多3个数字，
	
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
		root["Type"] = "SubBetsResponse";//算法更新
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
	std::string DataSerial;//要获取的数据批次号
	int CurSubName = -1;//客户端当前要打的局号
	std::string SubNumbers;//要获取数据的局号，可以多个，以英文逗号分隔，若为空表示取小于当前局号的所有局数据
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "DataSource";//算法更新
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