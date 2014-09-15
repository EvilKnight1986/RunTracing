#include "stdafx.h"
#include "LayOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * @fn CLayout
 * @brief  构造函数    
 * @param[in] HWND hParentWnd,父窗口句柄 
 * @param[in] int nLayout 布局类型
 * @param[out] 
 * @return void
 */
CLayout::CLayout(HWND hParentWnd, int nLayout)
:m_hParentWnd(hParentWnd)
,m_nLayOut(nLayout)
,m_nParentWidth(0)
,m_nParentHeight(0)
{
    RECT rect;
	GetClientRect(hParentWnd, &rect);
	m_nParentWidth = rect.right - rect.left;
	m_nParentHeight = rect.bottom - rect.top;
}

CLayout::~CLayout()
{
    for (int i = 0; i < m_arrLayoutWnd.GetSize(); i++)
    {
		delete m_arrLayoutWnd.GetAt(i);
    }
}


/**
 * @fn AddWnd
 * @brief 添加需要进行布局控制的窗口      
 * @param[in] HWND hWnd， 窗口句柄 
 * @return void
 */
void CLayout::AddWnd(HWND hWnd)
{
	LayoutWnd *pLayoutWnd = new LayoutWnd;
	pLayoutWnd->hWnd = hWnd;
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	::ScreenToClient(m_hParentWnd, (LPPOINT)&rect);
	::ScreenToClient(m_hParentWnd, (LPPOINT)&rect + 1);
	//第一个元素，需要计算他和父窗口的相对位置

		pLayoutWnd->nXDistance = rect.left;
		pLayoutWnd->nYDistance = rect.top;
		m_arrLayoutWnd.Add(pLayoutWnd);
}


/**
 * @fn Resize
 * @brief 在父窗口的OnSize函数中调用，用于移动子窗口      
 * @param[in] int nWidth, int nHeight 父窗口宽高
 * @param[out] 
 * @return int
 * @retval  0 成功
 * @retval  其它值 错误码
 */
void CLayout::Resize(int nWidth, int nHeight)
{
	if (0 == m_arrLayoutWnd.GetSize())
	{
		return;
	}

	LayoutWnd *pLayoutWnd = NULL;
	RECT rect;
	for (int i = 0; i < m_arrLayoutWnd.GetSize(); i++)
	{
		pLayoutWnd = m_arrLayoutWnd.GetAt(i);
		::GetWindowRect(pLayoutWnd->hWnd, &rect);
		int nNewX = pLayoutWnd->nXDistance; 
		int nNewY = pLayoutWnd->nYDistance;
        int nNewWidth = rect.right - rect.left;
		int nNewHeight = rect.bottom - rect.top;
		///如果是右对齐，计算X坐标
		if (m_nLayOut & LAYOUT_RIGHT)
		{
			nNewX = nWidth - (m_nParentWidth - pLayoutWnd->nXDistance);
		}
		///如果是下对齐，计算Y坐标
		if (m_nLayOut & LAYOUT_BOTTOM)
		{
			nNewY = nHeight - (m_nParentHeight - pLayoutWnd->nYDistance);
		}

		///如果是尺寸对齐，计算大小
		if (m_nLayOut & LAYOUT_WIDTH)
		{
			nNewWidth = nWidth - (m_nParentWidth - nNewWidth) ;
		}
		if (m_nLayOut & LAYOUT_HEIGHT)
		{
			nNewHeight = nHeight - (m_nParentHeight - nNewHeight) ;
		}
		::MoveWindow(pLayoutWnd->hWnd, nNewX, nNewY, nNewWidth, nNewHeight, TRUE);
		pLayoutWnd->nXDistance = nNewX;
		pLayoutWnd->nYDistance = nNewY;

	}
    m_nParentWidth = nWidth;
	m_nParentHeight = nHeight;
}