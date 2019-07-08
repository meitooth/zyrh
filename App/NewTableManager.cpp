#include "stdafx.h"
#include "NewTableManager.h"
#include "TableManager.h"
#include "curl/CurlhttpClient.h"
#include "libyuv/libyuv.h"
#include <stdio.h>
#include<string>
#include<vector>
#include "Logger.h"
#include "utils.h"
#include "ocr.h"
#include "Message.h"
#include "desktopCapture.h"
#include "appclient.h"
#include "cmdMainSub.h"
#include "desktopCaptureSecond.h"
#include "CDialogConfig.h"
#include <list>
#include "cmd.h"
#include <memory>
extern std::mutex g_mutextable;
int GetMoney(int index)
{
	return GetDesktopCapture()->m_Moneys[index].nMoney;
}
std::map<int, int> CNewTableManager::GetChipList(int colin)
{
	std::map<int, int> ret;
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	for (int i = 6; i > 0; i--)
	{
		int nDivisor = colin / pScreenInfo->ChipList[i].ChipMoney;
		int nRemainder = colin % pScreenInfo->ChipList[i].ChipMoney;
		if (nDivisor > 0)
		{
			ret[i] = nDivisor;
			colin = nRemainder;
		}
	}
	return ret;
}
std::map<int, int> CNewTableManager::GetChipListSecond(int colin)
{
	std::map<int, int> ret;
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	for (int i = 6; i > 0; i--)
	{
		int nDivisor = colin / pScreenInfo->ChipList2[i].ChipMoney;
		int nRemainder = colin % pScreenInfo->ChipList2[i].ChipMoney;
		if (nDivisor > 0)
		{
			ret[i] = nDivisor;
			colin = nRemainder;
		}
	}
	return ret;
}

bool CNewTableManager::HandleXiaZhuSecond(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin)
{
	DesktopCapturer* p = GetDesktopCapture();
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	std::map<int, int>::iterator it = chiplist.begin();
	bool IsXiazhu = false;
	int x1, y1;
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	g_mutextable.lock();
	if (IsZhuang)
	{
		mouse_click_second(m_bet[3].betInfo.ClickTypeNo.x, m_bet[3].betInfo.ClickTypeNo.y);
	}
	else
	{
		mouse_click_second(m_bet[4].betInfo.ClickTypeNo.x, m_bet[4].betInfo.ClickTypeNo.y);
	}
	int nmoney = p->GetAppNameInfo(0);
	if (nmoney == 0)
	{
		Sleep(1000);
		nmoney = p->GetAppNameInfo(0);
	}

	//	Sleep(10);
	mouse_click_second((p->m_ScreenInfo.GameModeRegion.left + p->m_ScreenInfo.GameModeRegion.right) / 2
		, (p->m_ScreenInfo.GameModeRegion.bottom + p->m_ScreenInfo.GameModeRegion.top) / 2);
	CString str;
	while (it != chiplist.end())
	{
		int x = (pScreenInfo->ChipList[it->first].ChipNumRegion.left + pScreenInfo->ChipList[it->first].ChipNumRegion.right) / 2;
		int y = (pScreenInfo->ChipList[it->first].ChipNumRegion.top + pScreenInfo->ChipList[it->first].ChipNumRegion.bottom) / 2;
		CString tmp;
		tmp.Format("%d:%d  ", it->first, it->second);
		mouse_click_second(x, y);;
		int clickcount = it->second;
		if (!IsXiazhu)
		{
			IsXiazhu = true;
			if (IsZhuang)
			{
				x1 = m_bet[1].betInfo.Click.x;
				y1 = m_bet[1].betInfo.Click.y;
				mouse_click_second(x1, y1);
			}
			else
			{
				x1 = m_bet[0].betInfo.Click.x;
				y1 = m_bet[0].betInfo.Click.y;
				mouse_click_second(x1, y1);
			}
		}
		if (IsXiazhu)
		{
			while (clickcount - 1 > 0)
			{
				clickcount--;
				mouse_click_second(x, y);
			}

		}
		str += tmp;
		it++;
	}
	mouse_move(10, 200);

	//HandleTableRegion(buf, nWidth, nHeight);
	Sleep(1000);
	LOG_FILE("TableId:%d %02d:%02d  click in x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);
	if (p->m_bDemo)
	{
		Sleep(2000);
		m_lastBoardInfoRequest.PlayType = 0;
		if (IsZhuang)
		{
			x1 = m_bet[1].betInfo.ClickTypeNo.x;
			y1 = m_bet[1].betInfo.ClickTypeNo.y;
			mouse_click_second(x1, y1);
		}
		else
		{
			x1 = m_bet[0].betInfo.ClickTypeNo.x;
			y1 = m_bet[0].betInfo.ClickTypeNo.y;
			mouse_click_second(x1, y1);
		}
	}
	else
	{
		m_lastBoardInfoRequest.PlayType = 1;

		if (num.nStatus == StatusDJS)
		{
			if (IsZhuang)
			{
				x1 = m_bet[1].betInfo.ClickTypeYes.x;
				y1 = m_bet[1].betInfo.ClickTypeYes.y;
				mouse_click_second(x1, y1);
			}
			else
			{
				x1 = m_bet[0].betInfo.ClickTypeYes.x;
				y1 = m_bet[0].betInfo.ClickTypeYes.y;
				mouse_click_second(x1, y1);
			}
		}
		else
		{
			mouse_move(10, 200);
			g_mutextable.unlock();
			LOG_FILE("TableId:%d %02d:%02d     检测到状态非倒计时 金额:%d 当前状态:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, num.nStatus, num.strStatus.c_str());
			return false;
		}


	}

	LOG_FILE("TableId:%d %02d:%02d  click out x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);

	mouse_click_second(10, 200);
	if (p->m_bDemo)
	{
		g_mutextable.unlock();
		return false;
	}
	Sleep(200);
	int nmoney1 = p->GetAppNameInfo(0);
	for (int i = 0; i < 3; i++)
	{
		if (nmoney1 == 0 || nmoney1 == nmoney)
		{
			Sleep(500);
			nmoney1 = p->GetAppNameInfo(0);
		}
		else
		{
			break;
		}
	}
	g_mutextable.unlock();
	if (nmoney - nmoney1 >= nConlin)
	{
		LOG_FILE("TableId:%d %02d:%02d     下注检测金额成功，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, nmoney, nmoney1);
		return true;
	}
	LOG_FILE("TableId:%d %02d:%02d     下注检测金额失败，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, nmoney, nmoney1);
	return false;

}
bool CNewTableManager::HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin)
{
	DesktopCapturer* p = GetDesktopCapture();
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	std::map<int, int>::iterator it = chiplist.begin();
	bool IsXiazhu = false;
	int x1, y1;
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	g_mutextable.lock();
	if (IsZhuang)
	{
		mouse_click(m_bet[3].betInfo.ClickTypeNo.x, m_bet[3].betInfo.ClickTypeNo.y);
	}
	else
	{
		mouse_click(m_bet[4].betInfo.ClickTypeNo.x, m_bet[4].betInfo.ClickTypeNo.y);
	}
	int nmoney = p->GetAppNameInfo(1);
	if (nmoney == 0)
	{
		Sleep(1000);
		nmoney = p->GetAppNameInfo(1);
	}

	//	Sleep(10);
// 	mouse_click((p->m_ScreenInfo.GameModeRegion.left + p->m_ScreenInfo.GameModeRegion.right) / 2
// 		, (p->m_ScreenInfo.GameModeRegion.bottom + p->m_ScreenInfo.GameModeRegion.top) / 2);
	CString str;
	while (it != chiplist.end())
	{
		int x = (pScreenInfo->ChipList[it->first].ChipNumRegion.left + pScreenInfo->ChipList[it->first].ChipNumRegion.right) / 2;
		int y = (pScreenInfo->ChipList[it->first].ChipNumRegion.top + pScreenInfo->ChipList[it->first].ChipNumRegion.bottom) / 2;
		CString tmp;
		tmp.Format("%d:%d  ", it->first, it->second);
		mouse_click(x, y);;
		int clickcount = it->second;
		if (!IsXiazhu)
		{
			IsXiazhu = true;
			if (IsZhuang)
			{
				x1 = m_bet[1].betInfo.Click.x;
				y1 = m_bet[1].betInfo.Click.y;
				mouse_click(x1, y1);
			}
			else
			{
				x1 = m_bet[0].betInfo.Click.x;
				y1 = m_bet[0].betInfo.Click.y;
				mouse_click(x1, y1);
			}
		}
		if (IsXiazhu)
		{
			while (clickcount - 1 > 0)
			{
				clickcount--;
				mouse_click(x, y);
			}

		}
		str += tmp;
		it++;
	}
	mouse_move(10, 200);

	//HandleTableRegion(buf, nWidth, nHeight);
	Sleep(2000);
	LOG_FILE("TableId:%d %02d:%02d  click in x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);
	if (p->m_bDemo)
	{
		Sleep(2000);
		m_lastBoardInfoRequest.PlayType = 0;
		if (IsZhuang)
		{
			x1 = m_bet[1].betInfo.ClickTypeNo.x;
		 	y1 = m_bet[1].betInfo.ClickTypeNo.y;
		 	mouse_click(x1, y1);
		 }
		else
		{
		 	x1 = m_bet[0].betInfo.ClickTypeNo.x;
		 	y1 = m_bet[0].betInfo.ClickTypeNo.y;
		 	mouse_click(x1, y1);
		 }
	}
	else
	{
		m_lastBoardInfoRequest.PlayType = 1;

		if (num.nStatus == StatusDJS)
		{
			if (IsZhuang)
			{
				x1 = m_bet[1].betInfo.ClickTypeYes.x;
				y1 = m_bet[1].betInfo.ClickTypeYes.y;
				mouse_click(x1, y1);
			}
			else
			{
				x1 = m_bet[0].betInfo.ClickTypeYes.x;
				y1 = m_bet[0].betInfo.ClickTypeYes.y;
				mouse_click(x1, y1);
			}
		}
		else
		{
			mouse_move(10, 200);
			g_mutextable.unlock();
			LOG_FILE("TableId:%d %02d:%02d     检测到状态非倒计时 金额:%d 当前状态:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, num.nStatus, num.strStatus.c_str());
			return false;
		}


	}

	LOG_FILE("TableId:%d %02d:%02d  click out x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);

	mouse_move(10, 200);
	if (p->m_bDemo)
	{
		g_mutextable.unlock();
		return true;
	}
	Sleep(200);
	int nmoney1 = p->GetAppNameInfo(1);
	for (int i = 0; i < 3; i++)
	{
		if (nmoney1 == 0 || nmoney1 == nmoney)
		{
			Sleep(500);
			nmoney1 = p->GetAppNameInfo(1);
		}
		else
		{
			break;
		}
	}
	g_mutextable.unlock();
	if (nmoney - nmoney1 >= nConlin)
	{
		LOG_FILE("TableId:%d %02d:%02d     下注检测金额成功，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, nmoney, nmoney1);
		return true;
	}
	LOG_FILE("TableId:%d %02d:%02d     下注检测金额失败，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, nmoney, nmoney1);
	return false;

}
void CNewTableManager::HandleNextBets()
{
	std::string strVaule;
	bool bNeedNextBet = false;
	int nNextBetType = 0;
	for (int i = m_BoardList.size() - 1; i >= 0; i--)
	{
		if (m_BoardList[i].BetResult[0] == 0)
		{
			strVaule += "0";
		}
		else
		{
			strVaule += "1";
		}
		for (int j = 0; j < GetDesktopCapture()->m_Ais.size(); j++)
		{
			if (strVaule == GetDesktopCapture()->m_Ais[j].rule)
			{
				bNeedNextBet = true;
				nNextBetType  = GetDesktopCapture()->m_Ais[j].value;
				break;
			}
		}

		 
	}
	if (bNeedNextBet)
	{
		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = m_nLastSetNumber;
		pTabInfoMsg->nSubNumber = m_nLastSubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		pTabInfoMsg->nextBetType = nNextBetType;
		CString str;
		str.Format("%d", GetDesktopCapture()->m_Moneys[m_lastBoardInfoRequest.methodIndex].nMoney);
		pTabInfoMsg->nextCoinsBet = str;
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}
}
bool CNewTableManager::SetNeedFan()
{
	bool ret;
	m_mutex->lock();
// 	if (m_nStatus == StatusDJS)
// 	{
// 		m_mutex->unlock();
// 		return false;
// 	}
	m_BNeedFan = !m_BNeedFan;
	ret = m_BNeedFan;
	m_mutex->unlock();
	return true;
}
void CNewTableManager::HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight)
{
	if (m_bCheckBet)
	{
		return;
	}
	
	m_bNeedBet = false;
	for (int j = 0; j < GetDesktopCapture()->m_Ais.size(); j++)
	{
		std::string strVaule;
		for (int i = m_BoardList.size() - 1; i >= 0; i--)
		{
			if (m_BoardList[i].BetResult[0] == 0)
			{
				strVaule += "0";
			}
			else
			{
				strVaule += "1";
			}
			if (strVaule == GetDesktopCapture()->m_Ais[j].rule)
			{
				m_bNeedBet = true;
				m_lastBoardInfoRequest.BetType = GetDesktopCapture()->m_Ais[j].value;
				if (m_BNeedFan)
				{
					m_lastBoardInfoRequest.BetType = !m_lastBoardInfoRequest.BetType;
				}
				break;
			}
		}
		if (m_bNeedBet)
		{
			break;
		}
	}
	if (m_bNeedBet)
	{
		bool bSuss = false;
		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = m_nLastSetNumber;
		pTabInfoMsg->nSubNumber = m_nLastSubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		m_lastBoardInfoRequest.CoinsBet = GetDesktopCapture()->m_Moneys[m_lastBoardInfoRequest.methodIndex].nMoney;
		if (m_lastBoardInfoRequest.CoinsBet >= GetDesktopCapture()->m_nSecond)
		{
			m_mutex->lock();
			std::shared_ptr<rbgbuf> bufptr = m_buf;
			m_mutex->unlock();
			if (bufptr)
			{
				if (HandleXiaZhuSecond(GetChipListSecond(m_lastBoardInfoRequest.CoinsBet), m_lastBoardInfoRequest.BetType, bufptr->buf, nWidth, nHeight, m_lastBoardInfoRequest.CoinsBet))
				{
					bSuss = true;
					m_bCheckBet = true;
				}
			}
			
		}
		else
		{
			if (HandleXiaZhu(GetChipList(m_lastBoardInfoRequest.CoinsBet), m_lastBoardInfoRequest.BetType, buf, nWidth, nHeight, m_lastBoardInfoRequest.CoinsBet))
			{
				bSuss = true;
				m_bCheckBet = true;
			}
		}
		CString str;
		if (bSuss)
		{
			str.Format("%d %d OK ",m_lastBoardInfoRequest.BetType,m_lastBoardInfoRequest.CoinsBet);
		}
		else
		{
			str.Format("%d %d NO ", m_lastBoardInfoRequest.BetType, m_lastBoardInfoRequest.CoinsBet);
		}
		pTabInfoMsg->messageCoinsBet = str;
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}
	else
	{
		m_lastBoardInfoRequest.CoinsBet = 0;
		m_bCheckBet = true;
	}
	
	
}
//新的结果
bool CNewTableManager::CheckNewBet(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	rgbcount[0x307f68] = 0;
	rgbcount[0x318069] = 0;
	rgbcount[0x32816A] = 0;
	rgbcount[0x33826b] = 0;
	rgbcount[0x34836C] = 0;

	UINT value = 0;
	int maxcount = 0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			uint8_t r, g, b;
			UINT tmpvalue = 0;
			r = *(buf + (i*nWidth + j) * 4 + 2);
			g = *(buf + (i*nWidth + j) * 4 + 1);
			b = *(buf + (i*nWidth + j) * 4 + 0);
			tmpvalue += r << 16;
			tmpvalue += g << 8; 
			tmpvalue += b;
			if (rgbcount.find(tmpvalue) == rgbcount.end())
			{
				rgbcount[tmpvalue] = 1;
			}
			else
			{
				
				int nrgbcount = rgbcount[tmpvalue];
				rgbcount[tmpvalue] = nrgbcount + 1;
				if (nrgbcount + 1 > maxcount)
				{
					maxcount = nrgbcount + 1;
					value = tmpvalue;
				}
			}
			
		}
	}
	int per = (rgbcount[0x307f68] + rgbcount[0x318069] + rgbcount[0x32816A] + rgbcount[0x33826b] + rgbcount[0x34836C])  * 100 / (nWidth*nHeight);
	LOG_INFO("max:%d %02x per:%d\n", maxcount, value, per);
// 	if (value != 0x104F05)
// 	{
// 		
// 		per = rgbcount[value] * 100 / (nWidth*nHeight);
// 	}

	if (per < 70)
	{
		m_checkStartkTime == 0;
		return true;
	}
	return false;
}
void CNewTableManager::HandleBet(uint8_t* buf, int nWidth, int nHeight)
{
	
	LOG_FILE("TableId:%d %02d:%02d 开始检测结果 当前状态%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus);
	std::vector<int> BetResultList;
	bool Ishe = false;
	for (int i = 0; i < 3; i++)
	{
		int StartX = m_bet[i].m_BetTypeRegion.left;
		int StartY = m_bet[i].m_BetTypeRegion.top;
		int bet_width = m_bet[i].m_BetTypeRegion.right - m_bet[i].m_BetTypeRegion.left;
		int bet_height = m_bet[i].m_BetTypeRegion.bottom - m_bet[i].m_BetTypeRegion.top;
		if (betBuf == NULL)
		{
			betBuf = new uint8_t[bet_width*bet_height * 100];
		}
		libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, betBuf, bet_width * 4, StartX, StartY, nWidth, nHeight, bet_width, bet_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
		if (CheckNewBet(betBuf, bet_width, bet_height))
		{
			if (m_bet[i].betType == 2)
			{
				Ishe = true;
			}
			BetResultList.push_back(m_bet[i].betType);
		}
	}

	LOG_FILE("TableId:%d %02d:%02d 开始检测结果 当前状态%d 结果数:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus, BetResultList.size());
	if (m_BoardAllList.size() > 0)
	{
		if (m_BoardAllList[m_BoardAllList.size() - 1].SetNumber == m_nLastSetNumber &&
			m_BoardAllList[m_BoardAllList.size() - 1].SubNumber == m_nLastSubNumber)
		{
			return;
		}
	}
	if (BetResultList.size() > 0 && BetResultList.size() < 4)
	{
		
		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = m_nLastSetNumber;
		pTabInfoMsg->nSubNumber = m_nLastSubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		pTabInfoMsg->Betlist = BetResultList;
		pTabInfoMsg->CoinsBet = m_lastBoardInfoRequest.CoinsBet;
		//有下注
		if (m_bNeedBet)
		{
			if (Ishe)//和
			{
				m_lastBoardInfoRequest.Profit = 0;
			}
			else
			{
				
				if (m_lastBoardInfoRequest.BetType == BetResultList[0])
				{
					m_lastBoardInfoRequest.methodIndex = GetDesktopCapture()->m_Moneys[m_lastBoardInfoRequest.methodIndex].nWinindex - 1;
					if (m_lastBoardInfoRequest.BetType == 1)
					{
						m_lastBoardInfoRequest.Profit = m_lastBoardInfoRequest.CoinsBet*0.9;
					}
					else
					{
						m_lastBoardInfoRequest.Profit = m_lastBoardInfoRequest.CoinsBet;
					}
					pTabInfoMsg->IsLost = false;
					m_nLianxuWinCount++;
				}
				else
				{
					m_lastBoardInfoRequest.methodIndex = GetDesktopCapture()->m_Moneys[m_lastBoardInfoRequest.methodIndex].nLossIndex - 1;
					m_lastBoardInfoRequest.Profit = m_lastBoardInfoRequest.CoinsBet*-1;
					pTabInfoMsg->IsLost = true;
					m_nLianxuWinCount = 0;
				}
				
				m_nTotaCoinsBetMoney += m_lastBoardInfoRequest.CoinsBet;
				m_nTotalProfitMoney += m_lastBoardInfoRequest.Profit;
				
			}
			LOG_FILE("TableId:%d %02d:%02d  当前下注金额 主:%d  获利:%d  下注:%d 结果:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_lastBoardInfoRequest.CoinsBet, m_lastBoardInfoRequest.Profit, m_lastBoardInfoRequest.BetType, m_lastBoardInfoRequest.BetResultType);
		}
		else
		{
			
			m_lastBoardInfoRequest.Profit = 0;
			m_lastBoardInfoRequest.CoinsBet = 0;
		}
		pTabInfoMsg->Profit = m_lastBoardInfoRequest.Profit;
		pTabInfoMsg->nTotaCoinsBetMoney = m_nTotaCoinsBetMoney;
		pTabInfoMsg->nTotalProfitMoney = m_nTotalProfitMoney;
		pTabInfoMsg->CoinsBet = m_lastBoardInfoRequest.CoinsBet;
		m_lastBoardInfoRequest.TableId = m_TableId;
		m_lastBoardInfoRequest.EndTime = GetDesktopCapture()->m_nPlayTime;
		m_lastBoardInfoRequest.BetResult = BetResultList;
		m_lastBoardInfoRequest.time = (char*)(LPCTSTR)GetCurrentTime2Str();
		m_lastBoardInfoRequest.SetNumber = m_nLastSetNumber;
		m_lastBoardInfoRequest.SubNumber = m_nLastSubNumber;
		m_BoardAllList.push_back(m_lastBoardInfoRequest);
		if (!Ishe)
		{
			m_BoardList.push_back(m_lastBoardInfoRequest);
		}
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}
	HandleNextBets();
}
void CNewTableManager::HandleARGB(uint8_t* buf, int nWidth, int nHeight)
{
	HanldeNum();
	if (m_nLastSetNumber == -1 || m_nLastSubNumber == -1)
	{
		return;
	}
	if (m_nLastSubNumber >= GetDesktopCapture()->m_nStopSubNumber&& m_nLianxuWinCount >= GetDesktopCapture()->m_nplaytype)
	{

		return;
	}
//	m_mutex->lock();
// 
// 	if (CHeckXiandui(buf,nWidth,nHeight) || CHeckZhuangdui(buf, nWidth, nHeight))
// 	{
// 		m_mutex->unlock();
// 		Sleep(1000);
// 	}
// 	if (HandleTableCancelClick(buf, nWidth, nHeight))
// 	{
// 		LOG_FILE("TableId:%d %02d:%02d 检测到x", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
// 		m_mutex->unlock();
// 		return;
// 	}
// 	if (GetScreenCount() == 2)
// 	{
// 		if (m_buf&&HandleTableCancelClickSecond(m_buf->buf,nWidth,nHeight))
// 		{
// 			LOG_FILE("TableId:%d %02d:%02d 第二屏幕检测到x", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
// 			m_mutex->unlock();
// 			return;
// 		}
// 	}
//	m_mutex->unlock();
	
	if (m_nStatus == StatusDJS) //倒计时
	{		
// 		m_mutex->lock();
// 		if (CHeckXiandui(buf, nWidth, nHeight) )
// 		{
// 			mouse_click()
// 			m_mutex->unlock();
// 			return;
// 			Sleep(1000);
// 		}
// 		m_mutex->unlock();
// 
// 		m_mutex->lock();
// 		if (CHeckZhuangdui(buf, nWidth, nHeight))
// 		{
// 			m_mutex->unlock();
// 			return;
// 			Sleep(1000);
// 		}
// 		m_mutex->unlock();
		if (CHeckXiandui(buf, nWidth, nHeight))
		{
			m_bCheckBet = true;
			return;
		}
		if (CHeckZhuangdui(buf, nWidth, nHeight))
		{
			m_bCheckBet = true;
			return;
		}
		HandleCountdownRegion(buf,nWidth,nHeight);
	}
	else if (m_nStatus == StatusJSZ) //结算中
	{
		HandleBet(buf, nWidth, nHeight);
	}
}
bool CNewTableManager::HanldeNum()
{
	int nSetNumber = -1;
	int nSubNumber = -1;
	{
		TableNum tableNum1 = GetDesktopCapture()->GetNumber(m_TableId);
// 		tableNum1.SetNumber = 1;
// 		tableNum1.SubNumber = 1;
		nSetNumber = tableNum1.SetNumber;
		nSubNumber = tableNum1.SubNumber;
		
		m_nStatus = tableNum1.nStatus;
		if (nSetNumber == -1 || nSubNumber == -1)
		{
			return false;
		}
		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = nSetNumber;
		pTabInfoMsg->nSubNumber = nSubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		if (nSetNumber != m_nLastSetNumber || nSubNumber != m_nLastSubNumber)
		{			 
			m_bCheckBet = false;
			m_bNeedBet = false;
			m_lastBoardInfoRequest.CoinsBet = 0;
			if (m_nLastSubNumber == -1)
			{
				m_nLianxuWinCount = GetDesktopCapture()->m_nplaytype;
			}
			if (nSubNumber != -1 && nSetNumber != -1 && nSubNumber != 1 && m_BoardAllList.size() > 0)
			{
				int nTmpLastSubNumber = -1;
				int nTmpLastSetNumber = -1;
				if (m_BoardAllList.size() > 0)
				{
					nTmpLastSubNumber = m_BoardAllList[m_BoardAllList.size() - 1].SubNumber;
					nTmpLastSetNumber = m_BoardAllList[m_BoardAllList.size() - 1].SetNumber;
				}
				else
				{
					nTmpLastSetNumber = m_nLastSetNumber;
				}				 
			}
			LOG_FILE("TableId:%d %02d:%02d  old %02d:%02d m_isCheck=false", m_TableId, nSetNumber, nSubNumber, m_nLastSetNumber, m_nLastSubNumber);
			m_nLastSubNumber = nSubNumber;
			m_nLastSetNumber = nSetNumber;
			//m_BNeedFan = false;
			if (m_nLastSubNumber == 1)
			{				
				m_BoardList.clear();
				m_nLianxuWinCount = 1;
				m_bNeedBet = false;
				m_bCheckBet = false;
				LOG_FILE("TableId:%d %02d:%02d  当前局数1 清空算法", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
			
			}
		}
		pTabInfoMsg->message = tableNum1.strStatus;

		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}
	return false;
}
void CNewTableManager::SetBuf(std::shared_ptr<rbgbuf> buf)
{
	m_mutex->lock();
	m_buf = buf;
	m_mutex->unlock();
}
bool CNewTableManager::HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight)
{

	int StartX = m_TableCancelClick.left;
	int StartY = m_TableCancelClick.top;
	int num_width = m_TableCancelClick.right - m_TableCancelClick.left;
	int num_height = m_TableCancelClick.bottom - m_TableCancelClick.top;
	if (TableCancelBuf == NULL)
	{
		TableCancelBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, TableCancelBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	{
		if (CheckX(TableCancelBuf, num_width, num_height))
		{
			if (!m_bclickCancel)
			{
				m_bclickCancel = true;
				ClickCancleInfo* ClickCancleInfoPtr = &GetDesktopCapture()->m_ClickCancleInfoList[m_TableId - 1];
				ClickCancleInfoPtr->x = m_TableCancelClick.left + num_width / 2;
				ClickCancleInfoPtr->y = m_TableCancelClick.top + num_height / 2;
				ClickCancleInfoPtr->TabldId = m_TableId;
				ClickCancleInfoPtr->nSetNumber = m_nLastSetNumber;
				ClickCancleInfoPtr->nSubNumber = m_nLastSubNumber;
				if (ClickCancleInfoPtr->bclick)
				{
					ClickCancleInfoPtr->bclick = false;
				}
				//g_mutextable.lock();
				//mouse_click(m_TableCancelClick.left + num_width / 2, m_TableCancelClick.top + num_height / 2);
				//mouse_move(0, 10);
				//g_mutextable.unlock();
				m_CheckCancelDuration = time(NULL);

			}
			else
			{
				if (time(NULL) - m_CheckCancelDuration > 6)
				{
					m_bclickCancel = false;
				}
			}
			return true;
		}
		else
		{
			m_bclickCancel = false;
		}
		return false;
	}
}
bool CNewTableManager::HandleTableCancelClickSecond(uint8_t* buf, int nWidth, int nHeight)
{

	int StartX = m_TableCancelClick.left;
	int StartY = m_TableCancelClick.top;
	int num_width = m_TableCancelClick.right - m_TableCancelClick.left;
	int num_height = m_TableCancelClick.bottom - m_TableCancelClick.top;
	if (TableCancelBuf == NULL)
	{
		TableCancelBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, TableCancelBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	{
		if (CheckX(TableCancelBuf, num_width, num_height))
		{
			if (!m_bclickCancel)
			{
				m_bclickCancel = true;
				ClickCancleInfo* ClickCancleInfoPtr = &GetDesktopCapture()->m_ClickCancleInfoList[m_TableId - 1];
				ClickCancleInfoPtr->x = m_TableCancelClick.left + num_width / 2;
				ClickCancleInfoPtr->y = m_TableCancelClick.top + num_height / 2;
				ClickCancleInfoPtr->TabldId = m_TableId;
				ClickCancleInfoPtr->nSetNumber = m_nLastSetNumber;
				ClickCancleInfoPtr->nSubNumber = m_nLastSubNumber;
				if (ClickCancleInfoPtr->bclick)
				{
					ClickCancleInfoPtr->bclick = false;
				}
				//g_mutextable.lock();
				//mouse_click(m_TableCancelClick.left + num_width / 2, m_TableCancelClick.top + num_height / 2);
				//mouse_move(0, 10);
				//g_mutextable.unlock();
				m_CheckCancelDuration = time(NULL);

			}
			else
			{
				if (time(NULL) - m_CheckCancelDuration > 6)
				{
					m_bclickCancel = false;
				}
			}
			return true;
		}
		else
		{
			m_bclickCancel = false;
		}
		return false;
	}
}
bool CNewTableManager::CheckX(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	UINT value;
	int maxcount = 0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			UINT tmpvalue = 0;
			uint8_t r, g, b;
			r = *(buf + (i*nWidth + j) * 4 + 2);
			g = *(buf + (i*nWidth + j) * 4 + 1);
			b = *(buf + (i*nWidth + j) * 4 + 0);
			tmpvalue += r << 16;
			tmpvalue += g << 8;
			tmpvalue += b;
			int nrgbcount = rgbcount[tmpvalue];
			rgbcount[tmpvalue] = nrgbcount + 1;
			if (nrgbcount + 1 > maxcount)
			{
				maxcount = nrgbcount + 1;
				value = tmpvalue;
			}
		}
	}

	int per = rgbcount[0x00989898] * 100 / (nWidth*nHeight);
	//LOG_TABLE("value%X per%d", value, per);//
	if (per < 50)
	{
		return false;
	}

	return true;
}
bool CNewTableManager::CHeckXiandui(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_bet[0].betInfo.ChipRegion.left;
	int StartY = m_bet[0].betInfo.ChipRegion.top;
	int num_width = m_bet[0].betInfo.ChipRegion.right - m_bet[0].betInfo.ChipRegion.left;
	int num_height = m_bet[0].betInfo.ChipRegion.bottom - m_bet[0].betInfo.ChipRegion.top;
	if (TableCancelBuf == NULL)
	{
		TableCancelBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, TableCancelBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	{
		if (CheckChip(TableCancelBuf, num_width, num_height))
		{
			 
			return true;
		}
		else
		{
			m_bclickCancel = false;
		}
		return false;
	}
	return true;
}
bool  CNewTableManager::CHeckZhuangdui(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_bet[1].betInfo.ChipRegion.left;
	int StartY = m_bet[1].betInfo.ChipRegion.top;
	int num_width = m_bet[1].betInfo.ChipRegion.right - m_bet[1].betInfo.ChipRegion.left;
	int num_height = m_bet[1].betInfo.ChipRegion.bottom - m_bet[1].betInfo.ChipRegion.top;
	if (TableCancelBuf == NULL)
	{
		TableCancelBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, TableCancelBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	{
		if (CheckChip(TableCancelBuf, num_width, num_height))
		{

			return true;
		}
		else
		{
			m_bclickCancel = false;
		}
		return false;
	}
	return true;
}
bool CNewTableManager::CheckChip(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	UINT value;
	int maxcount = 0;
	rgbcount[0x0008150f] = 0;
	rgbcount[0x00302f2f] = 0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			UINT tmpvalue = 0;
			uint8_t r, g, b;
			r = *(buf + (i*nWidth + j) * 4 + 2);
			g = *(buf + (i*nWidth + j) * 4 + 1);
			b = *(buf + (i*nWidth + j) * 4 + 0);
			tmpvalue += r << 16;
			tmpvalue += g << 8;
			tmpvalue += b;
			int nrgbcount = rgbcount[tmpvalue];
			rgbcount[tmpvalue] = nrgbcount + 1;
			if (nrgbcount + 1 > maxcount)
			{
				maxcount = nrgbcount + 1;
				value = tmpvalue;
			}
		}
	}
	if (value == 0x002bf7 || value == 0xff101a || value == 0x34836c || value == 0x33826b)
	{
		return false;
	}
	else
	{
		return true;
	}
	//529679
   int per = (rgbcount[0x0008150f] + rgbcount[0x00302f2f]) * 100 / (nWidth*nHeight);
	//LOG_TABLE("value%X per%d", value, per);//
	if (per < 50)
	{
		return false;
	}

	return true;
}