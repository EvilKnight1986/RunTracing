#pragma once
#include "PublicHead.h"
#include "afxwin.h"

// CRecordDialog �Ի���

class CRecordDialog : public CDialog
{
	DECLARE_DYNAMIC(CRecordDialog)

public:
	CRecordDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRecordDialog();
    BOOL GetUserData(IN OUT PRunData pRunData) ;
    void SetUserData(IN PRunData pRunData) ;
    void MyUpdateData() ;

// �Ի�������
	enum { IDD = IDD_RECORD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
    // �Ƿ��¼��ַ��Χ��ĵ��ú���
    BOOL m_bIsRecordOther;
    // �Ƿ��¼api��־(demo)
    BOOL m_bIsRecordAPI;
    // �Ƿ���ǿ��ģʽ
    BOOL m_bIsForce ;
    // ��־·��
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
