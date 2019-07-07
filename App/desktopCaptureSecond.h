#pragma once
#include <stdint.h>
#include <vector>
#include "desktopCapture.h"


class DesktopCapturerSecond
{
public:
	DesktopCapturerSecond();
	~DesktopCapturerSecond();
	void Init(std::vector<CTableManagerSub>& TableSubList)
	{
		m_TableSubList = TableSubList;	
	}
	void Start();
	void Pause();
	void Stop();
	void Capturer();
	void GetAccountName(uint8_t* buf, int nWidth, int nHeight);
	void GetAccountMoney(uint8_t* buf, int nWidth, int nHeight);
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
	std::string GetPicStr()
	{
		return m_picstr;
	}
	bool HandleCoinsBet(int index,int nCoinsBet, int nSetNumber, int nSubNumber, int nBetType);
	
	void HandleAccountMoneyNamePic(uint8_t* buf, int nWidth, int nHeight);
	int CheckFlash(uint8_t* buf, int nWidth, int nHeight);
	std::vector<CTableManagerSub> m_TableSubList;
private:
	int64_t m_desktopFrameRate = 15;
	uint8_t* m_buf;
	uint8_t* m_buf1;
	
	std::vector<std::shared_ptr<rbgbuf>> m_Buflist;
	CaptureScreen  m_CaptureScreen;
	int m_Status = STATUS_STOP;// 
	bool m_bStartThread = false;
	std::string m_AccountName;
	int m_AccountMoney = -1;
	std::mutex m_mutex;
	std::mutex m_mutexnum;
	std::string m_picstr;
	int m_HandlePicTime = 0;
public:
	ScreenInfo m_ScreenInfo;
	bool m_bDemo = true;
	int m_nFrist = 1000;
	int m_nSecond = 10000;
	int nFlashX = 0;
	int nFlashY = 33;
	

};
extern DesktopCapturerSecond* GetDesktopCaptureSecond();

