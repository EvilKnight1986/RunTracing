/*******************************************************************************
*  
*  Copyright (c) all 2011 北京锐安 新技术研究 All rights reserved
*  FileName : AntiAntiDebugDialog.cpp
*  D a t e  : 2011.9.8
*  功   能  : 
*  说   明  : 反反调试对话框实现文件
*
*
*******************************************************************************/
#include "stdafx.h"
#include "RunTracing.h"
#include "AntiAntiDebugDialog.h"


// CAntiAntiDebugDialog 对话框

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


// CAntiAntiDebugDialog 消息处理程序

BOOL CAntiAntiDebugDialog::GetUserData(IN OUT PRunData pRunData)
{
    return TRUE ;
}

void CAntiAntiDebugDialog::SetUserData(IN PRunData pRunData)
{

}

BOOL CAntiAntiDebugDialog::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN: //回车
             return TRUE;
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}
