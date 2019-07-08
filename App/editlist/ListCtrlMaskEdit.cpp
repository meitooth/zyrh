// ListCtrlMaskEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlMaskEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlMaskEdit

CListCtrlMaskEdit::CListCtrlMaskEdit()
{
   m_hWnd = NULL;	
   m_pParentList = NULL;
   m_nCurrentItem = -1;
   m_nCurrentSubItem = -1;
   m_bExchange = false;
}

CListCtrlMaskEdit::~CListCtrlMaskEdit()
{
}


BEGIN_MESSAGE_MAP(CListCtrlMaskEdit, CAMSMaskedEdit)
	//{{AFX_MSG_MAP(CListCtrlMaskEdit)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlMaskEdit message handlers


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// CListCtrlMaskEdit message handlers





//DEL void CListCtrlMaskEdit::OnSetFocus(CWnd* pOldWnd) 
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



BOOL CListCtrlMaskEdit::CreateEx(CWnd *pParent, CListCtrl *pCtrl, DWORD dwStyle, int nID)
{
//  if (!Create(dwStyle,CRect(0,0,0,0),pParent,IDC_EDIT)) return FALSE;
  if (!Create(dwStyle,CRect(0,0,0,0),pParent, nID)) return FALSE;
  m_pParentList = pCtrl;
  m_pInEdit = this;
  CFont* font = GetParent()->GetFont();
  SetFont(font);
  return TRUE;
}









BOOL CListCtrlMaskEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
   if(PreTranslateMessageEx(pMsg))
	   return TRUE;
	
	return CAMSMaskedEdit::PreTranslateMessage(pMsg);
}

void CListCtrlMaskEdit::OnDestroy() 
{
	CAMSMaskedEdit::OnDestroy();
	
	// TODO: Add your message handler code here
    OnDestroyEx();
	
}

void CListCtrlMaskEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CAMSMaskedEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
    OnKillFocusEx();	
}
