// ListCtrlCurrencyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlCurrencyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlCurrencyEdit

CListCtrlCurrencyEdit::CListCtrlCurrencyEdit()
{
   m_hWnd = NULL;	
   m_pParentList = NULL;
   m_nCurrentItem = -1;
   m_nCurrentSubItem = -1;
   m_bExchange = false;
}

CListCtrlCurrencyEdit::~CListCtrlCurrencyEdit()
{
}


BEGIN_MESSAGE_MAP(CListCtrlCurrencyEdit, CAMSCurrencyEdit)
	//{{AFX_MSG_MAP(CListCtrlCurrencyEdit)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlCurrencyEdit message handlers


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// CListCtrlCurrencyEdit message handlers





//DEL void CListCtrlCurrencyEdit::OnSetFocus(CWnd* pOldWnd) 
//DEL {
//DEL 	CEdit::OnSetFocus(pOldWnd);
//DEL     m_bExchange = TRUE;	
//DEL }





////////////////////////////////////////////////////////////////
//进入编辑状态
//IN :  BOOL bShow  是否显示
//		int nItem   行号
//      int nIndex  列号
//      DWORD dwAddStyle 新增类型

//////////////////////////////////////////////////////////////////////
//返回当前行和列
//In: pCtrl 所选的CListCtrl控件
//    pNMHDR 双击时的参数
//nItem 返回的行号
//nSubItem 返回的列号
//DEL BOOL CListCtrlCurrencyEdit::HitTestEx(NMHDR* pNMHDR, int &nItem, int &nSubItem)
//DEL {
//DEL 	ASSERT(m_pParentList);
//DEL     ASSERT(pNMHDR);
//DEL 
//DEL 	int colnum;
//DEL 	CPoint point;
//DEL //	CComboBox *pBox;
//DEL 	CListCtrl *pCtrl = m_pParentList;
//DEL 	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
//DEL 	point = pNMListView->ptAction;
//DEL 	
//DEL 	
//DEL 	
//DEL 	
//DEL 	nItem = pCtrl->HitTest( point, NULL );
//DEL 	nSubItem = 0;
//DEL 	
//DEL 	// 只针对LVS_REPORT样式
//DEL 	if( (GetWindowLong(pCtrl->m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
//DEL 		return TRUE;
//DEL 	
//DEL 	// 获取可见的底顶行
//DEL 	nItem = pCtrl->GetTopIndex();
//DEL 	int bottom = nItem + pCtrl->GetCountPerPage();
//DEL 	if( bottom > pCtrl->GetItemCount() )
//DEL 		bottom = pCtrl->GetItemCount();
//DEL 	
//DEL 	// 获取列数
//DEL 	CHeaderCtrl* pHeader = (CHeaderCtrl*)pCtrl->GetDlgItem(0);
//DEL 	int nColumnCount = pHeader->GetItemCount();
//DEL 	
//DEL 	
//DEL 	// 从可见的行中找
//DEL 	for( ;nItem <=bottom;nItem++)
//DEL 	{
//DEL 		// 获取光标落在的位置的BOUNDS
//DEL 		CRect rect;
//DEL 		pCtrl->GetItemRect( nItem, &rect, LVIR_BOUNDS );
//DEL 		if( rect.PtInRect(point) )
//DEL 		{
//DEL 			// 通过比较宽度，找到列
//DEL 			for( colnum = 0; colnum < nColumnCount; colnum++ )
//DEL 			{
//DEL 				int colwidth = pCtrl->GetColumnWidth(colnum);
//DEL 				if( point.x >= rect.left
//DEL 					&& point.x <= (rect.left + colwidth ) )
//DEL 				{
//DEL 					//if( col ) *col = colnum;
//DEL 					nSubItem = colnum;
//DEL 					return TRUE;
//DEL 				}
//DEL 				rect.left += colwidth;
//DEL 			}
//DEL 		}
//DEL 	}
//DEL 	return FALSE;
//DEL }


BOOL CListCtrlCurrencyEdit::CreateEx(CWnd *pParent, CListCtrl *pCtrl, DWORD dwStyle, int nID)
{
//  if (!Create(dwStyle,CRect(0,0,0,0),pParent,IDC_EDIT)) return FALSE;
  if (!Create(dwStyle,CRect(0,0,0,0),pParent, nID)) return FALSE;
  m_pParentList = pCtrl;
  m_pInEdit = this;
  CFont* font = GetParent()->GetFont();
  SetFont(font);
  return TRUE;
}



//DEL void CListCtrlCurrencyEdit::Insert(int nIndex)
//DEL {
//DEL  m_CtrlMap.Insert(m_pParentList, this, nIndex);
//DEL }


///////////////////////////////////////////////////



BOOL CListCtrlCurrencyEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
   if(PreTranslateMessageEx(pMsg))
	   return TRUE;
	return CAMSCurrencyEdit::PreTranslateMessage(pMsg);
}

void CListCtrlCurrencyEdit::OnDestroy() 
{
	CAMSCurrencyEdit::OnDestroy();
	
	// TODO: Add your message handler code here
    OnDestroyEx();
	
}

void CListCtrlCurrencyEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CAMSCurrencyEdit::OnKillFocus(pNewWnd);
	
 	// TODO: Add your message handler code here
    OnKillFocusEx();	
}
