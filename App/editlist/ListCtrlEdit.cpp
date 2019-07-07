// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEdit.h"
#include <windowsx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define IDC_EDIT  0X01
//#define PROPERTY_ITEM 0x02
//#define PROPERTY_SUB  0x03
/////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CListCtrlEdit1::CListCtrlEdit1()
{
   m_hWnd = NULL;	
}

CListCtrlEdit1::~CListCtrlEdit1()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEdit1, CEdit)
	//{{AFX_MSG_MAP(CListCtrlEdit)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEdit message handlers





//DEL void CListCtrlEdit::OnSetFocus(CWnd* pOldWnd) 
//DEL {
//DEL 	CEdit::OnSetFocus(pOldWnd);
//DEL     m_bExchange = TRUE;	
//DEL }





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



BOOL CListCtrlEdit1::CreateEx(CWnd *pParent, CListCtrl *pCtrl, DWORD dwStyle, int nID)
{
  if (!Create(dwStyle,CRect(0,0,0,0),pParent, nID)) return FALSE;
  m_pParentList = pCtrl;
  m_pInEdit = this;
  CFont* font = GetParent()->GetFont();
  SetFont(font);
  return TRUE;
}







///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////                ������Ϣ����                                      ///
///////////////////////////////////////////////////////////////////////////

void CListCtrlEdit1::OnDestroy()
 {
 	CEdit::OnDestroy();
 	
	// TODO: Add your message handler code here
   OnDestroyEx();
 	
}
void CListCtrlEdit1::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	OnKillFocusEx();
}

BOOL CListCtrlEdit1::PreTranslateMessage(MSG* pMsg)
{
   if(PreTranslateMessageEx(pMsg))
	   return TRUE;
	
   return CEdit::PreTranslateMessage(pMsg);
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



