#pragma once
#include <stdint.h>
#include <vector>
#include "json/json.h"
#include <time.h>
#include <windows.h>
#include <mutex>
#include "cmd.h"
#include "TableManager.h"
#include "cmd.h"
#include <memory>
class CTableManagerSub
{
public:
	CTableManagerSub();
	~CTableManagerSub();
	bool CheckNew(uint8_t* buf, int nWidth, int nHeight);
	bool CheckGreen(uint8_t* buf, int nWidth, int nHeight);
	void HandleARGB(uint8_t* buf, int nWidth, int nHeight);
	void HandleBet(uint8_t* buf,int nWidth, int nHeight);
	void HandleNumberRegion(uint8_t* buf, int nWidth, int nHeigh,int& nSetNumber,int& SubNumber);
	void HandleNumberRegion1(uint8_t* buf, int nWidth, int nHeight);
	void HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight, int& nCountdown);
	bool HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang,uint8_t* buf, int nWidth, int nHeight,int nConlin);
	std::map<int, int> GetChipList(int colin);
	void HandleTableRegion(uint8_t* buf, int nWidth, int nHeight);
	bool HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight);
	bool HandleTableCancelClick();
	bool CheckX(uint8_t* buf, int nWidth, int nHeight);
	bool HandleCoinsBet(int nCoinsBet, int nSetNumber, int nSubNumber, int nBetType);
	void SetBuf(std::shared_ptr<rbgbuf>);
	bool CheckClickNo(uint8_t* buf, int nWidth, int nHeight);
	bool  CheckNo(uint8_t* buf, int nWidth, int nHeight);
	bool CheckChipRGB(uint8_t* buf, int nWidth, int nHeight);
	bool CheckChipRegion(bool iSzhuang, uint8_t* buf, int nWidth, int nHeight);
private:
	int m_nReportTime = 0;
	std::mutex* m_mutex = NULL;
public:
	std::string name;
	uint8_t* buf = NULL;
	uint8_t* buf1 = NULL;
	uint8_t* buf2 = NULL;
	uint8_t* betBuf = NULL;
	uint8_t* NumBuf = NULL;
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
	byte m_nStatus = StatusInit;
	int m_CheckCancelDuration = 0;
	bool m_bclickCancel = false;
	
	std::shared_ptr<rbgbuf> m_buf;
	bool m_bclickNo = false;
	int m_CheckNoDuration = 0;
};