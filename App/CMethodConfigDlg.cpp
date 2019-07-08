// CMethodConfigDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "App.h"
#include "CMethodConfigDlg.h"
#include "afxdialogex.h"
#include "CDialogConfig.h"
#include "afxdialogex.h"
#include "zlib.h"
#include "utils.h"
#include "appclient.h"

// CMethodConfigDlg 对话框

IMPLEMENT_DYNAMIC(CMethodConfigDlg, CDialogEx)

CMethodConfigDlg::CMethodConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_METHOD, pParent)
{

}

CMethodConfigDlg::~CMethodConfigDlg()
{
}

void CMethodConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_METHOD, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CMethodConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CMethodConfigDlg::OnBnClickedBtnSave)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_METHOD, &CMethodConfigDlg::OnNMDblclkListMethod)
END_MESSAGE_MAP()


// CMethodConfigDlg 消息处理程序

bool CheckMethod(CString str)
{
	std::string str1 = str;
	 
	if (str1.length() != gAppConfig.methodlength)
	{
		return false;
	}
	for (int i = 0;i < str1.length();i++)
	{
		if (*(str1.c_str() + i) == '1' || *(str1.c_str() + i) == '0'
			|| *(str1.c_str() + i) == '2' ||*(str1.c_str() + i) == '3')
		{
		}
		else
		{
			return false;
		}
	}
	return true;
}
void CMethodConfigDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string path = GetAppStartupPath() + methodname;;
	for (int i = 0;i<m_nRowCount;i++)
	{	
		CString str = m_listCtrl.GetItemText(i, 2);
		if (!CheckMethod(str)) {
			AfxMessageBox("算法长度或者格式不对");
			return;
		}
	}
	for (int i = 0;i < m_nRowCount;i++)
	{

		CString str = m_listCtrl.GetItemText(i, 2);
		gAppConfig.m_Methodlist[i + 1] = str;
	}
	remove(path.c_str());
	FILE* file = fopen(path.c_str(), "ab+");
	if (file)
	{
		std::string ret = gAppConfig.ToJsonMethod();
		uLongf blen = 20480;
		Byte outbufbuf[20480];
		int err = compress((Byte*)outbufbuf, &blen, (Byte*)ret.c_str(), ret.length());
		fwrite(outbufbuf, 1, blen, file);
		fclose(file);
	}
}
BOOL CMethodConfigDlg::OnInitDialog()
{
	m_ListEdit.CreateEx(this, &m_listCtrl);	
	CDialogEx::OnInitDialog();
	m_listCtrl.InsertColumn(0, _T("选择"), LVCFMT_CENTER | LVCFMT_IMAGE, 60);
	m_listCtrl.InsertColumn(1, _T("桌号"), LVCFMT_CENTER , 100);
	m_listCtrl.InsertColumn(2, _T("经验打法"), LVCFMT_CENTER, 200);
	m_listCtrl.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	for (int i = 0; i < m_nRowCount; ++i)
	{
		m_listCtrl.InsertItem(i, "");
		char buf[256];
		sprintf(buf, "%d", i + 1);
		m_listCtrl.SetItemText(i, 1, buf);
		m_listCtrl.SetCheck(i, 1);
	}
	m_listCtrl.InsertItem(m_nRowCount, "");
	m_listCtrl.SetItemText(m_nRowCount, 1, "算法长度");
	CString strlen;
	strlen.Format("%d", gAppConfig.methodlength);
	m_listCtrl.SetItemText(m_nRowCount, 2, strlen);
	m_ListEdit.Insert(2);
	ReadConfig();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CMethodConfigDlg::UpdateView()
{
	CString strlen;
	strlen.Format("%d", gAppConfig.methodlength);
	m_listCtrl.SetItemText(m_nRowCount, 2, strlen);
	if (gAppConfig.m_Methodlist.size() == 0)
	{
		for (int i = 0; i < m_nRowCount; ++i)
		{	
			m_listCtrl.SetItemText(i, 2, "");
		}
	}

}
void CMethodConfigDlg::OnNMDblclkListMethod(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CListCtrlEditBase::OnOpen(&m_listCtrl, pNMHDR);
	*pResult = 0;
}
void CMethodConfigDlg::ReadConfig()
{
	std::string path = GetAppStartupPath() + methodname;;
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
	gAppConfig.ParseMethod(ret);
	for (int i = 0;i<m_nRowCount;i++)
	{
		if (gAppConfig.m_Methodlist.size() > 0&&gAppConfig.methodlength == gAppConfig.m_Methodlist[i + 1].length())
		{
			m_listCtrl.SetItemText(i, 2, gAppConfig.m_Methodlist[i + 1].c_str());
		}
		
	}
	
}