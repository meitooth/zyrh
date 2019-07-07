#include "stdafx.h"
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
#define USEMY 0
std::mutex g_mutextable;
CTableManager::CTableManager()
{
	//m_nReportTime = time(NULL);
	m_pmutex = new std::mutex;
	for (int i = 0;i < 2;i++)
	{
		m_betcountlist[i] = 0;
	}
}
CTableManager::~CTableManager()
{

}




bool CTableManager::CheckNew(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	UINT value = 0;
	int maxcount = 0;
	for (int i = 0;i < nHeight;i++)
	{
		for (int j = 0;j < nWidth;j++)
		{
			uint8_t r, g, b;
			UINT tmpvalue = 0;
			r = *(buf + (i*nWidth + j) * 4 + 2);
			g = *(buf + (i*nWidth + j) * 4 + 1);
			b = *(buf + (i*nWidth + j) * 4 + 0);
			tmpvalue += r << 16;
			tmpvalue += g << 8;
			tmpvalue += b;

			int nrgbcount = rgbcount[value];
			rgbcount[tmpvalue] = nrgbcount + 1;
			if (nrgbcount + 1 > maxcount)
			{
				maxcount = nrgbcount + 1;
				value = tmpvalue;
			}
		}
	}

	int per = rgbcount[0x487618] * 100 / (nWidth*nHeight);
	if (value != 0x104F05)
	{
		//LOG_INFO("max:%d %02x per:%d\n", maxcount, value, per);
		per = rgbcount[value] * 100 / (nWidth*nHeight);
	}

	if (per < 70)
	{
		m_checkStartkTime == 0;
		return false;
	}
	return true;
}
bool CTableManager::CheckGreen(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	UINT value;
	int maxcount = 0;
	for (int i = 0;i < nHeight;i++)
	{
		for (int j = 0;j < nWidth;j++)
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

	int per = rgbcount[0x104F05] * 100 / (nWidth*nHeight);

	if (per < 50)
	{
		return false;
	}
	return true;
}
bool CTableManager::CheckX(uint8_t* buf, int nWidth, int nHeight)
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
void CTableManager::HandleARGB(uint8_t* buf, int nWidth, int nHeight)
{
	bool bneedreturn = false;
	HanldeNum(bneedreturn);
	if (bneedreturn)
	{

		return;
	}
	m_pmutex->lock();

	if (m_nLastSetNumber == -1 && m_nLastSubNumber == -1)
	{
		m_pmutex->unlock();
		return;
	}
	if (HandleTableCancelClick(buf, nWidth, nHeight))
	{
		LOG_FILE("TableId:%d %02d:%02d 检测到x", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
		m_pmutex->unlock();
		return;
	}
	if (GetScreenCount() == 2)
	{
		if (GetDesktopCaptureSecond()->m_TableSubList[m_TableId - 1].HandleTableCancelClick())
		{
			LOG_FILE("TableId:%d %02d:%02d 第二屏幕检测到x", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
			m_pmutex->unlock();
			return;
		}
	}

	if (m_nStatus == StatusDJS)
	{
		if (!GetDesktopCapture()->m_bDemo&&CheckClickNo(buf, nWidth, nHeight))
		{
			LOG_FILE("TableId:%d %02d:%02d  检测上一个误点击", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
			m_pmutex->unlock();
			return;
		}
	}
	else
	{

	}
	int nCountdown = 0;
	if (m_BoardAllList.size() > 0)
	{
		if (m_nLastSubNumber == m_BoardAllList[m_BoardAllList.size() - 1].SubNumber)
		{
			m_pmutex->unlock();
			return;
		}
	}

	HandleCountdownRegion(buf, nWidth, nHeight, nCountdown);
	HandleBet(buf, nWidth, nHeight);
	m_pmutex->unlock();
}
bool CTableManager::CheckIsZhuang(int betReslut)
{
	if (betReslut == 1)
	{
		return true;
	}
	return false;
}
bool CTableManager::CheckIsXian(int betReslut)
{
	if (betReslut == 0)
	{
		return true;
	}
	return false;
}
std::map<int, int> CTableManager::GetChipList(int colin)
{
	std::map<int, int> ret;
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	for (int i = 6;i > 0;i--)
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

void CTableManager::GetConlin(bool IsZhuang, int& nCoinsBet, int& nCoinsBetbak)
{
	//return 30;
	if (GetAppClient()->m_bEnbaleChip&&GetAppClient()->m_chipList.size() > 0)
	{
		//nCoinsBetbak = nCoinsBet = 0;
		std::string strChip = GetAppClient()->m_chipList[m_nMoneyIndex];
		std::vector<std::string> strvlist = splitWithStl(strChip, "-");
		if (strvlist.size() == 1)
		{
			nCoinsBet = atoi(strvlist[0].c_str());
		}
		else if (strvlist.size() == 2)
		{
			nCoinsBet = atoi(strvlist[0].c_str());
			nCoinsBetbak = atoi(strvlist[1].c_str());
			if (nCoinsBet < nCoinsBetbak)
			{
				int tmp = nCoinsBetbak;
				nCoinsBetbak = nCoinsBet;
				nCoinsBet = tmp;
			}
		}
	}
	int ret = 0;
	if (m_nMethodTotalMoney == 0)
	{
		ret = GetDesktopCapture()->m_nFrist;
	}
	else
	{
		if (IsZhuang)
		{
			ret = (m_nMethodTotalMoney + GetDesktopCapture()->m_nFrist*(m_nMoneyIndex + 1)) / 0.95;
		}
		else
		{
			ret = m_nMethodTotalMoney + GetDesktopCapture()->m_nFrist*(m_nMoneyIndex + 1);
		}
		if (ret > 1000)
		{
			if (ret % 100 != 0)
			{
				ret = ret / 100;
				ret = (ret + 1) * 100;
			}
			while (1)
			{
				int profit = ret - m_nMethodTotalMoney;
				if (ret / profit > 33)
				{
					ret += 100;
				}
				else
				{
					break;
				}

			}
		}
		else
		{
			if (ret % 10 != 0)
			{
				ret = ret / 10;
				ret = (ret + 1) * 10;
			}
		}

	}
	LOG_INFO(" TableId:%d Conlin %d", m_TableId, ret);
	LOG_TABLE("TableId:%d %02d_%02d Conlin %d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, ret);
}



std::string GetNextConlin(bool IsZhuang, int nMethodTotalMoney, int nMethodIndex)
{
	//return 30;
	if (GetAppClient()->m_bEnbaleChip)
	{
		return GetAppClient()->m_chipList[nMethodIndex];
	}
	int ret = 0;
	if (nMethodTotalMoney == 0)
	{
		ret = GetDesktopCapture()->m_nFrist;
	}
	else
	{
		if (IsZhuang)
		{
			ret = (nMethodTotalMoney + GetDesktopCapture()->m_nFrist*(nMethodIndex + 1)) / 0.95;
		}
		else
		{
			ret = nMethodTotalMoney + GetDesktopCapture()->m_nFrist*(nMethodIndex + 1);
		}
		if (ret > 1000)
		{
			if (ret % 100 != 0)
			{
				ret = ret / 100;
				ret = (ret + 1) * 100;
			}
			while (1)
			{
				int profit = ret - nMethodTotalMoney;
				if (ret / profit > 33)
				{
					ret += 100;
				}
				else
				{
					break;
				}

			}
		}
		else
		{
			if (ret % 10 != 0)
			{
				ret = ret / 10;
				ret = (ret + 1) * 10;
			}
		}

	}
	char buf[100];
	sprintf(buf, "%d", ret);
	std::string str = buf;
	return str;
}

bool CTableManager::HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin)
{
	DesktopCapturer* p = GetDesktopCapture();
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	std::map<int, int>::iterator it = chiplist.begin();
	bool IsXiazhu = false;
	int x1, y1;
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	g_mutextable.lock();
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
	mouse_move(10, 20);

	//HandleTableRegion(buf, nWidth, nHeight);
	Sleep(1000);
	LOG_FILE("TableId:%d %02d:%02d  click in x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);
	if (p->m_bDemo)
	{
		//Sleep(2000);
		m_lastBoardInfoRequest.PlayType = 0;
// 		if (IsZhuang)
// 		{
// 			x1 = m_bet[1].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[1].betInfo.ClickTypeNo.y;
// 			mouse_click(x1, y1);
// 		}
// 		else
// 		{
// 			x1 = m_bet[0].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[0].betInfo.ClickTypeNo.y;
// 			mouse_click(x1, y1);
// 		}
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
			mouse_move(10, 20);
			g_mutextable.unlock();
			LOG_FILE("TableId:%d %02d:%02d     检测到状态非倒计时 金额:%d 当前状态:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, num.nStatus, num.strStatus.c_str());
			return false;
		}


	}

	LOG_FILE("TableId:%d %02d:%02d  click out x1:%d y1:%d %s 主:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nConlin);

	mouse_move(10, 20);
	if (p->m_bDemo)
	{
		g_mutextable.unlock();
		return false;
	}
	Sleep(200);
	int nmoney1 = p->GetAppNameInfo(1);
	for (int i = 0;i < 3;i++)
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
	if (nmoney - nmoney1 == nConlin)
	{
		LOG_FILE("TableId:%d %02d:%02d     下注检测金额成功，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, nmoney, nmoney1);
		return true;
	}
	LOG_FILE("TableId:%d %02d:%02d     下注检测金额失败，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, nmoney, nmoney1);
	Sleep(2000);
	if (p->CheckColin(1, m_TableId, nConlin))
	{
		return true;
	}
	Sleep(3000);
	if (p->CheckColin(1, m_TableId, nConlin))
	{
		return true;
	}
	return false;

}

bool check_correct(int nSetNumber, int nSubNumber, int nSetNumber1, int nSubNumber1)
{
	if (nSetNumber == nSetNumber1)
	{
		if (nSubNumber1 == nSubNumber + 1 || nSubNumber1 == nSubNumber)
		{
			return true;
		}
	}
	if ((nSetNumber + 1 == nSetNumber1 || nSetNumber1 == 1) && nSubNumber1 == 1)
	{
		return true;
	}
	return false;
}
void CTableManager::ChcekNum(int& nSetNumber, int& nSubNumber)
{
	if (m_TableNumlist.size() > 4)
	{
		int size = m_TableNumlist.size() - 1;
		int nCorrectCount = 0;
		int bcorrect12 = check_correct(m_TableNumlist[size].SetNumber, m_TableNumlist[size].SubNumber, nSetNumber, nSubNumber);
		int bcorrect23 = check_correct(m_TableNumlist[size - 1].SetNumber, m_TableNumlist[size - 1].SubNumber, m_TableNumlist[2].SetNumber, m_TableNumlist[2].SubNumber);
		if (bcorrect12)
		{
			return;
		}
		else
		{
			nSetNumber = m_TableNumlist[size].SetNumber;
			nSubNumber = m_TableNumlist[size].SubNumber + 1;
		}
	}
	if (m_TableNumlist.size() == 4)
	{
		int bcorrect12 = check_correct(m_TableNumlist[0].SetNumber, m_TableNumlist[0].SubNumber, m_TableNumlist[1].SetNumber, m_TableNumlist[1].SubNumber);
		int bcorrect23 = check_correct(m_TableNumlist[1].SetNumber, m_TableNumlist[1].SubNumber, m_TableNumlist[2].SetNumber, m_TableNumlist[2].SubNumber);
		int bcorrect34 = check_correct(m_TableNumlist[2].SetNumber, m_TableNumlist[2].SubNumber, m_TableNumlist[3].SetNumber, m_TableNumlist[3].SubNumber);
		int all = bcorrect12 + bcorrect23 + bcorrect34;
		if (all == 3)
		{
			return;
		}
		if (all == 1)
		{
			LOG_TABLE("识别只有一个是正确的");
		}
		if (!bcorrect12)
		{
			if (bcorrect23)
			{
				m_TableNumlist[0].SetNumber = m_TableNumlist[1].SetNumber;
				m_TableNumlist[0].SubNumber = m_TableNumlist[1].SubNumber - 1;
			}
		}
		else if (!bcorrect23)
		{
			if (bcorrect12)
			{
				m_TableNumlist[2].SetNumber = m_TableNumlist[1].SetNumber;
				m_TableNumlist[2].SubNumber = m_TableNumlist[1].SubNumber + 1;
				m_BoardAllList[2].SetNumber = m_TableNumlist[1].SetNumber;
				m_BoardAllList[2].SubNumber = m_TableNumlist[1].SubNumber + 1;
			}
		}
		else if (!bcorrect34)
		{
			if (bcorrect23)
			{
				m_TableNumlist[3].SetNumber = m_TableNumlist[2].SetNumber;
				m_TableNumlist[3].SubNumber = m_TableNumlist[2].SubNumber + 1;
				nSetNumber = m_TableNumlist[2].SetNumber;
				nSubNumber = m_TableNumlist[2].SubNumber + 1;
			}
		}
		if (!m_BoardAllList[0].bReport)
		{
			for (int i = 0;i < m_BoardAllList.size();i++)
			{
				GetAppClient()->Report(m_BoardAllList[i]);
				m_BoardAllList[i].bReport = true;
			}
		}
	}
}


bool  CTableManager::CheckNo(uint8_t* buf, int nWidth, int nHeight)
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

	int per = rgbcount[0x00F6F6F6] * 100 / (nWidth*nHeight);
	//LOG_TABLE("value%X per%d", value, per);//
	if (per < 20)
	{
		return false;
	}

	return true;
}
bool CTableManager::CheckClickNo(uint8_t* buf, int nWidth, int nHeight)
{
	if (GetDesktopCapture()->m_bDemo)
	{
		return false;
	}
	int StartX = m_bet[0].betInfo.ClickTypeNoRect.left;
	int StartY = m_bet[0].betInfo.ClickTypeNoRect.top;
	int num_width = m_bet[0].betInfo.ClickTypeNoRect.right - m_bet[0].betInfo.ClickTypeNoRect.left;
	int num_height = m_bet[0].betInfo.ClickTypeNoRect.bottom - m_bet[0].betInfo.ClickTypeNoRect.top;
	if (m_NoBuf == NULL)
	{
		m_NoBuf = new uint8_t[num_width*num_height * 4];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_NoBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	if (CheckNo(m_NoBuf, num_width, num_height))
	{
		if (!m_bclickNo)
		{
			m_bclickNo = true;
			g_mutextable.lock();
			mouse_click(m_bet[0].betInfo.ClickTypeNo.x, m_bet[0].betInfo.ClickTypeNo.y);
			g_mutextable.unlock();
			m_CheckNoDuration = time(NULL);

		}
		else
		{
			if (time(NULL) - m_CheckNoDuration > 3)
			{
				m_bclickNo = false;
			}
		}
		return true;
	}
	{
		int StartX = m_bet[1].betInfo.ClickTypeNoRect.left;
		int StartY = m_bet[1].betInfo.ClickTypeNoRect.top;
		int num_width = m_bet[1].betInfo.ClickTypeNoRect.right - m_bet[1].betInfo.ClickTypeNoRect.left;
		int num_height = m_bet[1].betInfo.ClickTypeNoRect.bottom - m_bet[1].betInfo.ClickTypeNoRect.top;
		if (m_NoBuf == NULL)
		{
			m_NoBuf = new uint8_t[num_width*num_height * 4];
		}
		libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_NoBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
		if (CheckNo(m_NoBuf, num_width, num_height))
		{
			if (!m_bclickNo)
			{
				m_bclickNo = true;
				g_mutextable.lock();
				mouse_click(m_bet[1].betInfo.ClickTypeNo.x, m_bet[1].betInfo.ClickTypeNo.y);
				g_mutextable.unlock();
				m_CheckNoDuration = time(NULL);
			}
			else
			{
				if (time(NULL) - m_CheckNoDuration > 3)
				{
					m_bclickNo = false;
				}
			}
			return true;
		}
	}
	m_bclickNo = false;
	return false;
}

bool CTableManager::HanldeNum(bool& bneedreturn)
{
	bneedreturn = false;
	int nSetNumber = -1;
	int nSubNumber = -1;

	{
		TableNum tableNum1 = GetDesktopCapture()->GetNumber(m_TableId);
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
			// 			if (m_nLastSetNumber == -1)
			// 			{
			// 				if (time(NULL) - m_GetDataSerialTime > 60)
			// 				{
			// 					GetAppClient()->GetDataSerial(m_TableId, m_nLastSetNumber, m_nLastSubNumber);
			// 					m_GetDataSerialTime = time(NULL);
			// 				}
			// 			}

			if (GetAppClient()->GetDataSerialStr(m_TableId).length() == 0)
			{
				GetAppClient()->GetDataSerial(m_TableId, nSetNumber, nSubNumber);
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

				{
					int nTmpBetResult = -1;
					if (nTmpLastSubNumber + 1 != nSubNumber)
					{
						m_pmutex->lock();
						m_BoardlistHistory.clear();
						m_pmutex->unlock();
						GetAppClient()->GetHistoryData(GetAppClient()->GetDataSerialStr(m_TableId), m_TableId, nSubNumber);
						LOG_FILE("TableId:%d %02d m_BoardAllList 最后一个局数:%02d:%02d copy的局数:%02d:%02d,不是连续的获取历史数据，", m_TableId, nTmpLastSetNumber,nTmpLastSetNumber, nTmpLastSubNumber, nSetNumber, nSubNumber);
						//可能已下注，没有识别结果
						int nStartTime = time(NULL);
						vector<BoardInfoRequest> BoardlistHistory;
						while (1)
						{
							m_pmutex->lock();
							if (m_BoardlistHistory.size() > 0)
							{
								BoardlistHistory = m_BoardlistHistory;
							}
							m_pmutex->unlock();
							if (BoardlistHistory.size() > 0)
							{
								break;
							}
							Sleep(100);
							if (time(NULL) - nStartTime > 10)
							{
								break;
							}
						}
						if (nTmpLastSubNumber == -1)
						{
							for (int i = 0;i < BoardlistHistory.size();i++)
							{
								std::string str;
								BoardlistHistory[i].BetType = -1;
								HandleBet(BoardlistHistory[i], false);

							}
						}
						else
						{
							if (nTmpLastSubNumber + 1 != nSubNumber)
							{
								int i = 0;
								bool bFind = false;
								for (i = 0;i < BoardlistHistory.size();i++)
								{
									if (nTmpLastSubNumber + 1 == BoardlistHistory[i].SubNumber)
									{
										nTmpBetResult = BoardlistHistory[i].BetResultType;
										bFind = true;
										HandleBet(BoardlistHistory[i]);
										continue;
									}
									if (bFind)
									{
										std::string str;
										if (GetCurrentBetType(BoardlistHistory[i].BetType, str))
										{
											HandleBet(BoardlistHistory[i]);
										}
										m_nLastSubNumber = BoardlistHistory[i].SubNumber;
									}
								}
							}
						}
						
						int nCurrentBetType = 0;
						std::string strmoney;
						GetCurrentBetType(nCurrentBetType, strmoney);
						bneedreturn = true;
						Sleep(3500);

					}

				}

			}

			LOG_FILE("TableId:%d %02d:%02d  old %02d:%02d m_isCheck=false", m_TableId, nSetNumber, nSubNumber, m_nLastSetNumber, m_nLastSubNumber);
			m_nLastSubNumber = nSubNumber;
			m_nLastSetNumber = nSetNumber;
			m_nCountDown = -1;
			m_isCheck = false;

			if (method.length() == 0)
			{

				if (GetAppClient()->IsMehtodHome())
				{
					if (time(NULL) - m_GetDataSerialTime > 60)
					{
						GetAppClient()->GetDataSerial(m_TableId, m_nLastSetNumber, m_nLastSubNumber);
						m_GetDataSerialTime = time(NULL);
					}
				}
			}
			if (m_nLastSubNumber == 1)
			{
				m_bNeedMethod = true;
				std::list<BoardInfoRequest> boardlist;
				for (int j = m_BoardAllList.size();j > 0;j--)
				{
					if (j == m_BoardAllList.size())
					{
						if (m_BoardAllList[j - 1].SubNumber != 1)
						{
							boardlist.push_back(m_BoardAllList[j - 1]);
						}
					}
					else
					{
						if (m_BoardAllList[j - 1].SubNumber != 1)
						{
							boardlist.push_front(m_BoardAllList[j - 1]);
						}
						else
						{
							boardlist.push_front(m_BoardAllList[j - 1]);
							break;
						}
					}
				}
				method = "";
				LOG_FILE("TableId:%d %02d:%02d  当前局数1 清空算法", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
				GetAppClient()->SendMethodTest(boardlist);
				GetAppClient()->GetDataSerial(m_TableId, m_nLastSetNumber, m_nLastSubNumber);


			}
		}
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}
	return false;
}
void CTableManager::HandleBet(uint8_t* buf, int nWidth, int nHeight)
{

	if (!m_bNeedResult)
	{
		return;
	}

	LOG_FILE("TableId:%d %02d:%02d 开始检测结果 当前状态%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus);
	std::vector<int> BetResultList;
	if (m_nStatus == StatusJSZ)
	{

		for (int i = 0;i < 5;i++)
		{
			int StartX = m_bet[i].m_BetTypeRegion.left;
			int StartY = m_bet[i].m_BetTypeRegion.top;
			int bet_width = m_bet[i].m_BetTypeRegion.right - m_bet[i].m_BetTypeRegion.left;
			int bet_height = m_bet[i].m_BetTypeRegion.bottom - m_bet[i].m_BetTypeRegion.top;
			if (betBuf == NULL)
			{
				betBuf = new uint8_t[bet_width*bet_height * 8];
			}
			libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, betBuf, bet_width * 4, StartX, StartY, nWidth, nHeight, bet_width, bet_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
			if (CheckNew(betBuf, bet_width, bet_height))
			{
				BetResultList.push_back(m_bet[i].betType);
			}
		}
	}


	LOG_FILE("TableId:%d %02d:%02d 开始检测结果 当前状态%d 结果数:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus, BetResultList.size());



	if (BetResultList.size() > 0 && BetResultList.size() < 4)
	{

		if (!GetDesktopCapture()->m_bDemo)
		{
			if (!m_bXiaZhuSuccess)
			{
				m_nCoinsBet = 0;
				m_nCoinsBetSub = 0;
				m_lastBoardInfoRequest.Profit = 0;
				m_lastBoardInfoRequest.CoinsBet = 0;
			}
		}
		m_nReportTime = time(NULL);
		m_bNeedResult = false;
		if (m_BoardAllList.size() > 0)
		{
			if (m_BoardAllList[m_BoardAllList.size() - 1].SubNumber == m_nLastSubNumber
				&& m_BoardAllList[m_BoardAllList.size() - 1].SetNumber == m_nLastSetNumber)
			{
				LOG_FILE("TableId:%d %02d:%02d 重复的结果 当前状态%d 结果数:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus, BetResultList.size());
				return;
			}
		}


		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = m_nLastSetNumber;
		pTabInfoMsg->nSubNumber = m_nLastSubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		pTabInfoMsg->Betlist = BetResultList;
		pTabInfoMsg->CoinsBet = m_nCoinsBet;

		if (m_nCoinsBetMain)
		{
			m_lastBoardInfoRequest.MainSub = MAIN_TYPE;
			m_lastBoardInfoRequest.AccountMoney = GetDesktopCaptureSecond()->GetAccountMoney();
			m_lastBoardInfoRequest.AccountName = GetDesktopCaptureSecond()->GetAccountName();
		}
		else
		{
			m_lastBoardInfoRequest.MainSub = SUB_TYPE;
			m_lastBoardInfoRequest.AccountMoney = GetDesktopCapture()->GetAccountMoney();
			m_lastBoardInfoRequest.AccountName = GetDesktopCapture()->GetAccountName();
		}

		m_lastBoardInfoRequest.TableId = m_TableId;
		m_lastBoardInfoRequest.EndTime = GetDesktopCapture()->m_nPlayTime;
		m_lastBoardInfoRequest.Method = method;
		m_lastBoardInfoRequest.BetResult = BetResultList;
		m_lastBoardInfoRequest.CoinsBet = m_nCoinsBet;
		m_lastBoardInfoRequest.time = (char*)(LPCTSTR)GetCurrentTime2Str();
		m_lastBoardInfoRequest.BetType = m_nCurrentBetType;
		m_lastBoardInfoRequest.methodIndex = m_nMethodIndex;
		m_lastBoardInfoRequest.SetNumber = m_nLastSetNumber;
		m_lastBoardInfoRequest.SubNumber = m_nLastSubNumber;
		m_lastBoardInfoRequest.DataSerial = GetAppClient()->GetDataSerialStr(m_TableId);
		m_lastBoardInfoRequest.AIPlay = m_nAIPlay;
		//if (m_nAIPlay == PLAYAI)
		{
			m_lastBoardInfoRequest.strAi = m_sAIStr;
		}
		m_lastBoardInfoRequest.StopLoss = GetDesktopCapture()->StopLoss;
		m_lastBoardInfoRequest.StopProfit = GetDesktopCapture()->StopProfit;
		if (method.length() == 0)
		{
			if (GetAppClient()->IsMehtodHome())
			{
				if (time(NULL) - m_GetMethodTime > 60)
				{
					m_GetMethodTime = time(NULL);
					GetAppClient()->UpdateMehtod(GetAppClient()->GetDataSerialStr(m_TableId), gAppConfig.methodlength, m_TableId);
				}
			}
			if (GetAppClient()->IsMehtodJingYang())
			{
				method = GetAppClient()->GetNewMethod(m_TableId);
			}

			m_lastBoardInfoRequest.DataSerial = GetAppClient()->GetDataSerialStr(m_TableId);
			m_lastBoardInfoRequest.Batch = GetAppClient()->m_TableMethodSerialList[m_TableId];
		}
		for (int i = 0;i < BetResultList.size();i++)
		{
			if (BetResultList[i] == 0 || BetResultList[i] == 1 || BetResultList[i] == 2)
			{
				m_lastBoardInfoRequest.BetResultType = BetResultList[i];
				pTabInfoMsg->nBetTypeResult = BetResultList[i];
				LOG_FILE("TableId:%d %02d:%02d  当前状态%d 结果数:%d  识别到结果:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus, BetResultList.size(), BetResultList[i]);
			}
		}
		pTabInfoMsg->method = method;
		if (GetAppClient()->IsMehtodHome())
		{
			m_lastBoardInfoRequest.Random = 0;
			m_lastBoardInfoRequest.Batch = GetAppClient()->m_TableMethodSerialList[m_TableId];
		}
		else
		{
			if (GetAppClient()->IsMehtodRandom())
			{
				m_lastBoardInfoRequest.Random = 1;
			}
			else
			{
				m_lastBoardInfoRequest.Random = 2;
			}

		}

		if (m_lastBoardInfoRequest.BetResultType == 2)
		{
			if (m_nAIPlay == PLAYAI)
			{
				if (m_nUseTimesNumber > 0)
				{
					m_nUseTimesNumber--;
				}
			}
			m_BNeedFan = false;
			m_betcountlist[2]++;
			pTabInfoMsg->IsLost = -1;
			pTabInfoMsg->nTotalProfitMoney = m_nTotalProfitMoney;
			pTabInfoMsg->nTotaCoinsBetMoney = m_nTotaCoinsBetMoney;
			//m_lastBoardInfoRequest.CoinsBet = 0;
			m_lastBoardInfoRequest.Profit = 0;
			if (m_BoardAllList.size() > 0)
			{
				m_BoardAllList.push_back(m_lastBoardInfoRequest);
			}
			else
			{
				m_BoardAllList.push_back(m_lastBoardInfoRequest);
			}
			if (!GetDesktopCapture()->m_bDemo)
			{
				if (m_lastBoardInfoRequest.MainSub == SUB_TYPE)
					CMoneyMgr::GetInstance()->SetBetResult(m_TableId, m_lastBoardInfoRequest.CoinsBet);
			}
			//if (m_BoardAllList.size() > 3)
			{
				GetAppClient()->Report(m_lastBoardInfoRequest);
			}
			LOG_FILE("TableId:%d %02d:%02d  当前下注金额 主:%d 从:%d,获利:%d  下注:%d 结果:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, m_lastBoardInfoRequest.Profit, m_nCurrentBetType, m_lastBoardInfoRequest.BetResultType);
		}
		else
		{

			if (m_lastBoardInfoRequest.BetResultType == m_nCurrentBetType)//xian
			{
				m_BNeedFan = false;
				pTabInfoMsg->IsLost = 0;
				if (CheckIsZhuang(m_nCurrentBetType))
				{

					m_lastBoardInfoRequest.Profit = m_lastBoardInfoRequest.CoinsBet*0.95;
					pTabInfoMsg->CoinsBet = m_lastBoardInfoRequest.CoinsBet*0.95;

				}
				else
				{
					m_lastBoardInfoRequest.Profit = m_nCoinsBet;
				}
				if (m_nCoinsBetSub > 0)
				{
					pTabInfoMsg->CoinsBet -= m_nCoinsBetSub;
					m_lastBoardInfoRequest.Profit -= m_nCoinsBetSub;
				}
				if (!GetDesktopCapture()->m_bDemo)
				{
					if (m_lastBoardInfoRequest.MainSub == SUB_TYPE)
						CMoneyMgr::GetInstance()->SetBetResult(m_TableId, m_lastBoardInfoRequest.Profit + m_lastBoardInfoRequest.CoinsBet);
				}
			}
			else
			{
				pTabInfoMsg->IsLost = 1;
				m_lastBoardInfoRequest.Profit = m_nCoinsBet * -1;
				pTabInfoMsg->CoinsBet = m_nCoinsBet * -1;
				if (m_lastBoardInfoRequest.BetResultType)
				{
					if (m_nCoinsBetSub > 0)
					{
						pTabInfoMsg->CoinsBet += m_nCoinsBetSub * 0.95;
						m_lastBoardInfoRequest.Profit += m_nCoinsBetSub * 0.95;
					}
				}
				else
				{
					if (m_nCoinsBetSub > 0)
					{
						pTabInfoMsg->CoinsBet += m_nCoinsBetSub;
						m_lastBoardInfoRequest.Profit += m_nCoinsBetSub;
					}
				}
			}

			pTabInfoMsg->Profit = m_lastBoardInfoRequest.Profit;
			if (m_lastBoardInfoRequest.BetResultType == 0 || m_lastBoardInfoRequest.BetResultType == 1)
			{
				m_betcountlist[m_lastBoardInfoRequest.BetResultType]++;
				m_BoardList.push_back(m_lastBoardInfoRequest);
				if (m_nLastSubNumber == 1)
				{
					m_sAIStr = "";
					m_Rulelist.clear();
				}
				if (m_Rulelist.size() > 0)
				{
					if (m_BoardAllList[m_BoardAllList.size() - 1].SubNumber != m_nLastSubNumber)
					{
						if (m_BoardAllList[m_BoardAllList.size() - 1].SubNumber + 1 != m_nLastSubNumber)
						{
							m_Rulelist.clear();
						}
					}

					if (m_Rulelist.size() > 0 && m_Rulelist[m_Rulelist.size() - 1].SubNumber != m_nLastSubNumber)
					{
						if (m_nLastSubNumber == 1)
						{
							m_Rulelist.clear();
						}
						m_Rulelist.push_back(m_lastBoardInfoRequest);
					}
				}
				else
				{
					m_Rulelist.push_back(m_lastBoardInfoRequest);
				}
			}
			if (m_BoardAllList.size() > 0)
			{
				m_BoardAllList.push_back(m_lastBoardInfoRequest);
			}
			else
			{
				m_BoardAllList.push_back(m_lastBoardInfoRequest);
			}
			//if (m_nCoinsBetMain)
			{
				//if (m_BoardAllList.size() > 4)
				{
					GetAppClient()->Report(m_lastBoardInfoRequest);
				}

			}
			pTabInfoMsg->nTotalProfitMoney = m_nTotalProfitMoney;
			pTabInfoMsg->nTotaCoinsBetMoney = m_nTotaCoinsBetMoney;
			pTabInfoMsg->nAIplay = m_nAIPlay;
			if (method.length() == 0 && GetAppClient()->IsMehtodRandom())
			{
				if (m_BoardList.size() >= gAppConfig.methodlength)
				{
					std::string newmethod;
					char newbuf[20];
					memset(newbuf, 0, 20);
					if (m_BoardList.size() >= gAppConfig.methodlength)
					{
						for (int j = 0;j < gAppConfig.methodlength;j++)
						{
							if (m_BoardList[m_BoardList.size() - gAppConfig.methodlength + j].BetResultType == 0)
							{
								newbuf[j] = '3';
							}
							else
							{
								newbuf[j] = '2';
							}
						}
					}
					method = newbuf;
				}
			}
			// 			if (!m_bXiaZhuSuccess&&!GetDesktopCapture()->m_bDemo)
			// 			{
			// 				m_nCoinsBet = 0;
			// 			}
			if (m_nCoinsBet > 0)
			{
				m_nMethodTotalMoney += m_nCoinsBet;
				GetDesktopCapture()->AddCurTotalMoney(m_lastBoardInfoRequest.Profit);
				GetDesktopCapture()->AddTotalWaterMoney(m_nCoinsBet + m_nCoinsBetSub);
				//GetDesktopCapture()->AddTotalWaterMoney(m_nCoinsBet);
				m_nTotaCoinsBetMoney += (m_nCoinsBet + m_nCoinsBetSub);
				m_nTotalProfitMoney += m_lastBoardInfoRequest.Profit;
				pTabInfoMsg->nTotalProfitMoney = m_nTotalProfitMoney;
				pTabInfoMsg->nTotaCoinsBetMoney = m_nTotaCoinsBetMoney;
				m_nMethodIndex++;
				m_nMethodIndex = m_nMethodIndex % method.length();
				m_nMoneyIndex++;
				m_nMoneyIndex = m_nMoneyIndex % method.length();



				LOG_FILE("TableId:%d %02d:%02d  当前下注金额 主:%d 从:%d,获利:%d  下注:%d 结果:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, m_lastBoardInfoRequest.Profit, m_nCurrentBetType, m_lastBoardInfoRequest.BetResultType);
				if (m_lastBoardInfoRequest.Profit > 0)
				{
					if (gAppConfig.nBackType == BOTHBACK || 1)
					{
						m_nMethodIndex = 0;
						m_nMoneyIndex = 0;
						m_bEnableAI = true;
						m_nUseTimesNumber = 0;
					}
					else if (gAppConfig.nBackType == MONEYBACK)
					{
						m_nMoneyIndex = 0;
					}
					else if (gAppConfig.nBackType == RANDOMBACK)
					{
						srand(time(NULL) + GetTickCount64());
						int randnum = rand();
						if (randnum % 2 == 1)
						{
							m_nMethodIndex = 0;
							m_nMoneyIndex = 0;
						}
						else
						{
							m_nMoneyIndex = 0;
						}

					}
				}
				else
				{
					if (GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end())
					{
						MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
						if (rule.UnmatchClose)
						{
							LOG_FILE("TableId:%d %02d:%02d  当前开启AI 打不中一次就关闭", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
							m_bEnableAI = false;
						}
					}
				}
				if (m_nCoinsBetSub > 0)
				{
					m_nCoinsBetSub = 0;
				}
				if (m_nMethodIndex == 0)
				{
					if (GetAppClient()->IsMehtodRandom() && m_BoardList.size() >= gAppConfig.methodlength)
					{
						std::string newmethod;
						char newbuf[20];
						memset(newbuf, 0, 20);
						if (m_BoardList.size() >= gAppConfig.methodlength)
						{
							for (int j = 0;j < gAppConfig.methodlength;j++)
							{
								if (m_BoardList[m_BoardList.size() - gAppConfig.methodlength + j].BetResultType == 0)
								{
									newbuf[j] = '3';
								}
								else
								{
									newbuf[j] = '2';
								}
							}
						}
						method = newbuf;
					}
					m_nMethodTotalMoney = 0;
				}
			}

#if USEMY
			if (m_nTotalProfitMoney < -500 && m_nTotalProfitMoney *-100 / m_nTotaCoinsBetMoney >25)
			{
				RevertMeothod();
			}
#endif

		}
		int nCurrentBetType = 0;
		std::string strmoney;
		GetCurrentBetType(nCurrentBetType, strmoney);
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}

}
void CTableManager::HandleBet(BoardInfoRequest& CurrentBoardInfoRequest,bool breport)
{
	int nCurrentBetType = 0;
	int nMethodIndex = 0;
	if (!GetDesktopCapture()->m_bDemo)
	{
		if (!m_bXiaZhuSuccess)
		{
			m_nCoinsBet = 0;
			m_nCoinsBetSub = 0;
			nMethodIndex = m_nMethodIndex;
			CurrentBoardInfoRequest.Profit = 0;
			CurrentBoardInfoRequest.CoinsBet = 0;
			m_nAIPlay = PLAYNONE;
			m_sAIStr = "";
		}
		else
		{

			nCurrentBetType = m_nCurrentBetType;
			nMethodIndex = m_nMethodIndex;
			CurrentBoardInfoRequest.CoinsBet = m_nCoinsBet;
			CurrentBoardInfoRequest.CoinsBetSub = m_nCoinsBetSub;
		}
	}
	if (CurrentBoardInfoRequest.BetResult.size() > 0 && CurrentBoardInfoRequest.BetResult.size() < 4)
	{

		int nLastSetNumber = CurrentBoardInfoRequest.SetNumber;
		int nLastSubNumber = CurrentBoardInfoRequest.SubNumber;
		if (CurrentBoardInfoRequest.CoinsBet > gAppConfig.m_SplitMoney&& GetScreenCount() == 2)
		{
			CurrentBoardInfoRequest.MainSub = SUB_TYPE;
			CurrentBoardInfoRequest.AccountMoney = GetDesktopCaptureSecond()->GetAccountMoney();
			CurrentBoardInfoRequest.AccountName = GetDesktopCaptureSecond()->GetAccountName();
		}
		else
		{
			CurrentBoardInfoRequest.AccountMoney = GetDesktopCaptureSecond()->GetAccountMoney();
			CurrentBoardInfoRequest.AccountName = GetDesktopCaptureSecond()->GetAccountName();
			CurrentBoardInfoRequest.MainSub = MAIN_TYPE;
		}

		CurrentBoardInfoRequest.Method = method;
		CurrentBoardInfoRequest.time = (char*)(LPCTSTR)GetCurrentTime2Str();
		CurrentBoardInfoRequest.BetType = nCurrentBetType;
		CurrentBoardInfoRequest.methodIndex = nMethodIndex;
		CurrentBoardInfoRequest.DataSerial = GetAppClient()->GetDataSerialStr(m_TableId);
		CurrentBoardInfoRequest.AIPlay = m_nAIPlay;
		//if (m_nAIPlay == PLAYAI)
		{
			CurrentBoardInfoRequest.strAi = m_sAIStr;
		}
		CurrentBoardInfoRequest.StopLoss = GetDesktopCapture()->StopLoss;
		CurrentBoardInfoRequest.StopProfit = GetDesktopCapture()->StopProfit;
		std::vector<int> BetResultList = CurrentBoardInfoRequest.BetResult;
		LOG_FILE("TableId:%d %02d:%02d    历史结果数:%d  识别到结果:%d", m_TableId, nLastSetNumber, nLastSubNumber, BetResultList.size(), BetResultList[0]);
		if (GetAppClient()->IsMehtodHome())
		{
			CurrentBoardInfoRequest.Random = 0;
			CurrentBoardInfoRequest.Batch = GetAppClient()->m_TableMethodSerialList[m_TableId];
		}
		else
		{
			if (GetAppClient()->IsMehtodRandom())
			{
				CurrentBoardInfoRequest.Random = 1;
			}
			else
			{
				CurrentBoardInfoRequest.Random = 2;
			}

		}
		TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
		pTabInfoMsg->nSetNumber = CurrentBoardInfoRequest.SetNumber;
		pTabInfoMsg->nSubNumber = CurrentBoardInfoRequest.SubNumber;
		pTabInfoMsg->nTableId = m_TableId;
		pTabInfoMsg->Betlist = CurrentBoardInfoRequest.BetResult;

		if (CurrentBoardInfoRequest.BetResultType == 2)
		{
			m_BNeedFan = false;
			m_betcountlist[2]++;
			CurrentBoardInfoRequest.Profit = 0;
			if (m_BoardAllList.size() > 0)
			{
				m_BoardAllList.push_back(CurrentBoardInfoRequest);
			}
			else
			{
				m_BoardAllList.push_back(CurrentBoardInfoRequest);
			}

			if (breport)
			{
				GetAppClient()->Report(CurrentBoardInfoRequest);
			}
			LOG_FILE("TableId:%d %02d:%02d  历史下注金额 主:%d 从:%d,获利:%d  下注:%d 结果:%d", m_TableId, nLastSetNumber, nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, CurrentBoardInfoRequest.Profit, m_nCurrentBetType, CurrentBoardInfoRequest.BetResultType);
		}
		else
		{

			if (CurrentBoardInfoRequest.BetResultType == CurrentBoardInfoRequest.BetType)//xian
			{
				m_BNeedFan = false;
				if (CheckIsZhuang(m_nCurrentBetType))
				{

					CurrentBoardInfoRequest.Profit = CurrentBoardInfoRequest.CoinsBet*0.95;

				}
				else
				{
					CurrentBoardInfoRequest.Profit = CurrentBoardInfoRequest.CoinsBet;
				}
				if (CurrentBoardInfoRequest.CoinsBetSub > 0)
				{
					CurrentBoardInfoRequest.Profit -= CurrentBoardInfoRequest.CoinsBetSub;
				}
			}
			else
			{
				CurrentBoardInfoRequest.Profit = CurrentBoardInfoRequest.CoinsBet * -1;
				if (CurrentBoardInfoRequest.BetResultType)
				{
					if (CurrentBoardInfoRequest.CoinsBetSub > 0)
					{
						CurrentBoardInfoRequest.Profit += CurrentBoardInfoRequest.CoinsBetSub * 0.95;
					}
				}
				else
				{
					if (CurrentBoardInfoRequest.CoinsBetSub > 0)
					{
						CurrentBoardInfoRequest.Profit += CurrentBoardInfoRequest.CoinsBetSub;
					}
				}
			}
			pTabInfoMsg->Profit = CurrentBoardInfoRequest.Profit;
			pTabInfoMsg->method = method;
			if (CurrentBoardInfoRequest.BetResultType == 0 || CurrentBoardInfoRequest.BetResultType == 1)
			{
				m_betcountlist[CurrentBoardInfoRequest.BetResultType]++;
				m_BoardList.push_back(CurrentBoardInfoRequest);

				if (m_Rulelist.size() > 0)
				{
					if (m_BoardAllList[m_BoardAllList.size() - 1].SubNumber != nLastSubNumber)
					{
						if (m_BoardAllList[m_BoardAllList.size() - 1].SubNumber + 1 != nLastSubNumber)
						{
							m_Rulelist.clear();
						}
					}

					if (m_Rulelist.size() > 0 && m_Rulelist[m_Rulelist.size() - 1].SubNumber != nLastSubNumber)
					{
						if (nLastSubNumber == 1)
						{
							m_Rulelist.clear();
						}
						m_Rulelist.push_back(CurrentBoardInfoRequest);
					}
				}
				else
				{
					m_Rulelist.push_back(CurrentBoardInfoRequest);
				}
			}
			if (m_BoardAllList.size() > 0)
			{
				m_BoardAllList.push_back(CurrentBoardInfoRequest);
			}
			else
			{
				m_BoardAllList.push_back(CurrentBoardInfoRequest);
			}
			if (breport)
				GetAppClient()->Report(CurrentBoardInfoRequest);
			LOG_FILE("TableId:%d %02d:%02d  历史下注金额 主:%d 从:%d,获利:%d  下注:%d 结果:%d", m_TableId, nLastSetNumber, nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, CurrentBoardInfoRequest.Profit, m_nCurrentBetType, CurrentBoardInfoRequest.BetResultType);
			if (method.length() == 0 && GetAppClient()->IsMehtodRandom())
			{
				if (m_BoardList.size() >= gAppConfig.methodlength)
				{
					std::string newmethod;
					char newbuf[20];
					memset(newbuf, 0, 20);
					if (m_BoardList.size() >= gAppConfig.methodlength)
					{
						for (int j = 0;j < gAppConfig.methodlength;j++)
						{
							if (m_BoardList[m_BoardList.size() - gAppConfig.methodlength + j].BetResultType == 0)
							{
								newbuf[j] = '3';
							}
							else
							{
								newbuf[j] = '2';
							}
						}
					}
					method = newbuf;
				}
			}
			if (CurrentBoardInfoRequest.CoinsBet > 0 && method.length() > 0)
			{
				m_nMethodTotalMoney += CurrentBoardInfoRequest.CoinsBet;
				GetDesktopCapture()->AddCurTotalMoney(CurrentBoardInfoRequest.Profit);
				GetDesktopCapture()->AddTotalWaterMoney(CurrentBoardInfoRequest.CoinsBet + CurrentBoardInfoRequest.CoinsBetSub);
				//GetDesktopCapture()->AddTotalWaterMoney(m_nCoinsBet);
				m_nTotaCoinsBetMoney += (CurrentBoardInfoRequest.CoinsBet + CurrentBoardInfoRequest.CoinsBetSub);
				m_nTotalProfitMoney += CurrentBoardInfoRequest.Profit;
				m_nMethodIndex++;
				m_nMethodIndex = m_nMethodIndex % method.length();
				m_nMoneyIndex++;
				m_nMoneyIndex = m_nMoneyIndex % method.length();



				LOG_FILE("TableId:%d %02d:%02d  历史当前下注金额 主:%d 从:%d,获利:%d  下注:%d 结果:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, CurrentBoardInfoRequest.CoinsBet, CurrentBoardInfoRequest.CoinsBetSub, CurrentBoardInfoRequest.Profit, CurrentBoardInfoRequest.BetType, CurrentBoardInfoRequest.BetResultType);
				if (CurrentBoardInfoRequest.Profit > 0)
				{
					if (gAppConfig.nBackType == BOTHBACK || 1)
					{
						m_nMethodIndex = 0;
						m_nMoneyIndex = 0;
						m_bEnableAI = true;
						m_nUseTimesNumber = 0;
					}
					else if (gAppConfig.nBackType == MONEYBACK)
					{
						m_nMoneyIndex = 0;
					}
					else if (gAppConfig.nBackType == RANDOMBACK)
					{
						srand(time(NULL) + GetTickCount64());
						int randnum = rand();
						if (randnum % 2 == 1)
						{
							m_nMethodIndex = 0;
							m_nMoneyIndex = 0;
						}
						else
						{
							m_nMoneyIndex = 0;
						}

					}
				}
				else
				{
					if (GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end())
					{
						MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
						if (rule.UnmatchClose)
						{
							LOG_FILE("TableId:%d %02d:%02d  历史当前开启AI 打不中一次就关闭", m_TableId, nLastSetNumber, nLastSubNumber);
							m_bEnableAI = false;
						}
					}
				}


				if (m_nMethodIndex == 0)
				{
					if (GetAppClient()->IsMehtodRandom() && m_BoardList.size() >= gAppConfig.methodlength)
					{
						std::string newmethod;
						char newbuf[20];
						memset(newbuf, 0, 20);
						if (m_BoardList.size() >= gAppConfig.methodlength)
						{
							for (int j = 0;j < gAppConfig.methodlength;j++)
							{
								if (m_BoardList[m_BoardList.size() - gAppConfig.methodlength + j].BetResultType == 0)
								{
									newbuf[j] = '3';
								}
								else
								{
									newbuf[j] = '2';
								}
							}
						}
						method = newbuf;
					}
					m_nMethodTotalMoney = 0;
				}
				m_bXiaZhuSuccess = false;
			}
		}
		if (m_nCoinsBetSub > 0)
		{
			m_nCoinsBetSub = 0;
		}
		pTabInfoMsg->nTotalProfitMoney = m_nTotalProfitMoney;
		pTabInfoMsg->nTotaCoinsBetMoney = m_nTotaCoinsBetMoney;
		::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	}

}
void CTableManager::ResetBet()
{
	//g_mutextable.lock();
	m_lastBoardInfoRequest.CoinsBet = 0;
	m_lastBoardInfoRequest.Profit = 0;
	m_nCoinsBet = 0;
	m_lastBoardInfoRequest.AIPlay = PLAYNONE;
	m_lastBoardInfoRequest.strAi = "";
	//g_mutextable.unlock();
}
bool  CTableManager::CheckChipRGB(uint8_t* buf, int nWidth, int nHeight)
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
	if (value == 0x104F05)
	{
		return false;
	}
	int per = rgbcount[0x212121] * 100 / (nWidth*nHeight);
	if (per < 20)
	{
		return false;
	}
	LOG_TABLE("CheckChipRGB value%X per%d", value, per);
	return true;
}
bool CTableManager::CheckChipRegion(bool iSzhuang, uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = 0;
	int StartY = 0;
	int num_width = 0;
	int num_height = 0;
	if (iSzhuang)
	{
		StartX = m_bet[1].betInfo.ChipRegion.left;
		StartY = m_bet[1].betInfo.ChipRegion.top;
		num_width = m_bet[1].betInfo.ChipRegion.right - m_bet[1].betInfo.ChipRegion.left;
		num_height = m_bet[1].betInfo.ChipRegion.bottom - m_bet[1].betInfo.ChipRegion.top;
	}
	else
	{
		StartX = m_bet[0].betInfo.ChipRegion.left;
		StartY = m_bet[0].betInfo.ChipRegion.top;
		num_width = m_bet[0].betInfo.ChipRegion.right - m_bet[0].betInfo.ChipRegion.left;
		num_height = m_bet[0].betInfo.ChipRegion.bottom - m_bet[0].betInfo.ChipRegion.top;
	}
	if (RegionBuf == NULL)
	{
		RegionBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, RegionBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	return CheckChipRGB(RegionBuf, num_width, num_height);
}
bool CTableManager::HandleCheckXiaZhuBak(int  nCoinsBetBak, bool IsZhuang, uint8_t* argbbuf, int nWidth, int nHeight)
{
	bool bSuccess = false;
	if (nCoinsBetBak > 0)
	{
		if (!GetDesktopCapture()->m_bDemo&&CheckChipRegion(!m_nCurrentBetType, argbbuf, nWidth, nHeight))
		{

		}
		else
		{

			{
				m_bXiaZhuSuccess = false;
				std::map<int, int> chiplist = GetChipList(nCoinsBetBak);
				bSuccess = HandleXiaZhu(chiplist, !IsZhuang, argbbuf, nWidth, nHeight, nCoinsBetBak);
				for (int k = 0;k < 1;k++)
				{
					if (!bSuccess && !GetDesktopCapture()->m_bDemo)
					{
						m_isCheck = false;
						return false;
					}

				}
			}

		}
	}
	if (bSuccess)
	{
		m_bXiaZhuSuccess = true;
	}
	return true;
}
bool CTableManager::HandleXiaZhuAll(uint8_t* buf, int nWidth, int nHeight, bool IsZhuang, int nChipMoneyMain, int nChipMoneySub)
{

	std::map<int, int> chiplistMain = GetChipList(nChipMoneyMain - 1);
	std::map<int, int> chiplistSub = GetChipList(nChipMoneySub - 1);
	ScreenInfo* pScreenInfo = GetDesktopCapture()->GetScreenInfo();
	std::map<int, int>::iterator it = chiplistMain.begin();
	bool IsXiazhu = false;
	int x1, y1;
	DesktopCapturer*  p = GetDesktopCapture();
	if (p->CheckColin(1, m_TableId, nChipMoneyMain) || p->CheckColin(1, m_TableId, nChipMoneyMain))
	{
		return true;
	}
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	g_mutextable.lock();
	int nmoney = p->GetAppNameInfo(1);
	if (nmoney == 0)
	{
		Sleep(1000);
		nmoney = p->GetAppNameInfo(1);
	}
	//	Sleep(10);
	// 	mouse_click((GetDesktopCapture()->m_ScreenInfo.GameModeRegion.left + GetDesktopCapture()->m_ScreenInfo.GameModeRegion.right) / 2
	// 		, (GetDesktopCapture()->m_ScreenInfo.GameModeRegion.bottom + GetDesktopCapture()->m_ScreenInfo.GameModeRegion.top) / 2);
	CString str;
	while (it != chiplistMain.end())
	{
		int x = (pScreenInfo->ChipList[it->first].ChipNumRegion.left + pScreenInfo->ChipList[it->first].ChipNumRegion.right) / 2;
		int y = (pScreenInfo->ChipList[it->first].ChipNumRegion.top + pScreenInfo->ChipList[it->first].ChipNumRegion.bottom) / 2;
		CString tmp;
		tmp.Format("%d:%d  ", it->first, it->second);
		mouse_click(x, y);
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
	int xone = (pScreenInfo->ChipList[1].ChipNumRegion.left + pScreenInfo->ChipList[1].ChipNumRegion.right) / 2;
	int yone = (pScreenInfo->ChipList[1].ChipNumRegion.top + pScreenInfo->ChipList[1].ChipNumRegion.bottom) / 2;
	Sleep(100);
	mouse_click(xone, yone);
	Sleep(100);
	bool SubZhuang = !IsZhuang;
	if (SubZhuang)
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
	it = chiplistSub.begin();
	while (it != chiplistSub.end())
	{
		int x = (pScreenInfo->ChipList[it->first].ChipNumRegion.left + pScreenInfo->ChipList[it->first].ChipNumRegion.right) / 2;
		int y = (pScreenInfo->ChipList[it->first].ChipNumRegion.top + pScreenInfo->ChipList[it->first].ChipNumRegion.bottom) / 2;
		CString tmp;
		tmp.Format("%d:%d  ", it->first, it->second);
		mouse_click(x, y);
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
			Sleep(100);
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
	/*mouse_click(10, 20);*/

	//HandleTableRegion(buf, nWidth, nHeight);
	Sleep(1000);
	LOG_FILE("TableId:%d %02d:%02d  click in x1:%d y1:%d %s 主:%d 从:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nChipMoneyMain, nChipMoneySub);
	if (p->m_bDemo)
	{
		m_lastBoardInfoRequest.PlayType = 0;
// 		Sleep(2000);
// 		if (SubZhuang)
// 		{
// 			x1 = m_bet[1].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[1].betInfo.ClickTypeNo.y;
// 			mouse_click(x1, y1);
// 		}
// 		else
// 		{
// 			x1 = m_bet[0].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[0].betInfo.ClickTypeNo.y;
// 			mouse_click(x1, y1);
// 		}
	}
	else
	{
		m_lastBoardInfoRequest.PlayType = 1;

		if (num.nStatus == StatusDJS)
		{
			if (time(NULL) - m_nCountDown < 35)
			{
				if (SubZhuang)
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
		}
		else
		{
			mouse_move(10, 20);
			g_mutextable.unlock();
			LOG_FILE("TableId:%d %02d:%02d     检测到状态非倒计时 金额:%d 当前状态:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, num.nStatus, num.strStatus.c_str());
			return false;
		}


	}
	LOG_FILE("TableId:%d %02d:%02d  click x1:%d y1:%d %s 主:%d 从:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, x1, y1, (char*)(LPCTSTR)str, nChipMoneyMain, nChipMoneySub);

	//mouse_move(GetAppClient()->m_rect.left + 10, GetAppClient()->m_rect.top + 3);
	mouse_click(200, 20);
	if (p->m_bDemo)
	{
		g_mutextable.unlock();
		return false;
	}
	Sleep(500);
	int nmoney1 = p->GetAppNameInfo(1);
	for (int i = 0;i < 3;i++)
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
	if (nmoney - nmoney1 == (nChipMoneyMain + nChipMoneySub))
	{
		LOG_FILE("TableId:%d %02d:%02d     下注检测金额成功，  主:%d 从:%d，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, nmoney, nmoney1);
		return true;
	}
	LOG_FILE("TableId:%d %02d:%02d     下注检测金额失败，检测copy  主:%d 从:%d，金额旧:%d 金额新:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub, nmoney, nmoney1);
	Sleep(2000);
	if (p->CheckColin(1, m_TableId, nChipMoneyMain) || GetDesktopCapture()->CheckColin(1, m_TableId, nChipMoneyMain))
	{
		return true;
	}
	return false;
}
void CTableManager::AddHistoryData(std::vector<BoardInfoRequest> requests)
{

	if (requests.size() < 0)
	{
		return;
	}
	m_pmutex->lock();
	m_BoardlistHistory = requests;
	m_pmutex->unlock();
}
std::string CTableManager::GetCurrentBetType()
{
	int nCurrentBetType = 0;
	std::string  nCurrentBetMoney;
	if (m_BoardList.size() == 0)
	{
		return "";
	}
	int nBetResultType = m_BoardList[m_BoardList.size() - 1].BetResultType;
	int nBetType = m_BoardList[m_BoardList.size() - 1].BetType;
	nCurrentBetType =  (nBetResultType);
	std::string strMessage;
	char c = *(method.c_str() + m_nMethodIndex);

	if (c == '1')
	{
		nCurrentBetType = !nCurrentBetType;

	}
	else if (c == '0')
	{
		nCurrentBetType = nCurrentBetType;

	}
	else if (c == '2')
	{
		nCurrentBetType = 0;
	}
	else if (c == '3')
	{
		nCurrentBetType = 1;
	}
	else if (c == '4')
	{
		nCurrentBetType = nBetType;
	}
	else if (c == '5')
	{
		nCurrentBetType = !nBetType;
	}
	if (m_BNeedFan)
	{
		nCurrentBetType = !nCurrentBetType;
		m_nAIPlay = PLAYMAN;
	}
	else
	{
		m_nAIPlay = PLAYNONE;
	}
	bool bSuccess = true;

	if (gAppConfig.bEnableAIPlayLocal&&gAppConfig.nPlayType == PLAYHOME && !m_BNeedFan&&GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end() && GetAppClient()->m_TableRules[m_TableId].Enabled&&m_Rulelist.size() > 0)
	{

		char methodname[20];
		int methodcount = 0;
		memset(methodname, 0, 20);
		MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
		if (m_nUseTimesNumber < rule.UseTimesNumber)
		{
			int maxRuleLen = rule.maxRuleLen;
			int minRuleLen = rule.minRuleLen;
			if (m_Rulelist.size() >= maxRuleLen)
			{
				for (int i = 0;i < maxRuleLen;i++)
				{
					if (m_Rulelist[m_Rulelist.size() - maxRuleLen + i].BetResultType == 0)
					{
						methodname[methodcount++] = '0';
					}
					else
					{
						methodname[methodcount++] = '1';
					}
				}
			}
			else if (m_Rulelist.size() < maxRuleLen&&m_Rulelist.size() >= minRuleLen)
			{
				for (int i = 0;i < m_Rulelist.size();i++)
				{
					if (m_Rulelist[i].BetResultType == 0)
					{
						methodname[methodcount++] = '0';
					}
					else
					{
						methodname[methodcount++] = '1';
					}
				}
			}
			//while (1)
			{
				m_sAIStr = "";
				for (int i = methodcount;i >= rule.minRuleLen;i--)
				{
					if (rule.AIPlayDic.find(methodname + methodcount - i) != rule.AIPlayDic.end())
					{
						int old = nCurrentBetType;
						nCurrentBetType = rule.AIPlayDic[methodname + methodcount - i];
						//LOG_INFO("ai paly %s %d", methodname + methodcount - i, nCurrentBetType);

						//LOG_FILE("TableId:%d %02d:%02d  检测到符合AI规则 %s 原来下注:%d 现在下注:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, methodname + methodcount - i, old, nCurrentBetType);
						m_nAIPlay = PLAYAI;
						m_sAIStr = methodname + methodcount - i;
						break;
					}
				}

			}
		}

	}
	int  nCoinsBet = 0;
	int nCoinsBetSub = 0;
	GetConlin(nCurrentBetType, nCoinsBet, nCoinsBetSub);
	char bufMoney[1024];
	if (nCoinsBetSub > 0)
	{
		sprintf(bufMoney, "%d-%d", nCoinsBet, nCoinsBetSub);
	}
	else
	{
		sprintf(bufMoney, "%d", nCoinsBet);
	}
	char buf[1000];
	if (nCurrentBetType == 0)
	{
		sprintf(buf, "真实 闲-%s %s", bufMoney, GetAIStr(m_nAIPlay));
	}
	else
	{
		sprintf(buf, "真实 庄-%s %s", bufMoney, GetAIStr(m_nAIPlay));
	}
	std::string ret = buf;
	LOG_FILE("TableId:%d %02d:%02d  当前真实下注 %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, buf);
	return ret;
}
bool CTableManager::GetCurrentBetType(int& nCurrentBetType, std::string &nCurrentBetMoney)
{
	if (m_BoardList.size() == 0)
	{
		return false;
	}
	int nBetResultType = m_BoardList[m_BoardList.size() - 1].BetResultType;
	int nBetType = m_BoardList[m_BoardList.size() - 1].BetType;
	nCurrentBetType = CheckIsZhuang(nBetResultType);
	std::string strMessage;
	char c = *(method.c_str() + m_nMethodIndex);

	if (c == '1')
	{
		nCurrentBetType = !nCurrentBetType;

	}
	else if (c == '0')
	{
		nCurrentBetType = nCurrentBetType;

	}
	else if (c == '2')
	{
		nCurrentBetType = 0;
	}
	else if (c == '3')
	{
		nCurrentBetType = 1;
	}
	else if (c == '4')
	{
		nCurrentBetType = nBetType;
	}
	else if (c == '5')
	{
		nCurrentBetType = !nBetType;
	}
	if (m_BNeedFan)
	{
		nCurrentBetType = !nCurrentBetType;
		m_nAIPlay = PLAYMAN;
	}
	else
	{
		m_nAIPlay = PLAYNONE;
	}
	bool bSuccess = true;

	if (gAppConfig.bEnableAIPlayLocal&&gAppConfig.nPlayType == PLAYHOME && !m_BNeedFan&&GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end() && GetAppClient()->m_TableRules[m_TableId].Enabled&&m_Rulelist.size() > 0)
	{

		char methodname[20];
		int methodcount = 0;
		memset(methodname, 0, 20);
		MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
		if (m_nUseTimesNumber < rule.UseTimesNumber)
		{
			int maxRuleLen = rule.maxRuleLen;
			int minRuleLen = rule.minRuleLen;
			if (m_Rulelist.size() >= maxRuleLen)
			{
				for (int i = 0;i < maxRuleLen;i++)
				{
					if (m_Rulelist[m_Rulelist.size() - maxRuleLen + i].BetResultType == 0)
					{
						methodname[methodcount++] = '0';
					}
					else
					{
						methodname[methodcount++] = '1';
					}
				}
			}
			else if (m_Rulelist.size() < maxRuleLen&&m_Rulelist.size() >= minRuleLen)
			{
				for (int i = 0;i < m_Rulelist.size();i++)
				{
					if (m_Rulelist[i].BetResultType == 0)
					{
						methodname[methodcount++] = '0';
					}
					else
					{
						methodname[methodcount++] = '1';
					}
				}
			}
			//while (1)
			{
				m_sAIStr = "";
				for (int i = methodcount;i >= rule.minRuleLen;i--)
				{
					if (rule.AIPlayDic.find(methodname + methodcount - i) != rule.AIPlayDic.end())
					{
						int old = nCurrentBetType;
						nCurrentBetType = rule.AIPlayDic[methodname + methodcount - i];
						LOG_INFO("ai paly %s %d", methodname + methodcount - i, nCurrentBetType);

						LOG_FILE("TableId:%d %02d:%02d  检测到符合AI规则 %s 原来下注:%d 现在下注:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, methodname + methodcount - i, old, nCurrentBetType);
						m_nAIPlay = PLAYAI;
						m_sAIStr = methodname + methodcount - i;
						break;
					}
				}

			}
		}

	}
	int  nCoinsBet = 0;
	int nCoinsBetSub = 0;
	GetConlin(nCurrentBetType, nCoinsBet, nCoinsBetSub);
	char bufMoney[1024];
	if (nCoinsBetSub > 0)
	{
		sprintf(bufMoney, "%d-%d", nCoinsBet, nCoinsBetSub);
	}
	else
	{
		sprintf(bufMoney, "%d", nCoinsBet);
	}
	char buf[1000];
	if (nCurrentBetType == 0)
	{
		sprintf(buf, "真实 闲-%s %s", bufMoney, GetAIStr(m_nAIPlay));
	}
	else
	{
		sprintf(buf, "真实 庄-%s %s", bufMoney, GetAIStr(m_nAIPlay));
	}
	TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
	pTabInfoMsg->nTableId = m_TableId;
	pTabInfoMsg->nextmessage = buf;
	LOG_FILE("TableId:%d %02d:%02d  当前真实下注 %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, buf);
	::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
	return true;
}
void CTableManager::HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight, int& nCountdown)
{

	// 	nCountdown = 0;
	std::string str;
	TabInfoMsg* pTabInfoMsg = new TabInfoMsg;
	pTabInfoMsg->nTableId = m_TableId;
	LOG_FILE("TableId:%d %02d:%02d  将要检测状态", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	m_nStatus = num.nStatus;
	pTabInfoMsg->message = num.strStatus;
	if (m_nStatus == StatusDJS)
	{
		if (m_isCheck)
		{
			return;
		}
		int StartX = m_CountdownRegion.left;
		int StartY = m_CountdownRegion.top;
		int num_width = m_CountdownRegion.right - m_CountdownRegion.left;
		int num_height = m_CountdownRegion.bottom - m_CountdownRegion.top;
		if (CountdownBuf == NULL)
		{
			CountdownBuf = new uint8_t[num_width*num_height * 8];
		}
		libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, CountdownBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
		char  namebuf[256];
		ULONGLONG tick = GetTickCount64();
		sprintf(namebuf, "./2/%d_%I64u_countdown", m_TableId, tick);
		bmp_write(CountdownBuf, num_width, num_height, namebuf);
		std::string result;
		strcat(namebuf, ".bmp");
		doocrcoutdown(namebuf, result);
		remove(namebuf);
		
		if (!(result.size() == 2 && checkisdigit((char*)result.c_str(), result.length()) == 1))
		{
			return;
		}
		m_bNeedResult = true;
		pTabInfoMsg->message = num.strStatus;
		if (m_nCountDown == -1)
		{
			m_nCountDown = time(NULL);
		}

		LOG_FILE("TableId:%d %02d:%02d  当前状态倒计时，开始下注,是否下注:%d 当前算法:%s index:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_isCheck, method.c_str(), m_nMethodIndex);
		if (GetAppClient()->IsMehtodRandom() && m_BoardList.size() < gAppConfig.methodlength)
		{
			return;
		}

		if (m_BoardList.size() > 0)
		{
			if (GetAppClient()->IsMehtodHome() || GetAppClient()->IsMehtodJingYang())
			{
				if (m_nLastSubNumber > 50 && m_nMethodIndex == 0)
				{
					LOG_FILE("TableId:%d %02d:%02d  当前局数>50,不下注", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
					::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
					ResetBet();
					return;
				}

				if (m_bNeedMethod)
				{
					if (time(NULL) - m_GetMethodTime > 60)
					{
						GetAppClient()->UpdateMehtod(GetAppClient()->GetDataSerialStr(m_TableId), gAppConfig.methodlength, m_TableId);
						m_GetMethodTime = time(NULL);
					}

				}
				if (m_nLastSubNumber == 1)
				{
					LOG_FILE("TableId:%d %02d:%02d  当前局数=1,不下注", m_TableId, m_nLastSetNumber, m_nLastSubNumber);
					::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
					ResetBet();
					return;
				}
			}


			if (!m_isCheck&&method.length() > 0)
			{
				if (m_nLastSubNumber == 1)
				{
					std::string oldmethod = method;

					if (!GetAppClient()->IsMehtodHome() && method.length() == 0)
					{

						method = GetAppClient()->GetNewMethod(m_TableId);
						LOG_FILE("TableId:%d %02d:%02d  第一局 更新算法%s to %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, oldmethod.c_str(), method.c_str());
					}

				}
				if (GetDesktopCapture()->m_bDemo)
				{
					m_lastBoardInfoRequest.PlayType = 1;
				}
				else
				{
					m_lastBoardInfoRequest.PlayType = 0;
				}
				m_isCheck = true;
				int nBetResultType = m_BoardList[m_BoardList.size() - 1].BetResultType;
				int nBetType = m_BoardList[m_BoardList.size() - 1].BetType;
				m_nCurrentBetType = CheckIsZhuang(nBetResultType);
				std::string strMessage;
				char c = *(method.c_str() + m_nMethodIndex);

				if (c == '1')
				{
					m_nCurrentBetType = !m_nCurrentBetType;

				}
				else if (c == '0')
				{
					m_nCurrentBetType = m_nCurrentBetType;

				}
				else if (c == '2')
				{
					m_nCurrentBetType = 0;
				}
				else if (c == '3')
				{
					m_nCurrentBetType = 1;
				}
				else if (c == '4')
				{
					m_nCurrentBetType = nBetType;
				}
				else if (c == '5')
				{
					m_nCurrentBetType = !nBetType;
				}
				m_sAIStr = "";
				if (m_BNeedFan)
				{
					LOG_FILE("TableId:%d %02d:%02d  用户主动设置反   原来下注:%d 现在下注:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCurrentBetType, !m_nCurrentBetType);
					m_nCurrentBetType = !m_nCurrentBetType;
					m_nAIPlay = PLAYMAN;
				}
				else
				{
					m_nAIPlay = PLAYNONE;
				}
				bool bSuccess = true;
				m_bXiaZhuSuccess = false;
				if (gAppConfig.bEnableAIPlayLocal&&gAppConfig.nPlayType == PLAYHOME && !m_BNeedFan&&GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end() && GetAppClient()->m_TableRules[m_TableId].Enabled&&m_Rulelist.size() > 0)
				{

					char methodname[20];
					int methodcount = 0;
					memset(methodname, 0, 20);
					MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
					if (m_nUseTimesNumber < rule.UseTimesNumber&&m_nLastSubNumber >= m_Rulelist[m_Rulelist.size() - 1].SubNumber + 1)
					{
						int maxRuleLen = rule.maxRuleLen;
						int minRuleLen = rule.minRuleLen;
						if (m_Rulelist.size() >= maxRuleLen)
						{
							for (int i = 0;i < maxRuleLen;i++)
							{
								if (m_Rulelist[m_Rulelist.size() - maxRuleLen + i].BetResultType == 0)
								{
									methodname[methodcount++] = '0';
								}
								else
								{
									methodname[methodcount++] = '1';
								}
							}
						}
						else if (m_Rulelist.size() < maxRuleLen&&m_Rulelist.size() >= minRuleLen)
						{
							for (int i = 0;i < m_Rulelist.size();i++)
							{
								if (m_Rulelist[i].BetResultType == 0)
								{
									methodname[methodcount++] = '0';
								}
								else
								{
									methodname[methodcount++] = '1';
								}
							}
						}
						//while (1)
						{
							m_sAIStr = "";
							for (int i = methodcount;i >= rule.minRuleLen;i--)
							{
								if (rule.AIPlayDic.find(methodname + methodcount - i) != rule.AIPlayDic.end())
								{
									int old = m_nCurrentBetType;
									m_nCurrentBetType = rule.AIPlayDic[methodname + methodcount - i];
									LOG_INFO("ai paly %s %d", methodname + methodcount - i, m_nCurrentBetType);

									LOG_FILE("TableId:%d %02d:%02d  检测到符合AI规则 %s 原来下注:%d 现在下注:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, methodname + methodcount - i, old, m_nCurrentBetType);
									m_nAIPlay = PLAYAI;
									m_sAIStr = methodname + methodcount - i;
									break;
								}
							}

						}
					}

				}
				if (m_nCurrentBetType)
				{
					strMessage += "1-";
					//strMessage += "庄";
				}
				else
				{
					strMessage += "0-";
					//strMessage += "闲";
				}
				GetConlin(m_nCurrentBetType, m_nCoinsBet, m_nCoinsBetSub);
				LOG_FILE("TableId:%d %02d:%02d  获取当前筹码,当前下注:%d   主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCurrentBetType, m_nCoinsBet, m_nCoinsBetSub);
				if (m_nCoinsBetSub > 0)
				{
					m_lastBoardInfoRequest.MainSub = MAIN_TYPE;
					m_nCoinsBetMain = true;
					if (!GetDesktopCapture()->m_bDemo &&
						(CheckChipRegion(m_nCurrentBetType, buf, nWidth, nHeight)
							|| CheckChipRegion(!m_nCurrentBetType, buf, nWidth, nHeight)))
					{
						m_BNeedFan = false;
						LOG_FILE("TableId:%d %02d:%02d     判断已下注，返回 主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub);
						return;
					}
					else
					{

						

						bSuccess = HandleXiaZhuAll(buf, nWidth, nHeight, m_nCurrentBetType, m_nCoinsBet, m_nCoinsBetSub);
						if (!bSuccess && !GetDesktopCapture()->m_bDemo)
						{
							LOG_FILE("TableId:%d %02d:%02d    下注失败,重新下注   主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub);
							m_isCheck = false;
							CString str;
							if (bSuccess)
							{
								str.Format("%s %d-%d OK %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
							}
							else
							{
								str.Format("%s %d-%d NO %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
							}
							pTabInfoMsg->messageCoinsBet = str;
							pTabInfoMsg->nSetNumber = m_nLastSetNumber;
							pTabInfoMsg->nSubNumber = m_nLastSubNumber;
							::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
							return;
						}
					}
				}
				else
				{


					if (m_nCoinsBet >= gAppConfig.m_SplitMoney&& GetScreenCount() == 2)
					{
						m_bXiaZhuSuccess = false;
						m_nCoinsBetMain = false;
						m_lastBoardInfoRequest.MainSub = SUB_TYPE;
						std::vector<int> betMoneylist;
						CMoneyMgr::GetInstance()->GetResultMoneyList(betMoneylist);
						ScreenInfo* pScreenInfo = GetDesktopCaptureSecond()->GetScreenInfo();
						while (betMoneylist.size() > 0)
						{
							bool bHas = false;
							for (auto v : betMoneylist)
							{
								int money = v;
								if (v == m_nCoinsBet)
								{
									bHas = true;
								}
							}
							if (!bHas)
							{
								break;
							}
							int ntmp = m_nCoinsBet;
							m_nCoinsBet += pScreenInfo->ChipList[1].ChipMoney;
							LOG_FILE("TableId:%d %02d:%02d 当前已下注:%d,新的下注筹码:%d ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, ntmp, m_nCoinsBet);
						}
						bSuccess = GetDesktopCaptureSecond()->HandleCoinsBet(m_TableId, m_nCoinsBet, m_nLastSetNumber, m_nLastSubNumber, m_nCurrentBetType);
						for (int k = 0;k < 1;k++)
						{
							if (!bSuccess && !GetDesktopCapture()->m_bDemo)
							{
								m_isCheck = false;
								CString str;
								if (bSuccess)
								{
									str.Format("%s %d-%d OK %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
								}
								else
								{
									str.Format("%s %d-%d NO %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
								}
								pTabInfoMsg->messageCoinsBet = str;
								pTabInfoMsg->nSetNumber = m_nLastSetNumber;
								pTabInfoMsg->nSubNumber = m_nLastSubNumber;
								::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
								return;
							}

						}
// 						if (m_nAIPlay == PLAYAI)
// 						{
// 							LOG_FILE("TableId:%d %02d:%02d     检测到AIPLAY， 计数加1:%d 主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nUseTimesNumber + 1, m_nCoinsBet, m_nCoinsBetSub);
// 							m_nUseTimesNumber++;
// 						}
					}
					else
					{
						if (!GetDesktopCapture()->m_bDemo&&CheckChipRegion(m_nCurrentBetType, buf, nWidth, nHeight))
						{
							m_BNeedFan = false;
							LOG_FILE("TableId:%d %02d:%02d     判断已下注，返回 主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub);
							return;
						}
						else
						{
							m_nCoinsBetMain = true;
							m_lastBoardInfoRequest.MainSub = MAIN_TYPE;
							//if (!GetDesktopCapture()->m_bDemo)

							{
								
								m_bXiaZhuSuccess = false;
								std::map<int, int> chiplist = GetChipList(m_nCoinsBet);
								bSuccess = HandleXiaZhu(chiplist, m_nCurrentBetType, buf, nWidth, nHeight, m_nCoinsBet);
								for (int k = 0;k < 1;k++)
								{
									if (!bSuccess && !GetDesktopCapture()->m_bDemo)
									{
										LOG_FILE("TableId:%d %02d:%02d    下注失败,重新下注   主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub);
										m_isCheck = false;
										CString str;
										if (bSuccess)
										{
											str.Format("%s %d-%d OK %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
										}
										else
										{
											str.Format("%s %d-%d NO %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
										}
										pTabInfoMsg->messageCoinsBet = str;
										pTabInfoMsg->nSetNumber = m_nLastSetNumber;
										pTabInfoMsg->nSubNumber = m_nLastSubNumber;
										::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);
										return;
									}

								}
							}

						}



					}

				}
				if (!GetDesktopCapture()->m_bDemo)
				{
					if (m_lastBoardInfoRequest.MainSub == SUB_TYPE)
					{
						if (m_nCurrentBetType)
						{
							CMoneyMgr::GetInstance()->SetBetResult(m_TableId, m_lastBoardInfoRequest.CoinsBet*0.95 + m_lastBoardInfoRequest.CoinsBet);
						}
						else
						{
							CMoneyMgr::GetInstance()->SetBetResult(m_TableId, m_lastBoardInfoRequest.CoinsBet * 2);
						}

					}
					else
					{
						CMoneyMgr::GetInstance()->clear(m_TableId);
					}
				}
				if (bSuccess)
				{
					m_bXiaZhuSuccess = true;
					if (m_nAIPlay == PLAYAI)
					{
						LOG_FILE("TableId:%d %02d:%02d     检测到AIPLAY， 计数加1:%d 主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nUseTimesNumber + 1, m_nCoinsBet, m_nCoinsBetSub);
						m_nUseTimesNumber++;
					}
				}
				LOG_FILE("TableId:%d %02d:%02d    下注成功   主金额:%d 从金额:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCoinsBetSub);
				if (m_nMoneyIndex + 1 >= GetAppClient()->m_n - 1 && GetAppClient()->m_n > 0)
				{
					std::string audioname = GetAppStartupPath() + "alarm.wav";
					PlayWav(audioname.c_str());
				}
				m_BNeedFan = false;

				if (m_nMethodIndex < method.length() - 1)
				{

					int nextbets = !m_nCurrentBetType;
					char c1 = *(method.c_str() + m_nMethodIndex + 1);
					if (c1 == '1')
					{
						nextbets = !nextbets;
					}
					else if (c1 == '0')
					{
						nextbets = nextbets;
					}
					else if (c1 == '2')
					{
						nextbets = 0;
					}
					else if (c1 == '3')
					{
						nextbets = 1;
					}
					pTabInfoMsg->nextAIPlay = PLAYNONE;
					if (gAppConfig.bEnableAIPlayLocal&&gAppConfig.nPlayType == PLAYHOME && m_bEnableAI&&GetAppClient()->m_TableRules.find(m_TableId) != GetAppClient()->m_TableRules.end() && GetAppClient()->m_TableRules[m_TableId].Enabled)
					{
						char methodname[20];
						int methodcount = 0;
						memset(methodname, 0, 20);
						MethodRule rule = GetAppClient()->m_TableRules[m_TableId];
						if (m_nUseTimesNumber < rule.UseTimesNumber)
						{
							int maxRuleLen = rule.maxRuleLen;
							int minRuleLen = rule.minRuleLen;
							if (m_Rulelist.size() >= maxRuleLen)
							{
								for (int i = 0;i < maxRuleLen;i++)
								{
									if (m_Rulelist[m_Rulelist.size() - maxRuleLen + i].BetResultType == 0)
									{
										methodname[methodcount++] = '0';
									}
									else
									{
										methodname[methodcount++] = '1';
									}
								}

							}
							else if (m_Rulelist.size() < maxRuleLen&&m_Rulelist.size() >= minRuleLen)
							{
								for (int i = 0;i < m_Rulelist.size();i++)
								{
									if (m_Rulelist[i].BetResultType == 0)
									{
										methodname[methodcount++] = '0';
									}
									else
									{
										methodname[methodcount++] = '1';
									}
								}
							}
							if (nextbets == 0)
							{
								methodname[methodcount++] = '0';
							}
							else
							{
								methodname[methodcount++] = '1';
							}
							//while (1)
							{
								for (int i = methodcount;i >= rule.minRuleLen;i--)
								{
									if (rule.AIPlayDic.find(methodname + methodcount - i) != rule.AIPlayDic.end())
									{
										int oldnextbets = nextbets;
										nextbets = rule.AIPlayDic[methodname + methodcount - i];
										pTabInfoMsg->nextAIPlay = PLAYAI;
										LOG_FILE("TableId:%d %02d:%02d  下把下注检测到符合AI规则 %s 原来下注:%d 现在下注:%d", m_TableId, m_nLastSetNumber, m_nLastSubNumber, methodname + methodcount - i, oldnextbets, nextbets);
										break;
									}
								}

							}
						}

					}

					std::string nnextCoinsBet = GetNextConlin(nextbets, m_nMethodTotalMoney + m_nCoinsBet, m_nMoneyIndex + 1);
					pTabInfoMsg->nextBetType = nextbets;
					pTabInfoMsg->nextCoinsBet = nnextCoinsBet;
					{
						char buf[1000];
						if (nextbets == 0)
						{
							sprintf(buf, "预测 闲-%s %s", nnextCoinsBet.c_str(), GetAIStr(pTabInfoMsg->nextAIPlay));
						}
						else
						{
							sprintf(buf, "预测 庄-%s %s", nnextCoinsBet.c_str(), GetAIStr(pTabInfoMsg->nextAIPlay));
						}
						pTabInfoMsg->nextmessage = buf;
					}
					LOG_FILE("TableId:%d %02d:%02d  下把下注 金额%d-%s  ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, nextbets, nnextCoinsBet.c_str());
				}
				//GetDesktopCapture()->AddCurTotalMoney(m_nCoinsBet*-1);
				//GetDesktopCapture()->AddTotalWaterMoney(m_nCoinsBet);
				CString str;
				if (bSuccess)
				{
					str.Format("%s %d-%d OK %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
				}
				else
				{
					str.Format("%s %d-%d NO %d", (LPCSTR)strMessage.c_str(), m_nCoinsBet, m_nCoinsBetSub, m_nAIPlay);
				}
				pTabInfoMsg->messageCoinsBet = str;
				pTabInfoMsg->nSetNumber = m_nLastSetNumber;
				pTabInfoMsg->nSubNumber = m_nLastSubNumber;
				//LOG_INFO("%d_%d:%d m_isCheck = true m_nCoinsBet:%d IsZhuang:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCurrentBetType, newnamebuf);
				//LOG_TABLE("TableId:%d %02d_%02d m_isCheck = true m_nCoinsBet:%d IsZhuang:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nCoinsBet, m_nCurrentBetType, newnamebuf);

			}
		}



	}
	else
	{
		// 		char newnamebuf[256];
		// 
		// 		if (result == "JSZ")
		// 		{
		// 			 = "结算中";
		// 			m_nStatus = StatusJSZ;
		// 			result = "JSZ";
		// 
		// 		}
		// 		else if (result == "ABCD" || result == "ABC-D")
		// 		{
		// 			pTabInfoMsg->message = "停止投注";
		// 			m_nStatus = StatusTZTZ;
		// 			result = "TZTZ";
		// 		}
		// 		else if (result == "GZ" || result == "LMZ")
		// 		{
		// 			pTabInfoMsg->message = "洗牌中";
		// 			m_nStatus = StatusXPZ;
		// 			result = "XPZ";
		// 		}
		// 		else if (result == "EF" || result == "HNF" || result == "HIKF")
		// 		{
		// 			pTabInfoMsg->message = "等待新局";
		// 			m_nStatus = StatusDDXJ;
		// 			result = "DDXJ";
		// 		}
		pTabInfoMsg->message = num.strStatus;
		m_nCountDown = -1;
	}
	LOG_FILE("TableId:%d %02d:%02d  当前状态:%d %s ", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_nStatus, pTabInfoMsg->message.c_str());
	::PostMessage(m_hwnd, WM_USER_TABLENUM, m_TableId, (LPARAM)pTabInfoMsg);


}
void CTableManager::SetTableNum(int nSetNumber, int nSubNumber)
{
	//m_nLastSetNumber = nSetNumber;
	//m_nLastSubNumber = nSubNumber;
	//m_bUseUser = true;
	LOG_INFO("SetTableNum TableId:%d nSetNumber:%d nSubNumber:%d", m_TableId, nSetNumber, nSubNumber);
}
void CTableManager::HandleTableRegion(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_TableIdRegion.left;
	int StartY = m_TableIdRegion.top;
	int num_width = m_TableIdRegion.right - m_TableIdRegion.left;
	int num_height = m_TableIdRegion.bottom - m_TableIdRegion.top;
	if (TableIdRegionBuf == NULL)
	{
		TableIdRegionBuf = new uint8_t[num_width*num_height * 8];
	}
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, TableIdRegionBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	CString strtime = GetCurrentTime2Str();
	sprintf(namebuf, "./2/%d_%02d%02d_%s_tableID", m_TableId, m_nLastSetNumber, m_nLastSubNumber, (char*)(LPCTSTR)strtime);
	bmp_write(TableIdRegionBuf, num_width, num_height, namebuf);
}
std::string httpheadstart = "<html><body><table width = \"1000\" border = \"1\">\
<tr>\
<th align = \"center\">序号</th>\
<th align = \"center\">桌号</th>\
<th align = \"center\">靴号局号</th>\
<th align = \"center\">下注</th>\
<th align = \"center\">下注金额</th>\
<th align = \"center\">结果</th>\
<th align = \"center\">输赢</th>\
<th align = \"center\">主(0)从(1)下注</th>\
<th align = \"center\">method</th>\
<th align = \"center\">methodindex</th>\
<th align = \"center\">时间</th>\
</tr>";
const char* httpformat = "<tr>\
<td align = \"center\">%d</td>\
<td align = \"center\">%d</td>\
<td align = \"center\">%02d-%02d</td>\
<td align = \"center\">%s-%d-%s</td>\
<td align = \"center\">%d</td>\
<td align = \"center\">%s</td>\
<td align = \"center\">%d</td>\
<td align = \"center\">%d</td>\
<td align = \"center\">%s</td>\
<td align = \"center\">%d</td>\
<td align = \"center\">%s</td>\
<tr>";
const char* httpformat1 = "<tr>\
<td align = \"center\">总计</td>\
<td align = \"center\">0 %d</td>\
<td align = \"center\">1 %d</td>\
<td align = \"center\">2 %d</td>\
<td align = \"center\">%d</td>\
<td></td>\
<td align = \"center\">%d</td>\
<td></td>\
<td></td>\
<td></td>\
<td></td>\
<tr></table></br></br></br></br></body></html>";
std::string CTableManager::GetTableInfo()
{
	std::string str = httpheadstart;
	std::vector<BoardInfoRequest> BoardList;
	g_mutextable.lock();
	BoardList = m_BoardAllList;
	g_mutextable.unlock();
	int nTotalWaterMoney = 0;
	int nTotalProfit = 0;
	char bufhttp[10240];
	int betType0, betType1, betType2;
	betType0 = betType1 = betType2 = 0;
	std::map<int, int> methodlist;
	for (int i = 0;i < 11;i++)
	{
		methodlist[i] = 0;
	}
	for (int i = 0;i < BoardList.size();i++)
	{
		sprintf(bufhttp, httpformat, i + 1, m_TableId,
			BoardList[i].SetNumber, BoardList[i].SubNumber, betType2Str(BoardList[i].BetType).c_str(), BoardList[i].AIPlay, BoardList[i].strAi.c_str(),
			BoardList[i].CoinsBet, betTypelist2Str(BoardList[i].BetResult).c_str(), BoardList[i].Profit, BoardList[i].MainSub,
			BoardList[i].Method.c_str(), BoardList[i].methodIndex, BoardList[i].time.c_str());
		str += bufhttp;
		nTotalWaterMoney += BoardList[i].CoinsBet;
		nTotalProfit += BoardList[i].Profit;
		if (BoardList[i].BetType != 2)
		{
			if (BoardList[i].BetType == 0)
			{
				betType0++;
			}
			else
			{
				betType1++;
			}
			methodlist[BoardList[i].methodIndex] ++;
		}
	}
	sprintf(bufhttp, httpformat1, betType0, betType1, betType2, nTotalWaterMoney, nTotalProfit);
	str += bufhttp;
	return str;
}

void CTableManager::RevertMeothod()
{
	char buftmp[20];
	memset(buftmp, 0, 20);
	for (int i = 0;i < method.length();i++)
	{
		char c = *(method.c_str() + i);
		if (c == '0')
		{
			c = '1';
		}
		else
		{
			c = '0';
		}
		*(buftmp + i) = c;
	}
	method = buftmp;
}
bool CTableManager::HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight)
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
