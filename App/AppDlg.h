
// AppDlg.h: 头文件
//

#pragma once
#include "CDialogHome.h"
#include "CDialogConfig.h"
#include "CDialogNeibu.h"
#include "CMethodConfigDlg.h"
#include "CDialogTest.h"
enum PageType
{
	PageNebu = 0,
	PageHome = 1,
	PageTest = 2,
};
#define PAGECOUNT 3
extern int gPageCount;
// CAppDlg 对话框
class CAppDlg : public CDialogEx
{
// 构造
public:
	CAppDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APP_DIALOG };
#endif
	void OnKerEvent(int value);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void ReadJyConfig();
	DECLARE_MESSAGE_MAP()
private:
	CTabCtrl m_tab;
	CDialogEx* m_pDialog[PAGECOUNT];
	CDialogHome* m_DialogHome = NULL;
	CDialogNeibu* m_DialogNeibu = NULL;
	CDialogConfig m_DialogConfig;
	CMethodConfigDlg m_MethodConfigDlg;
	CDialogTest* m_dlgTest = NULL;
	int m_nCurSel = 0;
	int pageType = PageHome;
protected:
	void MovePage();
	void SelectPage(int index);
	void InitServerJosn();
public:
	afx_msg void OnTcnSelchangeTabAll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
};
