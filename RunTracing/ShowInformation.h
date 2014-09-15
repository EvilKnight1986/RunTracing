#pragma once
#include "PublicHead.h"
#include "LayOut.h"
#include "afxcmn.h"

// CShowInformation �Ի���

class CShowInformation : public CDialog
{
	DECLARE_DYNAMIC(CShowInformation)

public:
	CShowInformation(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowInformation();

// �Ի�������
	enum { IDD = IDD_SHOW_INFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    BOOL SetUserData(PRunData pRunData) ;
    BOOL UpdateTree() ;
private:
    CTreeCtrl m_Tree;
    PRunData  m_pRunData ;
    CLayout *m_lpLayoutSize;  // ��С align
    CLayout *m_lpLayoutBottom;// �ײ� align
    CRect    m_orignalSize;
    CToolTipCtrl m_tt;
public:
    virtual BOOL OnInitDialog();
//    afx_msg void OnNMRDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
