#include "stdafx.h"
#include "PublicHead.h"
#include "About.h"
#include "SettingDialog.h"
#include "resource.h"
#include "publicFunction.h"
#include "ShowInformation.h"
#include <shlwapi.h>

RunData runData ;
char g_szPragmaName[MAX_PATH] = {0} ;
char g_szUDDFileName[MAX_PATH] = {0} ;
HANDLE g_hFile = INVALID_HANDLE_VALUE ;


//******************************************************************************
// ������: ODBG_Plugindata
// Ȩ  ��: public 
// ����ֵ: ���ز���汾��
// ��  ��: char shortname[32]
// ˵  ��: ����Ǳ���ģ�����ָ�����������
// ��  ��:
//******************************************************************************
extc int _export cdecl ODBG_Plugindata(char shortname[32])
{
    // Name of plugin
    strcpy_s(shortname, 32, PLUGINNAME) ;
    return PLUGIN_VERSION ;
}


//******************************************************************************
// ������: ODBG_Plugininit
// Ȩ  ��: public 
// ����ֵ: ��������ڳɹ�ʱ���뷵�� 0������ʧ��ʱ�����ͷ���Դ������-1��
// ��  ��: ollydbgversion   -- OllyDbg�İ汾��,������ǰ�淵�ص�PLUGIN_VERSION���бȽ�
// ��  ��: hw               -- OllyDbg �����ڵľ����һ�㽫�䱣�浽һ��ȫ�ֱ�����
// ��  ��: features         -- Ϊ��������չ����������ʹ����
// ˵  ��: �����ڳ�ʼ�������һ��������а�����ʼ��������������Դ
// ��  ��:
//******************************************************************************
extc int _export cdecl ODBG_Plugininit( int ollydbgversion, HWND hw, ulong * features) 
{ 
    if(ollydbgversion < PLUGIN_VERSION)
    {
        return -1 ;
    }

    // ����ʹ�� Addtolist()���������������Ϣ�������¼���ڣ�log window��,
    // �� OllyDbg �У�����Alt+L ���Բ鿴�ô��ڡ�����������������������еڶ�
    // ��ֵȡ 0��1��-1 �ֱ��ʾ������ʾ��������������OllyDbg �����Ƽ�ʹ�ô�
    // ������ʾ���м�¼��ʽ����һ��˵��������ƺͰ汾���ڶ���˵����Ȩ��Ϣ�� 
    Addtolist(0,0,PLUGINVERSIONINFOR); 
    Addtolist(0,-1,COPYRIGHTINFOR); 

    runData.hwnd = hw ;

//  ������ص�
//     TestIsHexString() ;
//     TestStringToDWORD() ;
//     TestListExist() ;
//     TestDestroyTree() ;
    //TestRemoveValueByList() ;
    return 0; 
}

//******************************************************************************
// ������: ODBG_Pluginmenu
// Ȩ  ��: public 
// ����ֵ: �ɹ�ʱ���� 1��ʧ��ʱ���� 0
// ��  ��: int origin -- origin ����ָ���˴����˵���������Դ�ںδ���������˵���
//                       ����ര�ڡ��ϵ㴰�ڵȵ�
// ��  ��: char data[4096] -- ����˵�����Ŀ����ʾ��ʽ
// ��  ��: void * item -- ָ����ʾ�ڴ��ڵ��������ݵ�ѡ��Ԫ�أ�����ָ�����ݴ��ڵ�
//                          ��������ͨ�����������ܸ���ϸ�µؾ��������ʾ�˵�
// ˵  ��: ���ڴ���������Ӳ˵�
// ��  ��:
//******************************************************************************
extc int _export cdecl ODBG_Pluginmenu( int origin, char data[4096], void *item) 
{ 
    t_dump *ptd = NULL;

    // ����˵�
    if(PM_MAIN == origin) 
    { 
        strcpy_s(data,4096, "0 Clear BreakPoint | 1 Option | 2 Report | 3 About"); 
        return 1; 
    } 

    if(PM_DISASM == origin) 
    { 
        ptd = (t_dump *)item; 

        // ����޵�Ϸ���̵Ļ����������κβ˵�
        if (NULL == ptd || STAT_NONE == Getstatus() )
        {
            return 0 ;
        }

        strcpy_s(data,4096, "RunTracing {0 Clear BreakPoint | 1 Option | 2 Report | 3 About}");

        return 1; 
    }

    return 0; 
}

//******************************************************************************
// ������: ODBG_Pluginaction
// Ȩ  ��: public 
// ����ֵ: ��
// ��  ��: int origin -- origin ����ָ���˴����˵���������Դ�ںδ���������˵���
//                       ����ര�ڡ��ϵ㴰�ڵȵ�
// ��  ��: int action -- ������Ŀʱ��������ֵ
// ��  ��: void * item -- ָ����ʾ�ڴ��ڵ��������ݵ�ѡ��Ԫ�أ�����ָ�����ݴ��ڵ�
//                          ��������ͨ�����������ܸ���ϸ�µؾ��������ʾ�˵�
// ˵  ��: ����ʵ�ָ��˵���Ĺ���
// ��  ��:
//******************************************************************************
extc void _export cdecl ODBG_Pluginaction( int origin, int action, void *item) 
{ 
    t_table *pTable = NULL ;

    switch(origin)
    {
    case PM_MAIN:
    case PM_DISASM:
        {
            // ������жϵ�
            if (0 == action)
            {
                if (0 != Plugingetvalue(VAL_PROCESSID))
                {
                    RemoveAllBreakPoint() ;
                }
                break ;
            }
            // ����ǵ���Option
            if (1 == action)
            {
                if (FALSE == runData.bIsStart)
                {
                    if(FALSE == runData.bIsInitialized)
                    {
                        InitRunData(&runData) ;
                        pTable = (t_table *)Plugingetvalue(VAL_MEMORY) ;
                    }

                    CSettingDialog SettingDialog ;
                    SettingDialog.SetUserData(&runData) ;
                    AFX_MANAGE_STATE(AfxGetStaticModuleState());
                    if(IDNO == SettingDialog.DoModal())
                    {
                        ;
                    }
                }
                else
                {
                    AfxMessageBox(_T("��֮ǰ�������Ѿ���Ч!")) ;
                }
                break; 
            }
            // �������report
            else if(2 == action)
            {
                CShowInformation ShowInfor ;
                //�����������ȷ���л�MFCģ��״̬
                AFX_MANAGE_STATE(AfxGetStaticModuleState());
                ShowInfor.SetUserData(&runData) ;
                ShowInfor.DoModal() ;
                break ;
            }
            // �������About
            else if (3 == action)
            {
                CAbout AboutDlg;
	            AFX_MANAGE_STATE(AfxGetStaticModuleState());
                AboutDlg.DoModal();
                break; 
            }
        }
    default:
        break;
    }
}

//******************************************************************************
// ������: ODBG_Paused
// Ȩ  ��: public 
// ����ֵ: �������1,��ֹ�����ػ棬���򷵻�0
// ��  ��: int reason -- Ӧ�ó�����ͣԭ��
//                       PP_EVENT	��ͣ�ڵ����¼�
//                       PP_PAUSE	��ͣ���û�����
//                       PP_TERMINATED	Ӧ�ó�����ֹ
// ��  ��: t_reg *reg -- ָ������ͣӦ�ó�����̼߳Ĵ�������ΪNULL
// ˵  ��: OllyDbg �ڱ����Գ�����ͣʱ��һ���ڲ��������ʱ���ñ�������
// ��  ��:
//******************************************************************************
extc int ODBG_Paused(int reason, t_reg *reg)
{
    if (NULL != reg)
    {
        runData.dwEIP = reg->ip ;
    }
    // �ж��ǲ�����ͣ�ڵ����쳣
    if (PP_EVENT != reason)
    {
        OutputDebugString(_T("ODBG_Paused PP_EVENT != reason!\r\n")) ;
        return 1 ;
    }

    // �ж��ǲ������ǵĶϵ��Ƿ��Ѿ�����
    if (FALSE == runData.bIsStart)
    {
        OutputDebugString(_T("ODBG_Paused runData.bIsStart no start!\r\n")) ;
        return 1 ;
    }

    char szSrcDec[1024] = {0};
    char szLine[1024] = {0};
    //char szBuffer[MAX_PATH] = {0} ;
    char szFunctionName[MAX_PATH] = {0} ;
    char szModuleName[SHORTLEN + 1] = {0} ;
    unsigned long uEsp = 0, uAddr = 0, uTemp = 0;
    CString str ;
    int nSize = 0,
        i = 0;
    t_disasm disasm = {0};
    ulong uJmpAddr = 0 ;
    t_module * pModule = NULL ;

    NULLVALUE_CHECK(reg, ODBG_Paused) ;

    // �ж϶ϵ��ǲ����Լ���
    if(FALSE == ListExist(&runData.BreakPointList, reg->ip))
    {
        return 1;
    }

    //// ���Լ��Ļ����д�����¼
    //   ��ȡ��eip����ָ��
    nSize = Readcommand(reg->ip, szLine) ;
    if (0 == nSize)
    {
        OutputDebugString(_T("ODBG_Paused Readcommand failed!\r\n")) ;
        return 1 ;
    }
    // ���ж��ǲ���call
    // ����������ָ��
    Disasm((uchar *)szLine, nSize, reg->ip, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);

    uJmpAddr = disasm.jmpaddr ;

    // �������call�Ļ����ж��ǲ���call api,�ǵĻ���������������Ļ���ȡ������
    // �ж��Ƿ�Ҫ����
    if (StrStrI(disasm.result, "CALL "))
    {
        // ��Ŀ¼��ַѹջ
        OutputDebugString(disasm.result) ;
        OutputDebugString("\r\n") ;

        // ���գ������Ժ����api��¼
        int nType = 0 ;

        if (0 == uJmpAddr)
        {
            // ȡ��call�Ĵ��� �����Ļ�� �Ĵ�����������jmpֵ
            uJmpAddr = GetCallRegAddress(&disasm, reg) ;
        }

        nType = Findlabel(uJmpAddr, szFunctionName) ;

        // ���ﻹҪ����
        str = szFunctionName ;

        // ���ﴦ��api������

        // ���ﻹҪ��������һ��notepad���Ǹ�����
        if (0 == nType && strstr(disasm.result, "<") && strstr(disasm.result, ">"))
        {
            //��������Ļ�Ҳ�ǲ��ø�����
            str = disasm.result ;
            AppendCall(&runData, disasm.ip, str) ;
            Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
            return 0 ;

        }

        // ���ж�api����,������û������Ÿ���
        switch(nType)
        {
        case NM_IMPORT:
        case NM_EXPORT:
        case NM_LIBRARY:
            //******************************************************************
            //
            // api��¼
            //
            //******************************************************************
            str = disasm.result ;
            if (NULL == StrStrI(str.GetBuffer(0), szFunctionName))
            {
                str += _T("  ") ;
                pModule = Findmodule(uJmpAddr) ;
                str += _T("[<&") ;
                if (NULL != pModule)
                {
                    strncpy_s(szModuleName, pModule->name, SHORTLEN) ;
                    str += szModuleName ;
                    str += _T(".") ;
                }
                str += szFunctionName ;
                str += _T(">]") ;
            }
            AppendCall(&runData, disasm.ip, str) ;
            Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
            return 0 ;
        default:
            {
                str = disasm.result ;
                // �������ж��ǲ���Ҫ������
                // ������ø����Ļ�
                if (FALSE == runData.bIsRecordOther 
                    && (uJmpAddr < runData.uStartAddress || uJmpAddr > runData.uEndAddress))
                {
                    AppendCall(&runData, disasm.ip, str) ;
                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
                    return 0 ;
                }
                // ���浱ǰ�Ļ���
                // �൱�ں������õ�push esp
                // ���ﻹ���������Ľ��
                SaveEnvironment(&runData, disasm.ip, str) ;


                ///// Ŀ�꺯�����¶ϵ�
                ulong uStart, uEnd ;
                if(0 == Getproclimits(uJmpAddr, &uStart, &uEnd))
                {
                    SetCallBreakPoint(&runData, uStart, uEnd) ;
                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
                    return 1 ;
                }
                else
                {
                    /*
                    // �ж���û�п�ǿ���¶ϵ�, �еĻ�ǿ���¶ϵ�
                    if(runData.bIsForceSetBreakPoint)
                    {
                        // ǿ���¶ϵ���
                        ForceSetCallBreakPoint(&runData, uJmpAddr, MAXFUNSIZE) ;
                    }
                    else
                    {
                        MessageBox(NULL, _T("Ŀ�꺯���߽粻�ɼ���\r\n"), disasm.result, MB_OK) ;
                    }
                    */

                    // ���������ϵĺ�����ʱ���ٸ��û��޸ĵĻ��ᣬ����ͣ�ĵ�����������
                    if (FALSE == runData.bIsForceSetBreakPoint)
                    {
                        if(IDYES == MessageBox(NULL, _T("Ŀ�꺯���߽粻�ɼ��㣬�Ƿ���ǿ���¶ϵ�\r\n"), disasm.result, MB_OK))
                        {
                            runData.bIsForceSetBreakPoint = TRUE ;
                        }
                    }

                    // �ж��Ƿ�ǿ���¶ϵ�
                    if (TRUE == runData.bIsForceSetBreakPoint)
                    {
                        // ǿ���¶ϵ���
                        ForceSetCallBreakPoint(&runData, uJmpAddr, MAXFUNSIZE) ;
                    }
                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
                    return 1 ;
                }
            }
            break ;
        }
    }
    // �����ret�Ļ����������ڵĶϵ�ȡ�����ٻ�ԭ����
    else if (StrStrI(disasm.result, "ret"))
    {
        OutputDebugString(disasm.result) ;
        OutputDebugString("\r\n") ;

        // �Ƴ������ڵ����жϵ�
        Deletebreakpoints(runData.pCurrentNode->dwFuncStart,
                          runData.pCurrentNode->dwFuncEnd + 1,
                          0) ;

        RemoveValueByList(&runData.BreakPointList,
                            runData.pCurrentNode->dwFuncStart,
                            runData.pCurrentNode->dwFuncEnd) ;

        // ��ջ
        ResumeEnvironment(&runData) ;
        Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
        return 1 ;
    }

    return 1 ;
}

//******************************************************************************
// ������: ODBG_Pluginreset
// Ȩ  ��: public 
// ����ֵ: ��
// ��  ��: ��
// ˵  ��: OllyDbg ���û��򿪻�����һ���µ�Ӧ�ó���ʱ���ñ�����
// ��  ��:
//******************************************************************************
extc void _export cdecl ODBG_Pluginreset(void)
{
    DestroyTree(runData.pTreeHead) ;
    InitRunData(&runData) ;
    
    //UnlockUDDFile() ;
}

//******************************************************************************
// ������: ODBG_Pluginclose
// Ȩ  ��: public 
// ����ֵ: �����ɲ�������ĵ� MDI ���ڽ�һֱ���ڣ������Ǻܿ���Ҫ����һЩ�������
//         ��ini�ļ��������ȫ�˳��� OllyDbg���������뷵��0���κη� 0 ֵ���ؽ���
//          ֹ�رն��У���Ҫ�������ֿ��ܣ�����֪ͨ�û���ֹ��ԭ����ܲ����Ǻܺõģ�
//          �����ѯ����������������δ���
// ˵  ��: ���û���Ҫ��ֹ OllyDbg ʱ���ñ�������
// ��  ��:
//******************************************************************************
extc int  _export cdecl ODBG_Pluginclose(void)
{
    DestroyTree(runData.pTreeHead) ;
    // ��ԭudd�ļ�

    //UnlockUDDFile() ;

    return 0 ;
}

void ODBG_Pluginmainloop(DEBUG_EVENT *debugevent)
{
    char szModuleFileName[MAX_PATH] = {0} ;
    char szUddFileName[MAX_PATH] = {0} ;

    if (NULL != debugevent)
    {
        ZeroMemory(g_szUDDFileName, sizeof(char) * MAX_PATH) ;
        if (CREATE_PROCESS_DEBUG_EVENT == debugevent->dwDebugEventCode)
        {

            // �õ�udd�ļ���
            char *pProcessName = (char *)Plugingetvalue(VAL_PROCESSNAME) ;
            strcpy_s(szUddFileName, MAX_PATH, pProcessName) ;
            PathAddExtensionA(szUddFileName, ".udd") ;
            
            // �õ�od·��
            GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH) ;
            PathRemoveFileSpecA(szModuleFileName) ;

            // ƴ��uddȫ·��
            strcpy_s(g_szUDDFileName, MAX_PATH, szModuleFileName) ;
            PathAppendA(g_szUDDFileName, "UDD") ;
            PathAppendA(g_szUDDFileName, szUddFileName) ;

            OutputDebugStringA("UDD Path:") ;
            OutputDebugStringA(g_szUDDFileName) ;
            OutputDebugStringA("\r\n") ;
        }
    }

    return ;
}