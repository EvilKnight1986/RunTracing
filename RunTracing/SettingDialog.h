#pragma once
#include "stdafx.h"
#include "afxcmn.h"
#include "RecordDialog.h"
#include "AntiAntiDebugDialog.h"

// CSettingDialog 对话框

class CSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingDialog)

public:
	CSettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDialog();

// 对话框数据
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    virtual BOOL OnInitDialog();
    afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
            void SetUserData(PRunData pRunData) ;
private:
    CTabCtrl             m_Tab;
    CRecordDialog        m_RecordDialog ;
    CAntiAntiDebugDialog m_AntiAntiDebugDialog ;
    PRunData             m_pRunData ;
protected:
    virtual void OnOK();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedOk();
};
