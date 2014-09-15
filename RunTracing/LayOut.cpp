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
 * @brief  ���캯��    
 * @param[in] HWND hParentWnd,�����ھ�� 
 * @param[in] int nLayout ��������
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
 * @brief �����Ҫ���в��ֿ��ƵĴ���      
 * @param[in] HWND hWnd�� ���ھ�� 
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
	//��һ��Ԫ�أ���Ҫ�������͸����ڵ����λ��

		pLayoutWnd->nXDistance = rect.left;
		pLayoutWnd->nYDistance = rect.top;
		m_arrLayoutWnd.Add(pLayoutWnd);
}


/**
 * @fn Resize
 * @brief �ڸ����ڵ�OnSize�����е��ã������ƶ��Ӵ���      
 * @param[in] int nWidth, int nHeight �����ڿ��
 * @param[out] 
 * @return int
 * @retval  0 �ɹ�
 * @retval  ����ֵ ������
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
		///������Ҷ��룬����X����
		if (m_nLayOut & LAYOUT_RIGHT)
		{
			nNewX = nWidth - (m_nParentWidth - pLayoutWnd->nXDistance);
		}
		///������¶��룬����Y����
		if (m_nLayOut & LAYOUT_BOTTOM)
		{
			nNewY = nHeight - (m_nParentHeight - pLayoutWnd->nYDistance);
		}

		///����ǳߴ���룬�����С
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