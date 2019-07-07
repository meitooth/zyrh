/********************************************/
/* ListCtrlEdit.h                           */
/* 编辑框子窗口                              */
/********************************************/
#pragma once
#include "afxwin.h"
// 大气象：自定义编辑结束消息
#define WM_USER_EDIT_END WM_USER + 1001
class CListCtrlEdit : public CEdit
{
    // 大气象：声明动态运行时CRuntimeClass
    DECLARE_DYNAMIC(CListCtrlEdit)
    // 大气象 2012-11-16 ↓
public:
    DECLARE_MESSAGE_MAP()
    // 大气象：控件中消息定义，需要以CWnd为参数
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    // 大气象 2012-11-16 ↑
public:
    CListCtrlEdit(void);
    ~CListCtrlEdit(void);
	CWnd* pParent = NULL;
};