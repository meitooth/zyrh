/********************************************/
/* EditListCtrl.h                            */
/* 可编辑ListCtrl                            */
/********************************************/
#pragma once
#include "afxcmn.h"
#include "ListCtrlEdit1.h"
// 大气象：定义一个ID
#define IDC_EDIT 1001
class CEditListCtrl :public CListCtrl
{
    // 大气象：声明动态运行时CRuntimeClass
    DECLARE_DYNAMIC(CEditListCtrl)
    // 大气象 2012-11-16 ↓
public:
    int nItem;                // 行
    int nSubItem;            // 列
    CListCtrlEdit m_Edit;
	HWND m_Parent = NULL;
    void ShowEdit(bool bShow, int nItem, int nSubItem, CRect rcCtrl);
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnEditEnd(WPARAM wParam,LPARAM lParam = FALSE);
    // 大气象 2012-11-16 ↑
public:
    CEditListCtrl(void);
    ~CEditListCtrl(void);
};
