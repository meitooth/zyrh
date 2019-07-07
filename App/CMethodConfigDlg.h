#pragma once
#include "editlist/ListCtrlEdit.h"

// CMethodConfigDlg 对话框
#define methodname "./method.sdk"
class CMethodConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMethodConfigDlg)

public:
	CMethodConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMethodConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_METHOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSave();
	void UpdateView();
private:
	CListCtrl m_listCtrl;
	CListCtrlEdit1 m_ListEdit;
	int m_nRowCount = 8;
public:
	afx_msg void OnNMDblclkListMethod(NMHDR *pNMHDR, LRESULT *pResult);
	void ReadConfig();
};
