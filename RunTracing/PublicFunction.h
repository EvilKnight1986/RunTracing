/*******************************************************************************
*  
*  Copyright (c) all 2011 北京锐安 新技术研究 All rights reserved
*  FileName : PublicFunction.h
*  D a t e  : 2011.9.8
*  功   能  : 
*  说   明  : 一些公有函数，与界面无关的
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

// unicode定义
typedef struct _LSA_UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// 用户模块列表
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

// 将Dll从链表中脱链
BOOL RemoveSelfFromLdr(PVOID DllBase) ;

// 判断是否是十六进制字符串
BOOL  IsHexString(CString &str) ;

// 将字符串转成DWORD类型整数
DWORD StringToDWORD(CString &str) ;

// 取得程序入口
DWORD GetOEP(DWORD dwImageBase) ;

// 枚举函数地址,并下断点
BOOL  EnumerateFunctionAddress(PRunData pRunData) ;

// 在地址范围内下断点
BOOL  SetCallBreakPoint(PRunData pRunData, ulong uFunctionStart, ulong uFunctionEnd) ;

// 取得函数的结束地址
ulong GetFunctionEnd(ulong uAddr) ;

// 强制在指定函数起码地址下断点
BOOL  ForceSetCallBreakPoint(PRunData pRunData,
                             ulong uFunctionStart,
                             ulong nMaxLen) ;
// 判断地址是否存在断点链表中
// 在list中查找指定键值
BOOL ListExist(list<unsigned int> *pList, unsigned int uValue) ;

// 将uAddr0至uAddr1之间的值全部从链表中移除
BOOL RemoveValueByList(list<unsigned int> *pList, 
                       unsigned int uAddr0,
                       unsigned int uAddr1) ;

// 释放树资源
BOOL DestroyTree(PTreeNode pTree) ;

// 初始化调用树
BOOL InitTreeHead(PRunData pRunData) ;

// 保存环境
PTreeNode SaveEnvironment(PRunData pRunData, ulong uAddr, CString &str) ;

// 恢复环境
ulong ResumeEnvironment(PRunData pRunData) ;

// 添加节点
// 与SvaeEnvironment不同之处在于不会修改pCurrentNode
PTreeNode AppendCall(PRunData pRunData, ulong uAddr, CString &str) ;

// 取得call寄存器 这样的汇编 寄存器所包含的值
ulong GetCallRegAddress(t_disasm *pDisasm, t_reg *pReg) ;

// 移除所有的断点
BOOL RemoveAllBreakPoint(void) ;

// 设置反汇编窗口的地址
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