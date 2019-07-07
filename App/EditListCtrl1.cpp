/***********************************************/
/* EditListCtrl.cpp                            */
/***********************************************/
#include "StdAfx.h"
#include "EditListCtrl.h"

// 大气象：确定运行时对象
IMPLEMENT_DYNAMIC(CEditListCtrl,CListCtrl)

// 大气象 2012-11-16 ↓
BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
    ON_WM_LBUTTONDBLCLK() // 可以切换到类视图，右击属性生成。
    ON_MESSAGE(WM_USER_EDIT_END,CEditListCtrl::OnEditEnd)
END_MESSAGE_MAP()

void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // 大气象：取得行列信息的固定方法
    LVHITTESTINFO lvhti;    // 大气象：保存ListCtrl行列信息的结构
    lvhti.pt = point;
    nItem = CListCtrl::SubItemHitTest(&lvhti);
    if (nItem == -1) return;
    nSubItem = lvhti.iSubItem;

    CRect rect;
    CListCtrl::GetSubItemRect(nItem,nSubItem,LVIR_LABEL,rect);

    ShowEdit(TRUE,nItem,nSubItem,rect);

    CListCtrl::OnLButtonDblClk(nFlags, point);
}
void CEditListCtrl::ShowEdit(bool bShow, int nItem, int nSubItem, CRect rcCtrl)
{
	if (nSubItem != 2)
	{
		return;
	}
    if (m_Edit.m_hWnd == NULL)
    {
        m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,CRect(0,0,0,0),this,IDC_EDIT);
        m_Edit.ShowWindow(SW_HIDE);

        CFont tpFont;
        tpFont.CreateStockObject(DEFAULT_GUI_FONT);
        m_Edit.SetFont(&tpFont);
        tpFont.DeleteObject();
    }
    if (bShow == TRUE)
    {
        CString strItem = CListCtrl::GetItemText(nItem,nSubItem);
        rcCtrl.top -= 2;    // 大气象：防止上边框被遮挡
        m_Edit.MoveWindow(&rcCtrl);
        m_Edit.ShowWindow(SW_SHOW);
        m_Edit.SetWindowText(strItem);
        m_Edit.SetFocus();
        m_Edit.SetSel(-1); 
    }
    else
        m_Edit.ShowWindow(SW_HIDE);
}
LRESULT CEditListCtrl::OnEditEnd(WPARAM wParam,LPARAM lParam)
{
    CString strText;
    m_Edit.GetWindowText(strText);
    CListCtrl::SetItemText(nItem,nSubItem,strText);
    m_Edit.ShowWindow(SW_HIDE);
	::PostMessage(m_Parent, WM_USER_EDIT_END, nItem, nSubItem);
    return 0;
}
// 大气象 2012-11-16 ↑
CEditListCtrl::CEditListCtrl(void)
{
}
CEditListCtrl::~CEditListCtrl(void)
{
}