/**********************************************/
/* ListCtrlEdit.cpp                           */
/**********************************************/
#include "StdAfx.h"
#include "ListCtrlEdit1.h"

// 大气象：确定运行时对象
IMPLEMENT_DYNAMIC(CListCtrlEdit,CEdit)

// 大气象 2012-11-16 ↓
BEGIN_MESSAGE_MAP(CListCtrlEdit, CEdit)
    ON_WM_KILLFOCUS()// 大气象：与对话框中控件消息映射定义不同。
END_MESSAGE_MAP()
void CListCtrlEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);
    // 大气象：向父窗口发消息
    CWnd* pParent = this->GetParent();
    ::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,0,0);
}
// 大气象 2012-11-16 ↑
CListCtrlEdit::CListCtrlEdit(void)
{
}
CListCtrlEdit::~CListCtrlEdit(void)
{
}