#pragma once
#include "PublicHead.h"
#include "afxwin.h"

// CRecordDialog 对话框

class CRecordDialog : public CDialog
{
	DECLARE_DYNAMIC(CRecordDialog)

public:
	CRecordDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRecordDialog();
    BOOL GetUserData(IN OUT PRunData pRunData) ;
    void SetUserData(IN PRunData pRunData) ;
    void MyUpdateData() ;

// 对话框数据
	enum { IDD = IDD_RECORD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
    // 是否记录地址范围外的调用函数
    BOOL m_bIsRecordOther;
    // 是否记录api日志(demo)
    BOOL m_bIsRecordAPI;
    // 是否开启强制模式
    BOOL m_bIsForce ;
    // 日志路径
    CString m_strLogPath;
    CString m_StrStartAddress;
    CString m_StrEndAddress;
    PRunData m_pRunData ;
    CToolTipCtrl m_tt;
public:
    afx_msg void OnBnClickedRecordapi();
    afx_msg void OnBnClickedBrower();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedGetcurrentip();
    virtual BOOL OnInitDialog();
    ulong        GetCurrentEIP() ;
    afx_msg void OnBnClickedBrower2();
private:
    BOOL m_bIsBackupUDD;
    CString m_strBackupPath;
public:
    afx_msg void OnBnClickedBackupudd();
};
