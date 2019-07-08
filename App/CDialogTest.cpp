// CDialogTest.cpp: 实现文件
//

#include "stdafx.h"
#include "App.h"
#include "CDialogTest.h"
#include "afxdialogex.h"
#include "utils.h"
#include "Logger.h"
#include "LogManager.h"
#include "libyuv/libyuv.h"
#include "json/json.h"
#include "desktopCapture.h"
#include "curl/CurlhttpClient.h"
#include "utils.h"
#include "ocr.h"
#include "Message.h"
#include <direct.h>
#include "aes256.h"
#include <corecrt_io.h>
#include "appclient.h"
#include <list>
#include "zlib.h"
#include "CDialogConfig.h"
// CDialogTest 对话框

IMPLEMENT_DYNAMIC(CDialogTest, CDialogEx)

CDialogTest::CDialogTest(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TEST, pParent)
{

}

CDialogTest::~CDialogTest()
{
}



void CDialogTest::OnTimer(UINT_PTR nIDEvent)           //定时读取数据
{

	m_btnPause.GetWindowRect(&m_rect);//获取控件的屏幕坐标
									  //ScreenToClient(&m_rect);//转换为对话框上的客户坐标
	GetAppClient()->m_rect = m_rect;
	if (GetAppClient()->MachineCode.length() > 0)
	{
		static bool bset = false;
		if (!bset)
		{
			bset = true;
			m_btnStart.EnableWindow(true);
			this->ShowWindow(SW_SHOW);
		}
		m_listCtrl.SetItemText(m_nRowCount, 2, GetAppClient()->MachineCode.c_str());
	}
	if (GetAppClient()->IsConnect())
	{
		CString str;
		GetDlgItem(IDC_STATIC_STATUS)->GetWindowText(str);
		if (str != "Status:Online")
		{
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("Status:Online");
			GetDlgItem(IDC_STATIC)->InvalidateRect(NULL);
		}
		//GetDlgItem(IDC_STATIC_STATUS)->SetDialogBkColor(RGB(0, 0, 255), RGB(255, 0, 0));
	}
	else
	{
		CString str;
		GetDlgItem(IDC_STATIC_STATUS)->GetWindowText(str);
		if (str != "Status:Offline")
		{
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("Status:Offline");
			GetDlgItem(IDC_STATIC)->InvalidateRect(NULL);
		}
		//GetDlgItem(IDC_STATIC_STATUS)->SetDialogBkColor(RGB(0, 0, 255), RGB(255, 0, 0));
	}
	if (!GetAppClient()->m_bPermissions)
	{
		// 		static bool bpost = false;
		// 		static int exit_time = 0;
		// 		if (!bpost)
		// 		{
		// 			exit_time = time(NULL);
		// 			bpost = true;
		// 			CString str;
		// 			str.Format("鉴权失败 程序退出");
		// 			AfxMessageBox(str);		
		// 			
		// 		}
		// 		if (exit_time != 0&& time(NULL) - exit_time > 3 )
		// 		{
		// 			exit(0);
		// 		}	
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CDialogTest::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_HOME, m_listCtrl);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_CHECK_MODEl, m_isCheck);
	DDX_Control(pDX, IDC_CHECK_USERAND, m_bUserRand);
	DDX_Control(pDX, IDC_LIST_TOTAL, m_ListTotal);
	DDX_Control(pDX, IDC_COMBO_BACK, m_comboxBack);
	CDialogEx::DoDataExchange(pDX);

}
void CDialogTest::OnClose()
{
	CDialogEx::OnClose();
	m_desktopCapturer.Stop();
	m_desktopCapturerSecond.Stop();
}

void CDialogTest::InitChipJosn()
{
	
}
BOOL CDialogTest::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	m_comboxBack.AddString("Both");
	m_comboxBack.AddString("Money");
	m_comboxBack.AddString("Random");
	m_comboxBack.SetCurSel(2);
	//textzlib();
	std::string outstr;
	//doocrtif("../1234/new.font.exp1.tif", outstr,"../1234/new.font.exp1");
	//doocrtifbox("../1234/new.font.exp1.tif", outstr, "../1234/new.font.exp1");
	// 	unsigned char* pngbuf = new unsigned char[300 * 40 * 4];
	// 	memset(pngbuf, 0xaa, 300 * 40 * 4);
	// 	std::string strpic  = encoder_pic(pngbuf, 300, 40);
	// 	decoder_pic(strpic);
	// 	FILE* file = fopen("d:/1.png", "ab+");
	// 	svpng(file, 300, 40, pngbuf, 0);
	// 	bmp_write(pngbuf, 300, 40, "d:/1.bmp");
	// 	fclose(file);
	//png_write(pngbuf, 100, 100, "./1.png");  D:\svn\trunk\duwk\App\Release\9
	std::string result;
	//doocrdigit("../all3/0-44.bmp", result);
	//result = handleResult(result);

	m_btnPause.GetWindowRect(&m_rect);//获取控件的屏幕坐标
	ScreenToClient(&m_rect);//转换为对话框上的客户坐标
	GetAppClient()->m_rect = m_rect;
#if 0
	std::vector<std::string> filelist;
	std::string path = "F:\\zyrh\\trunk\\App\\6017\\";
	findfile((char*)path.c_str(), filelist);
	for (int i = 0;i < filelist.size();i++)
	{

		std::string oldname = path + filelist[i];
		// 		if (0)
		// 		{
		// 			bmp_sacle15(oldname.c_str());
		// 			continue;
		// 		}

		doocrdigit(oldname.c_str(), result);

		result = handleResult(result);

		if (result.length() == 0)
		{
			continue;
		}
		std::string srename = path + result + ".bmp";
		int index = 0;
		while (1)
		{
			char buf[1024];
			if (_access(srename.c_str(), 0) == 0)
			{
				sprintf(buf, "%s%s_%d.bmp", path.c_str(), result.c_str(), index);
				srename = buf;
				index++;
			}
			else
			{
				rename(oldname.c_str(), srename.c_str());
				break;
			}
		}

	}
#endif
	// 	
	GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("Status:Offline");
	//std::string str = aes128encoder((unsigned char*)"{\"Type\":\"Register\",\"SeqNum\":1,\"Body\":{\"DeviceId\":\"003311000000001AA821\",\"LocalIp\":\"10.8.115.12\",\"LocalPort\":0,\"MainSub\":0,\"Version\":\"V1.0\"}}");
	//std::string str1 = aes128decoder((unsigned char*)str.c_str(), str.length());
	CDialogEx::OnInitDialog();
	std::string str = queryMachineGUID();
	m_listCtrl.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
#if 1
	m_listCtrl.InsertColumn(0, _T("选择"), LVCFMT_CENTER | LVCFMT_IMAGE, 45);
	m_listCtrl.InsertColumn(1, _T("桌号"), LVCFMT_CENTER, 55);
	m_listCtrl.InsertColumn(2, _T("局号"), LVCFMT_CENTER, 60);
	m_listCtrl.InsertColumn(3, _T("当前下注"), LVCFMT_CENTER, 100);
	m_listCtrl.InsertColumn(4, _T("下把下注"), LVCFMT_CENTER, 70);
	m_listCtrl.InsertColumn(5, _T("输赢"), LVCFMT_CENTER, 70);
	m_listCtrl.InsertColumn(6, _T("结果"), LVCFMT_CENTER, 70);
	m_listCtrl.InsertColumn(7, _T("状态"), LVCFMT_CENTER, 70);

#else
	m_listCtrl.InsertColumn(0, _T("Select"), LVCFMT_CENTER | LVCFMT_IMAGE, 45);
	m_listCtrl.InsertColumn(1, _T("Table"), LVCFMT_CENTER, 55);
	m_listCtrl.InsertColumn(2, _T("Number"), LVCFMT_CENTER, 60);
	m_listCtrl.InsertColumn(3, _T("Bets"), LVCFMT_CENTER, 120);
	m_listCtrl.InsertColumn(4, _T("NextBets"), LVCFMT_CENTER, 70);
	m_listCtrl.InsertColumn(5, _T("Profit"), LVCFMT_CENTER, 70);
	m_listCtrl.InsertColumn(6, _T("Result"), LVCFMT_CENTER, 70);
	//m_listCtrl.InsertColumn(6, _T("Status"), LVCFMT_CENTER, 70);

#endif
	for (int i = 0; i < m_nRowCount; ++i)
	{
		m_listCtrl.InsertItem(i, "");
		char buf[256];
		sprintf(buf, "%d", i + 1);
		m_listCtrl.SetItemText(i, 1, buf);
		m_listCtrl.SetCheck(i, 1);
	}
	m_listCtrl.InsertItem(m_nRowCount, "");
	m_listCtrl.SetItemText(m_nRowCount, 1, "code");

	m_bUserRand.SetCheck(0);

	m_ListTotal.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
#if 1
	m_ListTotal.InsertColumn(0, _T("桌号"), LVCFMT_CENTER, 100);
	m_ListTotal.InsertColumn(1, _T("流水"), LVCFMT_CENTER, 100);
	m_ListTotal.InsertColumn(2, _T("输赢"), LVCFMT_CENTER, 100);
	m_ListTotal.InsertColumn(3, _T("当前打法"), LVCFMT_CENTER, 160);
#else
	m_ListTotal.InsertColumn(0, _T("Table"), LVCFMT_CENTER, 70);
	m_ListTotal.InsertColumn(1, _T("COMM"), LVCFMT_CENTER, 70);
	m_ListTotal.InsertColumn(2, _T("Total"), LVCFMT_CENTER, 70);
	m_ListTotal.InsertColumn(3, _T("method"), LVCFMT_CENTER, 100);
#endif
	for (int i = 0; i < m_nRowCount; ++i)
	{
		m_ListTotal.InsertItem(i, "");
		char buf[256];
		sprintf(buf, "%d", i + 1);
		m_ListTotal.SetItemText(i, 0, buf);
	}
	m_ListTotal.InsertItem(m_nRowCount, "TotalSum");
	//	m_listCtrl.m_Parent = GetSafeHwnd();

	app_path_ = GetAppStartupPath();
	SetCurrentPath(app_path_);
	CLogManager::GetInstance();
#if 0
	g_logPtr.reset(new Logger);
	g_logPtr1.reset(new Logger(1, "log", "table"));
#endif

	SetCurrentPath(app_path_);
	_mkdir("./1");
	_mkdir("./2");
	InitChipJosn();
	InitTableInfo();
	InitTableInfoSecond();
	m_isCheck.SetCheck(1);
	m_desktopCapturer.Init(m_TableList);
	m_desktopCapturerSecond.Init(m_TableSubList);

	GetAppClient()->Init();

	m_desktopCapturer.m_bDemo = true;
	m_desktopCapturerSecond.m_bDemo = true;
	SetTimer(1, 1000, NULL);
	m_httpServer.Run(7999);
	if (GetScreenCount() == 1)
	{
		//AfxMessageBox("显示器只有一个只能模拟");
#if !SDKTEST
		m_isCheck.SetCheck(1);
		//m_isCheck.EnableWindow(0);
#else
		m_isCheck.SetCheck(1);
		m_bUserRand.SetCheck(1);
#endif
	}
	if (GetScreenCount() > 2)
	{
		AfxMessageBox("显示器大于三个，不支持程序即将退出");
		exit(0);
	}

	m_btnStart.EnableWindow(false);
	ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
LRESULT CDialogTest::OnUserTableNum(WPARAM wParam, LPARAM lParam)
{
	TabInfoMsg* pTabInfoMsg = (TabInfoMsg*)lParam;
	int nSetNumber = pTabInfoMsg->nSetNumber;
	int nSubNumber = pTabInfoMsg->nSubNumber;
	if (nSetNumber > 0 && nSubNumber > 0)
	{
		char buf[1024];
		sprintf(buf, "%02d-%02d", nSetNumber, nSubNumber);
		CString newstr = buf;
		CString newold = m_listCtrl.GetItemText(wParam - 1, 2);
		if (newstr != newold)
		{
			m_listCtrl.SetItemText(wParam - 1, 3, "");
			m_listCtrl.SetItemText(wParam - 1, 4, "");
			m_listCtrl.SetItemText(wParam - 1, 5, "");
			m_listCtrl.SetItemText(wParam - 1, 6, "");
			m_listCtrl.SetItemText(wParam - 1, 7, "");
		}

		m_listCtrl.SetItemText(wParam - 1, 2, newstr);

		if (pTabInfoMsg->Betlist.size() > 0)
		{
			CString str;
			switch (pTabInfoMsg->IsLost)
			{
			case -1:
				str.Format("+", 0);
				break;
			case 0:
				str.Format("+%d", pTabInfoMsg->CoinsBet);
				break;
			case 1:
				str.Format("-%d", pTabInfoMsg->CoinsBet);
				break;
			default:
				break;
			}
			m_listCtrl.SetItemText(wParam - 1, 5, str);
			//类型说明：闲、庄、和、庄对、闲对
			//对应数字：0 、1、  2、  3、  4
			str = "";
			for (int i = 0; i < pTabInfoMsg->Betlist.size(); i++)
			{
				if (pTabInfoMsg->Betlist[i] == 0)
				{
					str += "0 ";
				}
				else if (pTabInfoMsg->Betlist[i] == 1)
				{
					//pTabInfoMsg->CoinsBet = pTabInfoMsg->CoinsBet*0.95;
					str += "1 ";
				}
				else if (pTabInfoMsg->Betlist[i] == 2)
				{
					str += "2 ";
				}
				else if (pTabInfoMsg->Betlist[i] == 3)
				{
					str += "3 ";
				}
				else if (pTabInfoMsg->Betlist[i] == 4)
				{
					str += "4 ";
				}
			}
			m_listCtrl.SetItemText(wParam - 1, 6, str);

			if (m_desktopCapturer.StopProfit > 0 && pTabInfoMsg->nTotalProfitMoney > m_desktopCapturer.StopProfit)
			{
				OnBnClickedBtnPause();
			}
			if (m_desktopCapturer.StopLoss < 0 && pTabInfoMsg->nTotalProfitMoney < m_desktopCapturer.StopLoss)
			{
				OnBnClickedBtnPause();
			}


			str.Format("%d", pTabInfoMsg->nTotalProfitMoney);
			m_ListTotal.SetItemText(wParam - 1, 2, str);
			str.Format("%d", pTabInfoMsg->nTotaCoinsBetMoney);
			m_ListTotal.SetItemText(wParam - 1, 1, str);

		}
	}
	if (pTabInfoMsg->messageCoinsBet.length() > 0)
	{
		std::string str = pTabInfoMsg->messageCoinsBet;
		replace_all(str, "0-", "闲-");
		replace_all(str, "1-", "庄-");
		m_listCtrl.SetItemText(wParam - 1, 3, str.c_str());

		char buf[1024];
		sprintf(buf, "%d-%s", pTabInfoMsg->nextBetType, pTabInfoMsg->nextCoinsBet.c_str());
		str = buf;
		replace_all(str, "0-", "闲-");
		replace_all(str, "1-", "庄-");
		m_listCtrl.SetItemText(wParam - 1, 4, str.c_str());
	}
	if (pTabInfoMsg->message.length() > 0)
	{
		m_listCtrl.SetItemText(wParam - 1, 7, pTabInfoMsg->message.c_str());
	}
	if (pTabInfoMsg->method.length() > 0)
	{
		m_ListTotal.SetItemText(wParam - 1, 3, pTabInfoMsg->method.c_str());
	}
	CString str;
	str.Format("%d", GetDesktopCapture()->GetTotalWaterMoney());
	m_ListTotal.SetItemText(m_nRowCount, 1, str);
	str.Format("%d", GetDesktopCapture()->GetCurTotalMoney());
	m_ListTotal.SetItemText(m_nRowCount, 2, str);
	delete pTabInfoMsg;
	return 0;
}
BEGIN_MESSAGE_MAP(CDialogTest, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_MODEl, &CDialogTest::OnBnClickedCheckModel)
	ON_BN_CLICKED(IDC_BTN_START, &CDialogTest::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDialogTest::OnBnClickedBtnPause)
	ON_MESSAGE(WM_USER_TABLENUM, &CDialogTest::OnUserTableNum)
	ON_MESSAGE(WM_USER_EDIT_END, &CDialogTest::OnEditEnd)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_STATIC_STATUS, &CDialogTest::OnStnClickedStaticStatus)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_HOME, &CDialogTest::OnLvnItemchangedListHome)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHECK_MODEl2, &CDialogTest::OnBnClickedCheckModel2)
END_MESSAGE_MAP()



void CDialogTest::OnBnClickedCheckModel()
{
	// TODO: 在此添加控件通知处理程序代码
	int state = ((CButton *)GetDlgItem(IDC_CHECK_MODEl))->GetCheck();
	//if state == 0，按钮处于未选中状态。
	//if state == 1，按钮处于选中状态。
	if (state == 1)
	{
		m_desktopCapturer.m_bDemo = state;
		m_desktopCapturerSecond.m_bDemo = state;
	}
	else
	{
		m_desktopCapturer.m_bDemo = 0;
		m_desktopCapturerSecond.m_bDemo = 0;
	}

}


void CDialogTest::OnBnClickedBtnStart()
{

	
	m_btnPause.EnableWindow(true);
	m_btnStart.EnableWindow(false);
	CString strFrist;

	CString strSecond;
	if (m_bUserRand.GetCheck() == 1)
	{
		gAppConfig.nPlayType = PLAYRADOM;
		gAppConfig.nBackType = RANDOMBACK;
	}
	else
	{
		gAppConfig.nPlayType = PLAYHOME;
	}
	m_desktopCapturer.m_nFrist = 30;
	m_desktopCapturer.m_nSecond = 1000;
	CString str;
	GetDlgItem(IDC_EDIT_LOSS)->GetWindowText(str);
	m_desktopCapturer.StopLoss = atoi(str);
	if (m_desktopCapturer.StopLoss > 0)
	{
		m_desktopCapturer.StopLoss = m_desktopCapturer.StopLoss*-1;
	}
	GetDlgItem(IDC_EDIT_PROFIT)->GetWindowText(str);
	m_desktopCapturer.StopProfit = atoi(str);
	if (!GetDesktopCapture()->m_bDemo)
	{
		GetDesktopCapture()->GetAppNameInfo(1);
		
	}
	m_desktopCapturer.Start();
	if (GetScreenCount() == 2)
	{
		GetDesktopCapture()->GetAppNameInfo(0);
		m_desktopCapturerSecond.Start();
	}
}


void CDialogTest::OnBnClickedBtnPause()
{
	SetStop();
	GetAppClient()->SendPictureUpload();
	// TODO: 在此添加控件通知处理程序代码
	m_btnPause.EnableWindow(false);
	//m_btnStart.EnableWindow(true);
	m_desktopCapturer.Pause();
	m_desktopCapturerSecond.Pause();
}
std::string CDialogTest::GetAppStartupPath()
{
#if defined(_WIN32)||defined(_WIN64)
	std::string szRootA;
	char szPath[MAX_PATH] = { 0 };
	DWORD dwLen = GetModuleFileNameA(NULL, szPath, MAX_PATH);
	if (dwLen == 0)
		return "";


	char szLongPath[MAX_PATH] = { 0 };
	GetLongPathNameA(szPath, szLongPath, MAX_PATH);

	for (DWORD i = dwLen - 1;i >= 0;--i)
	{
		if (szLongPath[i] == '\\')
		{
			szLongPath[i] = '\0';
			break;
		}
	}

	szRootA = szLongPath;
	szRootA += "\\";

	return szRootA;
#else
	return "";
#endif
}
void CDialogTest::SetCurrentPath(const std::string& path)
{
#if defined(_WIN32)||defined(_WIN64)
	::SetCurrentDirectoryA(path.c_str());
#else

#endif
}
#if 0
#define USEJSONTEST 1
#else
#define USEJSONTEST 0
#endif
void CDialogTest::InitTableInfo()
{
	m_btnPause.EnableWindow(false);

#if USEJSONTEST
	std::string filename = app_path_ + "client-config.json";
	char outbufbuf[1024 * 200];
	std::string ret = ReadFile(filename.c_str());
	uLongf blen = 1024 * 200;
	int err = compress((Byte*)outbufbuf, &blen, (Byte*)ret.c_str(), ret.length());
	std::string datapath = app_path_ + "clientsdk.dll";
	remove(datapath.c_str());
	FILE* file = fopen(datapath.c_str(), "ab+");
	fwrite(outbufbuf, 1, blen, file);
	fclose(file);
#else
	std::string filename = app_path_ + "clientsdk.dll";
	char buf[1024 * 200];
	char outbuf[1024 * 200];
	memset(buf, 0, 1024 * 200);
	memset(outbuf, 0, 1024 * 200);
	FILE* file = fopen(filename.c_str(), "ab+");
	int nReadLen = 0;
	if (file)
	{
		nReadLen = fread(buf, 1, 1024 * 200, file);
		fclose(file);
	}
	uLongf uncomprLen = 1024 * 200;
	uncompress((Byte*)outbuf, &uncomprLen, (const Bytef *)buf, nReadLen);
	std::string ret = outbuf;
#endif

	Json::Reader reader;
	Json::Value value;

	if (reader.parse(ret.c_str(), value))
	{
		InitJsonInfo(value);
		Json::Value JTableArray = value["TableArray"];
		for (int i = 0;i < JTableArray.size();i++)
		{
			CTableManager tableInfo;
			tableInfo.m_TableId = JTableArray[i]["TableId"].asInt();
			tableInfo.m_TableIdRegion = StrToRect(JTableArray[i]["TableIdRegion"].asCString());
			tableInfo.m_NumberRegion = StrToRect(JTableArray[i]["NumberRegion"].asCString());
			tableInfo.m_CountdownRegion = StrToRect(JTableArray[i]["CountdownRegion"].asCString());
			tableInfo.m_TableCancelClick = StrToRect(JTableArray[i]["TableCancelClick"].asCString());
			tableInfo.m_TableCancelRegion = StrToRect(JTableArray[i]["TableCancelRegion"].asCString());

			Json::Value JBetArray = JTableArray[i]["BetArray"];

			for (int j = 0;j < JBetArray.size();j++)
			{
				Bet bet;
				memset(&bet, 0, sizeof(Bet));
				bet.betType = JBetArray[j]["BetType"].asInt();
				bet.m_BetTypeRegion = StrToRect(JBetArray[j]["BetTypeRegion"].asCString());
				if (bet.betType == 0 || bet.betType == 1)
				{
					bet.betInfo.ChipRegion = StrToRect(JBetArray[j]["ChipRegion"].asCString());
					bet.betInfo.AmountBetRegion = StrToRect(JBetArray[j]["AmountBetRegion"].asCString());
					bet.betInfo.Click = StrToPOINT(JBetArray[j]["Click"].asCString());
					bet.betInfo.ClickTypeYes = StrToPOINT(JBetArray[j]["ClickTypeYes"].asCString());
					bet.betInfo.ClickTypeYesRect.left = bet.betInfo.ClickTypeYes.x - 5;
					bet.betInfo.ClickTypeYesRect.right = bet.betInfo.ClickTypeYes.x + 5;
					bet.betInfo.ClickTypeYesRect.top = bet.betInfo.ClickTypeYes.y - 5;
					bet.betInfo.ClickTypeYesRect.bottom = bet.betInfo.ClickTypeYes.y + 5;
					bet.betInfo.ClickTypeNo = StrToPOINT(JBetArray[j]["ClickTypeNo"].asCString());
					bet.betInfo.ClickTypeNoRect.left = bet.betInfo.ClickTypeNo.x - 5;
					bet.betInfo.ClickTypeNoRect.right = bet.betInfo.ClickTypeNo.x + 5;
					bet.betInfo.ClickTypeNoRect.top = bet.betInfo.ClickTypeNo.y - 5;
					bet.betInfo.ClickTypeNoRect.bottom = bet.betInfo.ClickTypeNo.y + 5;
				}
				tableInfo.m_bet[bet.betType] = bet;
				tableInfo.m_hwnd = this->GetSafeHwnd();
			}
			if (m_TableList.size() < m_nRowCount)
			{
				m_TableList.push_back(tableInfo);
			}

			//return;
		}

	}

}
void CDialogTest::InitTableInfoSecond()
{
#if USEJSONTEST
	std::string filename = app_path_ + "client-config.json";
	char outbufbuf[1024 * 200];
	std::string ret = ReadFile(filename.c_str());
	uLongf blen = 1024 * 200;
	int err = compress((Byte*)outbufbuf, &blen, (Byte*)ret.c_str(), ret.length());
	std::string datapath = app_path_ + "clientsdk.dll";
	remove(datapath.c_str());
	FILE* file = fopen(datapath.c_str(), "ab+");
	fwrite(outbufbuf, 1, blen, file);
	fclose(file);
#else
	std::string filename = app_path_ + "clientsdk.dll";
	char buf[1024 * 200];
	char outbuf[1024 * 20];
	memset(buf, 0, 1024 * 200);
	memset(outbuf, 0, 1024 * 20);
	FILE* file = fopen(filename.c_str(), "ab+");
	int nReadLen = 0;
	if (file)
	{
		nReadLen = fread(buf, 1, 1024 * 20, file);
		fclose(file);
	}
	uLongf uncomprLen = 1024 * 20;
	uncompress((Byte*)outbuf, &uncomprLen, (const Bytef *)buf, nReadLen);
	outbuf[uncomprLen] = 0;
	std::string ret = outbuf;
#endif
	Json::Reader reader;
	Json::Value value;

	if (reader.parse(ret.c_str(), value))
	{
		InitJsonInfoSecond(value);
		Json::Value JTableArray = value["TableArray"];
		for (int i = 0;i < JTableArray.size();i++)
		{

			Json::Value JBetArray = JTableArray[i]["BetArray"];

			CTableManagerSub tableInfoSub;
			tableInfoSub.m_TableId = JTableArray[i]["TableId"].asInt();
			tableInfoSub.m_TableIdRegion = StrToRect(JTableArray[i]["TableIdRegion"].asCString());
			tableInfoSub.m_NumberRegion = StrToRect(JTableArray[i]["NumberRegion"].asCString());
			tableInfoSub.m_CountdownRegion = StrToRect(JTableArray[i]["CountdownRegion"].asCString());
			tableInfoSub.m_TableCancelClick = StrToRect(JTableArray[i]["TableCancelClick"].asCString());
			tableInfoSub.m_TableCancelRegion = StrToRect(JTableArray[i]["TableCancelRegion"].asCString());
			for (int j = 0;j < JBetArray.size();j++)
			{
				Bet bet;
				memset(&bet, 0, sizeof(Bet));
				bet.betType = JBetArray[j]["BetType"].asInt();
				bet.m_BetTypeRegion = StrToRect(JBetArray[j]["BetTypeRegion"].asCString());
				if (bet.betType == 0 || bet.betType == 1)
				{
					bet.betInfo.ChipRegion = StrToRect(JBetArray[j]["ChipRegion"].asCString());
					bet.betInfo.AmountBetRegion = StrToRect(JBetArray[j]["AmountBetRegion"].asCString());
					bet.betInfo.Click = StrToPOINT(JBetArray[j]["Click"].asCString());
					bet.betInfo.ClickTypeYes = StrToPOINT(JBetArray[j]["ClickTypeYes"].asCString());
					bet.betInfo.ClickTypeYesRect.left = bet.betInfo.ClickTypeYes.x - 5;
					bet.betInfo.ClickTypeYesRect.right = bet.betInfo.ClickTypeYes.x + 5;
					bet.betInfo.ClickTypeYesRect.top = bet.betInfo.ClickTypeYes.y - 5;
					bet.betInfo.ClickTypeYesRect.bottom = bet.betInfo.ClickTypeYes.y + 5;
					bet.betInfo.ClickTypeNo = StrToPOINT(JBetArray[j]["ClickTypeNo"].asCString());
					bet.betInfo.ClickTypeNoRect.left = bet.betInfo.ClickTypeNo.x - 5;
					bet.betInfo.ClickTypeNoRect.right = bet.betInfo.ClickTypeNo.x + 5;
					bet.betInfo.ClickTypeNoRect.top = bet.betInfo.ClickTypeNo.y - 5;
					bet.betInfo.ClickTypeNoRect.bottom = bet.betInfo.ClickTypeNo.y + 5;
				}
				tableInfoSub.m_bet[bet.betType] = bet;
			}
			if (m_TableSubList.size() < m_nRowCount)
			{
				m_TableSubList.push_back(tableInfoSub);
			}

			//return;
		}

	}

}
void CDialogTest::OnCancel()
{
	OnBnClickedBtnPause();
}
void CDialogTest::OnKerEvent(int KeyValue)
{
	switch (KeyValue)
	{

	case '1':
	{
		int nTableId = 1;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '2':
	{
		int nTableId = 2;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '3':
	{
		int nTableId = 3;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '4':
	{
		int nTableId = 4;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '5':
	{
		int nTableId = 5;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '6':
	{
		int nTableId = 6;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '7':
	{
		int nTableId = 7;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
	}
	case '8':
	{
		int nTableId = 8;
		if (m_btnStart.IsWindowEnabled())
		{
			m_listCtrl.SetCheck(nTableId - 1, !m_listCtrl.GetCheck(nTableId - 1));
		}
		else
		{
			m_listCtrl.SetCheck(nTableId - 1, 0);
		}
		break;
		break;
	}
	case 'p':
	{
		if (m_btnPause.IsWindowEnabled())
		{
			OnBnClickedBtnPause();
		}
		break;
	}
	case 'P':
	{
		if (m_btnPause.IsWindowEnabled())
		{
			OnBnClickedBtnPause();
		}
		break;
	case 'S':
	{
		if (m_btnStart.IsWindowEnabled())
		{
			OnBnClickedBtnStart();
		}
		break;
	}
	case 's':
	{
		if (m_btnStart.IsWindowEnabled())
		{
			OnBnClickedBtnStart();
		}
		break;
	}
	case 'Q':
	case 'q':
	{
		int nTableId = 1;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'W':
	case 'w':
	{
		int nTableId = 2;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'E':
	case 'e':
	{
		int nTableId = 3;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'R':
	case 'r':
	{
		int nTableId = 4;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}

		break;
	}
	case 'T':
	case 't':
	{
		int nTableId = 5;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'Y':
	case 'y':
	{
		int nTableId = 6;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'U':
	case 'u':
	{
		int nTableId = 7;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	case 'I':
	case 'i':
	{
		int nTableId = 8;
		GetDesktopCapture()->SetFan(nTableId);
		CString str;
		str = m_listCtrl.GetItemText(nTableId - 1, 4);
		if (str.GetLength() > 0)
		{
			if (str.Find("1-", 0) > -1)
			{
				str.Replace("1-", "0-");
			}
			else if (str.Find("0-", 0) != -1)
			{
				str.Replace("0-", "1-");
			}
			m_listCtrl.SetItemText(nTableId - 1, 4, str);
		}
		break;
	}
	default:
		break;

	}
	}
}



void CDialogTest::InitJsonInfo(Json::Value& value)
{
	ScreenInfo screenInfo;
	memset(&screenInfo, 0, sizeof(ScreenInfo));
	screenInfo.AccountName = StrToRect(value["AccountName"].asCString());
	screenInfo.AccountMoney = StrToRect(value["AccountMoney"].asCString());
	screenInfo.BetsRange = StrToRect(value["BetsRange"].asCString());
	screenInfo.BetsRangeUp = StrToPOINT(value["BetsRangeUp"].asCString());
	screenInfo.BetsRangeDown = StrToPOINT(value["BetsRangeDown"].asCString());
	screenInfo.GameModeRegion = StrToRect(value["GameModeRegion"].asCString());
	screenInfo.AccountMoneyNamePic = StrToRect(value["AccountMoneyNamePic"].asCString());
	Json::Value JChipArrar = value["ChipArrar"];
	for (int i = 0;i < JChipArrar.size();i++)
	{
		ChipInfo chipInfo;
		chipInfo.ChipNum = JChipArrar[i]["ChipNum"].asInt();
		chipInfo.ChipMoney = JChipArrar[i]["ChipMoney"].asInt();
		chipInfo.ChipNumRegion = StrToRect(JChipArrar[i]["ChipNumRegion"].asCString());
		screenInfo.ChipList[chipInfo.ChipNum] = chipInfo;
	}
	memcpy(&m_desktopCapturer.m_ScreenInfo, &screenInfo, sizeof(ScreenInfo));

}
void CDialogTest::InitJsonInfoSecond(Json::Value& value)
{
	ScreenInfo screenInfo;
	memset(&screenInfo, 0, sizeof(ScreenInfo));
	screenInfo.AccountName = StrToRect(value["AccountName"].asCString());
	screenInfo.AccountMoney = StrToRect(value["AccountMoney"].asCString());
	screenInfo.BetsRange = StrToRect(value["BetsRange"].asCString());
	screenInfo.BetsRangeUp = StrToPOINT(value["BetsRangeUp"].asCString());
	screenInfo.BetsRangeDown = StrToPOINT(value["BetsRangeDown"].asCString());
	screenInfo.GameModeRegion = StrToRect(value["GameModeRegion"].asCString());
	screenInfo.AccountMoneyNamePic = StrToRect(value["AccountMoneyNamePic"].asCString());

	Json::Value JChipArrar = value["ChipArrar2"];
	for (int i = 0;i < JChipArrar.size();i++)
	{
		ChipInfo chipInfo;
		chipInfo.ChipNum = JChipArrar[i]["ChipNum"].asInt();
		chipInfo.ChipMoney = JChipArrar[i]["ChipMoney"].asInt();
		chipInfo.ChipNumRegion = StrToRect(JChipArrar[i]["ChipNumRegion"].asCString());
		screenInfo.ChipList[chipInfo.ChipNum] = chipInfo;
	}
	memcpy(&m_desktopCapturerSecond.m_ScreenInfo, &screenInfo, sizeof(ScreenInfo));

}
LRESULT CDialogTest::OnEditEnd(WPARAM wParam, LPARAM lParam)
{
	CString str = m_listCtrl.GetItemText(wParam, lParam);
	if (str.GetLength() > 0)
	{
		std::vector<std::string> strlist = splitWithStl((LPCSTR)str, "-");
		if (strlist.size() == 2)
		{
			int nSetNumber = atoi(strlist[0].c_str());
			int nSubNumber = atoi(strlist[1].c_str());
			m_desktopCapturer.SetTableNum(wParam, nSetNumber, nSubNumber);
		}
	}
	return 0;
}

void CDialogTest::OnStnClickedStaticStatus()
{
	// TODO: 在此添加控件通知处理程序代码
}



void CDialogTest::OnLvnItemchangedListHome(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMLV->iItem >= 0 && pNMLV->iItem < 9)
	{
		if (m_listCtrl.GetCheck(pNMLV->iItem))
		{
			m_desktopCapturer.Pause(pNMLV->iItem, false);
		}
		else
		{
			m_desktopCapturer.Pause(pNMLV->iItem, true);
		}
	}
	*pResult = 0;
}


HBRUSH CDialogTest::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if (IDC_STATIC_1 == pWnd->GetDlgCtrlID() ||
		IDC_STATIC_2 == pWnd->GetDlgCtrlID() ||
		IDC_STATIC_3 == pWnd->GetDlgCtrlID() ||
		IDC_STATIC_4 == pWnd->GetDlgCtrlID() ||
		IDC_STATIC_5 == pWnd->GetDlgCtrlID() ||
		IDC_STATIC_6 == pWnd->GetDlgCtrlID())
	{
		pDC->SetTextColor(RGB(255, 0, 0));	//红色		
	}
	if (IDC_STATIC_STATUS == pWnd->GetDlgCtrlID())
	{
		CString str;
		GetDlgItem(IDC_STATIC_STATUS)->GetWindowText(str);
		if (str == "Status:Online")
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		else
		{
			pDC->SetTextColor(RGB(0xB5, 0xB5, 0xB5));
		}
	}

	return hbr;
}


void CDialogTest::OnBnClickedCheckModel2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bUserRand.GetCheck() == 1)
	{
		gAppConfig.nPlayType = PLAYJINGYAN;
	}
	else
	{
		gAppConfig.nPlayType = PLAYHOME;
	}
}
BOOL CDialogTest::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		OnKerEvent(pMsg->wParam);
	}
	return __super::PreTranslateMessage(pMsg);
}



// CDialogTest 消息处理程序
