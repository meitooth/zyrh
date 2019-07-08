#pragma once
#include <stdint.h>
#include <vector>
#include "desktopCapture.h"
#include "json/json.h"
#include <time.h>
#include "CaptureScreen.h"
#include "TableManager.h"
#include "TableManagerSub.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "httpserver.h"
#include "cmd.h"
#include "NewTableManager.h"
#include "New1TableManager.h"
enum
{
	STATUS_START = 0,
	STATUS_PAUSE = 1,
	STATUS_STOP = 2,

};
struct ChipInfo
{
	int ChipNum;
	int ChipMoney = -1;
	RECT ChipNumRegion;
};
struct ScreenInfo{
	RECT AccountName;//下注帐户名
	RECT AccountMoney; //帐户余额
	RECT BetsRange;//下注范围
	//下注范围选择点击中心点，此中心点位置需要增加算法，因为随着账户名长短及帐户余额变化，这个中心点区域会左右移动，需要算法配合,
	POINT BetsRangeUp;
	POINT BetsRangeDown;
	//游戏模式识别，这个非常重要，只有这个模式正确的情况下，其它的涉及到桌面采集及控制的逻辑才能执行。识别颜色是不是黄色的背景。共识别10*10像素
	RECT GameModeRegion; 
	//账户名余额图片截取图片
	RECT AccountMoneyNamePic;
	ChipInfo ChipList[10];
	ChipInfo ChipList2[10];
};

// struct  TableNum
// {
// 	int nSetNumber = -1;
// 	int nSubNumber = -1;
// 	int nTableId = -1;
// };

class DesktopCapturer
{
public:
	DesktopCapturer();
	~DesktopCapturer();
	void Init(std::vector<CTableManager>& tableList)
	{
		
	}
	void Init(std::vector<CNewTableManager>& tableList)
	{
		m_NewTableList = tableList;
	}
	void Init(std::vector<CNew1TableManager>& tableList)
	{
		m_New1TableList = tableList;
	}
	void Start();
	void Pause();
	void Stop();
	void Capturer();
	void GetAccountName(uint8_t* buf, int nWidth, int nHeight);
	void GetAccountMoney(uint8_t* buf, int nWidth, int nHeight);
	void SetTableNum(int index, int nSetNumber, int nSubNumber);
	void HandleAccountMoneyNamePic(uint8_t* buf, int nWidth, int nHeight);
	void SetTableInfo(int nTableId, std::vector<std::string>& strlist, int i);
	std::string GetTableInfo(int index);
	ScreenInfo* GetScreenInfo()
	{
		return &m_ScreenInfo;
	}
	std::string GetAccountName()
	{
		return m_AccountName;
	}
	int GetAccountMoney()
	{
		return m_AccountMoney;
	}
	std::string GetAccountNameSecond()
	{
		return m_AccountNameSecond;
	}
	int GetAccountMoneySecond()
	{
		return m_AccountMoneySecond;
	}
	void AddTotalWaterMoney(int num)
	{
		m_mutexnum.lock();
		m_nAllTotalWaterMoney += num;
		m_mutexnum.unlock();
	}
	void AddCurTotalMoney(int num)
	{
		m_mutexnum.lock();
		m_nAllCurTotalMoney += num;
		m_mutexnum.unlock();
	}
	int GetTotalWaterMoney()
	{
		int ret = 0;
		m_mutexnum.lock();
		ret = m_nAllTotalWaterMoney;
		m_mutexnum.unlock();
		return ret;
	}
	int GetCurTotalMoney()
	{
		int ret = 0;
		m_mutexnum.lock();
		ret = m_nAllCurTotalMoney;
		m_mutexnum.unlock();
		return ret;
	}
	void AddMethod(int TableId, std::string method)
	{
		//m_mutexnum.lock();
		m_methodlist[TableId] = method;
		//m_mutexnum.unlock();

	}
	bool IsHasMethod(std::string method)
	{
		bool bret = false;
		//m_mutexnum.lock();		
		for (auto v:m_methodlist)
		{
			if (method == v.second)
			{
				bret = true;
			}
		}
		//m_mutexnum.unlock();
		return bret;

	}
	std::string GetPicStr()
	{
		return m_picstr;
	}
	void SetNeedCopy(bool IsMain);
	void Pause(int index,bool bPause);
	int GetAppNameInfo(bool IsMain);
	bool CheckColin(bool IsMain, int TableId,int Colin);
	bool SetFan(int TableId)
	{
		if(m_AiTYpe == 1)
		{
			if (m_NewTableList.size() > 0&&TableId < m_NewTableList.size())
			{
				return m_NewTableList[TableId - 1].SetNeedFan();
			}
			return false;
		}
		else{
			if (m_New1TableList.size() > 0&&TableId < m_New1TableList.size())
			{
				return m_New1TableList[TableId - 1].SetNeedFan();
			}
			return false;
		}
 		
		
	}
	void GetNumber();
	TableNum GetNumber(int nTableId);
	int GetStatus(int nTableId);
	std::string GetNextBetType(int nTableId);
	void SetBuf(std::shared_ptr<rbgbuf> buf);
private:
	int CheckFlash(uint8_t* buf, int nWidth, int nHeight);
	int64_t m_desktopFrameRate = 15;
	uint8_t* m_buf;
	uint8_t* m_buf1;
	
	CaptureScreen  m_CaptureScreen;
	int m_Status = STATUS_STOP;// 
	bool m_bStartThread = false;
	std::string m_AccountName;
	std::string m_MoneyLine;
	std::string m_strMoney;
	int m_AccountMoney = 0;

	std::string m_AccountNameSecond;
	std::string m_MoneyLineSecond;
	std::string m_strMoneySecond;
	int m_AccountMoneySecond = 0;
	std::vector<std::shared_ptr<rbgbuf>> m_Buflist;
	std::vector<std::shared_ptr<rbgbuf>> m_Buflistx;
	std::mutex m_mutex;
	std::mutex m_mutexnum;
	int m_nAllTotalWaterMoney = 0;//总流水
	int m_nAllCurTotalMoney = 0;//当前输赢	
	std::string m_picstr;
	int m_HandlePicTime = 0;
	std::map<int, int> m_CoinsBetlist;
	std::map<int, int> m_CoinsBetSecondlist;
	
	std::map<int, TableNum> m_TableNumberlist;
public:
	bool m_bDemo = true;
	ScreenInfo m_ScreenInfo;
	int m_nFrist = 30;
	int m_nSecond = 1000;
	int m_nStopSubNumber = 100;
	int m_nplaytype = 1;
	int m_AiTYpe = 1;
	int StopLoss = 0; //止损（“主”客户端允许输入，“从”客户端仅显示数据）
	int StopProfit = 0;//止盈（“主”客户端允许输入，“从”客户端仅显示数据）
	//std::vector<CTableManager> m_TableList;
	std::vector<CNewTableManager> m_NewTableList;
	std::vector<CNew1TableManager> m_New1TableList;
	bool m_bNeedCopy = false;
	bool m_bNeedCopySecond = false;
	int nFlashX = 0;
	int nFlashY = 33;
	int m_nStartMainMoney = 0;
	int m_nStartSubMoney = 0;
	int m_nPlayTime = -1;//网页时间
	std::string m_strPlayTime;//网页时间
	ClickCancleInfo m_ClickCancleInfoList[8];
	ClickCancleInfo* m_pClickCancl;
	ClickCancleInfo m_ClickCancleInfoListSecond[8];
	ClickCancleInfo* m_pClickCanclSecond;
	std::map<int, std::string> m_methodlist;

	std::vector<AI_S> m_Ais;
	std::vector<AI_Money> m_Moneys;
	std::map<int,AI_Ttoal>  m_AI_Ttoals;
};
extern DesktopCapturer* GetDesktopCapture();
extern DesktopCapturer* GetDesktopCapture1();

