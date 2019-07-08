#ifndef _MASKDEFINE_
#define _MASKDEFINE_
/////////////////////////////////////////////////////////////////////////////
// CDateEdit window

class CDateEdit : public CEdit
{
	DECLARE_DYNAMIC(CDateEdit)
// Construction
public:
	CDateEdit();

// Attributes
public:
	BOOL m_bUseMask;//是否使用掩码
	BOOL m_isDate;//是否是日期格式
	BOOL m_isTime;//是否是时间格式
	BOOL m_isDateTime;//是否是日期时间格式
	BOOL m_isNumber; //是否是日期时间格式
	CString m_str;//存储用户输入的字符串
	CString m_strMask;//掩码
	CString m_strLiteral;//表示某位是否可输入，“_”表示可输入
	CString m_strValid;
//	CString m_strHours;
//	CString m_strMins;
	CString m_strName;
	CString m_strMaskLiteral;//存储预设的字符串，当用户输入退格键的时候使用
	BOOL m_bMaskKeyInProgress;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaskEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDefault();
	void SetNow();
	void Reset();
	void SetClassType(LPCSTR pName, LPCSTR pMask = NULL);
	void SendChar(UINT nChar);
	void SetMask(LPCSTR lpMask,LPCSTR lpLiteral=NULL,LPCSTR lpValid=NULL);
	BOOL CheckChar(UINT nChar);
	virtual ~CDateEdit();
    LRESULT OnPaste(WPARAM wparam, LPARAM lparam);
	LRESULT OnCut(WPARAM wparam, LPARAM lparam);
	LRESULT OnClear(WPARAM wparam, LPARAM lparam);
    BOOL IsValid(const COleDateTime& date) const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMaskEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/*
class CDateEdit : public CMaskEdit
{
  DECLARE_DYNAMIC(CDateEdit)

// Constructors
public:
  CDateEdit();

// Implementation
public:
  void SetDate(COleDateTime& Date);
  COleDateTime GetDate();

protected:
  // Generated message map functions
  //{{AFX_MSG(CDateEdit)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

class CTimeEdit : public CMaskEdit
{
  DECLARE_DYNAMIC(CTimeEdit)

// Constructors
public:
  CTimeEdit();

// Implementation
public:
  void SetTime(COleDateTime& Date);
  void SetTime(CString Date);
  COleDateTime GetTime();
  CString GetTimeStr();
  void SetHours(int hrs);
  void SetMins(int mins);

protected:
  // Generated message map functions
  //{{AFX_MSG(CTimeEdit)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
*/
//void AFXAPI DDX_OleDate(CDataExchange* pDX, int nIDC, CDateEdit& rControl, COleDateTime& Date);
//added the line below
//void AFXAPI DDX_OleDate(CDataExchange* pDX, int nIDC, CTimeEdit& rControl, COleDateTime& Date);
#endif
