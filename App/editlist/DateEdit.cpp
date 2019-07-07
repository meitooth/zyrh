
#include "stdafx.h"
#include "DateEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleDateTime read /write
/*
COleDateTime ReadCOleDateTime(LPCSTR lpszData)
{
  COleDateTime DateTime;
  DateTime.ParseDateTime(lpszData);
  return DateTime;
}

void FormatCOleDateTime(CString& strData, COleDateTime DateTime, int len)
{
  strData = "";
  if (DateTime.m_dt == 0) return;
  if (len == 8)
    strData = DateTime.Format("%d/%m/%y");
  else if(len == 5) // added these two
	strData = DateTime.Format("%H:%M");
  else
    strData = DateTime.Format("%d/%m/%Y");
}

/////////////////////////////////////////////////////////////////////////////
// DDX for mask control

void AFXAPI DDX_OleDate(CDataExchange* pDX, int nIDC, CDateEdit& rControl, COleDateTime& Date)
{
  DDX_Control(pDX, nIDC, (CWnd&)rControl);
  if (!pDX->m_bSaveAndValidate)
    rControl.SetDate(Date);
  else
    Date = rControl.GetDate();
}
*/
/////////////////////////////////////////////////////////////////////////////
// CDateEdit

IMPLEMENT_DYNAMIC(CDateEdit,CEdit)
BEGIN_MESSAGE_MAP(CDateEdit, CEdit)
	//{{AFX_MSG_MAP(CDateEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CUT, OnCut)
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_MESSAGE(WM_CLEAR, OnClear)

END_MESSAGE_MAP()


CDateEdit::CDateEdit()
{
	Reset();
}

CDateEdit::~CDateEdit()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDateEdit message handlers

void CDateEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString str;
	GetWindowText(str);
	for (int i = 0; i<str.GetLength() && i<m_str.GetLength();i++)
		m_str.SetAt(i, str.GetAt(i));
	if(!m_bMaskKeyInProgress)
		if(!CheckChar(nChar)) return;
		if(m_bUseMask)  //要使用掩码
		{
			if(isprint(nChar))  //是可打印字符
			{
				int startPos,endPos;
				GetSel(startPos,endPos);
				SetSel(startPos,endPos+1);
				Clear();
			}
			else if(nChar==VK_BACK)
			{
				int startPos,endPos;
				GetSel(startPos,endPos);
				if((startPos==endPos) && (startPos>=1) && (startPos<=m_str.GetLength()))
				{
					char c;
					if(m_strMaskLiteral != _T(""))
						c=m_strMaskLiteral[startPos-1];
					TRACE("m_strMaskLiteral=[%s](%s)\n",m_strMaskLiteral,m_str);
					//回退光标
					SendMessage(WM_KEYDOWN,VK_LEFT,0);
					if(m_strMaskLiteral != _T(""))
					{
						//恢复在该位置预设的字符
						SendChar(c);
						//再次退回
						SendMessage(WM_KEYDOWN,VK_LEFT,0);

					}
				}
				else   //越界或者存在选择区域
					MessageBeep((UINT)-1);
				return;
			}

		}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if(!m_bMaskKeyInProgress && m_bUseMask && m_strLiteral != _T(""))
	{
		int startPos,endPos;
		GetSel(startPos,endPos);
		//确保字符串的长度小于m_strLiteral的长度
		if(endPos<m_strLiteral.GetLength())
		{
			UINT c=m_strLiteral.GetAt(endPos);
			if(c!='_')
				SendChar(c);
		}
	}
}

void CDateEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if(m_bUseMask)
	{
		switch(nChar)//忽略删除和插入键
		{
		case VK_DELETE:
		case VK_INSERT: return;
		}
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDateEdit::CheckChar(UINT nChar)
{
	UINT c;
	int nTime = 0;
	CString sText;
	//如果不使用掩码，则返回
	if(!m_bUseMask)	return TRUE;
	//如果是控制字符，则返回
	if(!isprint(nChar)) return TRUE;
	//如果存在选择区域，则取消选择
	int startPos,endPos;
	GetSel(startPos,endPos);
	SetSel(-1,0);
	//重新选中原选择区域的第一个字符
	SetSel(startPos,startPos);
	GetSel(startPos,endPos);
	//确保字符串的长度不超过掩码的长度
	if(endPos>=m_strMask.GetLength())
	{
		MessageBeep((UINT)-1);
		return FALSE;
	}
	//检查在当前位置是否应存在文字
	c='_';
	if(m_strLiteral != _T("") && 
	   m_strLiteral.GetLength()>endPos)
		c=m_strLiteral.GetAt(endPos);
	if(c!='_')
	{//发送默认值
		SendChar(c);
		GetSel(startPos,endPos);
	}
	//检查字符的有效性
	if(m_strValid.Find(nChar)!=-1) return TRUE;
	//检查当前位置的掩码
	c=m_strMask.GetAt(endPos);
	BOOL doit=TRUE;
	sText = m_str;
	sText.SetAt(endPos, nChar);
	if (nChar == VK_SPACE) return true; //空格
	switch(c)
	{
	case '0':    //只能是数字
		{
			BOOL doit=TRUE;
			if(isdigit(nChar))
			{
				if(m_isDate || m_isDateTime) //是否按照日期格式输入
				{
//					sText.SetAt(endPos, nChar);
					if (!IsValid(COleDateTime(COleVariant(sText))))
					{
						if(endPos < 8) 
						{//月、日设置为默认
						  sText.SetAt(4, '-');
						  sText.SetAt(5, '0');
						  sText.SetAt(6, '1');
						  sText.SetAt(7, '-');
						  sText.SetAt(8, '0');
						  sText.SetAt(9, '1');
						  SetWindowText(sText);
						  SetSel(endPos+1,endPos+1);
						}
						if(endPos >= 8) 
						{//日设置为少30
						  if (sText.GetAt(8) > '3')
						     sText.SetAt(8, '0');
						  sText.SetAt(9, '1');
						  SetWindowText(sText);
						  SetSel(endPos+1,endPos+1);
						}
						doit=FALSE;
					}
/*					if(endPos==8)
					{
						if(nChar>'3')  //每月只能有三十几天
							doit=FALSE;
					}
					if(endPos==9)
					{
						if(m_str.GetAt(5)=='0' && 
						   m_str.GetAt(6)=='2' &&
						   m_str.GetAt(8)=='2')
						{
							if(nChar>'8')//2月不超过29天
								doit=FALSE;
						}
						else if(m_str.GetAt(5)=='0' && 
						   m_str.GetAt(6)=='2' &&
						   m_str.GetAt(8)>'2')
						{//2月不起30
								doit=FALSE;
						}
						else if(m_str.GetAt(8)=='3')
						{
							if(nChar>'1')//每月不超过31天
								doit=FALSE;
						}
					}
					if(endPos==5)
					{
						if(nChar>'1') //一年只能有12个月
							doit=FALSE;
					}
					if(endPos==6)
					{
						if(m_str.GetAt(5)=='1')
						{
							if(nChar>'2') //一年只能有12个月
								doit=FALSE;
						}
					}
*/
				}//结束日期检查
				if (m_isDateTime) nTime = 11;
				if (m_isTime || m_isDateTime)   //按时间格式检查
				{
					//转换时间为HH0MM
//					for(int i = nTime; i < nTime+5; i++)
//					{
//						if (sText.GetAt(i) == TCHAR(' ') ||
//						    sText.GetAt(i) == TCHAR(':')) 
//						    sText.SetAt(i, TCHAR('0'));
//					}
					if (!IsValid(COleDateTime(COleVariant(sText))))
//                    if (atoi((LPCSTR)sText.Mid(nTime)) > 24000) 
					{
						if(endPos == nTime &&
						   nChar < '3') 
						{
						  sText.SetAt(nTime+1, '0');
						  sText.SetAt(nTime+2, ':');
						  sText.SetAt(nTime+3, '0');
						  sText.SetAt(nTime+4, '0');
						  SetWindowText(sText);
						  SetSel(endPos+1,endPos+1);
						}
						if(endPos == nTime + 1 && 
						   nChar < '4')
						{
						  sText.SetAt(nTime+2, ':');
						  sText.SetAt(nTime+3, '0');
						  sText.SetAt(nTime+4, '0');
						  SetWindowText(sText);
						  SetSel(endPos+1,endPos+1);
						}
						doit=FALSE;
					}
				}//结束 时间检查
				return doit;
			}
			break;
		}
    }

    MessageBeep((UINT)-1);
    return FALSE;

}

void CDateEdit::SetMask(LPCSTR lpMask, LPCSTR lpLiteral, LPCSTR lpValid)
{
	m_bUseMask=FALSE;
	TCHAR szMask[] = {'0', '9', '#', 'A', 'a', '?'};
	if(lpMask==NULL) return;
	m_strMask=lpMask;
	if(m_strMask == _T("")) return;
	if(lpLiteral!=NULL)
	{
		m_strLiteral=lpLiteral;
		if(m_strLiteral.GetLength()!=m_strMask.GetLength())
			m_strLiteral.Empty();
	}
	else
	{//自动生成
		//m_strLiteral.Empty();
		m_strLiteral = m_strMask;
		for(int i = 0; i < 6; i++)
		{
		  int j = 0;
          j = m_strLiteral.Find(szMask[i], j);
		  while(j>=0)
		  {
           m_strLiteral.SetAt(j, TCHAR('_'));
           j = m_strLiteral.Find(szMask[i], j);
          }//end while
		}//end for
	}
	if(lpValid!=NULL)
		m_strValid=lpValid;
	else
		m_strValid.Empty();
	m_bUseMask=TRUE;
//分配存储空间
    m_str = CString(TCHAR(' '), m_strMask.GetLength());
}
BOOL CDateEdit::IsValid(const COleDateTime& date)const
{
	return (date.GetStatus() == COleDateTime::valid);
}




LRESULT CDateEdit::OnCut(WPARAM wparam, LPARAM lparam)
{//禁止CUT
/*	int nStart, nEnd;
	GetSel(nStart, nEnd);

	if (nStart < nEnd)
	{
		SendMessage(WM_COPY);				// copy the selection and...
		SendMessage(WM_KEYDOWN, VK_DELETE); // delete it
	}
*/	
	return 0;
}

// Clears the current selection.
LRESULT CDateEdit::OnClear(WPARAM wparam, LPARAM lparam)
{//禁止CLEAR
/*	int nStart, nEnd;
	GetSel(nStart, nEnd);

	if (nStart < nEnd)
		SendMessage(WM_KEYDOWN, VK_DELETE); // delete the selection
*/	
	return 0;
}
// Pastes the text from the clipboard onto the current selection.
LRESULT CDateEdit::OnPaste(WPARAM wparam, LPARAM lparam)
{//禁止PASTE
/*	int nStart, nEnd;
	GetSel(nStart, nEnd);

	CEdit::Default();
	CString strText = GetValidText();

	if (strText != GetText())
	{
		SetWindowText(strText);
		SetSel(nStart, nEnd);
	}
*/
	return 0;
}
// Insures that text set via SetWindowText is valid.


void CDateEdit::SetClassType(LPCSTR pName, LPCSTR pMask)
{
 CString sName = pName;
 sName.MakeUpper();
 Reset();
 if(sName == _T("DATEEDIT"))       //日期
 {
  m_bUseMask = TRUE;
  m_isDate = TRUE; //added this
  m_strMask = _T("0000-00-00");    //掩码
  m_strLiteral = _T("____-__-__"); //掩码有效设置 有效_ 其它-
         m_str = _T("          "); //数据保存空间

   SetWindowText(_T("    -  -  "));
 }
 else if(sName == _T("TIMEEDIT"))   //时间
 {
  m_bUseMask = TRUE;
  m_isTime = true; 
       m_strMask = _T("00:00");
    m_strLiteral = _T("__:__");
           m_str = _T("     ");
     SetWindowText(_T("  :  "));
 }
 else if(sName == _T("DATETIMEEDIT")) //日期+时间
 {
   m_bUseMask = TRUE;
   m_isDateTime = true;
      m_strMask = _T("0000-00-00 00:00");
   m_strLiteral = _T("____-__-__ __:__");
          m_str = _T("                ");
    SetWindowText(_T("    -  -     :  "));
 }
}

void CDateEdit::Reset()
{
	m_bUseMask=FALSE;
	m_strMask=_T("");
	m_strLiteral=_T("");
	m_strValid=_T("");
//	m_strHours=_T("24");
//	m_strMins=_T("60");
	m_bMaskKeyInProgress=FALSE;
	m_strMaskLiteral=_T("");
	m_str = _T("");

	m_isDate = FALSE;//是否是日期格式
	m_isTime = FALSE;//是否是时间格式
	m_isDateTime = FALSE;//是否是日期时间格式
    m_isNumber = FALSE;
}

void CDateEdit::SendChar(UINT nChar)
{
	m_bMaskKeyInProgress=TRUE;
#ifdef WIN32
	AfxCallWndProc(this,m_hWnd,WM_CHAR,nChar,1);
#else
	SendMessage(WM_CHAR,nChar,1);
#endif
	m_bMaskKeyInProgress=FALSE;
}
/*
/////////////////////////////////////////////////////////////////////////////
// CDateEdit class

IMPLEMENT_DYNAMIC(CDateEdit, CDateEdit)

BEGIN_MESSAGE_MAP(CDateEdit, CDateEdit)
  //{{AFX_MSG_MAP(CDateEdit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDateEdit::CDateEdit()
{
  m_bUseMask = TRUE;
  m_isDate = TRUE; //added this
  m_strMask = _T("0000-00-00");
  m_strLiteral = _T("____-__-__");
}

void CDateEdit::SetDate(COleDateTime& Date)
{
  CString strText;
  //FormatCOleDateTime(strText, Date, 10);
  strText = Date.Format("%Y-%m-%d");
  m_str = m_strMaskLiteral = strText;
  SetWindowText(strText);
}

COleDateTime CDateEdit::GetDate()
{
  CString strText;
  GetWindowText(strText);
  return ReadCOleDateTime(strText);
}

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit class completely new

IMPLEMENT_DYNAMIC(CTimeEdit, CDateEdit)

BEGIN_MESSAGE_MAP(CTimeEdit, CDateEdit)
  //{{AFX_MSG_MAP(CTimeEdit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTimeEdit::CTimeEdit()
{
  m_bUseMask = TRUE;
  m_isDate = FALSE; 
  m_strMask = _T("00:00");
  m_strLiteral = _T("__:__");
}

void CTimeEdit::SetTime(COleDateTime& Date)
{
  CString strText;
  FormatCOleDateTime(strText, Date, 5);
  m_str = m_strMaskLiteral = strText;
  SetWindowText(strText);
}

void CTimeEdit::SetTime(CString Date)
{
	m_str = m_strMaskLiteral = Date;
	SetWindowText(Date);
}

COleDateTime CTimeEdit::GetTime()
{
  CString strText;
  GetWindowText(strText);
  return ReadCOleDateTime(strText);
}

CString CTimeEdit::GetTimeStr()
{
	CString strText;
	GetWindowText(strText);
	return strText;
}

void CTimeEdit::SetHours(int hrs)
{
//	m_strHours.Format("%d", hrs);
}

void CTimeEdit::SetMins(int hrs)
{
//	m_strMins.Format("%d", hrs);
}
*/

void CDateEdit::SetDefault()
{
 if (m_isDate)
  SetWindowText(_T("    -  -  "));
 else if (m_isTime)
  SetWindowText(_T("  :  "));
 else if (m_isDateTime)
  SetWindowText(_T("    -  -     :  "));
}
void CDateEdit::SetNow()
{
 COleDateTime tNow = COleDateTime::GetCurrentTime();
 if (m_isDate)
  SetWindowText(tNow.Format("%Y-%m-%d"));
 else if (m_isTime)
  SetWindowText(tNow.Format("%H:%M"));
 else if (m_isDateTime)
  SetWindowText(tNow.Format("%Y-%m-%d %H:%M"));
}


