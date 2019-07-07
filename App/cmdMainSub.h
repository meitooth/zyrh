#include "cmd.h"
/*
struct SubRegisterRequest
{
	int SeqNum = 0;
	int bMainType = SUB_TYPE;
	std::string DeviceId;
	std::string MachineCode;
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "Register";
		root["SeqNum"] = SeqNum;
		root["DeviceId"] = DeviceId.c_str();
		root["MachineCode"] = MachineCode.c_str();
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
	void Parse(std::string str)
	{
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(str, root, false))
		{
			SeqNum = root["SeqNum"].asInt();
			DeviceId = root["DeviceId"].asCString();
			MachineCode = root["MachineCode"].asCString();
		}
	}

};

struct SubRegisterResponse
{
	int SeqNum = 0;
	int Respcode = 0;
	std::string RespMsg;//内容会依服务器上面的条件配置而不同。
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
		RespMsg = root["RespMsg"].asCString();
		return true;

	}
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "Register";
		root["SeqNum"] = SeqNum;
		root["RespMsg"] = RespMsg.c_str();
		root["Respcode"] = Respcode;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};
struct CoinsBetRequest
{
	int SeqNum = 0;
	int TableId = -1;
	int SetNumber = -1;//靴号，区域识别后"-"前的数字，最大99 ，
	int SubNumber = -1;//局号，区域识别后"-"后的数字，最大99
	int BetType;//压注类型说明及对应数字：闲（0）、庄（1）、和（2）、庄对（3）、闲对（4），实际软件中，只下“0”或“1”
	int CoinsBet = 0;////押注金额。默认依次为：100，300，800，1700，3600，7500，1550，32000，66000.136000。这种固定金额式打法，还有一种是统筹计算法打法。
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
		CoinsBet = root["CoinsBet"].asInt();
		return true;
	}
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "CoinsBetRequest";
		root["SeqNum"] = SeqNum;
		root["TableId"] = TableId;
		root["SetNumber"] = SetNumber;
		root["SubNumber"] = SubNumber;
		root["BetType"] = BetType;
		root["CoinsBet"] = CoinsBet;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};


struct CoinsBetResponse
{
	int SeqNum = 0;
	int Respcode = 0;
	std::string RespMsg;//内容会依服务器上面的条件配置而不同。
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
		RespMsg = root["RespMsg"].asCString();
		return true;

	}
	std::string ToStr()
	{
		Json::Value root;
		root["Type"] = "CoinsBetResponse";
		root["SeqNum"] = SeqNum;
		root["RespMsg"] = RespMsg.c_str();
		root["Respcode"] = Respcode;
		std::string ret = root.toStyledString();
		ret = replace_all(ret, "\r", "");
		ret = replace_all(ret, "\n", "");
		ret = replace_all(ret, " ", "");
		return ret;
	}
};
*/