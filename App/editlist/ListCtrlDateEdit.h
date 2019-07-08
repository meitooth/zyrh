#if !defined(AFX_LISTCTRLDATEEDIT_H__0A1F5FAD_C429_4C59_A550_2D823A359D7E__INCLUDED_)
#define AFX_LISTCTRLDATEEDIT_H__0A1F5FAD_C429_4C59_A550_2D823A359D7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlDateEdit.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CListCtrlDateEdit window
#include "DateEdit.H"
#include "ListCtrlEditBase.H"

class CListCtrlDateEdit : public CDateEdit, public CListCtrlEditBase
{
// Construction
private:
public:
	BOOL CreateEx(CWnd *pParent, CListCtrl *pCtrl ,DWORD dwStyle = EA_DEFAULT, int nID = -1);

public:
	CListCtrlDateEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlDateEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlDateEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlDateEdit)
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLDATEEDIT_H__0A1F5FAD_C429_4C59_A550_2D823A359D7E__INCLUDED_)
