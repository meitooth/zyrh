// CDialogConfig.cpp: 实现文件
//

#include "stdafx.h"
#include "App.h"
#include "CDialogConfig.h"
#include "afxdialogex.h"
#include "zlib.h"
#include "utils.h"
#include "appclient.h"


AppConifg gAppConfig;
// CDialogConfig 对话框

IMPLEMENT_DYNAMIC(CDialogConfig, CDialogEx)

CDialogConfig::CDialogConfig(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONFIG, pParent)
{
	
}
BOOL CDialogConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();
// 	m_list.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
// 	m_ListComboBoxMethodCount.CreateEx(this, &m_list);
// 	m_ListComboBoxMethodCount.AddString("5");
// 	m_ListComboBoxMethodCount.AddString("6");
// 	m_ListComboBoxMethodCount.AddString("7");
// 	m_ListComboBoxMethodCount.AddString("8");
// 	m_ListComboBoxMethodCount.AddString("9");
// 	m_ListComboBoxMethodCount.AddString("10");
// 	m_ListComboBoxMethodCount.AddString("11");
// 	m_ListComboBoxMethodCount.AddString("12");
// 	m_ListComboBoxMethodCount.AddString("13");
// 	m_ListComboBoxMethodCount.AddString("14");
// 	m_ListComboBoxMethodCount.Insert(1);
// 	
// 	m_list.InsertColumn(0, _T("index"), LVCFMT_LEFT, 100);
// 	m_list.InsertColumn(1, _T("methindex"), LVCFMT_LEFT, 100);
// 	for (int i = 0; i < 9; ++i)
// 	{
// 		m_list.InsertItem(i, "");
// 		char buf[256];
// 		sprintf(buf, "%d", i + 1);
// 		m_list.SetItemText(i, 0, buf);
// 	}
	m_n.AddString("3");
	m_n.AddString("4");
	m_n.AddString("5");
	m_n.AddString("6");
	m_n.AddString("7");
	m_n.AddString("8");
	m_n.AddString("9");
	m_n.AddString("10");
	m_n.AddString("11");
	m_n.AddString("12");
	m_n.AddString("13");
	m_n.AddString("14");
	m_n.AddString("15");
	m_n.AddString("16");
	m_n.SetCurSel(0);

	for (int i = m_nStart;i<=20;i++)
	{
		CString str;
		str.Format("%d", i);
		m_ComboxMethodLen.AddString(str);
	}	
	m_ComboxMethodLen.SetCurSel(2);
	ReadConfig();
	return true;
}
CDialogConfig::~CDialogConfig()
{
}

void CDialogConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_CONFIG, m_list);
	DDX_Control(pDX, IDC_COMBO_METHODLEN, m_ComboxMethodLen);
	DDX_Control(pDX, IDC_COMBO_N, m_n);
	DDX_Control(pDX, IDC_EDIT_CHIP, m_EditChip);
	DDX_Control(pDX, IDC_EDIT_MONEY, m_EditMoney);
	
}

//指定第4列COMBOX可以单击打开编辑功能
void CDialogConfig::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	int nItem, nSubItem;
	if (CListCtrlEditBase::HitTestEx(&m_list, pNMHDR, nItem, nSubItem) &&
		nSubItem == 1)
	{
		m_ListComboBoxMethodCount.ShowEdit(nItem, nSubItem);
	}
	if (CListCtrlEditBase::HitTestEx(&m_list, pNMHDR, nItem, nSubItem) &&
		nSubItem == 2)
	{
		m_ListComboBoxNCount.ShowEdit(nItem, nSubItem);
	}
	*pResult = 0;
}
//双击打开编辑功能
void CDialogConfig::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	//自动断判并打开编辑功能
	return;
	CListCtrlEditBase::OnOpen(&m_list, pNMHDR);
	*pResult = 0;
}

BEGIN_MESSAGE_MAP(CDialogConfig, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialogConfig::OnBnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONFIG, OnClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CONFIG, OnDblclkList1)
	ON_CBN_SELCHANGE(IDC_COMBO_N, &CDialogConfig::OnCbnSelchangeComboN)
END_MESSAGE_MAP()


// CDialogConfig 消息处理程序

#define configname "./config.sdk"
void CDialogConfig::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_EditChip.GetWindowText(str);
	gAppConfig.Chip = (char*)(LPCTSTR)str;

	gAppConfig.Chip = replace_all(gAppConfig.Chip, "，", ",");
	gAppConfig.Chip = replace_all(gAppConfig.Chip, " ", "");
	if (gAppConfig.Chip.length() > 0)
	{
		GetAppClient()->m_bEnbaleChip = true;
		std::vector<std::string> strvlist = splitWithStl(gAppConfig.Chip.c_str(), ",");
		GetAppClient()->m_chipList.clear();
		if (GetAppClient()->m_bEnbaleChip)
		{
			for (int i = 0;i < strvlist.size();i++)
			{
				GetAppClient()->m_chipList.push_back((strvlist[i].c_str()));
			}
		}
		CString str;
		m_ComboxMethodLen.GetWindowText(str);
		int tmpmethodlength = atoi((char*)(LPCTSTR)str);
		if (tmpmethodlength != strvlist.size()&& strvlist.size() != 0)
		{
			AfxMessageBox("error methodlen != chipcount");
			return;
		}
	}
	else
	{
		GetAppClient()->m_bEnbaleChip = false;
	}
	
	m_n.GetWindowText(str);
	gAppConfig.n = atoi((char*)(LPCTSTR)str);

	m_EditMoney.GetWindowText(str);
	gAppConfig.m_SplitMoney = atoi((char*)(LPCTSTR)str);



	m_ComboxMethodLen.GetWindowText(str);
	gAppConfig.methodlength = atoi((char*)(LPCTSTR)str);
	if (gAppConfig.m_Methodlist.size() > 0 && gAppConfig.methodlength != gAppConfig.m_Methodlist[0].length())
	{
		gAppConfig.m_Methodlist.clear();
		AfxMessageBox("算法长度改变，清空经验算法");
	}
	remove(configname);
	FILE* file = fopen(configname, "ab+");
	if (file)
	{
		std::string ret = gAppConfig.ToJson();
		uLongf blen = 20480;
		Byte outbufbuf[20480];
		int err = compress((Byte*)outbufbuf, &blen, (Byte*)ret.c_str(), ret.length());
		fwrite(outbufbuf, 1, blen, file);
		fclose(file);
	}
	
	GetAppClient()->m_n = gAppConfig.n;
}
 
void CDialogConfig::ReadConfig()
{
	std::string path = GetAppStartupPath() + configname;;
	char buf[1024 * 200];
	char outbuf[1024 * 200];
	memset(buf, 0, 1024 * 200);
	memset(outbuf, 0, 1024 * 200);
	FILE* file = fopen(path.c_str(), "ab+");
	int nReadLen = 0;
	if (file)
	{
		nReadLen = fread(buf, 1, 1024 * 200, file);
		fclose(file);
	}
	uLongf uncomprLen = 1024 * 200;
	uncompress((Byte*)outbuf, &uncomprLen, (const Bytef *)buf, nReadLen);
	std::string ret = outbuf;
	gAppConfig.Parse(ret);
	CString cstr;
	cstr.Format("%d", gAppConfig.n);
	int index = gAppConfig.n - m_nNodidyStart;
	m_n.SetWindowText(cstr);
	m_n.SetCurSel(index);

	m_EditChip.SetWindowText(gAppConfig.Chip.c_str());
	cstr.Format("%d", gAppConfig.methodlength);
	index = gAppConfig.methodlength - m_nStart;
	m_ComboxMethodLen.SetCurSel(index );
 
	cstr.Format("%d", gAppConfig.m_SplitMoney);
	m_EditMoney.SetWindowText(cstr);
	if (gAppConfig.Chip.length() > 0)
	{
		GetAppClient()->m_bEnbaleChip = true;
		std::vector<std::string> strvlist = splitWithStl(gAppConfig.Chip.c_str(), ",");
		GetAppClient()->m_chipList.clear();
		if (GetAppClient()->m_bEnbaleChip)
		{
			for (int i = 0;i < strvlist.size();i++)
			{
				GetAppClient()->m_chipList.push_back((strvlist[i].c_str()));
			}
		}
	}
	else
	{
		GetAppClient()->m_bEnbaleChip = false;
	}
	GetAppClient()->m_n = gAppConfig.n;
}



void CDialogConfig::OnCbnSelchangeComboN()
{
	// TODO: 在此添加控件通知处理程序代码
}
