// SettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "RunTracing.h"
#include "SettingDialog.h"
#include "About.h"
#include "RecordDialog.h"
#include "PublicHead.h"
#include "PublicFunction.h"

// CSettingDialog 对话框

#pragma comment(lib, "ollydbg.lib")

IMPLEMENT_DYNAMIC(CSettingDialog, CDialog)

CSettingDialog::CSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDialog::IDD, pParent)
    , m_pRunData(NULL)
{

}

CSettingDialog::~CSettingDialog()
{
}

void CSettingDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB, m_Tab);
}


BEGIN_MESSAGE_MAP(CSettingDialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CSettingDialog::OnBnClickedButton1)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CSettingDialog::OnTcnSelchangeTab)
    ON_BN_CLICKED(IDOK, &CSettingDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingDialog 消息处理程序

void CSettingDialog::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    CAbout aboutDig ;
    aboutDig.DoModal() ;
}

BOOL CSettingDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_Tab.InsertItem(RECORDINDEX, "记录") ;
    m_Tab.InsertItem(ANTIANTIDEBUG, "留空") ;

    m_RecordDialog.Create(IDD_RECORD_DIALOG, GetDlgItem(IDC_TAB)) ;
    m_RecordDialog.SetParent(GetDlgItem(IDC_TAB));

    m_RecordDialog.SetUserData(m_pRunData) ;

    m_AntiAntiDebugDialog.Create(IDD_ANTI_DIALOG, GetDlgItem(IDC_TAB)) ;
    m_AntiAntiDebugDialog.SetParent(GetDlgItem(IDC_TAB));

    m_AntiAntiDebugDialog.SetUserData(m_pRunData) ;

    CRect rc ;
    m_Tab.GetClientRect(&rc) ;
    rc.top += 21 ;
    rc.left += 2 ;
    rc.right -= 2 ;
    rc.bottom -= 2 ;

    m_RecordDialog.MoveWindow(&rc) ;
    m_AntiAntiDebugDialog.MoveWindow(&rc) ;

    m_RecordDialog.ShowWindow(TRUE) ;
    m_AntiAntiDebugDialog.ShowWindow(FALSE) ;

    m_Tab.SetCurSel(0) ;

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSettingDialog::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    int nSelect = m_Tab.GetCurSel() ;

    switch(nSelect)
    {
        // 
        case 0:
        {
            m_RecordDialog.ShowWindow(TRUE) ;
            m_AntiAntiDebugDialog.ShowWindow(FALSE) ;
            //m_Build.ShowWindow(FALSE) ;
        }
        break ;
        case 1:
        {
            m_RecordDialog.ShowWindow(FALSE) ;
            m_AntiAntiDebugDialog.ShowWindow(TRUE) ;
            //m_Build.ShowWindow(FALSE) ;
        }
        break ;
    }

    *pResult = 0;
}

void CSettingDialog::SetUserData(PRunData pRunData)
{
    m_pRunData = pRunData ;
    int nImageBase = 0 ;
    DWORD dwOEP = 0 ;
    ulong uStart = 0 ;
    ulong uEnd = 0 ;
    ulong uBase = 0;
    ulong uSize = 0 ;

    if (0 == pRunData->uStartAddress)
    {
        nImageBase = Plugingetvalue(VAL_MAINBASE) ;

        // 取得oep地址，默认程序设的断点地址是oep的
        dwOEP = GetOEP(nImageBase) ;

        if (0 != dwOEP)
        {
            Getproclimits(dwOEP, &uStart, &uEnd) ;
            m_pRunData->uStartAddress = uStart ;
            m_pRunData->uEndAddress = uEnd ;
        }
        else
        {
            m_pRunData->uStartAddress = nImageBase ;
            m_pRunData->uEndAddress   = nImageBase + 10 ;
        }

        m_RecordDialog.SetUserData(pRunData) ;
    }
}

void CSettingDialog::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    CDialog::OnOK();
}

BOOL CSettingDialog::PreTranslateMessage(MSG* pMsg)
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

void CSettingDialog::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码

    if(FALSE == m_RecordDialog.GetUserData(m_pRunData))
    {
        return ;
    }

    if (FALSE == m_AntiAntiDebugDialog.GetUserData(m_pRunData))
    {
        return ;
    }

    unsigned long uSize = m_pRunData->uEndAddress - m_pRunData->uStartAddress ;

    
    // 先移除所有的断点
    RemoveAllBreakPoint() ;

	if(TRUE == ForceSetCallBreakPoint(m_pRunData,
									m_pRunData->uStartAddress,
									uSize))
	{
		m_pRunData->bIsStart = TRUE ;
		InitTreeHead(m_pRunData) ;
	}

	/*

    if (m_pRunData->bIsForceSetBreakPoint)
    {
        if(TRUE == ForceSetCallBreakPoint(m_pRunData,
                                            m_pRunData->uStartAddress,
                                            uSize))
        {
            m_pRunData->bIsStart = TRUE ;
            InitTreeHead(m_pRunData) ;
        }
    }
    else
    {
        if (TRUE == EnumerateFunctionAddress(m_pRunData))
        {
            m_pRunData->bIsStart = TRUE ;
        }
    }
	*/

    OnOK();
}
