// RunTracing.h : RunTracing DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRunTracingApp
// �йش���ʵ�ֵ���Ϣ������� RunTracing.cpp
//

class CRunTracingApp : public CWinApp
{
public:
	CRunTracingApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
