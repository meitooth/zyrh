#pragma once

#include "./editlist/ListCtrlDateEdit.h"
#include "./editlist/ListCtrlEdit.h"
#include "./editlist/ListCtrlComboBox.H"
#include "./editlist/ListCtrlMaskEdit.H"
#include "./editlist/ListCtrlCurrencyEdit.H"
#include "json/json.h"
#include <map>
#include "appclient.h"
// CDialogConfig 对话框

extern AppConifg gAppConfig;


class CDialogConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogConfig)

public:
	CDialogConfig(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogConfig();
	CListCtrl	m_list;
	CListCtrlComboBox m_ListComboBoxMethodCount;
	CListCtrlComboBox m_ListComboBoxNCount;
	CComboBox m_n;
	CComboBox m_ComboxMethodLen;
	CEdit m_EditChip;
	CEdit m_EditMoney;
	int m_nStart = 5;
	int m_nNodidyStart = 3;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONFIG };
#endif
	void ReadConfig();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void  OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	void  OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeComboN();
};
