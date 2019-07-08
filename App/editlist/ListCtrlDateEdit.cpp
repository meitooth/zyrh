// ListCtrlDateEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlDateEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDateEdit

CListCtrlDateEdit::CListCtrlDateEdit()
{
   m_hWnd = NULL;	
}

CListCtrlDateEdit::~CListCtrlDateEdit()
{
}
/////////////////////////////////////////////////////////////////////////////






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



BOOL CListCtrlDateEdit::CreateEx(CWnd *pParent, CListCtrl *pCtrl, DWORD dwStyle, int nID)
{
//  if (!Create(dwStyle,CRect(0,0,0,0),pParent,IDC_EDIT)) return FALSE;
  if (!Create(dwStyle,CRect(0,0,0,0),pParent, nID)) return FALSE;
  m_pParentList = pCtrl;
  m_pInEdit = this;
  CFont* font = GetParent()->GetFont();
  SetFont(font);
  return TRUE;
}










/////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CListCtrlDateEdit, CDateEdit)
	//{{AFX_MSG_MAP(CListCtrlDateEdit)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDateEdit message handlers


BOOL CListCtrlDateEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
   if(PreTranslateMessageEx(pMsg))
	   return TRUE;
	return CDateEdit::PreTranslateMessage(pMsg);
}

void CListCtrlDateEdit::OnDestroy() 
{
	CDateEdit::OnDestroy();
	
	// TODO: Add your message handler code here
    OnDestroyEx();
	
}

void CListCtrlDateEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CDateEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
    OnKillFocusEx();	
}
