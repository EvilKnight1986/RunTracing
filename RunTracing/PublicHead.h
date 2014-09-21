/*******************************************************************************
*  
*  Copyright (c) all 2011 EvilKnight All rights reserved
*  FileName : PublicHead.h
*  D a t e  : 2011.9.8
*  功   能  : 
*  说   明  : 公有头文件，一些公有头和宏的定义
*
*
*******************************************************************************/
#pragma once

#include "stdafx.h"
#define PLUGINNAME          "RunTracing"
#define PLUGINVERSIONINFOR  "RunTracing v0.0.1"
#define COPYRIGHTINFOR      "code by 新技术研究(EvilKnight) @ 2011"

#define RECORDINDEX         (0)
#define ANTIANTIDEBUG       (1)


#define MAXFUNSIZE      (0x300)

#include <stack>
#include <string>
#include <list>
#include <windows.h>
using namespace std ;

// 属性
// 树的节点定义
 class TreeNode
{
public:
    TreeNode():dwAddress(0),dwFuncStart(0),dwFuncEnd(0),dwSequenceCallCount(1)
    {
    }
public:
    DWORD dwAddress ;                               // 地址
    list<TreeNode *>  list;                        // 儿子结点
    CString         strComment ;                    // 注释
    DWORD dwFuncStart ;                             // 函数起始地址
    DWORD dwFuncEnd   ;                             // 函数结束地址
    DWORD dwSequenceCallCount ;                     // 连续调用次数
} ;

typedef TreeNode *PTreeNode ;


// 定义
class RunData
{
public:
    RunData():hwnd(NULL), uStartAddress(0), uEndAddress(0)
        , pTreeHead(NULL), pCurrentNode(NULL), dwError(0)
        , bIsInitialized(FALSE),bIsStart(FALSE), bIsHit(FALSE)
        , bIsChange(FALSE), bIsRecordOther(TRUE), bIsRecordAPI(FALSE)
        , bIsForceSetBreakPoint(FALSE),dwEIP(0)
    {
    }
public:
    HWND                    hwnd ;
    unsigned int            uStartAddress ;        // 用户设的开始地址
    unsigned int            uEndAddress ;          // 用户设的结束地址

    list<unsigned int>      BreakPointList ;      // 断点列表
    stack<unsigned int>     CallStack ;           // 调用栈  是否可以用od的呢
    stack<PTreeNode>        TreeNodeStack ;       // 保存树访问节点当函数返回上一层的时候用
    PTreeNode               pTreeHead ;
    PTreeNode               pCurrentNode ;        // 指向当前正在使用的树结点
    CString                 strLogPath ;          // 日志存放路径
    DWORD                   dwError ;             // 错误码

    BOOL                    bIsInitialized ;       // 结构是否已经初始化 初始化之后置为1
    BOOL                    bIsStart ;             // 是否已经开始运行
    BOOL                    bIsHit ;               // 是否已经命中在范围内
    BOOL                    bIsChange ;            // 是否变更
    BOOL                    bIsRecordOther ;       // 是否记录地址范围外的
    BOOL                    bIsRecordAPI ;         // 是否记录api日志, Demo
    BOOL                    bIsForceSetBreakPoint ;// 是否强制下断点
    DWORD                   dwEIP ;                // 记录当前eip,当停下来的时候，更新一下
} ;
typedef RunData *PRunData ;

/*
#define InitTreeNode(x) {((PTreeNode)(x))->dwAddress = 0 ;\
                          ((PTreeNode)(x))->dwFuncStart = 0;\
                          ((PTreeNode)(x))->dwFuncEnd = 0;}
*/                     
#define InitRunData(x) {((PRunData)(x))->hwnd = NULL ;\
                        ((PRunData)(x))->uStartAddress = 0 ;\
                        ((PRunData)(x))->uEndAddress = 0 ;\
                        ((PRunData)(x))->pTreeHead = NULL ;\
                        ((PRunData)(x))->dwError = 0 ;\
                        ((PRunData)(x))->bIsInitialized = FALSE ;\
                        ((PRunData)(x))->bIsStart = FALSE ;\
                        ((PRunData)(x))->bIsHit = FALSE ;\
                        ((PRunData)(x))->bIsChange = FALSE ;\
                        ((PRunData)(x))->bIsRecordOther = TRUE ;\
                        ((PRunData)(x))->bIsRecordAPI = FALSE ;\
                        ((PRunData)(x))->bIsForceSetBreakPoint = FALSE ;}

#define ERROR_RUN   (1)
#define FUN_SIZE    (0X10000)

#define NULLVALUE_CHECK(value,fName) if(NULL == value){\
                                                OutputDebugString(#fName);\
                                                OutputDebugString(_T(" ")); \
                                                OutputDebugString(#value) ;\
                                                OutputDebugString(_T(" ")); \
                                                OutputDebugString(_T("can't NULL !\r\n")) ;\
                                                return FALSE;}

#define NULLVALUE_CHECK_NO_RETURN(value,fName) if(NULL == value){\
                                    OutputDebugString(#fName);\
                                    OutputDebugString(_T(" ")); \
                                    OutputDebugString(#value) ;\
                                    OutputDebugString(_T(" ")); \
                                    OutputDebugString(_T("can't NULL !\r\n")) ;\
                                    return ;}