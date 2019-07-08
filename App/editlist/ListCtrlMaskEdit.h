#if !defined(AFX_LISTCTRLMASKEDIT_H__E54664CB_ADF9_4E9D_927F_EB891AFCA189__INCLUDED_)
#define AFX_LISTCTRLMASKEDIT_H__E54664CB_ADF9_4E9D_927F_EB891AFCA189__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlMaskEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlMaskEdit window
#include "ListCtrlEditBase.H"
#include "amsEdit.H"


class CListCtrlMaskEdit : public CAMSMaskedEdit, public CListCtrlEditBase
{
// Construction
public:
	BOOL CreateEx(CWnd *pParent, CListCtrl *pCtrl ,DWORD dwStyle = EA_DEFAULT, int nID = -1);
public:
	CListCtrlMaskEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlMaskEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlMaskEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlMaskEdit)
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLMASKEDIT_H__E54664CB_ADF9_4E9D_927F_EB891AFCA189__INCLUDED_)
