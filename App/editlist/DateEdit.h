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
	BOOL m_bUseMask;//�Ƿ�ʹ������
	BOOL m_isDate;//�Ƿ������ڸ�ʽ
	BOOL m_isTime;//�Ƿ���ʱ���ʽ
	BOOL m_isDateTime;//�Ƿ�������ʱ���ʽ
	BOOL m_isNumber; //�Ƿ�������ʱ���ʽ
	CString m_str;//�洢�û�������ַ���
	CString m_strMask;//����
	CString m_strLiteral;//��ʾĳλ�Ƿ�����룬��_����ʾ������
	CString m_strValid;
//	CString m_strHours;
//	CString m_strMins;
	CString m_strName;
	CString m_strMaskLiteral;//�洢Ԥ����ַ��������û������˸����ʱ��ʹ��
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
