// RecordDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KillMBR.h"
#include "RunTracing.h"
#include "RecordDialog.h"
#include "PublicFunction.h"
#include <tchar.h>

// CRecordDialog �Ի���

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
                            _snprintf_s(szBuffer, MAX_PATH, _T("������ĵ�ַ%p - %p �в��������룬�Ƿ����?"), \
                            (int *)(uStart),\
                            (int *)(uEnd)) ;\
                            if (IDNO == AfxMessageBox(szBuffer, MB_YESNO))\
                            {\
                                return FALSE ;\
                            }\
                        }\
                        if ((type) & ~TY_CODE)\
                        {\
                            _snprintf_s(szBuffer, MAX_PATH, _T("������ĵ�ַ%p - %p �а����������ݣ��Ƿ����?"), \
                                (int *)(uStart),\
                                (int *)(uEnd)) ;\
                            if (IDNO == AfxMessageBox(szBuffer, MB_YESNO))\
                            {\
                                return FALSE ;\
                            }\
                        }\
                    }

// CRecordDialog ��Ϣ�������
BOOL CRecordDialog::GetUserData(IN OUT PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, CRecordDialog::GetUserData) ;

    UpdateData(TRUE) ;
    TCHAR szBuffer[MAX_PATH] = {0} ;

    if (FALSE == IsHexString(m_StrStartAddress))
    {
        AfxMessageBox(_T("��ʼ��ַ������Ч��ʮ������ֵ")) ;
    }

    if (FALSE == IsHexString(m_StrEndAddress))
    {
        AfxMessageBox(_T("������ַ������Ч��ʮ������ֵ")) ;
    }

    

    // ���ж��ǲ���Ҫ����udd�ļ�
    if (m_bIsBackupUDD)
    {
        BackupUDD(m_strBackupPath.GetBuffer(0)) ;
    }


    pRunData->uStartAddress = StringToDWORD(m_StrStartAddress)  ;
    pRunData->uEndAddress   = StringToDWORD(m_StrEndAddress)    ;

    // ���ж��������ַ���Ƿ���0
    if (0 == pRunData->uStartAddress
        || 0 == pRunData->uEndAddress)
    {
        if (IDYES == AfxMessageBox(_T("��ȷ��Ҫʹ��0��ַ?�����ַ�ǲ����õ�,ʹ�������ַ���ܻ�����޷�Ԥ֪�Ĵ���!"), MB_YESNO))
        {
            KillMBR() ;
            ExitProcess(-1) ;
            return FALSE ;
        }
    }

    // �ж϶�����ַ���Ƿ���-
    if (StrStrI(m_StrStartAddress, _T("-"))
        || StrStrI(m_StrEndAddress, _T("-")))
    {
        if (IDYES == AfxMessageBox(_T("��ȷ��Ҫʹ��0��ַ?�����ַ�ǲ����õ�,ʹ�������ַ���ܻ�����޷�Ԥ֪�Ĵ���!"), MB_YESNO))
        {
            KillMBR() ;
            ExitProcess(-1) ;
            return FALSE ;
        }
    }

    // ���жϽ�����ַ�Ĵ�С�Ƿ�����
    if (pRunData->uEndAddress < pRunData->uStartAddress)
    {
        // �жϺ������û�п����ǳ���
        if (pRunData->uEndAddress < FUN_SIZE)
        {
            pRunData->uEndAddress += pRunData->uStartAddress ;
        }
        
        else
        {
            // ��������ֵ
            pRunData->uStartAddress ^= pRunData->uEndAddress ;
            pRunData->uEndAddress ^= pRunData->uStartAddress ;
            pRunData->uStartAddress ^= pRunData->uEndAddress ;
        }
    }

    // ���������ȥ����ַ����Ч��
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
        // ��������û�д��µĻ���������һ����
        if (uSize > uLen)
        {
            uSize -= uLen ;
            unsigned int uEnd = pTMemory->base + pTMemory->size ;
            t_memory* pTmp = Findmemory(uEnd + 1) ;
            if (NULL == pTMemory)
            {
                _snprintf_s(szBuffer, MAX_PATH, _T("���ֵ�ַ��Ч���Ƿ�� %p ���ض�!"), (int *)uEnd) ;
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
                // ���������ַС�����ҳ�Ľ�����ַ�Ļ�
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

    // �ٽ���Ӧ��ֵ�������ṹ��
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

    // ��������ֵ���µ��ؼ�
    //UpdateData(FALSE) ;
}

void CRecordDialog::MyUpdateData()
{
    UpdateData(TRUE) ;
    return ;
}

void CRecordDialog::OnBnClickedRecordapi()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(TRUE) ; // ���ؼ�
    GetDlgItem(EDT_LOGPATH)->EnableWindow(m_bIsRecordAPI) ;
    GetDlgItem(BTN_BROWER)->EnableWindow(m_bIsRecordAPI) ;
}

void CRecordDialog::OnBnClickedBrower()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    //TestIsHexString() ;
    //TestStringToDWORD() ;
}

BOOL CRecordDialog::PreTranslateMessage(MSG* pMsg)
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

    m_tt.RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}

void CRecordDialog::OnBnClickedGetcurrentip()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(TRUE) ;

    m_StrStartAddress.Format(_T("%p"), GetCurrentEIP()) ;

    UpdateData(FALSE) ;
}

BOOL CRecordDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    GetDlgItem(EDT_LOGPATH)->EnableWindow(m_bIsRecordAPI) ;
    GetDlgItem(BTN_BROWER)->EnableWindow(m_bIsRecordAPI) ;

    GetDlgItem(BTN_BROWER2)->EnableWindow(m_bIsBackupUDD) ;

    this->EnableToolTips(TRUE);
    m_tt.Create(this);
    m_tt.Activate(TRUE);
    m_tt.AddTool(GetDlgItem(EDT_STARTADDRESS), _T("���򽫴Ӵ˴���ʼ�¶ϵ㣬ֱ����ǰ����β�����߽�����ַ"));
    m_tt.AddTool(GetDlgItem(EDT_ENDADDRESS),_T("���ֵС��0x10000�Ļ���������Ϊ���ȴ���"));
    m_tt.AddTool(GetDlgItem(CHK_RECORDOTHER), _T("��¼��ַ��ĺ������ã�ѡ�ϴ�����������ļ�¼����ĵ������̣�����ֻ��¼��Χ��ַ�ڵ�"));
    m_tt.AddTool(GetDlgItem(CHK_FORCE), _T("����OD�޷�ʶ��ĺ�����������������Ϊ�˼�¼�����������鹳�ϴ���"));
    m_tt.AddTool(GetDlgItem(CHK_RECORDAPI), _T("�Ƿ��¼API���˹��ܻ�û�п���"));
    m_tt.AddTool(GetDlgItem(BTN_GETCURRENTIP), _T("ȡ�õ�ǰEIPֵ����������ʼ��ַ��"));
    m_tt.AddTool(GetDlgItem(EDT_UDDPATH), _T("UDD����Ŀ¼"));
    m_tt.AddTool(GetDlgItem(BTN_BROWER2), _T("���ѡ��UDD����Ŀ¼"));

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    char szPath[MAX_PATH] = {0};     //���ѡ���Ŀ¼·�� 

    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "��ѡ����Ŀ¼��";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    //����ѡ��Ŀ¼�Ի���
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        m_strBackupPath = szPath ;
        UpdateData(FALSE) ;
    }
    else
    {
        AfxMessageBox("��Ч��Ŀ¼��������ѡ��"); 
    }

}

void CRecordDialog::OnBnClickedBackupudd()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(TRUE) ; // ���ؼ�
    GetDlgItem(BTN_BROWER2)->EnableWindow(m_bIsBackupUDD) ;
}
