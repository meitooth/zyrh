
// AppDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "App.h"
#include "AppDlg.h"
#include "afxdialogex.h"
#include <Shlwapi.h>

#include<direct.h> 
#include "Logger.h"
#include "LogManager.h"
#include "libyuv/libyuv.h"
#include "json/json.h"
#include "desktopCapture.h"
#include "curl/CurlhttpClient.h"
#include "utils.h"
#include "ocr.h"
#include "appclient.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAppDlg 对话框

CAppDlg *pAppDlag = NULL;
int gPageCount = 2;
CAppDlg::CAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pAppDlag = this;
}

void CAppDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TAB_ALL, m_tab);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ALL, &CAppDlg::OnTcnSelchangeTabAll)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAppDlg 消息处理程序
using namespace std;

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
//LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)

{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			// a key (non-system) is pressed.
			if (kbdStruct.vkCode == VK_F1)
			{
				// F1 is pressed!
				//MessageBox(NULL, "F1 is pressed!", "key pressed", MB_ICONINFORMATION);
				cout << "f1 pressed" << endl;

				//return 1;
			}
			pAppDlag->OnKerEvent(kbdStruct.vkCode);
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook. If you create a hack you will not need the callback function
	// in another place then your own code file anyway. Read more about it at MSDN.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0)))
	{
		MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}
void CAppDlg::OnKerEvent(int value)
{
	if (pageType == PageNebu)
	{
		m_DialogNeibu->OnKerEvent(value);
	}
	else if (pageType == PageHome)
	{
		m_DialogHome->OnKerEvent(value);
	}
	else if (pageType == PageTest)
	{
		m_dlgTest->OnKerEvent(value);
	}
	
}
void CAppDlg::InitServerJosn()
{
	std::string filename = GetAppStartupPath() + "server-config.json";
	std::string ret = ReadFile(filename.c_str());
	Json::Reader reader;
	Json::Value value;

	if (reader.parse(ret.c_str(), value))
	{
		if (value["EnableJY"].isInt())
		{
			GetAppClient()->bEnableJY = value["EnableJY"].asInt();
		}
		if (!value["ip"].isNull() && !value["port"].isNull())
		{
			GetAppClient()->m_ip = value["ip"].asCString();
			GetAppClient()->m_nPort = value["port"].asInt();
			GetAppClient()->m_bUseProxy = value["proxy"].asInt();
			GetAppClient()->m_ProxyIp = value["proxyip"].asCString();
			GetAppClient()->m_nProxyPort = value["proxyport"].asInt();
			GetAppClient()->m_n = value["n"].asInt();
		}

	}
}
 
BOOL CAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ReadJyConfig();
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	
	std::string ret;
	

	SetHook();
	
	
// 	m_tab.InsertItem(2, _T("报表"));
// 	m_tab.InsertItem(3, _T("玩法"));
	memset(m_pDialog, 0, sizeof(CDialogEx*) * PAGECOUNT);
	m_DialogConfig.Create(IDD_DIALOG_CONFIG, NULL);
	//InitServerJosn();
	CDialogEx* pDialog = NULL;
	switch (pageType)
	{
	case PageTest:
	{
		gPageCount = 3;
		m_tab.InsertItem(0, _T("主页"));
		m_tab.InsertItem(1, _T("基本配置"));
		m_tab.InsertItem(2, _T("经验打法配置"));
		m_dlgTest = new CDialogTest;
		m_dlgTest->Create(IDD_DIALOG_TEST, NULL);
		m_dlgTest->ShowWindow(SW_HIDE);
		m_dlgTest->SetParent(&m_tab);
		pDialog = m_dlgTest;
		m_MethodConfigDlg.Create(IDD_DIALOG_METHOD, NULL);
		m_MethodConfigDlg.ShowWindow(SW_HIDE);
		m_MethodConfigDlg.SetParent(&m_tab);
		
		SetWindowText("内部版本V1.3 严禁传播");
		break;
	}
	case PageNebu:
	{
		gPageCount = 3;
		m_tab.InsertItem(0, _T("主页"));
		m_tab.InsertItem(1, _T("基本配置"));
		if (GetAppClient()->bEnableJY)
		{
			m_tab.InsertItem(2, _T("经验打法配置"));
		}		
		m_DialogNeibu = new CDialogNeibu;
		m_DialogNeibu->Create(IDD_DIALOG_NEIBU, NULL);
		m_DialogNeibu->ShowWindow(SW_HIDE);
		m_DialogNeibu->SetParent(&m_tab);
		pDialog = m_DialogNeibu;
		m_MethodConfigDlg.Create(IDD_DIALOG_METHOD, NULL);
		m_MethodConfigDlg.ShowWindow(SW_HIDE);
		m_MethodConfigDlg.SetParent(&m_tab);
		
		SetWindowText("内部版本V1.3 严禁传播");
		break;
	}
		
	case PageHome:
	{
		gPageCount = 1;
		m_tab.InsertItem(0, _T("HOME"));
		//m_tab.InsertItem(1, _T("CONFIG"));
		m_DialogHome = new CDialogHome;
		m_DialogHome->Create(IDD_DIALOG_HOME, NULL);
		m_DialogHome->ShowWindow(SW_SHOW);
		m_DialogHome->SetParent(&m_tab);
		pDialog = m_DialogHome;
		
		break;
	}		
	default:
		break;
	}
	

	//m_DialogConfig.ShowWindow(SW_HIDE);
	m_DialogConfig.SetParent(&m_tab);
	
	
	//设定在Tab内显示的范围	
	m_pDialog[0] = pDialog;
	m_pDialog[1] = &m_DialogConfig;
	if (gPageCount == 3)
	{
		m_pDialog[2] = &m_MethodConfigDlg;
	}
	MovePage();
	//SelectPage(m_nCurSel);
	//SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_HIDEWINDOW);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAppDlg::OnTcnSelchangeTabAll(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!GetAppClient()->m_bRegister)
	{
	 
		return;
	}
	// TODO: 在此添加控件通知处理程序代码
	if (m_pDialog[m_nCurSel])
	{
		m_pDialog[m_nCurSel]->ShowWindow(SW_HIDE);
	}
	//得到新的页面索引    
	m_nCurSel = m_tab.GetCurSel();       
	//把新的页面显示出来     
	if (m_pDialog[m_nCurSel])
	{
		m_pDialog[m_nCurSel]->ShowWindow(SW_SHOW);
		if (m_nCurSel == 2)
		{
			m_MethodConfigDlg.UpdateView();
		}
	}
	*pResult = 0;
}
void CAppDlg::MovePage()
{
	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	for (int i = 0;i<gPageCount;i++)
	{
		if (m_pDialog[i] != NULL)
		{
			m_pDialog[i]->MoveWindow(&rc);
		}
	}
}
void CAppDlg::SelectPage(int index)
{
	for (int i = 0;i < gPageCount;i++)
	{
		if (m_pDialog[i] != NULL)
		{
			m_pDialog[i]->ShowWindow(SW_HIDE);
		}
	}
}


void CAppDlg::ReadJyConfig()
{
	std::string filename = GetAppStartupPath() + "jyconfig/jyconfig.json";
	char buf[1024 * 100];
	FILE* file = fopen(filename.c_str(), "ab+");
	if (file)
	{
		int len = fread(buf, 1, 1024 * 100, file);
		fclose(file);
		buf[len] = 0;
	}
	std::string str = buf;
	str = replace_all(str, " ", "");
	str = replace_all(str, "\r", "");
	str = replace_all(str, "\n", "");
	gAppConfig.ParseAiPaly(str);
	
}


void CAppDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//exit(0);
	CDialogEx::OnClose();
}
