#include <stdafx.h>
#include "desktopCaptureSecond.h"
#include "libyuv/libyuv.h"
#include <windows.h>
#include <thread>
#include <atlimage.h>
#include<iostream>
#include<thread>
#include <stdio.h>
#include<string>
#include<vector>
#include "Logger.h"
#include "Gdiplus.h"
#include "utils.h"
#include "ocr.h"
#include "cmd.h"

//#include "gdiplusheaders.h"
static DesktopCapturerSecond* gDesktopCapturerSecond= NULL;
DesktopCapturerSecond* GetDesktopCaptureSecond()
{
	return gDesktopCapturerSecond;
}
DesktopCapturerSecond::DesktopCapturerSecond()
{
	gDesktopCapturerSecond = this;
	m_buf = new uint8_t[2048 * 2048 * 4];
	m_buf1 = new uint8_t[2048 * 2048 * 4];
}
DesktopCapturerSecond::~DesktopCapturerSecond()
{
	Stop();
	delete[] m_buf;
	delete[] m_buf1;
}
void DesktopCapturerSecond::Start()
 {
// 	mouse_click_second((m_ScreenInfo.GameModeRegion.left + m_ScreenInfo.GameModeRegion.right) / 2
// 		, (m_ScreenInfo.GameModeRegion.bottom + m_ScreenInfo.GameModeRegion.top) / 2);
	m_Status = STATUS_START;
	if (m_bStartThread == true)
	{
		return;
	}
	m_bStartThread = true;
	
	std::thread thread([=]
	{
		m_CaptureScreen.ScreenInit(false);
		RECT rect;
		rect.left = SCREEN_WIDTH;
		rect.right = SCREEN_WIDTH*2;
		rect.top = 0;
		rect.bottom = m_CaptureScreen.nScreenHeight;
		m_CaptureScreen.ScreenSetWindow(&rect);
		Capturer();
	});
	thread.detach();
	
}
void DesktopCapturerSecond::Stop()
{ 
	m_Status = STATUS_STOP;
	
}

void DesktopCapturerSecond::Capturer()
{
	int width = m_CaptureScreen.rWidth;
	int height = m_CaptureScreen.rHeight;
	int j = 0;
	while ( m_Status != STATUS_STOP)
	{	
		if (m_Status == STATUS_PAUSE)
		{
			continue;
		}
		//
		//mouse_click_second((m_ScreenInfo.GameModeRegion.left + m_ScreenInfo.GameModeRegion.right) / 2
			//, (m_ScreenInfo.GameModeRegion.bottom + m_ScreenInfo.GameModeRegion.top) / 2);
		uint8_t* newbuf = new uint8_t[width*height * 4];
		std::shared_ptr<rbgbuf> bufptr(new rbgbuf);
		bufptr->buf = newbuf;
		m_CaptureScreen.ScreenCopySecond(newbuf);
// 		if (nFlashX == 0)
// 		{
// 			
// 			bufptr->buf = newbuf;
// 			int StartX = 1400;
// 			int StartY = 33;
// 			int num_width = 400;
// 			int num_height = 1;
// 			static uint8_t* flashBuf = NULL;
// 			if (flashBuf == NULL)
// 			{
// 				flashBuf = new uint8_t[num_width*num_height * 8];
// 			}
// 
// 			libyuv::ConvertToARGB(newbuf, width*height * 3, flashBuf, num_width * 4, StartX, StartY, width, height, num_width, num_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
// 			nFlashX = CheckFlash(flashBuf, num_width, num_height);
// 			if (nFlashX != 0)
// 			{
// 				nFlashX += StartX+1;
// 			}
// 		}	
		GetDesktopCapture()->SetBuf(bufptr);
// 		libyuv::ConvertToARGB(m_buf, width*height * 3, m_buf1, width * 4, 0, 0, width, height, width, height, libyuv::kRotate0, libyuv::FOURCC_24BG);
// 		char  namebuf[256];
// 		sprintf(namebuf, "./3/12321%I64u", GetTickCount64());
// 		bmp_write(m_buf1, width, height, namebuf);
// 		if (m_AccountName.length() == 0)
// 		{
// 			GetAccountName(m_buf, width, height);
// 		}
// 		static int gAccountMoneyTime = 0;
// 		if (time(NULL) - gAccountMoneyTime > 2)
// 		{
// 			gAccountMoneyTime = time(NULL);
// 			GetAccountMoney(m_buf, width, height);
// 		}
// 		if (time(NULL) - m_HandlePicTime > 5)
// 		{
// 			HandleAccountMoneyNamePic(m_buf, width, height);
// 			m_HandlePicTime = time(NULL);
// 		}
		

		Sleep(1000);
		
	}
	m_bStartThread = false;
}

void DesktopCapturerSecond::GetAccountName(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountName.left;
	int StartY = m_ScreenInfo.AccountName.top;
	int crop_width = m_ScreenInfo.AccountName.right - StartX;
	int crop_height = m_ScreenInfo.AccountName.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	sprintf(namebuf, "./2/AccountNameSecond%I64u",GetTickCount64());
	bmp_write(m_buf1, crop_width, crop_height, namebuf);
	std::string result;
	strcat(namebuf, ".bmp");
	doocr(namebuf, result);
	m_AccountName= result;
	//LOG_INFO("AccountName %s ", result.c_str());

}
void DesktopCapturerSecond::GetAccountMoney(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountMoney.left;
	int StartY = m_ScreenInfo.AccountMoney.top;
	int crop_width = m_ScreenInfo.AccountMoney.right - StartX;
	int crop_height = m_ScreenInfo.AccountMoney.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	sprintf(namebuf, "./2/MoneySecond%I64u",GetTickCount64());
	bmp_write(m_buf1, crop_width, crop_height, namebuf);
	std::string result;
	strcat(namebuf, ".bmp");
	doocr(namebuf, result);
	m_AccountMoney = atoi(result.c_str());
	//LOG_INFO("Money %s ", result.c_str());
}
void DesktopCapturerSecond::Pause()
{
	m_Status = STATUS_PAUSE;
}
bool DesktopCapturerSecond::HandleCoinsBet(int index, int nCoinsBet, int nSetNumber, int nSubNumber, int nBetType)
{
	return m_TableSubList[index - 1].HandleCoinsBet(nCoinsBet, nSetNumber, nSubNumber, nBetType);
}



void DesktopCapturerSecond::HandleAccountMoneyNamePic(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountMoneyNamePic.left;
	int StartY = m_ScreenInfo.AccountMoneyNamePic.top;
	int crop_width = m_ScreenInfo.AccountMoneyNamePic.right - StartX;
	int crop_height = m_ScreenInfo.AccountMoneyNamePic.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	m_picstr = encoder_pic(m_buf1, crop_width, crop_height);
}
int DesktopCapturerSecond::CheckFlash(uint8_t* buf, int nWidth, int nHeight)
{
	std::map<UINT, int> rgbcount;
	UINT value;
	int maxcount = 0;
	int maxj = 0;
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
			if (tmpvalue == 0xfee2ab)
			{
				maxj = j;
			}

		}
	}

	return maxj;
}