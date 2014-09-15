/*******************************************************************************
*  
*  Copyright (c) all 2011 ������ �¼����о� All rights reserved
*  FileName : PublicFunction.h
*  D a t e  : 2011.9.8
*  ��   ��  : 
*  ˵   ��  : һЩ���к�����������޹ص�
*
*
*******************************************************************************/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <shlwapi.h>
#include "PublicHead.h"

using namespace std ;

#define APIFUNCTION     1
#define LIBRARYFUNCTION 2
#define USERFUNCTION    3

// unicode����
typedef struct _LSA_UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// �û�ģ���б�
typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY HashLinks;
        PVOID SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;

} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

// ��Dll������������
BOOL RemoveSelfFromLdr(PVOID DllBase) ;

// �ж��Ƿ���ʮ�������ַ���
BOOL  IsHexString(CString &str) ;

// ���ַ���ת��DWORD��������
DWORD StringToDWORD(CString &str) ;

// ȡ�ó������
DWORD GetOEP(DWORD dwImageBase) ;

// ö�ٺ�����ַ,���¶ϵ�
BOOL  EnumerateFunctionAddress(PRunData pRunData) ;

// �ڵ�ַ��Χ���¶ϵ�
BOOL  SetCallBreakPoint(PRunData pRunData, ulong uFunctionStart, ulong uFunctionEnd) ;

// ȡ�ú����Ľ�����ַ
ulong GetFunctionEnd(ulong uAddr) ;

// ǿ����ָ�����������ַ�¶ϵ�
BOOL  ForceSetCallBreakPoint(PRunData pRunData,
                             ulong uFunctionStart,
                             ulong nMaxLen) ;
// �жϵ�ַ�Ƿ���ڶϵ�������
// ��list�в���ָ����ֵ
BOOL ListExist(list<unsigned int> *pList, unsigned int uValue) ;

// ��uAddr0��uAddr1֮���ֵȫ�����������Ƴ�
BOOL RemoveValueByList(list<unsigned int> *pList, 
                       unsigned int uAddr0,
                       unsigned int uAddr1) ;

// �ͷ�����Դ
BOOL DestroyTree(PTreeNode pTree) ;

// ��ʼ��������
BOOL InitTreeHead(PRunData pRunData) ;

// ���滷��
PTreeNode SaveEnvironment(PRunData pRunData, ulong uAddr, CString &str) ;

// �ָ�����
ulong ResumeEnvironment(PRunData pRunData) ;

// ��ӽڵ�
// ��SvaeEnvironment��֮ͬ�����ڲ����޸�pCurrentNode
PTreeNode AppendCall(PRunData pRunData, ulong uAddr, CString &str) ;

// ȡ��call�Ĵ��� �����Ļ�� �Ĵ�����������ֵ
ulong GetCallRegAddress(t_disasm *pDisasm, t_reg *pReg) ;

// �Ƴ����еĶϵ�
BOOL RemoveAllBreakPoint(void) ;

// ���÷���ര�ڵĵ�ַ
BOOL SetASMCpu(DWORD dwAddr) ;

inline BOOL CheckType(ulong uType, ulong uStart, ulong uEnd) ;

BOOL GetDebugPragramUDDPath(char *pOutBuffer) ;

BOOL BackupUDD(char *pDestPath) ;


#ifdef _DEBUG
BOOL TestIsHexString() ;
BOOL TestStringToDWORD() ;
BOOL TestListExist() ;
BOOL TestDestroyTree() ;
BOOL TestRemoveValueByList() ;
#endif