/*******************************************************************************
*  
*  Copyright (c) all 2011 EvilKnight All rights reserved
*  FileName : antiAntiDebugDialog.h
*  D a t e  : 2011.9.8
*  功   能  : 
*  说   明  : 反反调试对话框头文件
*
*
*******************************************************************************/
#pragma once
#include "PublicHead.h"

// CAntiAntiDebugDialog 对话框

class CAntiAntiDebugDialog : public CDialog
{
	DECLARE_DYNAMIC(CAntiAntiDebugDialog)

public:
	CAntiAntiDebugDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAntiAntiDebugDialog();
    BOOL GetUserData(IN OUT PRunData pRunData) ;
    void SetUserData(IN PRunData pRunData) ;

// 对话框数据
	enum { IDD = IDD_ANTI_DIALOG };
private:
    PRunData m_pRunData ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
