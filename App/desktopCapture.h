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
	RECT AccountName;//��ע�ʻ���
	RECT AccountMoney; //�ʻ����
	RECT BetsRange;//��ע��Χ
	//��ע��Χѡ�������ĵ㣬�����ĵ�λ����Ҫ�����㷨����Ϊ�����˻������̼��ʻ����仯��������ĵ�����������ƶ�����Ҫ�㷨���,
	POINT BetsRangeUp;
	POINT BetsRangeDown;
	//��Ϸģʽʶ������ǳ���Ҫ��ֻ�����ģʽ��ȷ������£��������漰������ɼ������Ƶ��߼�����ִ�С�ʶ����ɫ�ǲ��ǻ�ɫ�ı�������ʶ��10*10����
	RECT GameModeRegion; 
	//�˻������ͼƬ��ȡͼƬ
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
	int m_nAllTotalWaterMoney = 0;//����ˮ
	int m_nAllCurTotalMoney = 0;//��ǰ��Ӯ	
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
	int StopLoss = 0; //ֹ�𣨡������ͻ����������룬���ӡ��ͻ��˽���ʾ���ݣ�
	int StopProfit = 0;//ֹӯ���������ͻ����������룬���ӡ��ͻ��˽���ʾ���ݣ�
	//std::vector<CTableManager> m_TableList;
	std::vector<CNewTableManager> m_NewTableList;
	std::vector<CNew1TableManager> m_New1TableList;
	bool m_bNeedCopy = false;
	bool m_bNeedCopySecond = false;
	int nFlashX = 0;
	int nFlashY = 33;
	int m_nStartMainMoney = 0;
	int m_nStartSubMoney = 0;
	int m_nPlayTime = -1;//��ҳʱ��
	std::string m_strPlayTime;//��ҳʱ��
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

