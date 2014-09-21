/*******************************************************************************
*  
*  Copyright (c) all 2011 EvilKnight All rights reserved
*  FileName : PublicHead.h
*  D a t e  : 2011.9.8
*  ��   ��  : 
*  ˵   ��  : ����ͷ�ļ���һЩ����ͷ�ͺ�Ķ���
*
*
*******************************************************************************/
#pragma once

#include "stdafx.h"
#define PLUGINNAME          "RunTracing"
#define PLUGINVERSIONINFOR  "RunTracing v0.0.1"
#define COPYRIGHTINFOR      "code by �¼����о�(EvilKnight) @ 2011"

#define RECORDINDEX         (0)
#define ANTIANTIDEBUG       (1)


#define MAXFUNSIZE      (0x300)

#include <stack>
#include <string>
#include <list>
#include <windows.h>
using namespace std ;

// ����
// ���Ľڵ㶨��
 class TreeNode
{
public:
    TreeNode():dwAddress(0),dwFuncStart(0),dwFuncEnd(0),dwSequenceCallCount(1)
    {
    }
public:
    DWORD dwAddress ;                               // ��ַ
    list<TreeNode *>  list;                        // ���ӽ��
    CString         strComment ;                    // ע��
    DWORD dwFuncStart ;                             // ������ʼ��ַ
    DWORD dwFuncEnd   ;                             // ����������ַ
    DWORD dwSequenceCallCount ;                     // �������ô���
} ;

typedef TreeNode *PTreeNode ;


// ����
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
    unsigned int            uStartAddress ;        // �û���Ŀ�ʼ��ַ
    unsigned int            uEndAddress ;          // �û���Ľ�����ַ

    list<unsigned int>      BreakPointList ;      // �ϵ��б�
    stack<unsigned int>     CallStack ;           // ����ջ  �Ƿ������od����
    stack<PTreeNode>        TreeNodeStack ;       // ���������ʽڵ㵱����������һ���ʱ����
    PTreeNode               pTreeHead ;
    PTreeNode               pCurrentNode ;        // ָ��ǰ����ʹ�õ������
    CString                 strLogPath ;          // ��־���·��
    DWORD                   dwError ;             // ������

    BOOL                    bIsInitialized ;       // �ṹ�Ƿ��Ѿ���ʼ�� ��ʼ��֮����Ϊ1
    BOOL                    bIsStart ;             // �Ƿ��Ѿ���ʼ����
    BOOL                    bIsHit ;               // �Ƿ��Ѿ������ڷ�Χ��
    BOOL                    bIsChange ;            // �Ƿ���
    BOOL                    bIsRecordOther ;       // �Ƿ��¼��ַ��Χ���
    BOOL                    bIsRecordAPI ;         // �Ƿ��¼api��־, Demo
    BOOL                    bIsForceSetBreakPoint ;// �Ƿ�ǿ���¶ϵ�
    DWORD                   dwEIP ;                // ��¼��ǰeip,��ͣ������ʱ�򣬸���һ��
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