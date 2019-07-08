#include "stdafx.h"
#include "TableManagerSub.h"
#include "curl/CurlhttpClient.h"
#include "libyuv/libyuv.h"
#include <stdio.h>
#include<string>
#include<vector>
#include "Logger.h"
#include "utils.h"
#include "ocr.h"
#include "Message.h"
#include "desktopCaptureSecond.h"
#include "appclient.h"
#include <vector>
extern std::mutex g_mutextable;
CTableManagerSub::CTableManagerSub()
{
	m_mutex = new std::mutex;
	m_nReportTime = 0;
}
CTableManagerSub::~CTableManagerSub()
{
	if (buf)
	{
		delete[] buf;
	}
}





bool CTableManagerSub::CheckNew(uint8_t* buf, int nWidth, int nHeight)
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
		return false;
	}
	return true;
}
bool CTableManagerSub::CheckGreen(uint8_t* buf, int nWidth, int nHeight)
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
bool CTableManagerSub::CheckX(uint8_t* buf, int nWidth, int nHeight)
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
	//LOG_INFO("value%X per%d", value, per);
	if (per < 50)
	{
		return false;
	}
	return true;
}

void CTableManagerSub::HandleARGB(uint8_t* buf, int nWidth, int nHeight)
{
	//if (time(NULL) - m_CheckCancelDuration > 0)
	{
		//m_CheckCancelDuration = time(NULL);
		m_mutex->lock();
		HandleTableCancelClick(buf, nWidth, nHeight);
		m_mutex->unlock();
	}
//  	HandleBet(buf, nWidth, nHeight);
// 	if (m_nLastSetNumber < 0 || m_nLastSubNumber < 0)
// 	{
// 		return;
// 	}
//  	int nCountdown = 0;
//  	HandleCountdownRegion(buf, nWidth, nHeight, nCountdown);
//  	if (nCountdown > 0)
//  	{
//  
//  	}

}


std::map<int, int> CTableManagerSub::GetChipList(int colin)
{
	std::map<int, int> ret;
	ScreenInfo* pScreenInfo = GetDesktopCaptureSecond()->GetScreenInfo();
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
bool  CTableManagerSub::CheckNo(uint8_t* buf, int nWidth, int nHeight)
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
bool CTableManagerSub::CheckClickNo(uint8_t* buf, int nWidth, int nHeight)
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
bool CTableManagerSub::HandleTableCancelClick()
{
	m_mutex->lock();
	std::shared_ptr<rbgbuf> tmp = m_buf;;
	if (tmp)
	{
		if (HandleTableCancelClick(tmp->buf, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			//m_buf = NULL;
			m_mutex->unlock();
			return true;
		}
		
	}
	else
	{
		//m_mutex->unlock();
		//return true;
	}
	m_mutex->unlock();
	return false;
	
}
bool CTableManagerSub::HandleXiaZhu(std::map<int, int> chiplist, bool IsZhuang, uint8_t* buf, int nWidth, int nHeight, int nConlin)
{
	TableNum num = GetDesktopCapture()->GetNumber(m_TableId);
	m_mutex->lock();
	if (m_buf)
	{
		if (HandleTableCancelClick(m_buf->buf, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			LOG_FILE("TableId:%d %02d:%02d     副屏幕检测到X，返回 主金额:%d  ", m_TableId, num.SetNumber, num.SubNumber, nConlin);
			m_mutex->unlock();
			Sleep(200);
			return false;
		}
		if (CheckClickNo(m_buf->buf, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			LOG_FILE("TableId:%d %02d:%02d     副屏幕检测到no，返回 主金额:%d  ", m_TableId, num.SetNumber, num.SubNumber, nConlin);
			m_mutex->unlock();
			Sleep(200);
			return false;
		}
// 		if (!GetDesktopCapture()->m_bDemo&&CheckChipRegion(IsZhuang, m_buf->buf, SCREEN_WIDTH, SCREEN_HEIGHT))
// 		{
// 			LOG_FILE("TableId:%d %02d:%02d     副屏幕判断已下注，返回 主金额:%d  ", m_TableId, num.SetNumber, num.SubNumber, nConlin);
// 			m_mutex->unlock();
// 			return true;
// 		}
	}	
	m_mutex->unlock();
	ScreenInfo* pScreenInfo = GetDesktopCaptureSecond()->GetScreenInfo();
	std::map<int, int>::iterator it = chiplist.begin();
	bool IsXiazhu = false;
	int x1, y1;
	g_mutextable.lock();
	int nmoney = GetDesktopCapture()->GetAppNameInfo(0);
	if (nmoney == 0) {
		Sleep(1000);
		nmoney = GetDesktopCapture()->GetAppNameInfo(0);
	}
		
	CString str;
	while (it != chiplist.end())
	{
		int x = (pScreenInfo->ChipList[it->first].ChipNumRegion.left + pScreenInfo->ChipList[it->first].ChipNumRegion.right) / 2;
		int y = (pScreenInfo->ChipList[it->first].ChipNumRegion.top + pScreenInfo->ChipList[it->first].ChipNumRegion.bottom) / 2;
		CString tmp;
		tmp.Format("%d:%d  ", it->first, it->second);
		mouse_click_second(x, y);
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
		Sleep(100);
		if (IsXiazhu)
		{
			while (clickcount - 1 > 0)
			{
				clickcount--;
				mouse_click_second(x, y);
				Sleep(50);
			}

		}
		str += tmp;
		it++;
	}
//	mouse_move(10, 20);
// 	mouse_click_second((GetDesktopCapture()->m_ScreenInfo.GameModeRegion.left + GetDesktopCapture()->m_ScreenInfo.GameModeRegion.right) / 2
// 		, (GetDesktopCapture()->m_ScreenInfo.GameModeRegion.bottom + GetDesktopCapture()->m_ScreenInfo.GameModeRegion.top) / 2);
	Sleep(1000);
	if (GetDesktopCapture()->m_bDemo)
	{
// 		Sleep(2000);
// 		if (IsZhuang)
// 		{
// 			x1 = m_bet[1].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[1].betInfo.ClickTypeNo.y;
// 			mouse_click_second(x1, y1);
// 		}
// 		else
// 		{
// 			x1 = m_bet[0].betInfo.ClickTypeNo.x;
// 			y1 = m_bet[0].betInfo.ClickTypeNo.y;
// 			mouse_click_second(x1, y1);
// 		}
	}
	else
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
	LOG_FILE("TableId:%d %02d:%02d   click x1:%d y2:%d %s", m_TableId, num.SetNumber, num.SubNumber, x1, y1, (LPCSTR)str);
	mouse_click_second(200, 10);
	if (GetDesktopCapture()->m_bDemo)
	{
		g_mutextable.unlock();
		return false;
	}
	Sleep(500);
	int nmoney1 = GetDesktopCapture()->GetAppNameInfo(0);
	for (int i = 0;i < 3;i++)
	{
		if (nmoney1 == 0 || nmoney1 == nmoney)
		{
			Sleep(500);
			nmoney1 = GetDesktopCapture()->GetAppNameInfo(0);
		}
		else
		{
			break;
		}
	}	
	g_mutextable.unlock(); 
	
	if (nmoney - nmoney1 == nConlin)
	{
		LOG_FILE("TableId:%d %02d:%02d     副屏幕下注检测金额成功，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, num.SetNumber, num.SubNumber, nConlin, nmoney, nmoney1);
		return true;
	}
	LOG_FILE("TableId:%d %02d:%02d     副屏幕下注检测金额失败，  主金额:%d ，金额旧:%d 金额新:%d", m_TableId, num.SetNumber, num.SubNumber, nConlin, nmoney, nmoney1);
	std::vector<int> nomeylist;
	CMoneyMgr::GetInstance()->GetResultMoneyList(nomeylist);
	if (nmoney1 != nmoney)
	{
		int dif = nmoney1 + nConlin - nmoney;
		if (dif > 0&& dif<nConlin)
		{
			for (auto v : nomeylist)
			{
				if (v == dif)
				{
					return true;
				}
			}
			return true;
		}
		else if (dif < 0)
		{
			for (auto v : nomeylist)
			{
				if (v == dif)
				{
					return true;
				}
			}
		}
	}
	
	Sleep(3000);
	if (GetDesktopCapture()->CheckColin(0, m_TableId, nConlin))
	{
		return true;
	}
	Sleep(2000);
	if (GetDesktopCapture()->CheckColin(0, m_TableId, nConlin))
	{
		return true;
	}
	return false;
}
void CTableManagerSub::HandleNumberRegion(uint8_t* buf, int nWidth, int nHeight, int& nSetNumber, int& nSubNumber)
{
	int StartX = m_NumberRegion.left;
	int StartY = m_NumberRegion.top;
	int num_width = m_NumberRegion.right - m_NumberRegion.left;
	int num_height = m_NumberRegion.bottom - m_NumberRegion.top;
	if (NumBuf == NULL)
	{
		NumBuf = new uint8_t[num_width*num_height * 8];
	}

	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, NumBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	ULONGLONG tick = GetTickCount64();
	sprintf(namebuf, "./1/%d_%I64u", m_TableId, tick);
	bmp_write(NumBuf, num_width, num_height, namebuf);
	std::string result;
	std::string resultdigits;
	strcat(namebuf, ".bmp");
	doocrdigit(namebuf, result);
	result = handleResult(result);
	nSetNumber = -1;
	nSubNumber = -1;
	int nResult = -1;
	if (!checkisdigit((char*)result.c_str(), result.length()))
	{
		
	}
	else
	{


	}
	nResult = atoi(result.c_str());
	std::vector<std::string> numlist = splitWithStl(result, "-");
	if (numlist.size() == 2)
	{
		nSetNumber = atoi(numlist[0].c_str());
		nSubNumber = atoi(numlist[1].c_str());
	}
	else if (numlist.size() == 3)
	{
		nSetNumber = atoi(numlist[0].c_str());
		nSubNumber = atoi(numlist[1].c_str()) * 10 + atoi(numlist[2].c_str());
		char buf[1000];
		sprintf(buf, "%d-%d", nSetNumber, nSubNumber);
		result = buf;
	}
	remove(namebuf);


}
void CTableManagerSub::HandleNumberRegion1(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_NumberRegion.left;
	int StartY = m_NumberRegion.top;
	int num_width = m_NumberRegion.right - m_NumberRegion.left;
	int num_height = m_NumberRegion.bottom - m_NumberRegion.top;

	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, NumBuf, num_width * 4, StartX, StartY, nWidth, nHeight, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	ULONGLONG tick = GetTickCount64();
	sprintf(namebuf, "./1/%d_%I64u", m_TableId, tick);
	bmp_write(NumBuf, num_width, num_height, namebuf);
	std::string result;
	std::string resultdigits;
	strcat(namebuf, ".bmp");
	//doocr(namebuf, result);
	doocrdigit(namebuf, result);
	result = replace_all(result, ",", "-");
	result = replace_all(result, " ", "");
	result = handleResult(result);
	char newnamebuf[256];
	sprintf(newnamebuf, "./1/%d_%s_%I64u.bmp", m_TableId, result.c_str(), tick);
	//sprintf(newnamebuf, "./1/%02d%02d_%I64u.bmp", nSetNumber, nSubNumber, tick);
	rename(namebuf, newnamebuf);

	//LOG_INFO("%s %s %d SetNumber:%d SubNumber:%d", namebuf, result.c_str(), nResult, nSetNumber, nSubNumber);


}
void CTableManagerSub::HandleBet(uint8_t* buf, int nWidth, int nHeight)
{

// 	std::vector<int> BetResultList;
// 	for (int i = 0;i < 5;i++)
// 	{
// 		int StartX = m_bet[i].m_BetTypeRegion.left;
// 		int StartY = m_bet[i].m_BetTypeRegion.top;
// 		int bet_width = m_bet[i].m_BetTypeRegion.right - m_bet[i].m_BetTypeRegion.left;
// 		int bet_height = m_bet[i].m_BetTypeRegion.bottom - m_bet[i].m_BetTypeRegion.top;
// 		if (betBuf == NULL)
// 		{
// 			betBuf = new uint8_t[bet_width*bet_height * 8];
// 		}
// 		libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, betBuf, bet_width * 4, StartX, StartY, nWidth, nHeight, bet_width, bet_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
// 		if (CheckNew(betBuf, bet_width, bet_height))
// 		{
// 			BetResultList.push_back(m_bet[i].betType);
// 		}
// 	}
	int nSetNumber = -1;
	int nSubNumber = -1;

	HandleNumberRegion(buf, nWidth, nHeight, nSetNumber, nSubNumber);
	if (nSetNumber == -1 || nSubNumber == -1)
	{
		return;
	}
	m_nLastSubNumber = nSubNumber;
	m_nLastSetNumber = nSetNumber;
}

void CTableManagerSub::HandleCountdownRegion(uint8_t* buf, int nWidth, int nHeight, int& nCountdown)
{
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
	nCountdown = 0;
	std::string str;
	if (checkisdigit((char*)result.c_str(), result.length()))
	{
		nCountdown = atoi(result.c_str());
		char newnamebuf[256];
		sprintf(newnamebuf, "./2/%d_%I64u_%s.bmp", m_TableId, tick, result.c_str());
		rename(namebuf, newnamebuf);
		m_nStatus = StatusDJS;
		remove(newnamebuf);
// 		if (!m_isCheck)
// 		{
// 
// 			m_isCheck = true;
// 			std::map<int, int> chiplist = GetChipList(m_request.CoinsBet);
// 			HandleXiaZhu(chiplist, m_request.BetType, buf, nWidth, nHeight);
// 			LOG_TABLE("Sub TableId:%d %02d_%02d m_isCheck = true m_nCoinsBet:%d IsZhuang:%d %s", m_TableId, m_nLastSetNumber, m_nLastSubNumber, m_request.CoinsBet, m_request.BetType, newnamebuf);
// 		}
	}
	else
	{
		char newnamebuf[256];

		if (result == "JSZ")
		{

			m_nStatus = StatusJSZ;
			result = "JSZ";

		}
		else if (result == "ABCD" || result == "ABC-D")
		{
			m_nStatus = StatusTZTZ;
			result = "TZTZ";
		}
		else if (result == "GZ" || result == "LMZ")
		{
			m_nStatus = StatusXPZ;
			result = "XPZ";
		}
		else if (result == "EF" || result == "HNF" || result == "HIKF")
		{
			m_nStatus = StatusDDXJ;
			result = "DDXJ";
		}

		sprintf(newnamebuf, "./2/%d_%I64u_%s.bmp", m_TableId, tick, result.c_str());
		rename(namebuf, newnamebuf);
		remove(newnamebuf);
	}

}

void CTableManagerSub::HandleTableRegion(uint8_t* buf, int nWidth, int nHeight)
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
	sprintf(namebuf, "./2/%d_%02d%02d_%s_tableID", m_TableId, m_nLastSetNumber, m_nLastSubNumber, (LPCSTR)strtime);
	bmp_write(TableIdRegionBuf, num_width, num_height, namebuf);
}


bool CTableManagerSub::HandleTableCancelClick(uint8_t* buf, int nWidth, int nHeight)
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
	//char  namebuf[256];
	//ULONGLONG tick = GetTickCount64();
	//sprintf(namebuf, "./2/%d_%I64u_cancle", m_TableId, tick);
	//bmp_write(TableCancelBuf, num_width, num_height, namebuf);
	//std::string result;
	//strcat(namebuf, ".bmp");
	//doocr(namebuf, result);
	if (CheckX(TableCancelBuf, num_width, num_height))
	{
		if (!m_bclickCancel)
		{
			m_bclickCancel = true;
			//g_mutextable.lock();
			mouse_click_second(m_TableCancelClick.left + num_width / 2, m_TableCancelClick.top + num_height / 2);
			mouse_move(0, 10);
			//g_mutextable.unlock();
			m_CheckCancelDuration = time(NULL);
			
		}
		else
		{
			if (time(NULL) - m_CheckCancelDuration > 3)
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

bool CTableManagerSub::HandleCoinsBet(int nCoinsBet,int nSetNumber, int nSubNumber,int nBetType)
{
	std::map<int, int> chiplist = GetChipList(nCoinsBet);
	LOG_FILE("TabldId:%d %02d:%02%d nCoinsBet:%d betType:%d", m_TableId, nSetNumber, nSubNumber, nCoinsBet, nBetType);
	return HandleXiaZhu(chiplist, nBetType, NULL, 0, 0,nCoinsBet);
}
void CTableManagerSub::SetBuf(std::shared_ptr<rbgbuf> buf)
{
	m_mutex->lock();
	m_buf = buf;
	m_mutex->unlock();
}
bool  CTableManagerSub::CheckChipRGB(uint8_t* buf, int nWidth, int nHeight)
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
bool CTableManagerSub::CheckChipRegion(bool iSzhuang, uint8_t* buf, int nWidth, int nHeight)
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