#if !defined(AFX_LISTCTRLCURRENCYEDIT_H__D2D04077_A5F6_4CEC_AA7E_DC04B0947231__INCLUDED_)
#define AFX_LISTCTRLCURRENCYEDIT_H__D2D04077_A5F6_4CEC_AA7E_DC04B0947231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlCurrencyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlCurrencyEdit window
#include "ListCtrlEditBase.H"
#include "amsEdit.H"


class CListCtrlCurrencyEdit : public CAMSCurrencyEdit, public CListCtrlEditBase
{
// Construction
public:
	BOOL CreateEx(CWnd *pParent, CListCtrl *pCtrl ,DWORD dwStyle = EA_DEFAULT, int nID = -1);
public:
	CListCtrlCurrencyEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlCurrencyEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlCurrencyEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlCurrencyEdit)
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLCURRENCYEDIT_H__D2D04077_A5F6_4CEC_AA7E_DC04B0947231__INCLUDED_)
