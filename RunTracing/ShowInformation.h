#pragma once
#include "PublicHead.h"
#include "LayOut.h"
#include "afxcmn.h"

// CShowInformation 对话框

class CShowInformation : public CDialog
{
	DECLARE_DYNAMIC(CShowInformation)

public:
	CShowInformation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowInformation();

// 对话框数据
	enum { IDD = IDD_SHOW_INFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    BOOL SetUserData(PRunData pRunData) ;
    BOOL UpdateTree() ;
private:
    CTreeCtrl m_Tree;
    PRunData  m_pRunData ;
    CLayout *m_lpLayoutSize;  // 大小 align
    CLayout *m_lpLayoutBottom;// 底部 align
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
