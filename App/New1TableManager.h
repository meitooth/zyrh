#pragma once
#include "TableManager.h"
#include <mutex>
class CNew1TableManager
{
public:
	CNew1TableManager()
	{
		m_mutex = new std::mutex;
	}
	~CNew1TableManager()
	{

	}
	bool HanldeNum();
	void HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight);
	void HandleARGB(uint8_t* buf, int nWidth, int nHeight);
	void HandleBet(uint8_t* buf, int nWidth, int nHeight);
	bool CheckNewBet(uint8_t* buf, int nWidth, int nHeight);
	std::map<int, int> GetChipList(int colin);
	std::map<int, int> GetChipListSecond(int colin);
	bool HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin);
	bool HandleXiaZhuSecond(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin);
	bool IsPause()
	{
		return m_isPause;
	}
	void SetPause(bool value)
	{
		m_isPause = value;
	}
	void SetBuf(std::shared_ptr<rbgbuf> buf);
	bool SetNeedFan();
	void HandleNextBets();
	bool HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight);
	bool  HandleTableCancelClickSecond(uint8_t* buf, int nWidth, int nHeight);
	bool CheckX(uint8_t* buf, int nWidth, int nHeight);
	bool CHeckXiandui(uint8_t* buf, int nWidth, int nHeight);
	bool CHeckZhuangdui(uint8_t* buf, int nWidth, int nHeight);
	bool CheckChip(uint8_t* buf, int nWidth, int nHeight);
public:
	int m_nLastSetNumber = -1;
	int m_nLastSubNumber = -1;
	int m_nCurSetNumberResult = -1;
	int m_nCurSubNumberResult = -1;
	HWND m_hwnd = NULL;
	int64_t m_checkStartkTime = 0;
	int64_t m_checkLastTime = 0;
	byte m_nStatus = StatusInit;
	std::vector<BoardInfoRequest> m_BoardList;//�������� 
	std::vector<BoardInfoRequest> m_BoardAllList;//������
	bool m_bNeedBet = false;
	bool m_bCheckBet = false;
	uint8_t* betBuf = NULL;
	BoardInfoRequest m_lastBoardInfoRequest;

	//��������  
	RECT m_TableIdRegion = { 0,0,0,0 };
	//ѥ��+�ֺţ�����һ����Ҫ��������
	RECT m_NumberRegion = { 0,0,0,0 };
	//����ʱ����
	RECT m_CountdownRegion = { 0,0,0,0 };
	//����ȡ���������
	RECT m_TableCancelClick = { 0,0,0,0 };
	//���Ŵ���ʶ������
	RECT m_TableCancelRegion = { 0,0,0,0 };
	int m_TableId = 0;
	Bet m_bet[5];
	bool m_isPause = false;
	std::shared_ptr<rbgbuf> m_buf;
	uint8_t* TableCancelBuf = NULL;
	std::mutex* m_mutex;
	bool m_bclickCancel = false;
	int m_CheckCancelDuration = 0;
	int m_nTotaCoinsBetMoney = 0;//����ˮ
	int m_nTotalProfitMoney = 0;//��ǰ��Ӯ
	int m_nLianxuWinCount = 0;
	bool m_BNeedFan = false;
	int m_moneyIndex = 0;
};