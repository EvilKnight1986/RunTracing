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
// 函数名: ODBG_Plugindata
// 权  限: public 
// 返回值: 返回插件版本号
// 参  数: char shortname[32]
// 说  明: 这个是必需的，用于指定插件的名字
// 合  格:
//******************************************************************************
extc int _export cdecl ODBG_Plugindata(char shortname[32])
{
    // Name of plugin
    strcpy_s(shortname, 32, PLUGINNAME) ;
    return PLUGIN_VERSION ;
}


//******************************************************************************
// 函数名: ODBG_Plugininit
// 权  限: public 
// 返回值: 这个函数在成功时必须返回 0，而在失败时，须释放资源并返回-1。
// 参  数: ollydbgversion   -- OllyDbg的版本号,用于与前面返回的PLUGIN_VERSION进行比较
// 参  数: hw               -- OllyDbg 主窗口的句柄，一般将其保存到一个全局变量中
// 参  数: features         -- 为将来的扩展而保留，不使用它
// 说  明: 它用于初始化插件。一般会在其中包含初始化工作并分配资源
// 合  格:
//******************************************************************************
extc int _export cdecl ODBG_Plugininit( int ollydbgversion, HWND hw, ulong * features) 
{ 
    if(ollydbgversion < PLUGIN_VERSION)
    {
        return -1 ;
    }

    // 我们使用 Addtolist()函数将插件加载信息输出到记录窗口（log window）,
    // 在 OllyDbg 中，按下Alt+L 可以查看该窗口。这个函数有三个参数，其中第二
    // 的值取 0、1、-1 分别表示正常显示、高亮、低亮。OllyDbg 作者推荐使用代
    // 码中所示两行记录格式，第一行说明插件名称和版本，第二行说明版权信息。 
    Addtolist(0,0,PLUGINVERSIONINFOR); 
    Addtolist(0,-1,COPYRIGHTINFOR); 

    runData.hwnd = hw ;

//  测试相关的
//     TestIsHexString() ;
//     TestStringToDWORD() ;
//     TestListExist() ;
//     TestDestroyTree() ;
    //TestRemoveValueByList() ;
    return 0; 
}

//******************************************************************************
// 函数名: ODBG_Pluginmenu
// 权  限: public 
// 返回值: 成功时返回 1，失败时返回 0
// 参  数: int origin -- origin 参数指明了创建菜单的命令来源于何处——插件菜单、
//                       反汇编窗口、断点窗口等等
// 参  数: char data[4096] -- 定义菜单的项目和显示格式
// 参  数: void * item -- 指向显示于窗口的有序数据的选定元素，或者指向数据窗口的
//                          描述符，通过它，我们能更加细致地决定如何显示菜单
// 说  明: 用于创建插件的子菜单
// 合  格:
//******************************************************************************
extc int _export cdecl ODBG_Pluginmenu( int origin, char data[4096], void *item) 
{ 
    t_dump *ptd = NULL;

    // 插件菜单
    if(PM_MAIN == origin) 
    { 
        strcpy_s(data,4096, "0 Clear BreakPoint | 1 Option | 2 Report | 3 About"); 
        return 1; 
    } 

    if(PM_DISASM == origin) 
    { 
        ptd = (t_dump *)item; 

        // 如果无调戏进程的话，不弹出任何菜单
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
// 函数名: ODBG_Pluginaction
// 权  限: public 
// 返回值: 无
// 参  数: int origin -- origin 参数指明了创建菜单的命令来源于何处——插件菜单、
//                       反汇编窗口、断点窗口等等
// 参  数: int action -- 定义项目时的索引数值
// 参  数: void * item -- 指向显示于窗口的有序数据的选定元素，或者指向数据窗口的
//                          描述符，通过它，我们能更加细致地决定如何显示菜单
// 说  明: 用于实现各菜单项的功能
// 合  格:
//******************************************************************************
extc void _export cdecl ODBG_Pluginaction( int origin, int action, void *item) 
{ 
    t_table *pTable = NULL ;

    switch(origin)
    {
    case PM_MAIN:
    case PM_DISASM:
        {
            // 清除所有断点
            if (0 == action)
            {
                if (0 != Plugingetvalue(VAL_PROCESSID))
                {
                    RemoveAllBreakPoint() ;
                }
                break ;
            }
            // 如果是点了Option
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
                    AfxMessageBox(_T("您之前的设置已经生效!")) ;
                }
                break; 
            }
            // 如果点了report
            else if(2 == action)
            {
                CShowInformation ShowInfor ;
                //此语句用来正确地切换MFC模块状态
                AFX_MANAGE_STATE(AfxGetStaticModuleState());
                ShowInfor.SetUserData(&runData) ;
                ShowInfor.DoModal() ;
                break ;
            }
            // 如果点了About
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
// 函数名: ODBG_Paused
// 权  限: public 
// 返回值: 如果返回1,禁止窗口重绘，否则返回0
// 参  数: int reason -- 应用程序暂停原因
//                       PP_EVENT	暂停于调试事件
//                       PP_PAUSE	暂停于用户请求
//                       PP_TERMINATED	应用程序终止
// 参  数: t_reg *reg -- 指向已暂停应用程序的线程寄存器，可为NULL
// 说  明: OllyDbg 在被调试程序暂停时或一个内部进程完成时调用本函数。
// 合  格:
//******************************************************************************
extc int ODBG_Paused(int reason, t_reg *reg)
{
    if (NULL != reg)
    {
        runData.dwEIP = reg->ip ;
    }
    // 判断是不是暂停在调试异常
    if (PP_EVENT != reason)
    {
        OutputDebugString(_T("ODBG_Paused PP_EVENT != reason!\r\n")) ;
        return 1 ;
    }

    // 判断是不是我们的断点是否已经启动
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

    // 判断断点是不是自己的
    if(FALSE == ListExist(&runData.BreakPointList, reg->ip))
    {
        return 1;
    }

    //// 是自己的话进行处理，记录
    //   先取得eip处的指令
    nSize = Readcommand(reg->ip, szLine) ;
    if (0 == nSize)
    {
        OutputDebugString(_T("ODBG_Paused Readcommand failed!\r\n")) ;
        return 1 ;
    }
    // 再判断是不是call
    // 反汇编二进制指令
    Disasm((uchar *)szLine, nSize, reg->ip, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);

    uJmpAddr = disasm.jmpaddr ;

    // 如果不是call的话，判断是不是call api,是的话单步步过，否则的话再取参数，
    // 判断是否要步进
    if (StrStrI(disasm.result, "CALL "))
    {
        // 把目录地址压栈
        OutputDebugString(disasm.result) ;
        OutputDebugString("\r\n") ;

        // 留空，这里以后可以api记录
        int nType = 0 ;

        if (0 == uJmpAddr)
        {
            // 取得call寄存器 这样的汇编 寄存器所包含的jmp值
            uJmpAddr = GetCallRegAddress(&disasm, reg) ;
        }

        nType = Findlabel(uJmpAddr, szFunctionName) ;

        // 这里还要处理
        str = szFunctionName ;

        // 这里处理api参数了

        // 这里还要单独处理一下notepad的那个问题
        if (0 == nType && strstr(disasm.result, "<") && strstr(disasm.result, ">"))
        {
            //这种情况的话也是不用跟进的
            str = disasm.result ;
            AppendCall(&runData, disasm.ip, str) ;
            Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
            return 0 ;

        }

        // 再判断api类型,如果是用户函数才跟进
        switch(nType)
        {
        case NM_IMPORT:
        case NM_EXPORT:
        case NM_LIBRARY:
            //******************************************************************
            //
            // api记录
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
                // 这里再判断是不是要跟进步
                // 如果不用跟进的话
                if (FALSE == runData.bIsRecordOther 
                    && (uJmpAddr < runData.uStartAddress || uJmpAddr > runData.uEndAddress))
                {
                    AppendCall(&runData, disasm.ip, str) ;
                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
                    return 0 ;
                }
                // 保存当前的环境
                // 相当于函数调用的push esp
                // 这里还会申请树的结点
                SaveEnvironment(&runData, disasm.ip, str) ;


                ///// 目标函数内下断点
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
                    // 判断有没有开强制下断点, 有的话强制下断点
                    if(runData.bIsForceSetBreakPoint)
                    {
                        // 强制下断点了
                        ForceSetCallBreakPoint(&runData, uJmpAddr, MAXFUNSIZE) ;
                    }
                    else
                    {
                        MessageBox(NULL, _T("目标函数边界不可计算\r\n"), disasm.result, MB_OK) ;
                    }
                    */

                    // 遇到不可认的函数的时候，再给用户修改的机会，否则不停的调窗会累死的
                    if (FALSE == runData.bIsForceSetBreakPoint)
                    {
                        if(IDYES == MessageBox(NULL, _T("目标函数边界不可计算，是否开启强制下断点\r\n"), disasm.result, MB_OK))
                        {
                            runData.bIsForceSetBreakPoint = TRUE ;
                        }
                    }

                    // 判断是否强制下断点
                    if (TRUE == runData.bIsForceSetBreakPoint)
                    {
                        // 强制下断点了
                        ForceSetCallBreakPoint(&runData, uJmpAddr, MAXFUNSIZE) ;
                    }
                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
                    return 1 ;
                }
            }
            break ;
        }
    }
    // 如果是ret的话，将函数内的断点取消，再还原环境
    else if (StrStrI(disasm.result, "ret"))
    {
        OutputDebugString(disasm.result) ;
        OutputDebugString("\r\n") ;

        // 移除函数内的所有断点
        Deletebreakpoints(runData.pCurrentNode->dwFuncStart,
                          runData.pCurrentNode->dwFuncEnd + 1,
                          0) ;

        RemoveValueByList(&runData.BreakPointList,
                            runData.pCurrentNode->dwFuncStart,
                            runData.pCurrentNode->dwFuncEnd) ;

        // 出栈
        ResumeEnvironment(&runData) ;
        Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 0, VK_F9);
        return 1 ;
    }

    return 1 ;
}

//******************************************************************************
// 函数名: ODBG_Pluginreset
// 权  限: public 
// 返回值: 无
// 参  数: 无
// 说  明: OllyDbg 在用户打开或重启一个新的应用程序时调用本函数
// 合  格:
//******************************************************************************
extc void _export cdecl ODBG_Pluginreset(void)
{
    DestroyTree(runData.pTreeHead) ;
    InitRunData(&runData) ;
    
    //UnlockUDDFile() ;
}

//******************************************************************************
// 函数名: ODBG_Pluginclose
// 权  限: public 
// 返回值: 所有由插件创建的的 MDI 窗口将一直存在，这它们很可能要保存一些插件参数
//         到ini文件，如果安全退出了 OllyDbg，函数必须返回0，任何非 0 值返回将中
//          止关闭队列，不要滥用这种可能！总是通知用户终止的原因可能并不是很好的，
//          你可以询问他由他来决定如何处理。
// 说  明: 当用户想要终止 OllyDbg 时调用本函数。
// 合  格:
//******************************************************************************
extc int  _export cdecl ODBG_Pluginclose(void)
{
    DestroyTree(runData.pTreeHead) ;
    // 还原udd文件

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

            // 得到udd文件名
            char *pProcessName = (char *)Plugingetvalue(VAL_PROCESSNAME) ;
            strcpy_s(szUddFileName, MAX_PATH, pProcessName) ;
            PathAddExtensionA(szUddFileName, ".udd") ;
            
            // 得到od路径
            GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH) ;
            PathRemoveFileSpecA(szModuleFileName) ;

            // 拼出udd全路径
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