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
//����༭״̬
//IN :  BOOL bShow  �Ƿ���ʾ
//		int nItem   �к�
//      int nIndex  �к�
//      DWORD dwAddStyle ��������

//////////////////////////////////////////////////////////////////////
//���ص�ǰ�к���
//In: pCtrl ��ѡ��CListCtrl�ؼ�
//    pNMHDR ˫��ʱ�Ĳ���
//nItem ���ص��к�
//nSubItem ���ص��к�



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
