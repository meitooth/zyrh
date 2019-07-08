#pragma once
#include <stdint.h>
#include <vector>
#include "json/json.h"
#include <time.h>
#include <windows.h>
#include <mutex>
#include "cmd.h"
#include "MoneyMgr.h"
#include "Logger.h"
enum  TableStatus
{
	StatusInit  = 0,    //结算中
	StatusJSZ   = 1,    //结算中
	StatusDJS   = 2,   //倒计时
	StatusXPZ = 3,	  //洗牌
	StatusDDXJ   = 4,     //等待新局
	StatusTZTZ   = 5,     //停止下注
};
//闲庄
struct BetInfo
{
	// 闲筹码识别区域
	RECT ChipRegion;
	//下注点击中心
	POINT Click;
	// 确认下注点击中心
	POINT ClickTypeYes;
	// 确认下注点击区域
	RECT ClickTypeYesRect;
	//取消下注点击中心
	POINT ClickTypeNo;
	//取消下注点击中心区域
	RECT ClickTypeNoRect;
	//当前下注总量识别
	RECT AmountBetRegion;
};
struct Bet
{
	int betType;
	//区域
	RECT m_BetTypeRegion;
	BetInfo betInfo;
};
typedef struct  TableNum
{
	int SetNumber = -1;
	int SubNumber = -1;
	int nTableId = -1;
	int nStatus = StatusInit;
	std::string strStatus;
}TableNum;
struct ReportEvent_S
{
	int Event = 2;
	std::string Date;//当前日期，此字段可选（若不传，取系统当前日期）
	std::string TableId;//桌号，取图中完整桌号名称	
	int SetNumber = 0;//靴号，图中桌号下面的前一个数字
	int SubNumber = 0;//局号，图中桌号下面的后一个数字
	std::string UserName = "abc";//  客户端登录的用户名
	std::string AccountName = "12345";//下注帐户名
									  //param 
									  //当 Event == 2 时的参数
									  //牌面状态：最后开牌的结果，状态可能有多个
									  //类型说明：闲、庄、和、庄对、闲对
									  //对应数字：0 、1、  2、  3、  4
	std::vector<int> BetResultList;
	int BetResult = 0;
	int  EndTime = 0;
	int Profit = 0;
	ReportEvent_S()
	{
		EndTime = time(NULL);
	}
	~ReportEvent_S()
	{

	}
	std::string &   replace_all(std::string &   str, const   std::string &   old_value, const   std::string &   new_value)
	{
		while (true) {
			std::string::size_type   pos(0);
			if ((pos = str.find(old_value)) != std::string::npos)
				str.replace(pos, old_value.length(), new_value);
			else   break;
		}
		return   str;
	}
	std::string ToJons()
	{
		Json::Value root;
		time_t Date;
		time(&Date);
		char buf[256];
		tm* t_tm = localtime(&Date);
		sprintf(buf, "%d-%d-%d", t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday);
		root["Date"] = buf;
		root["TableId"] = TableId.c_str();
		root["SetNumber"] = SetNumber;
		root["SubNumber"] = SubNumber;
		root["UserName"] = UserName.c_str();
		root["AccountName"] = AccountName.c_str();
		root["Event"] = Event;
		Json::Value Params;
		if (Event == 2)
		{
			Json::Value JBetResultList;
			for (int i = 0;i < BetResultList.size();i++)
			{
				JBetResultList.append(BetResultList[i]);
			}
			Params["BetResult"] = JBetResultList;
			Params["EndTime"] = EndTime;
			Params["Profit"] = Profit;
		}
		root["Params"] = Params;
		std::string str = root.toStyledString();
		str = replace_all(str, "\n", "");
		str = replace_all(str, " ", "");
		return  str;
	}

};
class CTableManager
{
public:
	CTableManager();
	~CTableManager();
	bool CheckNew(uint8_t* buf, int nWidth, int nHeight);
	bool CheckGreen(uint8_t* buf, int nWidth, int nHeight);
	void HandleARGB(uint8_t* buf, int nWidth, int nHeight);
	void HandleBet(uint8_t* buf,int nWidth, int nHeight);
	void HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight, int& nCountdown);
	void SetTableNum(int nSetNumber, int nSubNumber);
	bool HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang,uint8_t* buf, int nWidth, int nHeight,int nConlin);
	bool CheckIsZhuang(int betReslut);
	bool CheckIsXian(int betReslut);
	void  GetConlin(bool IsZhuang, int& nCoinsBet, int& nCoinsBetbak);
	std::map<int, int> GetChipList(int colin);
	void HandleTableRegion(uint8_t* buf, int nWidth, int nHeight);
	std::string GetTableInfo();
	void RevertMeothod();
	void ResetBet();
	bool HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight);
	bool CheckX(uint8_t* buf, int nWidth, int nHeight);
	void ChcekNum(int& nSetNumber, int& nSubNumber);
	bool CheckClickNo(uint8_t* buf, int nWidth, int nHeight);
	bool CheckNo(uint8_t* buf, int nWidth, int nHeight);
	bool CheckChipRGB(uint8_t* buf, int nWidth, int nHeight);
	bool CheckChipRegion(bool iSzhuang, uint8_t* buf, int nWidth, int nHeight);
	bool HandleCheckXiaZhuBak(int  nCoinsBetBak, bool IsZhuang, uint8_t* argbbuf, int nWidth, int nHeight);
	bool HandleXiaZhuAll(uint8_t* buf, int nWidth, int nHeight, bool IsZhuang, int nChipMoneyMain, int nChipMoneySub);
	bool HanldeNum(bool& bneedreturn);
	bool IsPause()
	{
		return m_isPause;
	}
	void SetPause(bool value)
	{
		m_isPause = value;
	}
// 	void Clear()
// 	{
// 		m_pmutex->lock();
// 		m_nLastSetNumber = -1;
// 		m_nLastSubNumber = -1;
// 		m_checkStartkTime = 0;
// 		m_checkLastTime = 0;
// 		m_nStatus = StatusInit;
// 		method = "";
// 		m_nMethodIndex = 0;
// 		m_nMethodTotalMoney = 0;
// 		m_isCheck = false;
// 		m_nCoinsBet = 0;
// 		m_nCoinsBetMain = -1;    //是否主下注
// 		m_nCurrentBetType = -1; //当前下注
// 		m_BoardList.clear();
// 		m_BoardAllList.clear();
// 		BoardInfoRequest request;
// 		m_lastBoardInfoRequest = request;
// 		m_CheckCancelDuration = 0;
// 		m_pmutex->unlock();
// 	}
	void SetMethod(std::string str)
	{
		if (str.length() == 0)
		{
			return;
		}
		m_pmutex->lock();
		m_bNeedMethod = false;
		method = str;
		m_pmutex->unlock();
		LOG_FILE("TableId:%d %02d:%02d  获取算法%s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, str.c_str());
	}
	bool SetNeedFan()
	{
		bool ret;
		m_pmutex->lock();
		m_BNeedFan = !m_BNeedFan;
		ret = m_BNeedFan;
		m_pmutex->unlock();
		return ret;
	}
	void AddHistoryData(std::vector<BoardInfoRequest> requests);
	bool GetCurrentBetType(int& nCurrentBetType, std::string &nCurrentBetMoney);
	std::string GetCurrentBetType();
	void HandleBet(BoardInfoRequest& CurrentBoardInfoRequest,bool breport = true);
private:
	int m_nReportTime = 0;
public:
	std::string name;
	//uint8_t* buf = NULL;
	uint8_t* buf1 = NULL;
	uint8_t* buf2 = NULL;
	uint8_t* betBuf = NULL;
	uint8_t* NumBuf = NULL;
	uint8_t* NumBufScale = NULL;
	uint8_t* CountdownBuf = NULL;
	uint8_t* TableCancelBuf = NULL;
	uint8_t* TableIdRegionBuf = NULL;
	uint8_t* m_NoBuf = NULL;
	uint8_t* RegionBuf = NULL;
	//桌号区域  
	RECT m_TableIdRegion = { 0,0,0,0 };
	//靴号+局号，连在一起，需要规则区分
	RECT m_NumberRegion = { 0,0,0,0 };
	//倒计时区域
	RECT m_CountdownRegion = { 0,0,0,0 };
	//桌号取消点击中心
	RECT m_TableCancelClick = { 0,0,0,0 };
	//桌号错误识别区域
	RECT m_TableCancelRegion = { 0,0,0,0 };
	int m_TableId = 0;
	Bet m_bet[5];
	int m_nLastSetNumber = -1;
	int m_nLastSubNumber = -1;
	int m_nCurSetNumberResult = -1;
	int m_nCurSubNumberResult = -1;
	HWND m_hwnd = NULL;
	int64_t m_checkStartkTime = 0;
	int64_t m_checkLastTime = 0;
	byte m_nStatus = StatusInit;
	std::string method = "";
	int m_nMethodIndex = 0;
	int m_nMoneyIndex = 0;
	int m_nMethodTotalMoney = 0;
	int m_nTotaCoinsBetMoney = 0;//总流水
	int m_nTotalProfitMoney = 0;//当前输赢
	bool m_isCheck = false;
	bool m_isCheckBak = false;
	int m_nAIPlay = 0;
	std::string m_sAIStr;
	int m_nCoinsBet = 0;
	int m_nCoinsBetSub = 0;
	int m_nCoinsBetMain = -1;    //是否主下注
	int m_nCurrentBetType = -1; //当前下注
	int m_nCountDown = -1;
	BoardInfoRequest m_lastBoardInfoRequest;
	std::vector<BoardInfoRequest> m_SubList;//没医学
	std::vector<BoardInfoRequest> m_BoardList;//不包括和 
	std::vector<BoardInfoRequest> m_BoardAllList;//包括和
	std::vector<BoardInfoRequest> m_Rulelist;
	std::vector<BoardInfoRequest> m_BoardlistHistory;
	int m_CheckCancelDuration = 0;
	bool m_isPause = false;
	std::mutex* m_pmutex = NULL;
	bool m_bclickCancel = false;
	int m_nGetLastNumTime = 0;
	//int m_nMethodSerial = -1;
	std::map<int, int64_t> m_betcountlist;
	std::vector<TableNum> m_TableNumlist;
	bool m_bNeedMethod = true;
	bool m_bclickNo = false;
	int m_CheckNoDuration = 0;
	bool m_BNeedFan = false;
	bool m_bXiaZhuSuccess = false;
	bool m_bEnableAI = true;
	int m_nUseTimesNumber = 0;
	int m_GetDataSerialTime = 0;
	int m_GetMethodTime = 0;
	bool m_bNeedResult = true;
};