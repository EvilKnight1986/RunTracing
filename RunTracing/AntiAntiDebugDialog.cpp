/*******************************************************************************
*  
*  Copyright (c) all 2011 ������ �¼����о� All rights reserved
*  FileName : AntiAntiDebugDialog.cpp
*  D a t e  : 2011.9.8
*  ��   ��  : 
*  ˵   ��  : �������ԶԻ���ʵ���ļ�
*
*
*******************************************************************************/
#include "stdafx.h"
#include "RunTracing.h"
#include "AntiAntiDebugDialog.h"


// CAntiAntiDebugDialog �Ի���

IMPLEMENT_DYNAMIC(CAntiAntiDebugDialog, CDialog)

CAntiAntiDebugDialog::CAntiAntiDebugDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAntiAntiDebugDialog::IDD, pParent)
    , m_pRunData(NULL)
{

}

CAntiAntiDebugDialog::~CAntiAntiDebugDialog()
{
}

void CAntiAntiDebugDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAntiAntiDebugDialog, CDialog)
END_MESSAGE_MAP()


// CAntiAntiDebugDialog ��Ϣ�������

BOOL CAntiAntiDebugDialog::GetUserData(IN OUT PRunData pRunData)
{
    return TRUE ;
}

void CAntiAntiDebugDialog::SetUserData(IN PRunData pRunData)
{

}

BOOL CAntiAntiDebugDialog::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN: //�س�
             return TRUE;
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}
