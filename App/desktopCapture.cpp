#include <stdafx.h>
#include "desktopCapture.h"
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
#include "appclient.h"
//#include "gdiplusheaders.h"
static DesktopCapturer* gDesktopCapturer = NULL;
static DesktopCapturer* gDesktopCapturer1 = NULL;
DesktopCapturer* GetDesktopCapture()
{
	return gDesktopCapturer;
}
DesktopCapturer* GetDesktopCapture1()
{
	return gDesktopCapturer1;
}
DesktopCapturer::DesktopCapturer()
{
	
	gDesktopCapturer1 = this;
	m_pClickCancl = &m_ClickCancleInfoList[0];
	m_pClickCanclSecond = &m_ClickCancleInfoListSecond[0];
	gDesktopCapturer = this;
	m_buf = new uint8_t[2048 * 2048 * 4];
	m_buf1 = new uint8_t[2048 * 2048 * 4];
}
DesktopCapturer::~DesktopCapturer()
{
	Stop();
	delete[] m_buf;
	delete[] m_buf1;
}
extern std::mutex g_mutextable;
void DesktopCapturer::Start()
{
	//mouse_click((m_ScreenInfo.GameModeRegion.left + m_ScreenInfo.GameModeRegion.right) / 2
		//, (m_ScreenInfo.GameModeRegion.bottom + m_ScreenInfo.GameModeRegion.top) / 2);
// 	for (int i = 0; i < m_TableList.size(); i++)
// 	{
// 		m_TableList[i].ResetBet();
// 	}
	ReadAI(m_Ais, m_Moneys);
	ReadAI2(m_AI_Ttoals);
	m_Status = STATUS_START;
	if (m_bStartThread == true)
	{
		return;
	}
	m_bStartThread = true;
	
	std::thread thread([=]
	{
		m_CaptureScreen.ScreenInit();
		RECT rect;
		rect.left = 0;
		rect.right = SCREEN_WIDTH;
		rect.top = 0;
		rect.bottom = SCREEN_HEIGHT;
		m_CaptureScreen.ScreenSetWindow(&rect);
		Capturer();
	});
	thread.detach();
	std::thread thread1([=]
	{
		while (1)
		{
			g_mutextable.lock();
			static int getnumtime = 0;
			if (time(NULL) - getnumtime > 60)
			{
				GetAppNameInfo(0);
				getnumtime = time(NULL);
			}
			g_mutextable.unlock();
			GetNumber();
			
			Sleep(1500);
		}
	
	});
	thread1.detach();
}
void DesktopCapturer::Stop()
{ 
	m_Status = STATUS_STOP;
	
}

bool DesktopCapturer::CheckColin(bool IsMain, int TableId, int Colin)
{
	TableNum num = GetNumber(TableId);
	static int ncount = 1;
	g_mutextable.lock();
	std::string str1;
	if (IsMain)
	{
		for (int i = 0;i<10;i++)
		{
			str1 = GetCopy();
			if (str1.length() > 0)
			{
				break;;
			}
		}	
	}
	else
	{
		for (int i = 0;i < 10;i++)
		{
			str1 = GetCopy(false);
			if (str1.length() > 0)
			{
				break;;
			}
		}
	}

	std::vector<std::string> strlist;
	std::vector<std::string> strlisttmp = splitWithStl(str1, "\r\n");
	for (int j = 0;j < strlisttmp.size();j++)
	{
		std::string strtmp = strlisttmp[j];
		strtmp = replace_all(strtmp, "\r", "");
		strtmp = replace_all(strtmp, "\n", "");
		strtmp = replace_all(strtmp, ",", "");
		strtmp = replace_all(strtmp, "，", "");
		if (strtmp.length() > 0 && strtmp != "重新整理")
		{
			strlist.push_back(strtmp);
		}
	}
	if (strlist.size() > 10)
	{
		bool bfind = false;
		int ncount = 0;
		for (int i = 0;i < 10 ;i++)
		{
			std::string strtmp = strlist[i];
			if (IsHasBJL(strlist[i]))
			{			
				strtmp = strlist[i - 2];
// 				if (IsMain)
// 				{
// 					m_strMoney = (char*)strtmp.c_str();
// 					m_AccountMoney = atoi((char*)strtmp.c_str());
// 					if (m_nStartMainMoney == 0)
// 					{
// 						m_nStartMainMoney = m_AccountMoney;
// 					}
// 
// 				}
// 				else
// 				{
// 					m_strMoneySecond = (char*)strtmp.c_str();
// 					m_AccountMoneySecond = atoi((char*)strtmp.c_str());
// 					if (m_nStartSubMoney == 0)
// 					{
// 						m_nStartSubMoney = m_AccountMoneySecond;
// 					}
// 				}
				int diff = m_AccountMoney + m_AccountMoneySecond - m_nStartMainMoney - m_nStartSubMoney;
				if (!m_bDemo)
				{
					if ((diff >= StopProfit && StopProfit > 0) || (diff <= StopLoss && StopLoss < 0))
					{
						SetStop();
						keybd_event('P', 0, 0, 0);
						keybd_event('P', 0, KEYEVENTF_KEYUP, 0);
						LOG_INFO("SotpLoss:%d or StopProfit:%d Profit%d", StopLoss, StopProfit, diff);
					}
				}
				
				break;
			}
		}
		int nStart = 10;
		for (int i = 10;i<strlist.size();i++)
		{
			std::string strtmp = strlist[i];
			if (IsHasTBJ(strtmp))
			{
				nStart = i;
			}
		}
		if (nStart < 10)
		{
			return false;
		}
		for (int i = nStart;i < strlist.size();i++)
		{
			std::string strtmp = strlist[i];
			if (IsHasBJL(strtmp, TableId))
			{
				bfind = true;
				continue;
			}
			if (bfind)
			{
				if (ncount > 4)
				{
					LOG_FILE("TableId:%d %02d:%02d 是否主屏幕%d 尝试检测copy筹码失败  筹码金额%d copy:%s  ", TableId,num.SetNumber,num.SubNumber, IsMain, Colin,str1.c_str());
					g_mutextable.unlock();
					return false;
				}
				ncount++;
				if (atoi((char*)strtmp.c_str()) == Colin)
				{
					LOG_FILE("TableId:%d %02d:%02d 是否主屏幕%d 检测copy筹码成功 copy金额:%s 筹码金额:%d",TableId, num.SetNumber, num.SubNumber, IsMain, strtmp.c_str(),Colin);
					g_mutextable.unlock();
					return true;
				}
				else if (atoi((char*)strtmp.c_str()) > Colin)
				{
					LOG_FILE("TableId:%d %02d:%02d 是否主屏幕%d 检测copy筹码成功 copy金额:%s  筹码金额%d",TableId, num.SetNumber, num.SubNumber, IsMain, strtmp.c_str(),Colin);
					g_mutextable.unlock();
					return true;
				}
				LOG_FILE("TableId:%d %02d:%02d 是否主屏幕%d 尝试检测copy筹码失败 copy金额:%s  筹码金额%d",TableId, num.SetNumber, num.SubNumber, IsMain, strtmp.c_str(), Colin);
			}
		}
	}
	g_mutextable.unlock();
	LOG_FILE("TableId:%d %02d:%02d 是否主屏幕%d 尝试检测copy筹码失败  筹码金额%d copy:%s  ", TableId, num.SetNumber, num.SubNumber, IsMain, Colin, str1.c_str());
	return false;
}


int DesktopCapturer::GetAppNameInfo(bool IsMain)
{
	int nmoney = 0;
	//g_mutextable.lock();
	std::string str1;
	if (IsMain)
	{
		for (int i = 0;i < 15;i++)
		{
			str1 = GetCopy();
			if (str1.length() > 0)
			{
				break;;
			}
		}



	}
	else
	{
		for (int i = 0;i < 15;i++)
		{
			str1 = GetCopy(false);
			if (str1.length() > 0)
			{
				break;;
			}
		}
	}
	std::string strtmp1 = str1;
	std::vector<std::string> strlist;
	std::vector<std::string> strlisttmp = splitWithStl(strtmp1, "\r\n");
	for (int j = 0;j < strlisttmp.size();j++)
	{
		std::string strtmp = strlisttmp[j];
		strtmp = replace_all(strtmp, "\r", "");
		strtmp = replace_all(strtmp, "\n", "");
		strtmp = replace_all(strtmp, ",", "");
		strtmp = replace_all(strtmp, "，", "");
		if (strtmp.length() > 0&&strtmp != "重新整理")
		{
			strlist.push_back(strtmp);
		}
	}
	if (strlist.size() > 10)
	{
		int i = 0;
		while (i < strlist.size())
		{
			if (IsHasGPBJL(strlist[i]))
			{
				if (IsMain)
				{
					m_AccountName = strlist[i - 2];
					nmoney = m_AccountMoney = atoi(strlist[i - 1].c_str());
				}
				else
				{
					m_strMoneySecond = strlist[i - 2];
					nmoney = m_AccountMoneySecond = atoi(strlist[i - 1].c_str());
				}

				break;
			}
			i++;
		}
	}
	if (nmoney == 0&& str1.length() > 0)
	{
		LOG_FILE("获取的金额为0 %s", str1.c_str());
	}
	return nmoney;
	
}
time_t StringToDatetime(std::string str,std::string& outstr)
 {
	     char *cha = (char*)str.c_str();             // 将string转换成char*。
	     tm tm_;                                    // 定义tm结构体。
	     int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
	     sscanf(cha, "%d/%d/%d   %d:%d", &year, &month, &day, &hour, &minute);// 将string存储的日期时间，转换为int临时变量。
	     tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
	     tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
	     tm_.tm_mday = day;                         // 日。
	     tm_.tm_hour = hour;                        // 时。
	     tm_.tm_min = minute;                       // 分。
	     tm_.tm_sec = 0;                       // 秒。
	     tm_.tm_isdst = 0;                          // 非夏令时。
	     time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
		 char buf[1204];
		 sprintf(buf, "%d-%d-%d %d:%d", year, month, day, hour, minute);
		 outstr = buf;
	     return t_;                                 // 返回值。 
	}
void DesktopCapturer::SetTableInfo(int nTableId, std::vector<std::string>& strlist, int i)
{
	TableNum tableNum;
	tableNum.nTableId = nTableId;
	i++;
	if (strlist[i + 1].find('-') != std::string::npos)
	{
		std::string msg;
		std::vector<std::string> strlist1 = splitWithStl(strlist[i + 1], "-");
		if (strlist1.size() == 2)
		{
			tableNum.SetNumber = atoi(strlist1[0].c_str());
			tableNum.SubNumber = atoi(strlist1[1].c_str());
			m_TableNumberlist[tableNum.nTableId] = tableNum;
		}
		if (strlist[i + 2] == "结算中")
		{
			m_TableNumberlist[tableNum.nTableId].nStatus = StatusJSZ;
			msg = "结算中";
		}
		else if (strlist[i + 2] == "洗牌中")
		{
			m_TableNumberlist[tableNum.nTableId].nStatus = StatusXPZ;
			msg = "洗牌中";
		}
		else if (strlist[i + 2] == "等待新局")
		{
			msg = "等待新局";
			m_TableNumberlist[tableNum.nTableId].nStatus = StatusDDXJ;
		}
		else if (strlist[i + 2] == "停止下注")
		{
			msg = "停止下注";
			m_TableNumberlist[tableNum.nTableId].nStatus = StatusTZTZ;
		}
		else
		{		
			if (strlist[i + 2].length() == 2&&checkisdigit((char*)strlist[i + 2].c_str(), strlist[i + 2].length()))
			{
				m_TableNumberlist[tableNum.nTableId].nStatus = StatusDJS;
				msg = strlist[i + 2];
			}
			if (strlist[i + 2].length() == 2 && checkisdigit((char*)strlist[i + 2].c_str(), strlist[i + 2].length()))
			{
				m_TableNumberlist[tableNum.nTableId].nStatus = StatusDJS;
				msg = strlist[i + 2];
			}
		}
		m_TableNumberlist[tableNum.nTableId].strStatus = msg;
	}
}
void DesktopCapturer::GetNumber()
{
	
	g_mutextable.lock();
	std::string  str;
	for (int i = 0;i<10;i++)
	{
		str = GetCopy();
		if (str.length() > 0)
		{
			break;
		}
	}
	g_mutextable.unlock();
	std::vector<std::string> strlist;
	std::vector<std::string> strlisttmp = splitWithStl(str, "\r\n");
	for (int j = 0;j< strlisttmp.size();j++)
	{
		std::string strtmp = strlisttmp[j];
		strtmp = replace_all(strtmp, "\r", "");
		strtmp = replace_all(strtmp, "\n", "");
		strtmp = replace_all(strtmp, ",", "");
		strtmp = replace_all(strtmp, "，", "");
		if (strtmp.length() > 0 && strtmp != "dealerPicture")
		{
			strlist.push_back(strtmp);
		}
	}
	bool bFind = false;
	if (strlist.size() > 0)
	{
		m_nPlayTime = StringToDatetime(strlist[strlist.size() - 1], m_strPlayTime);
	}
	else
	{
		return;
	}
	m_mutex.lock();
	int i = 5;
	for (i = 5; i<strlist.size()-1;i++)
	{
		
		std::string stablenum = strlist[i] + strlist[i + 1];
		if (stablenum == "百家乐01")
		{
			SetTableInfo(1, strlist, i);
			
		}
		if (stablenum == "百家乐02")
		{
			SetTableInfo(2, strlist, i);
						
		}
		if (stablenum == "百家乐03")
		{
			SetTableInfo(3, strlist, i);
		}
		if (stablenum == "百家乐04")
		{
			SetTableInfo(4, strlist, i);
		}
		if (stablenum == "百家乐05")
		{
			SetTableInfo(5, strlist, i);
		}
		if (stablenum == "百家乐06")
		{
			SetTableInfo(6, strlist, i);
			break;
		}
	}
	while (i< strlist.size())
	{
		if (IsHasGPBJL(strlist[i]))
		{

			m_AccountName = strlist[i - 2];
			m_AccountMoney = atoi(strlist[i - 1].c_str());
			break;
		}
		i++;
	}
	m_mutex.unlock();
}
void DesktopCapturer::Capturer()
{
	int width = m_CaptureScreen.rWidth;
	int height = m_CaptureScreen.rHeight;
	int j = 0;
	if (m_Buflist.size() == 0)
	{
		if(m_AiTYpe == 1)
		{
			for (int j = 0;j < m_NewTableList.size();j++)
			{
				m_Buflist.push_back(NULL);
				m_Buflistx.push_back(NULL);
			}
			for (int j = 0;j<m_NewTableList.size();j++)
			{

			
				std::shared_ptr<std::thread> pthread;
				bool bStart = false;
				pthread.reset(new std::thread([=,&j,&bStart]
				{
					int index = j;
					bStart = true;				
					while (m_Status != STATUS_STOP )
					{
						std::shared_ptr<rbgbuf> bufptr;
						m_mutex.lock();
						if(m_Buflist.size() != 0)
							bufptr = m_Buflist[index];
						m_Buflist[index] = NULL;
						m_mutex.unlock();
						if (bufptr&&!m_NewTableList[index].IsPause())
						{						
							m_NewTableList[index].HandleARGB(bufptr->buf, width, height);
						}
						else
						{
							Sleep(100);						
						}
										
					}
				}
				));
				while (!bStart)
				{
					Sleep(1);
				}
				//pthreadx->detach();
				pthread->detach();
			}
		}
		else{
			for (int j = 0;j < m_New1TableList.size();j++)
			{
				m_Buflist.push_back(NULL);
				m_Buflistx.push_back(NULL);
			}
			for (int j = 0;j<m_New1TableList.size();j++)
			{

			
				std::shared_ptr<std::thread> pthread;
				bool bStart = false;
				pthread.reset(new std::thread([=,&j,&bStart]
				{
					int index = j;
					bStart = true;				
					while (m_Status != STATUS_STOP )
					{
						std::shared_ptr<rbgbuf> bufptr;
						m_mutex.lock();
						if(m_Buflist.size() != 0)
							bufptr = m_Buflist[index];
						m_Buflist[index] = NULL;
						m_mutex.unlock();
						if (bufptr&&!m_New1TableList[index].IsPause())
						{						
							m_New1TableList[index].HandleARGB(bufptr->buf, width, height);
						}
						else
						{
							Sleep(100);						
						}
										
					}
				}
				));
				while (!bStart)
				{
					Sleep(1);
				}
				//pthreadx->detach();
				pthread->detach();
			}
		}
	}
	Sleep(2000);
	while ( m_Status != STATUS_STOP)
	{
		
		Sleep(300);
//		g_mutextable.lock();
// 		mouse_click((m_ScreenInfo.GameModeRegion.left + m_ScreenInfo.GameModeRegion.right) / 2
// 			, (m_ScreenInfo.GameModeRegion.bottom + m_ScreenInfo.GameModeRegion.top) / 2);
//		g_mutextable.unlock();
		if (m_Status == STATUS_PAUSE)
		{
			continue;
		}
		
		//
		uint8_t* newbuf = new uint8_t[width*height * 4];
		m_CaptureScreen.ScreenCopy(newbuf);	
// 		static FILE* file = NULL;
// 		static int nTime = 0;
// 		static uint8_t* gbuf = new uint8_t[width*height * 4];
// 		if (file == NULL)
// 		{
// 			file = fopen("./1.rgb", "ab+");
// 			
// 		}
// 		if (time(NULL) - nTime > 0)
// 		{
// 			fread(gbuf, 1, width*height * 4, file);
// 			nTime = time(NULL);			
// 		}
// 		memcpy(newbuf, gbuf, width*height * 4);
		std::shared_ptr<rbgbuf> bufptr( new rbgbuf);
		bufptr->buf = newbuf;
		
		if (nFlashX == 0)
		{
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
		}
		
 		m_mutex.lock();
		if(m_AiTYpe == 1)
		{
			for (int i = 0;i<m_NewTableList.size();i++)
			{	
				m_Buflist[i] = bufptr;
			}
		}
		else{
			for (int i = 0;i<m_New1TableList.size();i++)
			{
				m_Buflist[i] = bufptr;
			}
		}
 		m_mutex.unlock();
	}
	m_bStartThread = false;
}
void DesktopCapturer::HandleAccountMoneyNamePic(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountMoneyNamePic.left;
	int StartY = m_ScreenInfo.AccountMoneyNamePic.top;
	int crop_width = m_ScreenInfo.AccountMoneyNamePic.right - StartX;
	int crop_height = m_ScreenInfo.AccountMoneyNamePic.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	m_picstr = encoder_pic(m_buf1, crop_width, crop_height);
}
void DesktopCapturer::GetAccountName(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountName.left;
	int StartY = m_ScreenInfo.AccountName.top;
	int crop_width = m_ScreenInfo.AccountName.right - StartX;
	int crop_height = m_ScreenInfo.AccountName.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	sprintf(namebuf, "./2/AccountName%I64u",GetTickCount64());
	bmp_write(m_buf1, crop_width, crop_height, namebuf);
	std::string result;
	strcat(namebuf, ".bmp");
	doocr(namebuf, result);
	m_AccountName= result;
	//LOG_INFO("AccountName %s ", result.c_str());

}
void DesktopCapturer::GetAccountMoney(uint8_t* buf, int nWidth, int nHeight)
{
	int StartX = m_ScreenInfo.AccountMoney.left;
	int StartY = m_ScreenInfo.AccountMoney.top;
	int crop_width = m_ScreenInfo.AccountMoney.right - StartX;
	int crop_height = m_ScreenInfo.AccountMoney.bottom - StartY;
	libyuv::ConvertToARGB(buf, nWidth*nHeight * 3, m_buf1, crop_width * 4, StartX, StartY, nWidth, nHeight, crop_width, crop_height, libyuv::kRotate0, libyuv::FOURCC_24BG);
	char  namebuf[256];
	sprintf(namebuf, "./2/Money%I64u",GetTickCount64());
	bmp_write(m_buf1, crop_width, crop_height, namebuf);
	std::string result;
	strcat(namebuf, ".bmp");
	doocr(namebuf, result);
	m_AccountMoney = atoi(result.c_str());
	//LOG_INFO("Money %s ", result.c_str());
}
void DesktopCapturer::Pause()
{
	m_Status = STATUS_PAUSE;
}
void DesktopCapturer::SetTableNum(int index, int nSetNumber, int nSubNumber)
{	 
	//m_TableList[index].SetTableNum(nSetNumber, nSubNumber);
}


std::string DesktopCapturer::GetTableInfo(int index)
{
	return "";
	//return m_TableList[index - 1].GetTableInfo();
}
void DesktopCapturer::Pause(int index, bool bPause)
{
	if(m_AiTYpe == 1)
	{
		if (m_NewTableList.size() > 0)
		{
			m_NewTableList[index].SetPause(bPause);
		}
	}
	else{
		if (m_New1TableList.size() > 0)
		{
			m_New1TableList[index].SetPause(bPause);
		}
	}
	
	
}

void DesktopCapturer::SetNeedCopy(bool IsMain)
{
	m_mutex.lock();
	if (IsMain)
	{
		m_bNeedCopy = true;
	}
	else
	{
		m_bNeedCopySecond = true;
	}
	m_mutex.unlock();
}
TableNum DesktopCapturer::GetNumber(int nTableId)
{
	TableNum num;
	m_mutex.lock();
	if (m_TableNumberlist.size() >= nTableId)
	{
		num =  m_TableNumberlist[nTableId];
	}
	m_mutex.unlock();
	return num;
}
int DesktopCapturer::CheckFlash(uint8_t* buf, int nWidth, int nHeight)
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
std::string DesktopCapturer::GetNextBetType(int nTableId)
{
// 	if (m_TableList.size() >= nTableId)
// 	{
// 		return m_TableList[nTableId - 1].GetCurrentBetType();
// 	}
	return "";
}
int DesktopCapturer::GetStatus(int nTableId)
{
	TableNum num;
	m_mutex.lock();
	if (m_TableNumberlist.size() >= nTableId)
	{
		num = m_TableNumberlist[nTableId];
	}
	m_mutex.unlock();
	return num.nStatus;
}
void DesktopCapturer::SetBuf(std::shared_ptr<rbgbuf> buf)
{
	if(m_AiTYpe== 1)
	{
		for (int i = 0;i<m_NewTableList.size();i++)
		{	
			m_NewTableList[i].SetBuf(buf);
		}
	}
    else{
		for (int i = 0;i<m_New1TableList.size();i++)
		{	
			m_New1TableList[i].SetBuf(buf);
		}
	}
	
}