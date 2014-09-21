/*******************************************************************************
*  
*  Copyright (c) all 2011 EvilKnight All rights reserved
*  FileName : antiAntiDebugDialog.h
*  D a t e  : 2011.9.8
*  ��   ��  : 
*  ˵   ��  : �������ԶԻ���ͷ�ļ�
*
*
*******************************************************************************/
#pragma once
#include "PublicHead.h"

// CAntiAntiDebugDialog �Ի���

class CAntiAntiDebugDialog : public CDialog
{
	DECLARE_DYNAMIC(CAntiAntiDebugDialog)

public:
	CAntiAntiDebugDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAntiAntiDebugDialog();
    BOOL GetUserData(IN OUT PRunData pRunData) ;
    void SetUserData(IN PRunData pRunData) ;

// �Ի�������
	enum { IDD = IDD_ANTI_DIALOG };
private:
    PRunData m_pRunData ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
