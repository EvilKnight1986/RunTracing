// LayOut.h: interface for the CLayOut class.
//
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>
#if !defined(AFX_LAYOUT_H__BC915E3B_ED89_4237_94C2_677324868A98__INCLUDED_)
#define AFX_LAYOUT_H__BC915E3B_ED89_4237_94C2_677324868A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
enum
{
	LAYOUT_LEFT  = 0x01,   ///                       
	LAYOUT_RIGHT = 0x02,   ///子窗口X坐标和父窗口的右边界面距离保持不变
	LAYOUT_TOP   = 0x04,   ///
	LAYOUT_BOTTOM= 0x08,   ///子窗口Y坐标和父窗口的下边界面距离保持不变
	LAYOUT_WIDTH = 0X10,   ///子窗口的右边界和父窗口的右边界距离保持不变
	LAYOUT_HEIGHT = 0X20,  ///子窗口的下边界和父窗口的下边界距离保持不变
	LAYOUT_SIZE = LAYOUT_WIDTH | LAYOUT_HEIGHT
};
typedef struct  
{
	int nXDistance;
	int nYDistance;
	HWND hWnd;
}LayoutWnd;

#define GET_HWND_BY_ID(x) (GetDlgItem(x)->GetSafeHwnd())

class CLayout  
{
public:
	CLayout(HWND hParentWnd, int nLayout);
	HWND m_hParentWnd;
	void AddWnd(HWND hWnd);
	void Resize(int nWidth, int nHeight);
	virtual ~CLayout();
private:
	int m_nLayOut;
	int m_nParentWidth;
	int m_nParentHeight;
	CArray<LayoutWnd*, LayoutWnd*> m_arrLayoutWnd;
};

#endif // !defined(AFX_LAYOUT_H__BC915E3B_ED89_4237_94C2_677324868A98__INCLUDED_)
