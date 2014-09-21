//*******************************************************************************
//  
//  Copyright (c) all 2011 EvilKnight All rights reserved
//  FileName : PublicFunction.cpp
//  D a t e  : 2011.9.8
//  ��   ��  : �� ��
//  ��   ��  : 0.1
//  ��   ��  : 
//  ˵   ��  : һЩ���к���ʵ�֣�������޹ص�
//  ��   ע  :
//
//  �޸ļ�¼ :
//  ��    ��        �汾     �޸���               �޸�����
//  YYYY/MM/DD      X.Y      <���߻��޸�����>     <�޸�����>
//
//*****************************************************************************/
#include "stdafx.h"
#include "PublicFunction.h"

/*******************************************************************************
*
*  �� �� �� : RemoveSelfFromLdr
*  �������� : ��Dll���û�ģ���б����Ƴ�
*  �����б� : DllBase       --     Dll���ص�ַ
*  ˵    �� : 
*  ���ؽ�� : ֱ�ӷ���TRUE�����򷵻�FALSE
*
*******************************************************************************/
BOOL RemoveSelfFromLdr(PVOID DllBase)
{
    NULLVALUE_CHECK(DllBase, RemoveSelfFromLdr) ;

    typedef struct LIST_ENTRY
    {
        struct LIST_ENTRY *Flink; // ָ����һ��
        struct LIST_ENTRY *Blink; // ָ����һ��
    } 	LIST_ENTRY, *PLIST_ENTRY;

    PLDR_DATA_TABLE_ENTRY pLdr = NULL ;
    PLIST_ENTRY pCurrent = NULL ;

    __asm
    {
        // NT!_TIB
        mov eax, fs:[0x18]
        // ȡ��peb
        mov eax, [eax+0x30]
        // ��ȥȡldr��ֵ
        mov eax, [eax+0xc]
        mov pLdr, eax
        // ȡ��InLoadOrderModuleList
        mov eax,[eax + 0xc]
        mov pCurrent, eax
    }

    PLIST_ENTRY pListStart = pCurrent ;


    for (pCurrent = pCurrent->Flink ; pCurrent != pListStart; pCurrent = pCurrent->Flink)
    {
        PLDR_DATA_TABLE_ENTRY pstEntry = CONTAINING_RECORD(pCurrent, 
            LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) ;

        // �ҵ��Ļ�
        if (DllBase == pstEntry->DllBase)
        {
            // ȡ�������λ��
            PLIST_ENTRY pItem = pCurrent->Blink->Flink ;
            // ����ǰ��һ��ָ��ǰ��һ��
            pCurrent->Blink->Flink = pCurrent->Flink ;
            // ����ǰ��һ��ָ��ǰ��һ��
            pCurrent->Flink->Blink = pCurrent->Blink ;
            // ���ܻ�����
            pCurrent->Blink = pCurrent->Flink = pItem ;
            return TRUE ;
        }
    }
    return FALSE ;
}

/*******************************************************************************
*
*  �� �� �� : IsHexString
*  �������� : �ж��Ƿ���ʮ�������ַ���
*  �����б� : str       --     �����ַ���
*  ˵    �� : 
*  ���ؽ�� : ��������ַ���Ϊ��ȷ��ʮ�������ַ��Ļ�������TRUE�����򷵻�FALSE
*
*******************************************************************************/
BOOL IsHexString(CString &str)
{
    // ���ж��ǲ�����0x��ͷ��
    int i = 0 ;
    str.Trim() ;
    int nCount = str.GetLength() ;
    // ���˵�ǰ���0x
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

    // �ж�������Ƿ�����ȷ��16�����ַ�
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
//  �� �� �� : StringToDWORD
//  �������� : ��ʮ�������ַ���תΪDWORD
//  �����б� : str       --     �����ַ���
//  ˵    �� : 
//  ���ؽ�� : ����DWORDֵ��������û�ж�str��ȷ�����ж�
//
//=============================================================================
//
//  �޸ļ�¼:
//  ��   ��       �汾    �޸���              �޸�����
//  2011.09.06    0.1     �� ��               �����޸�����
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
*  �� �� �� : GetOEP
*  �������� : ȡ�ó�����ڵ�ַ
*  �����б� : dwImageBase       --     ����ӳ�����
*  ˵    �� : ������û�жԳ�����ȷ�������
*  ���ؽ�� : ����ɹ����س���OEP,���򷵻�0
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
*  �� �� �� : EnumerateFunctionAddress
*  �������� : ö�ٺ�����ַ,���¶ϵ�
*  �����б� : pRunData       --     RunDataָ��
*  ˵    �� : 
*  ���ؽ�� : ��������ַ���Ϊ��ȷ��ʮ�������ַ��Ļ�������TRUE�����򷵻�FALSE
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
            // �����ٴ���
            sprintf_s(szBuffer, sizeof(szBuffer), _T("%x-%x\r\n"),uStart, uEnd) ;
            OutputDebugString(szBuffer) ;
            uCurrent = uEnd + 5;

            // �ж��Ƿ���ǿ���¶ϵ�
            // �������ǿ���¶ϵ�Ļ�
            if (FALSE == pRunData->bIsForceSetBreakPoint)
            {
                if(FALSE == SetCallBreakPoint(pRunData, uStart, uEnd))
                {
                    OutputDebugString(_T("EnumerateFunctionAddress SetCallBreakPoint failed!\r\n")) ;
                    return FALSE ;
                }
            }
            // ǿ���¶ϵ�
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
//  �� �� �� : SetCallBreakPoint
//  �������� : �ڵ�ַ��Χ���¶ϵ�
//  �����б� : pRunData       --     RunDataָ��
//             uFunctionStart --     ������ʼ��ַ
//             uFunctionEnd   --     ����������ַ
//  ˵    �� : ��û�жԺ�������ȷ�������
//  ���ؽ�� : �ɹ�����TRUE,ʧ�ܷ���FALSE
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

    // ����������ʼ��ַ
    pRunData->pCurrentNode->dwFuncStart = uFunctionStart ;
    // ��������������ַ
    pRunData->pCurrentNode->dwFuncEnd = uFunctionEnd ;

    for (; uIndex < uFunctionEnd; uIndex += uInstructSize)
    {
        uSize = Readcommand(uIndex, szLine) ;
        // ���ж��ǲ���call
        // ����������ָ��
        uInstructSize = Disasm((uchar *)szLine, uSize, uIndex, (uchar *)szSrcDec, &disasm, DISASM_ALL, 0);
        if (StrStrI(disasm.result, "CALL")
            || StrStrI(disasm.result, "ret"))
        {
            // ��callֱ���¶ϵ㣬�����պ�api��¼
            if(0 == Setbreakpointext(uIndex,TY_ACTIVE,0,0))
            {
                // ���ж϶ϵ��Ƿ���ڣ������ڵĻ�����������ȥ
                if(FALSE == ListExist(&pRunData->BreakPointList, uIndex))
                {
                    pRunData->BreakPointList.push_back(uIndex) ;
                }
            }
        }
    }

    // �жϵ�ǰ��Ķϵ��ǲ�������ʾ��Ļ�У��ǵĻ�ˢ��
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
*  �� �� �� : GetFunctionEnd
*  �������� : ȡ�ú����Ľ�����ַ
*  �����б� : uAddr       --     ��ʼ��ַ
*  ˵    �� : ����û�ж�uAddrֵ����飬���������һ��ָ���м�Ļ�...
*  ���ؽ�� : �ɹ����غ�����β��ַ�����򷵻�0
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
        // ���ж��ǲ���call
        // ����������ָ��
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
*  �� �� �� : ForceSetCallBreakPoint
*  �������� : ǿ����ָ�����������ַ�¶ϵ�
*  �����б� : pRunData       --     RunDataָ��
*             uFunctionStart --     ������ʼ��ַ
*             nMaxLen        --     ������󳤶�
*  ˵    �� : ��û�жԺ�������ȷ�������
*  ���ؽ�� : �ɹ�����TRUE,ʧ�ܷ���FALSE
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
        // ���ж��ǲ���call
        // ����������ָ��
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
            // ��callֱ���¶ϵ㣬�����պ�api��¼
            if(0 == Setbreakpointext(uIndex,TY_ACTIVE,0,0))
            {
                // ���ж϶ϵ��Ƿ���ڣ������ڵĻ�����������ȥ
                if(FALSE == ListExist(&pRunData->BreakPointList, uIndex))
                {
                    pRunData->BreakPointList.push_back(uIndex) ;
                }
            }
            // �����ret�Ļ�,ֱ��break 
            if (2 == nType)
            {
                // ��������������ַ
                pRunData->pCurrentNode->dwFuncEnd = disasm.ip ;
                break ;
            }
        }
    }

    // �жϵ�ǰ��Ķϵ��ǲ�������ʾ��Ļ�У��ǵĻ�ˢ��
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
*  �� �� �� : ListExist
*  �������� : ��list�в���ָ����ֵ
*  �����б� : pList    --  ָ�����ҵ�list
*             uValue   --  Ҫ���ҵļ�ֵ
*  ˵    �� : 
*  ���ؽ�� : ���pListΪ�ջ��߲��ҵļ�ֵ�����ڣ�����FALSE�����򷵻�TRUE
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
*  �� �� �� : RemoveValueByList
*  �������� : ��uAddr0��uAddr1֮���ֵȫ����pList���Ƴ�
*  �����б� : uAddr0    --  ��ʼֵ
*             uAddr1    --  ����ֵ
*  ˵    �� : 
*  ���ؽ�� : ���pListΪ�ջ��߲��ҵļ�ֵ�����ڣ�����FALSE�����򷵻�TRUE
*
*******************************************************************************/
BOOL RemoveValueByList(list<unsigned int> *pList, 
                       unsigned int uAddr0,
                       unsigned int uAddr1)
{
    NULLVALUE_CHECK(pList, RemoveValueByList) ;

    // ���ж�һ��uAddr0��uAddr1����ȷ��
    // ���������ַС�ڿ�ʼ��ַ�Ļ�����������ֵ
    if (uAddr1 < uAddr0)
    {
        uAddr0 ^= uAddr1 ;
        uAddr1 ^= uAddr0 ;
        uAddr0 ^= uAddr1 ;
    }
    // �������ֵ��ͬ�Ļ���ֱ��ȥ�������Ƴ����
    else if (uAddr0 == uAddr1)
    {
        pList->remove(uAddr0) ;
        return TRUE ;
    }

    // �Ƚ������ڵ�ֵ��������һ�������У�Ȼ��������ɾ��
    // ��Ϊ�ڲ����У�����һ�߱���һ��ɾ����������
    list<unsigned int>MyList ;

    // �Ȳ��������ڵģ�Ȼ�󱣴�����
    list<unsigned int>::iterator it ;
    for (it = pList->begin(); it != pList->end() ; ++it)
    {
        if (*it >= uAddr0 && *it <= uAddr1)
        {
            MyList.push_back(*it) ;
        }
    }

    // ���δ��������Ƴ�
    for (it = MyList.begin(); it != MyList.end() ; ++it)
    {
        pList->remove(*it) ;
    }
    return TRUE ;
}

/*******************************************************************************
*
*  �� �� �� : DestroyTree
*  �������� : �ͷ�����Դ
*  �����б� : pTree    --  ����
*  ˵    �� : ���ڵ㲻�ͷ�
*  ���ؽ�� : ����ɹ�����TRUE�����򷵻�FALSE
*
*******************************************************************************/
BOOL DestroyTree(PTreeNode pTree)
{
    NULLVALUE_CHECK(pTree, DestroyTree) ;

    list<PTreeNode> TreeList ;
    PTreeNode pPater = NULL ;
    PTreeNode pNode = NULL  ;

    // �Լ��ö��б���
    TreeList.push_back(pTree) ;

    // ������в�Ϊ��
    while (! TreeList.empty())
    {
        pPater = TreeList.front() ;
        TreeList.pop_front() ;

        // �����ӽ��ȫ��������
        while(! pPater->list.empty())
        {
            pNode = pPater->list.front() ;
            pPater->list.pop_front() ;

            // ���û�ж��ӣ�ֱ��ɾ��
            if (pNode->list.empty())
            {
                delete pNode ;
            }
            // �ж��ӵģ������Ӷ����ж�����ȥ
            else
            {
                TreeList.push_back(pNode) ;
            }          
        }
        // �ٽ������ɾ��
        delete pPater ;
    }

    return TRUE ;
}

/*******************************************************************************
*
*  �� �� �� : InitTreeHead
*  �������� : ��ʼ�������ڵ�
*  �����б� : pRunData    --  ָ��RunData
*  ˵    �� : ���ڵ㲻�ͷ�
*  ���ؽ�� : ����ɹ�����TRUE�����򷵻�FALSE
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
*  �� �� �� : SaveEnvironment
*  �������� : ���滷�����Լ��������ڵ�
*  �����б� : pRunData --  ָ��RunData�ṹ
*             uAddr    --  Ҫ��ȥ�ĵط�
*             str      --  �ڵ�����
*  ˵    �� : �������µ����Ľڵ㣬����Ϊ��ǰ���õ�
*  ���ؽ�� : ����ɹ�����TreeNode�ĵ�ַ�����򷵻�0
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

    // ���ж������ַ�Ƿ�����������
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
        // �����µĽڵ�
        pNode = new TreeNode ;
        NULLVALUE_CHECK(pNode, SaveEnvironment) ;
        //InitTreeNode(pNode) ;

        // ��ֵ
        pNode->dwAddress = uAddr ;
        pNode->strComment = str ;
        // �����������ΪpCurrentNode���ӽ��
        pRunData->pCurrentNode->list.push_back(pNode) ;
    }

    // ��������������Ĺؼ�
    // ��ԭ�������ڵ�ѹ��ջ��
    pRunData->TreeNodeStack.push(pRunData->pCurrentNode) ;

    // ��pCurrentNodeָ���������
    pRunData->pCurrentNode = pNode ;

    // ��ջ
    pRunData->CallStack.push(uAddr) ;

    return pNode ;
}

/*******************************************************************************
*
*  �� �� �� : ResumeEnvironment
*  �������� : �ָ�����
*  �����б� : uAddr    --  Ҫ��ȥ�ĵط�
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����ԭ����ֵַ�����򷵻�0
*
*******************************************************************************/
ulong ResumeEnvironment(PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, ResumeEnvironment) ;

    ulong uResult = 0 ;

    // ȡ�õ�ǰ�������׵�ַ
    // Ȼ���ջ���Ƴ�
    if (pRunData->CallStack.empty())
    {
        OutputDebugString(_T("pRunData->CallStack Ϊ�գ����麯�������Ƿ���ȷ!\r\n")) ;
        return 0 ;
    }
    uResult = pRunData->CallStack.top() ;
    pRunData->CallStack.pop() ;

    // pCurrentNode�ع�����һ���
    pRunData->pCurrentNode = pRunData->TreeNodeStack.top() ;
    pRunData->TreeNodeStack.pop() ;

    return uResult ;
}

/*******************************************************************************
*
*  �� �� �� : AppendCall
*  �������� : ��ӵ���
*  �����б� : pRunData --  ָ��RunData�ṹ
*             uAddr    --  Ҫ��ȥ�ĵط�
*             str      --  �ڵ�����
*  ˵    �� : ��SaveEnvironment��֮ͬ���ǲ����޸�pCurrentNode
*  ���ؽ�� : ����ɹ�����ԭ����ֵַ�����򷵻�0
*
*******************************************************************************/
PTreeNode AppendCall(PRunData pRunData, ulong uAddr, CString &str)
{
    NULLVALUE_CHECK(uAddr, AppendCall) ;
    NULLVALUE_CHECK(pRunData, AppendCall) ;

    // �����µĽڵ�
    PTreeNode pNode = NULL ;

    if (NULL == pRunData->pTreeHead)
    {
        InitTreeHead(pRunData) ;
    }

    // ���ж������ַ�Ƿ�����������
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

    // �����������ΪpCurrentNode���ӽ��
    pRunData->pCurrentNode->list.push_back(pNode) ;

    return pNode ;
}

/*******************************************************************************
*
*  �� �� �� : GetCallRegAddress
*  �������� : ȡ��call�Ĵ��� �����Ļ�� �Ĵ�����������ֵ
*  �����б� : pDisasm --  ָ��RunData�ṹ
*             pReg    --  Ҫ��ȥ�ĵط�
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����ԭ����ֵַ�����򷵻�0
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
*  �� �� �� : RemoveAllBreakPoint
*  �������� : �Ƴ����еĶϵ�
*  �����б� : ��
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
*
*******************************************************************************/
BOOL RemoveAllBreakPoint(void)
{
    // ɾ�����еĶϵ�
    Deletebreakpoints(0x40000, 0x80000000, 0);

    // ����������Ϣ�������Ѵ򿪵� MDI ����
    // ���޸��˵��Գ��������ʾѡ���OllyDbg �Ĵ��ڱ���Ҫ��ʱ���£��䴰�ڴ���
    // �����Ƴ�����ʹ��ʵ�����ݣ����ҷ��� CONT_BROADCAST��
    Broadcast(WM_USER_CHALL,0,0);
    return TRUE ;
}


/*******************************************************************************
*
*  �� �� �� : SetASMCpu
*  �������� : ���÷���ര�ڵĵ�ַ
*  �����б� : ��
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
*
*******************************************************************************/
BOOL SetASMCpu(DWORD dwAddr)
{
    Setcpu(Getcputhreadid(), dwAddr, 0,0, CPU_REDRAW | CPU_ASMFOCUS) ;
    return TRUE ;
}

/*******************************************************************************
*
*  �� �� �� : CheckType
*  �������� : ����ڴ�����
*  �����б� : ��
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
*
*******************************************************************************/
inline BOOL CheckType(ulong uType, ulong uStart, ulong uEnd)
{
    TCHAR szBuf[MAX_PATH] = {0} ;
    if (0 == (TY_CODE & (uType)))
    { 
        _snprintf_s(szBuf, MAX_PATH, _T("������ĵ�ַ%p - %p �в��������룬�Ƿ����?"),
                    (int *)(uStart),
                    (int *)(uEnd)) ;
        if (IDNO == AfxMessageBox(szBuf, MB_YESNO))
        {
            return FALSE ;
        }
    }

    if ((uType) & ~TY_CODE)
    {
        _snprintf_s(szBuf, MAX_PATH, _T("������ĵ�ַ%p - %p �а����������ݣ��Ƿ����?"), 
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
*  �� �� �� : GetDebugProcessName
*  �������� : ȡ�ñ���Ϸ���̵Ľ�����
*  �����б� : pOutBuffer        --    ȡ��buffer
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
*
*******************************************************************************/
BOOL GetDebugProcessName(char *pOutBuffer)
{
    NULLVALUE_CHECK(pOutBuffer, BackupUDD) ;  

    char szProcessName[MAX_PATH] = {0} ;

    // ȡ�ñ����Գ�������
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
*  �� �� �� : GetDebugPragramUDDPath
*  �������� : ȡ��od uddĿ¼
*  �����б� : pOutBuffer        --    ȡ��buffer
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
*
*******************************************************************************/
BOOL GetDebugPragramUDDPath(char *pOutBuffer)
{
    NULLVALUE_CHECK(pOutBuffer, BackupUDD) ;
    char szOdPath[MAX_PATH] = {0} ;

    // ȡ��od·��
    if(0 == GetModuleFileNameA(NULL, szOdPath, MAX_PATH))
    {
        OutputDebugString(_T("GetDebugPragramUDDPath GetModuleFileNameA failed")) ;
        return FALSE ;
    }

    // ����UDD·��
    // ������·����ȥ�ļ���
    PathRemoveFileSpecA(szOdPath) ;
    PathAppendA(szOdPath, "UDD") ;

    strcpy_s(pOutBuffer, MAX_PATH, szOdPath) ;
    return TRUE ;
}

/*******************************************************************************
*
*  �� �� �� : BackupUDD
*  �������� : ����udd�ļ�
*  �����б� : ��
*  ˵    �� : 
*  ���ؽ�� : ����ɹ�����TRUE��ʧ�ܷ���FALSE
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
        MessageBox(NULL, _T("����UDD�ļ�����"), _T("Tips"), MB_OK) ;
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

    // ������ڵ�
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