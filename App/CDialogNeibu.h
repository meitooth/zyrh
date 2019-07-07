#pragma once

#include <vector>
#include "desktopCapture.h"
#include "CaptureScreen.h"
#include "EditListCtrl.h"
#include "desktopCaptureSecond.h"
// CDialogNeibu 对话框

class CDialogNeibu : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNeibu)

public:
	CDialogNeibu(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogNeibu();
	void OnKerEvent(int KeyValue);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEIBU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
	DECLARE_MESSAGE_MAP()
	void InitTableInfo();
	void InitTableInfoSecond();
	void InitJsonInfo(Json::Value& value);
	void InitJsonInfoSecond(Json::Value& value);
	std::string GetAppStartupPath();
	void SetCurrentPath(const std::string& path);
	void OnClose();
	void OnCancel();
	void SetFan(int nTableId);
private:
	CListCtrl   m_listCtrl;
	CListCtrl   m_ListTotal;
	std::vector<CTableManager> m_TableList;
	std::vector<CTableManagerSub> m_TableSubList;
	DesktopCapturer m_desktopCapturer;
	DesktopCapturerSecond m_desktopCapturerSecond;
	std::string app_path_;
	CButton m_btnStart;
	CButton m_btnPause;
	CButton m_isCheck;
	CButton m_BntCheckAI;
	CButton m_bUserRand;
	CHttpServer m_httpServer;
	CRect m_rect;
	int m_nRowCount = 8;


public:
	afx_msg void OnBnClickedCheckModel();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnPause();
	afx_msg LRESULT OnUserTableNum(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditEnd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedStaticStatus();
	afx_msg void OnLvnItemchangedListHome(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCheckModel2();
	afx_msg BOOL PreTranslateMessage(MSG * pMsg);
	afx_msg void OnBnClickedCheckAi();
};
