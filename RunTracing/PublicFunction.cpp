//*******************************************************************************
//  
//  Copyright (c) all 2011 EvilKnight All rights reserved
//  FileName : PublicFunction.cpp
//  D a t e  : 2011.9.8
//  作   者  : 黄 奇
//  版   本  : 0.1
//  功   能  : 
//  说   明  : 一些公有函数实现，与界面无关的
//  备   注  :
//
//  修改记录 :
//  日    期        版本     修改人               修改内容
//  YYYY/MM/DD      X.Y      <作者或修改者名>     <修改内容>
//
//*****************************************************************************/
#include "stdafx.h"
#include "PublicFunction.h"

/*******************************************************************************
*
*  函 数 名 : RemoveSelfFromLdr
*  功能描述 : 将Dll从用户模块列表中移除
*  参数列表 : DllBase       --     Dll加载地址
*  说    明 : 
*  返回结果 : 直接返回TRUE，否则返回FALSE
*
*******************************************************************************/
BOOL RemoveSelfFromLdr(PVOID DllBase)
{
    NULLVALUE_CHECK(DllBase, RemoveSelfFromLdr) ;

    typedef struct LIST_ENTRY
    {
        struct LIST_ENTRY *Flink; // 指向下一个
        struct LIST_ENTRY *Blink; // 指向上一个
    } 	LIST_ENTRY, *PLIST_ENTRY;

    PLDR_DATA_TABLE_ENTRY pLdr = NULL ;
    PLIST_ENTRY pCurrent = NULL ;

    __asm
    {
        // NT!_TIB
        mov eax, fs:[0x18]
        // 取得peb
        mov eax, [eax+0x30]
        // 再去取ldr的值
        mov eax, [eax+0xc]
        mov pLdr, eax
        // 取得InLoadOrderModuleList
        mov eax,[eax + 0xc]
        mov pCurrent, eax
    }

    PLIST_ENTRY pListStart = pCurrent ;


    for (pCurrent = pCurrent->Flink ; pCurrent != pListStart; pCurrent = pCurrent->Flink)
    {
        PLDR_DATA_TABLE_ENTRY pstEntry = CONTAINING_RECORD(pCurrent, 
            LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) ;

        // 找到的话
        if (DllBase == pstEntry->DllBase)
        {
            // 取得自身的位置
            PLIST_ENTRY pItem = pCurrent->Blink->Flink ;
            // 将当前上一个指向当前下一个
            pCurrent->Blink->Flink = pCurrent->Flink ;
            // 将当前下一个指向当前上一个
            pCurrent->Flink->Blink = pCurrent->Blink ;
            // 可能会蓝屏
            pCurrent->Blink = pCurrent->Flink = pItem ;
            return TRUE ;
        }
    }
    return FALSE ;
}

/*******************************************************************************
*
*  函 数 名 : IsHexString
*  功能描述 : 判断是否是十六进制字符串
*  参数列表 : str       --     输入字符串
*  说    明 : 
*  返回结果 : 如果输入字符串为正确的十六进制字符的话，返回TRUE，否则返回FALSE
*
*******************************************************************************/
BOOL IsHexString(CString &str)
{
    // 先判断是不是以0x开头的
    int i = 0 ;
    str.Trim() ;
    int nCount = str.GetLength() ;
    // 过滤掉前面的0x
    if (nCount >= 2)
    {
        if ('0' == str[0])
        {
            if ('x' == str[1] || 'X' == str[1])
            {
                i = 2 ;
            }
        }
    }

    // 判断另外的是否是正确的16进制字符
    for (; i < nCount; ++i)
    {
        if (false == isxdigit(str[i]))
        {
            return FALSE ;
        }
    }

    return TRUE ;    
}

//*******************************************************************************
//
//  函 数 名 : StringToDWORD
//  功能描述 : 将十六进制字符串转为DWORD
//  参数列表 : str       --     输入字符串
//  说    明 : 
//  返回结果 : 返回DWORD值，函数并没有对str正确性做判断
//
//=============================================================================
//
//  修改记录:
//  日   期       版本    修改人              修改内容
//  2011.09.06    0.1     黄 奇               描述修改内容
//
//******************************************************************************
DWORD StringToDWORD(CString &str)
{
    DWORD dwResult = 0;

    sscanf_s(str.GetBuffer(0), _T("%x"), &dwResult) ;
    return dwResult ;
}


/*******************************************************************************
*
*  函 数 名 : GetOEP
*  功能描述 : 取得程序入口地址
*  参数列表 : dwImageBase       --     程序映像基础
*  说    明 : 函数并没有对程序正确性做检测
*  返回结果 : 如果成功返回程序OEP,否则返回0
*
*******************************************************************************/
DWORD GetOEP(DWORD dwImageBase)
{
    DWORD dwOEP = 0 ;
    char szBuffer[1024] ;
    if(0 == Readmemory(szBuffer, dwImageBase, 1024, 0))
    {
        return dwOEP ;
    }

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)szBuffer ;

    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pDosHeader->e_lfanew + szBuffer) ;
    
    dwOEP = pNtHeader->OptionalHeader.AddressOfEntryPoint + dwImageBase ; 
    return dwOEP ;
}

/*******************************************************************************
*
*  函 数 名 : EnumerateFunctionAddress
*  功能描述 : 枚举函数地址,并下断点
*  参数列表 : pRunData       --     RunData指针
*  说    明 : 
*  返回结果 : 如果输入字符串为正确的十六进制字符的话，返回TRUE，否则返回FALSE
*
*******************************************************************************/
BOOL  EnumerateFunctionAddress(PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, EnumerateFunctionAddress) ;

    ulong uStart,uEnd ,uCurrent;
    char szBuffer[MAX_PATH] = {0} ;

    for (uCurrent = pRunData->uStartAddress;
                uCurrent < pRunData->uEndAddress; )
    {
        if(-1 == Getproclimits(uCurrent, &uStart, &uEnd))
        {
            uCurrent += 5 ;
        }
		
        else
        {
            // 这里再处理
            sprintf_s(szBuffer, sizeof(szBuffer), _T("%x-%x\r\n"),uStart, uEnd) ;
            OutputDebugString(szBuffer) ;
            uCurrent = uEnd + 5;

            // 判断是否是强制下断点
            // 如果不是强制下断点的话
            if (FALSE == pRunData->bIsForceSetBreakPoint)
            {
                if(FALSE == SetCallBreakPoint(pRunData, uStart, uEnd))
                {
                    OutputDebugString(_T("EnumerateFunctionAddress SetCallBreakPoint failed!\r\n")) ;
                    return FALSE ;
                }
            }
            // 强制下断点
            else
            {
                if (FALSE == ForceSetCallBreakPoint(pRunData, uStart, FUN_SIZE))
                {
                    OutputDebugString(_T("EnumerateFunctionAddress ForceSetCallBreakPoint failed!\r\n")) ;
                    return FALSE ;
                }
            }
        }
    }
    return TRUE ;
}

//*******************************************************************************
//
//  函 数 名 : SetCallBreakPoint
//  功能描述 : 在地址范围内下断点
//  参数列表 : pRunData       --     RunData指针
//             uFunctionStart --     函数起始地址
//             uFunctionEnd   --     函数结束地址
//  说    明 : 并没有对函数的正确性做检查
//  返回结果 : 成功返回TRUE,失败返回FALSE
//
//******************************************************************************
BOOL  SetCallBreakPoint(PRunData pRunData,
                        ulong uFunctionStart,
                        ulong uFunctionEnd)
{
    char szSrcDec[1024] = {0};
    char szLine[1024] = {0};
    char szName[TEXTLEN] = {0} ;
    t_disasm disasm = {0};
    ulong uIndex = uFunctionStart ;
    ulong uSize = 0 ;
    ulong uInstructSize = 0 ;

    InitTreeHead(pRunData) ;

    // 保留函数起始地址
    pRunData->pCurrentNode->dwFuncStart = uFunctionStart ;
    // 保留函数结束地址
    pRunData->pCurrentNode->dwFuncEnd = uFunctionEnd ;

    for (; uIndex < uFunctionEnd; uIndex += uInstructSize)
    {
        uSize = Readcommand(uIndex, szLine) ;
        // 再判断是不是call
        // 反汇编二进制指令
        uInstructSize = Disasm((uchar *)szLine, uSize, uIndex, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);
        if (StrStrI(disasm.result, "CALL")
            || StrStrI(disasm.result, "ret"))
        {
            // 是call直接下断点，方便日后api记录
            if(0 == Setbreakpointext(uIndex,TY_ACTIVE,0,0))
            {
                // 先判断断点是否存在，不存在的话丢进链表中去
                if(FALSE == ListExist(&pRunData->BreakPointList, uIndex))
                {
                    pRunData->BreakPointList.push_back(uIndex) ;
                }
            }
        }
    }

    // 判断当前设的断点是不是在显示屏幕中，是的话刷新
    ulong uBase = 0 ;
    Getdisassemblerrange(&uBase, &uSize) ;
    if (uFunctionStart >= uBase && uFunctionStart <= (uBase + uSize))
    {
        Redrawdisassembler() ;
    }
    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : GetFunctionEnd
*  功能描述 : 取得函数的结束地址
*  参数列表 : uAddr       --     起始地址
*  说    明 : 并对没有对uAddr值做检查，如果他处于一个指令中间的话...
*  返回结果 : 成功返回函数结尾地址，否则返回0
*
*******************************************************************************/
ulong GetFunctionEnd(ulong uAddr)
{
    char szSrcDec[1024] = {0};
    char szLine[1024] = {0};
    t_disasm disasm = {0};
    ulong uIndex = uAddr ;
    ulong uSize = 0 ;
    ulong uInstructSize = 0 ;
    ulong uFunctionEnd = uAddr + MAXFUNSIZE ;


    for (; uIndex < uFunctionEnd; uIndex += uInstructSize)
    {
        uSize = Readcommand(uIndex, szLine) ;
        // 再判断是不是call
        // 反汇编二进制指令
        uInstructSize = Disasm((uchar *)szLine, uSize, uIndex, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);
        if (StrStrI(disasm.result, "ret"))
        {
            return uIndex + uInstructSize ;
        }
    }
    return 0 ;

}

/*******************************************************************************
*
*  函 数 名 : ForceSetCallBreakPoint
*  功能描述 : 强制在指定函数起码地址下断点
*  参数列表 : pRunData       --     RunData指针
*             uFunctionStart --     函数起始地址
*             nMaxLen        --     函数最大长度
*  说    明 : 并没有对函数的正确性做检查
*  返回结果 : 成功返回TRUE,失败返回FALSE
*
*******************************************************************************/
BOOL  ForceSetCallBreakPoint(PRunData pRunData,
                             ulong uFunctionStart,
                             ulong nMaxLen)
{
    NULLVALUE_CHECK(pRunData, ForceSetCallBreakPoint) ;
    NULLVALUE_CHECK(uFunctionStart, ForceSetCallBreakPoint) ;
    NULLVALUE_CHECK(nMaxLen, ForceSetCallBreakPoint) ;

    char szSrcDec[1024] = {0};
    char szLine[1024] = {0};
    char szName[TEXTLEN] = {0} ;
    t_disasm disasm = {0};
    ulong uIndex = uFunctionStart ;
    ulong uSize = 0 ;
    ulong uInstructSize = 0 ;
    ulong uFunctionEnd = uFunctionStart + nMaxLen ;
    int nType = 0 ;

    InitTreeHead(pRunData) ;

    pRunData->pCurrentNode->dwFuncStart = uFunctionStart ;

    for (; uIndex < uFunctionEnd; uIndex += uInstructSize)
    {
        uSize = Readcommand(uIndex, szLine) ;
        // 再判断是不是call
        // 反汇编二进制指令
        uInstructSize = Disasm((uchar *)szLine, uSize, uIndex, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);
        nType = 0 ;
        if (StrStrI(disasm.result, "CALL "))
        {
            nType = 1 ;
        }
        else if (StrStrI(disasm.result, "ret"))
        {
            nType = 2 ;
        }

        if (0 != nType)
        {
            // 是call直接下断点，方便日后api记录
            if(0 == Setbreakpointext(uIndex,TY_ACTIVE,0,0))
            {
                // 先判断断点是否存在，不存在的话丢进链表中去
                if(FALSE == ListExist(&pRunData->BreakPointList, uIndex))
                {
                    pRunData->BreakPointList.push_back(uIndex) ;
                }
            }
            // 如果是ret的话,直接break 
            if (2 == nType)
            {
                // 保留函数结束地址
                pRunData->pCurrentNode->dwFuncEnd = disasm.ip ;
                break ;
            }
        }
    }

    // 判断当前设的断点是不是在显示屏幕中，是的话刷新
    ulong uBase = 0 ;
    Getdisassemblerrange(&uBase, &uSize) ;
    if (uFunctionStart >= uBase && uFunctionStart <= (uBase + uSize))
    {
        Redrawdisassembler() ;
    }
    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : ListExist
*  功能描述 : 在list中查找指定键值
*  参数列表 : pList    --  指定查找的list
*             uValue   --  要查找的键值
*  说    明 : 
*  返回结果 : 如果pList为空或者查找的键值不存在，返回FALSE，否则返回TRUE
*
*******************************************************************************/
BOOL ListExist(list<unsigned int> *pList, unsigned int uValue)
{
    NULLVALUE_CHECK(pList, ListExist) ;

    list<unsigned int>::iterator it ;
    for (it = pList->begin(); it != pList->end() ; ++it)
    {
        if (uValue == *it)
        {
            return TRUE ;
        }
    }
    return FALSE ;
}


/*******************************************************************************
*
*  函 数 名 : RemoveValueByList
*  功能描述 : 将uAddr0至uAddr1之间的值全部从pList中移除
*  参数列表 : uAddr0    --  起始值
*             uAddr1    --  结束值
*  说    明 : 
*  返回结果 : 如果pList为空或者查找的键值不存在，返回FALSE，否则返回TRUE
*
*******************************************************************************/
BOOL RemoveValueByList(list<unsigned int> *pList, 
                       unsigned int uAddr0,
                       unsigned int uAddr1)
{
    NULLVALUE_CHECK(pList, RemoveValueByList) ;

    // 先判断一下uAddr0和uAddr1的正确性
    // 如果结束地址小于开始地址的话，交换二个值
    if (uAddr1 < uAddr0)
    {
        uAddr0 ^= uAddr1 ;
        uAddr1 ^= uAddr0 ;
        uAddr0 ^= uAddr1 ;
    }
    // 如果二个值相同的话，直接去链表中移除便可
    else if (uAddr0 == uAddr1)
    {
        pList->remove(uAddr0) ;
        return TRUE ;
    }

    // 先将区段内的值保存在另一个链表中，然后再依次删除
    // 因为在测试中，发现一边遍历一边删除会有问题
    list<unsigned int>MyList ;

    // 先查找区段内的，然后保存起来
    list<unsigned int>::iterator it ;
    for (it = pList->begin(); it != pList->end() ; ++it)
    {
        if (*it >= uAddr0 && *it <= uAddr1)
        {
            MyList.push_back(*it) ;
        }
    }

    // 依次从链表中移除
    for (it = MyList.begin(); it != MyList.end() ; ++it)
    {
        pList->remove(*it) ;
    }
    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : DestroyTree
*  功能描述 : 释放树资源
*  参数列表 : pTree    --  树根
*  说    明 : 根节点不释放
*  返回结果 : 如果成功返回TRUE，否则返回FALSE
*
*******************************************************************************/
BOOL DestroyTree(PTreeNode pTree)
{
    NULLVALUE_CHECK(pTree, DestroyTree) ;

    list<PTreeNode> TreeList ;
    PTreeNode pPater = NULL ;
    PTreeNode pNode = NULL  ;

    // 自己用队列保存
    TreeList.push_back(pTree) ;

    // 如果队列不为空
    while (! TreeList.empty())
    {
        pPater = TreeList.front() ;
        TreeList.pop_front() ;

        // 将儿子结点全丢进队列
        while(! pPater->list.empty())
        {
            pNode = pPater->list.front() ;
            pPater->list.pop_front() ;

            // 如果没有儿子，直接删掉
            if (pNode->list.empty())
            {
                delete pNode ;
            }
            // 有儿子的，将儿子丢进列队里面去
            else
            {
                TreeList.push_back(pNode) ;
            }          
        }
        // 再将父结点删除
        delete pPater ;
    }

    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : InitTreeHead
*  功能描述 : 初始化树根节点
*  参数列表 : pRunData    --  指向RunData
*  说    明 : 根节点不释放
*  返回结果 : 如果成功返回TRUE，否则返回FALSE
*
*******************************************************************************/
BOOL InitTreeHead(PRunData pRunData)
{
    PTreeNode pNode = NULL ;
 
    NULLVALUE_CHECK(pRunData, InitTreeHead) ;

    if( NULL == pRunData->pTreeHead)
    {
        pNode = new TreeNode ;
        if (NULL == pNode)
        {
            OutputDebugString(_T("InitTreeHead new TreeNode failed!\r\n")) ;
            return FALSE ;
        }

        //InitTreeNode(pNode) ;
        pRunData->pTreeHead = pNode ;
        pNode->dwAddress = pRunData->uStartAddress ;
        pRunData->pCurrentNode = pNode ;
        return TRUE ;
    }

    return FALSE ;
}

/*******************************************************************************
*
*  函 数 名 : SaveEnvironment
*  功能描述 : 保存环境、以及申请树节点
*  参数列表 : pRunData --  指向RunData结构
*             uAddr    --  要跳去的地方
*             str      --  节点描述
*  说    明 : 会申请新的树的节点，并设为当前可用的
*  返回结果 : 如果成功返回TreeNode的地址，否则返回0
*
*******************************************************************************/
PTreeNode SaveEnvironment(PRunData pRunData, ulong uAddr, CString &str)
{
    NULLVALUE_CHECK(uAddr, SaveEnvironment) ;
    NULLVALUE_CHECK(pRunData, SaveEnvironment) ;

    if (NULL == pRunData->pTreeHead)
    {
        InitTreeHead(pRunData) ;
    }

    
    PTreeNode pNode = NULL ;

    // 先判断这个地址是否存在链表最后
    if (NULL != pRunData->pCurrentNode
        && pRunData->pCurrentNode->list.size() > 0)
    {
        pNode = pRunData->pCurrentNode->list.back() ;
        if (pNode->dwAddress == uAddr)
        {
            pNode->dwSequenceCallCount++;
        }
        else
        {
            pNode = NULL ;
        }
    }


    if (NULL == pNode)
    {
        // 申请新的节点
        pNode = new TreeNode ;
        NULLVALUE_CHECK(pNode, SaveEnvironment) ;
        //InitTreeNode(pNode) ;

        // 赋值
        pNode->dwAddress = uAddr ;
        pNode->strComment = str ;
        // 将新申请的设为pCurrentNode的子结点
        pRunData->pCurrentNode->list.push_back(pNode) ;
    }

    // 这里是这个函数的关键
    // 将原来的树节点压入栈中
    pRunData->TreeNodeStack.push(pRunData->pCurrentNode) ;

    // 将pCurrentNode指向新申请的
    pRunData->pCurrentNode = pNode ;

    // 入栈
    pRunData->CallStack.push(uAddr) ;

    return pNode ;
}

/*******************************************************************************
*
*  函 数 名 : ResumeEnvironment
*  功能描述 : 恢复环境
*  参数列表 : uAddr    --  要跳去的地方
*  说    明 : 
*  返回结果 : 如果成功返回原来地址值，否则返回0
*
*******************************************************************************/
ulong ResumeEnvironment(PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, ResumeEnvironment) ;

    ulong uResult = 0 ;

    // 取得当前函数的首地址
    // 然后从栈中移除
    if (pRunData->CallStack.empty())
    {
        OutputDebugString(_T("pRunData->CallStack 为空，请检查函数调用是否正确!\r\n")) ;
        return 0 ;
    }
    uResult = pRunData->CallStack.top() ;
    pRunData->CallStack.pop() ;

    // pCurrentNode回滚到上一层的
    pRunData->pCurrentNode = pRunData->TreeNodeStack.top() ;
    pRunData->TreeNodeStack.pop() ;

    return uResult ;
}

/*******************************************************************************
*
*  函 数 名 : AppendCall
*  功能描述 : 添加调用
*  参数列表 : pRunData --  指向RunData结构
*             uAddr    --  要跳去的地方
*             str      --  节点描述
*  说    明 : 与SaveEnvironment不同之处是不会修改pCurrentNode
*  返回结果 : 如果成功返回原来地址值，否则返回0
*
*******************************************************************************/
PTreeNode AppendCall(PRunData pRunData, ulong uAddr, CString &str)
{
    NULLVALUE_CHECK(uAddr, AppendCall) ;
    NULLVALUE_CHECK(pRunData, AppendCall) ;

    // 申请新的节点
    PTreeNode pNode = NULL ;

    if (NULL == pRunData->pTreeHead)
    {
        InitTreeHead(pRunData) ;
    }

    // 先判断这个地址是否存在链表最后
    if (NULL != pRunData->pCurrentNode
        &&pRunData->pCurrentNode->list.size() > 0)
    {
        pNode = pRunData->pCurrentNode->list.back() ;
        if (pNode->dwAddress == uAddr)
        {
            pNode->dwSequenceCallCount++;
            return pNode ;
        }
        pNode = NULL ;
    }

    pNode = new TreeNode ;
    NULLVALUE_CHECK(pNode, AppendCall) ;
    //InitTreeNode(pNode) ;

    pNode->dwAddress = uAddr ;
    pNode->strComment = str ;

    // 将新申请的设为pCurrentNode的子结点
    pRunData->pCurrentNode->list.push_back(pNode) ;

    return pNode ;
}

/*******************************************************************************
*
*  函 数 名 : GetCallRegAddress
*  功能描述 : 取得call寄存器 这样的汇编 寄存器所包含的值
*  参数列表 : pDisasm --  指向RunData结构
*             pReg    --  要跳去的地方
*  说    明 : 
*  返回结果 : 如果成功返回原来地址值，否则返回0
*
*******************************************************************************/
ulong GetCallRegAddress(t_disasm *pDisasm, t_reg *pReg)
{
    NULLVALUE_CHECK(pDisasm, GetCallRegAddress) ;
    NULLVALUE_CHECK(pReg,    GetCallRegAddress) ;

    ulong uResult = 0 ;
    int   nRegIndex = -1 ;

    if (StrStrI(pDisasm->result, "eax"))
    {
        nRegIndex = REG_EAX ;
    }
    else if (StrStrI(pDisasm->result, "ebx"))
    {
        nRegIndex = REG_EBX ;
    }
    else if (StrStrI(pDisasm->result, "ecx"))
    {
        nRegIndex = REG_ECX ;
    }
    else if (StrStrI(pDisasm->result, "edx"))
    {
        nRegIndex = REG_EDX ;
    }
    else if (StrStrI(pDisasm->result, "esi"))
    {
        nRegIndex = REG_ESI ;
    }
    else if (StrStrI(pDisasm->result, "edi"))
    {
        nRegIndex = REG_EDI ;
    }
    else if (StrStrI(pDisasm->result, "ebp"))
    {
        nRegIndex = REG_EBP ;
    }
    else if (StrStrI(pDisasm->result, "esp"))
    {
        nRegIndex = REG_ESP ;
    }


    if (-1 != nRegIndex)
    {
        uResult = pReg->r[nRegIndex] ;
    }


    return uResult ;
}

/*******************************************************************************
*
*  函 数 名 : RemoveAllBreakPoint
*  功能描述 : 移除所有的断点
*  参数列表 : 无
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
BOOL RemoveAllBreakPoint(void)
{
    // 删除所有的断点
    Deletebreakpoints(0x40000, 0x80000000, 0);

    // 函数发送消息到所有已打开的 MDI 窗口
    // 当修改了调试程序或者显示选项后，OllyDbg 的窗口必需要及时更新，其窗口处理
    // 函将推迟重新使用实际数据，并且返回 CONT_BROADCAST。
    Broadcast(WM_USER_CHALL,0,0);
    return TRUE ;
}


/*******************************************************************************
*
*  函 数 名 : SetASMCpu
*  功能描述 : 设置反汇编窗口的地址
*  参数列表 : 无
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
BOOL SetASMCpu(DWORD dwAddr)
{
    Setcpu(Getcputhreadid(), dwAddr, 0,0, CPU_REDRAW | CPU_ASMFOCUS) ;
    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : CheckType
*  功能描述 : 检查内存类型
*  参数列表 : 无
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
inline BOOL CheckType(ulong uType, ulong uStart, ulong uEnd)
{
    TCHAR szBuf[MAX_PATH] = {0} ;
    if (0 == (TY_CODE & (uType)))
    { 
        _snprintf_s(szBuf, MAX_PATH, _T("你输入的地址%p - %p 中不包含代码，是否继续?"),
                    (int *)(uStart),
                    (int *)(uEnd)) ;
        if (IDNO == AfxMessageBox(szBuf, MB_YESNO))
        {
            return FALSE ;
        }
    }

    if ((uType) & ~TY_CODE)
    {
        _snprintf_s(szBuf, MAX_PATH, _T("你输入的地址%p - %p 中包含其它数据，是否继续?"), 
                    (int *)(uStart),
                    (int *)(uEnd)) ;
        if (IDNO == AfxMessageBox(szBuf, MB_YESNO))
        {
            return FALSE ;
        }
    }
}

/*******************************************************************************
*
*  函 数 名 : GetDebugProcessName
*  功能描述 : 取得被调戏进程的进程名
*  参数列表 : pOutBuffer        --    取出buffer
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
BOOL GetDebugProcessName(char *pOutBuffer)
{
    NULLVALUE_CHECK(pOutBuffer, BackupUDD) ;  

    char szProcessName[MAX_PATH] = {0} ;

    // 取得被调试程序名字
    char *pProcessName = (char *)Plugingetvalue(VAL_PROCESSNAME) ;
    if (NULL == pProcessName)
    {
        OutputDebugString(_T("GetDebugProcessName Plugingetvalue failed")) ;
        return FALSE ;
    }

    strcpy_s(pOutBuffer, MAX_PATH, pProcessName) ;

    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : GetDebugPragramUDDPath
*  功能描述 : 取得od udd目录
*  参数列表 : pOutBuffer        --    取出buffer
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
BOOL GetDebugPragramUDDPath(char *pOutBuffer)
{
    NULLVALUE_CHECK(pOutBuffer, BackupUDD) ;
    char szOdPath[MAX_PATH] = {0} ;

    // 取得od路径
    if(0 == GetModuleFileNameA(NULL, szOdPath, MAX_PATH))
    {
        OutputDebugString(_T("GetDebugPragramUDDPath GetModuleFileNameA failed")) ;
        return FALSE ;
    }

    // 构造UDD路径
    // 处理中路径，去文件名
    PathRemoveFileSpecA(szOdPath) ;
    PathAppendA(szOdPath, "UDD") ;

    strcpy_s(pOutBuffer, MAX_PATH, szOdPath) ;
    return TRUE ;
}

/*******************************************************************************
*
*  函 数 名 : BackupUDD
*  功能描述 : 备份udd文件
*  参数列表 : 无
*  说    明 : 
*  返回结果 : 如果成功返回TRUE，失败返回FALSE
*
*******************************************************************************/
BOOL BackupUDD(char *pDestPath)
{
    NULLVALUE_CHECK(pDestPath, BackupUDD) ;

    char szSourcePath[MAX_PATH] = {0} ;
    char szDestPath[MAX_PATH] = {0} ;
    char szProcessName[MAX_PATH] = {0} ;

    strcpy_s(szDestPath, MAX_PATH, pDestPath) ;

    if (FALSE == GetDebugPragramUDDPath(szSourcePath))
    {
        OutputDebugString(_T("BackupUDD GetDebugPragramUDDPath failed")) ;
        return FALSE ;
    }

    if (FALSE == GetDebugProcessName(szProcessName))
    {
        OutputDebugString(_T("BackupUDD GetDebugProcessName failed")) ;
        return FALSE ;
    }

    PathAddExtensionA(szProcessName, ".udd") ;

    PathAppendA(szSourcePath, szProcessName) ;
    PathAppendA(szDestPath,szProcessName) ;

    if(FALSE == CopyFile(szSourcePath, szDestPath, FALSE))
    {
        MessageBox(NULL, _T("备份UDD文件错误"), _T("Tips"), MB_OK) ;
        return FALSE ;
    }
    return TRUE ;
}

#ifdef _DEBUG
BOOL TestIsHexString()
{
    CString str ;

    str = _T("01234567") ;
    if (FALSE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"01234567\")")) ;
        return FALSE ;
    }

    str = _T("0x123456") ;
    if (FALSE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"0x123456\")")) ;
        return FALSE ;
    }

    str = _T("0xabcdef") ;
    if (FALSE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"0xabcdef\")")) ;
        return FALSE ;
    }

    str = _T("0xabcdefg") ;
    if (TRUE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"0xabcdefg\")")) ;
        return FALSE ;
    }

    str = _T("0xabcdefgh") ;
    if (TRUE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"0xabcdefgh\")")) ;
        return FALSE ;
    }

    str = _T("0xabcdefgh12") ;
    if (TRUE == IsHexString(str))
    {
        AfxMessageBox(_T("str = _T(\"0xabcdefgh12\")")) ;
        return FALSE ;
    }

    OutputDebugString(_T("TestIsHexString pass !\r\n")) ;
    return TRUE ;
}

BOOL TestStringToDWORD()
{
   CString str ;
    DWORD dwResult = 0 ;
    DWORD dwValue = 0x1234567 ;

    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0x1234567 Error")) ;
        return FALSE ;
    }

    dwValue = 0x1 ;
    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0x1 Error")) ;
        return FALSE ;
    }

    dwValue = 0x2 ;
    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0x0 Error")) ;
        return FALSE ;
    }

    dwValue = 0xff ;
    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0xff Error")) ;
        return FALSE ;
    }

    dwValue = 0xfe ;
    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0xfe Error")) ;
        return FALSE ;
    }

    dwValue = 0xcc ;
    str.Format(_T("%x"), dwValue) ;
    dwResult = StringToDWORD(str) ;
    if (dwValue != dwResult)
    {
        AfxMessageBox(_T("dwValue = 0xcc Error")) ;
        return FALSE ;
    }

    OutputDebugString(_T("TestStringToDWORD pass !\r\n")) ;
    return TRUE ;
}

BOOL TestListExist()
{
    list<unsigned int >list ;
    TCHAR szBuffer[MAX_PATH] ={0} ;
    int i = 1 ;

    for (; i < 20; ++i)
    {
        list.push_back(i) ;
    }

    if (TRUE == ListExist(NULL, 0))
    {
        OutputDebugString("TestListExist ListExist(NULL, 0) failed\r\n") ;
        return FALSE ;
    }

    for (i = 1; i < 20; ++i)
    {
        if (FALSE == ListExist(&list, i))
        {
            sprintf_s(szBuffer, MAX_PATH * sizeof(TCHAR), _T("TestListExist ListExist(&list, %d) failed\r\n"), i) ;
            OutputDebugString(szBuffer) ;
            return FALSE ;
        }
    }

    for (; i < 40; ++i)
    {
        if (TRUE == ListExist(&list, i))
        {
            sprintf_s(szBuffer, MAX_PATH * sizeof(TCHAR), _T("TestListExist ListExist(&list, %d) failed\r\n"), i) ;
            OutputDebugString(szBuffer) ;
            return FALSE ;
        }
    }

    OutputDebugString(_T("TestListExist pass !\r\n")) ;
    return TRUE ;
}

BOOL TestDestroyTree()
{
    PTreeNode pTreeHead = NULL;
    PTreeNode  pNode = NULL ;
    PTreeNode  pChild = NULL ;
    int sum = 0 ;
    int j = 0 ;

    pTreeHead = new TreeNode ;
    NULLVALUE_CHECK(pTreeHead, TestDestroyTree) ;
    //InitTreeNode(pTreeHead) ;

    // 添加树节点
    int i = 0 ;
    for (; i < 3; ++i)
    {
        pNode = new TreeNode ;
        //InitTreeNode(pNode) ;
        pNode->dwAddress = sum++ ;
        
        pTreeHead->list.push_back(pNode) ;

        pChild = new TreeNode ;
        //InitTreeNode(pChild) ;
        pChild->dwAddress = sum++ ;

        pNode->list.push_back(pChild) ;
    }

    DestroyTree(pTreeHead) ;
    OutputDebugString(_T("TestDestroyTree pass !\r\n")) ;
    return TRUE ;
}

BOOL TestRemoveValueByList()
{
    list<unsigned int >list ;
    TCHAR szBuffer[MAX_PATH] ={0} ;
    int i = 1 ;

    for (; i <= 20; ++i)
    {
        list.push_back(i) ;
    }

    for (i = 1; i <= 20; ++i)
    {
        RemoveValueByList(&list, i, i) ;
        if (20 - i != list.size())
        {
            sprintf_s(szBuffer, MAX_PATH * sizeof(TCHAR), _T("TestRemoveValueByList RemoveValueByList(&list, %d, %d) failed\r\n"), i, i) ;
            OutputDebugString(szBuffer) ;
            return FALSE ;
        }
    }

    for (i = 1; i <= 40; ++i)
    {
        list.push_back(i) ;
    }

    RemoveValueByList(&list, 1, 10) ;
    if (30 != list.size())
    {
        sprintf_s(szBuffer, MAX_PATH * sizeof(TCHAR), _T("TestRemoveValueByList RemoveValueByList(&list, 1, 10) failed\r\n")) ;
        OutputDebugString(szBuffer) ;
        return FALSE ;
    }

    OutputDebugString(_T("TestListExist pass !\r\n")) ;
    return TRUE ;
}
#endif