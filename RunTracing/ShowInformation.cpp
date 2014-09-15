// ShowInformation.cpp : 实现文件
//

#include "stdafx.h"
#include "RunTracing.h"
#include "ShowInformation.h"
#include "PublicFunction.h"

// CShowInformation 对话框

IMPLEMENT_DYNAMIC(CShowInformation, CDialog)

CShowInformation::CShowInformation(CWnd* pParent /*=NULL*/)
	: CDialog(CShowInformation::IDD, pParent),
    m_lpLayoutSize(NULL),m_lpLayoutBottom(NULL)
{

}

CShowInformation::~CShowInformation()
{
    if (NULL != m_lpLayoutSize)
    {
        delete m_lpLayoutSize ;
        m_lpLayoutSize = NULL ;
    }

    if (NULL != m_lpLayoutBottom)
    {
        delete m_lpLayoutBottom ;
        m_lpLayoutBottom = NULL ;
    }
}

void CShowInformation::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CShowInformation, CDialog)
//    ON_NOTIFY(NM_RDBLCLK, IDC_TREE, &CShowInformation::OnNMRDblclkTree)
ON_NOTIFY(NM_RCLICK, IDC_TREE, &CShowInformation::OnNMRClickTree)
ON_WM_SIZE()
ON_WM_SIZING()
END_MESSAGE_MAP()


// CShowInformation 消息处理程序
BOOL CShowInformation::SetUserData(PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, SetUserData) ;
    m_pRunData = pRunData ;

    return TRUE ;
}

//******************************************************************************
// 函数名: UpdateTree
// 权  限: public 
// 返回值: 如果成功，返回TRUE，如果失败返回FALSE
// 参  数: 无
// 说  明: 更新
// 合  格:
//******************************************************************************
BOOL CShowInformation::UpdateTree()
{
    NULLVALUE_CHECK(m_pRunData, UpdateTree) ;

    m_Tree.DeleteAllItems() ;

    list<PTreeNode> TreeList ; // 树的节点
    list<HTREEITEM> ItemList ; // Tree控件
    PTreeNode pNode = NULL ;
    HTREEITEM hLeft = NULL ;
    HTREEITEM hPater = NULL ;
    CString str ;
    CString strTmp ;

    // 判断有没有根节点，有的话添加
    if (NULL != m_pRunData->pTreeHead)
    {
        str.Format(_T("%p"), (int *)m_pRunData->pTreeHead->dwAddress) ;
        // 先添加根节点
        hLeft = m_Tree.InsertItem(str, 0,0) ;
        m_Tree.SetItemData(hLeft, m_pRunData->pTreeHead->dwAddress) ;
        ItemList.push_back(hLeft) ;
        TreeList.push_back(m_pRunData->pTreeHead) ;
    }
    else
    {
        hLeft = m_Tree.InsertItem(_T("记录为空"), 0,0) ;
    }

    while (! ItemList.empty())
    {
        // 从栈中取出
        pNode = TreeList.front() ;
        hPater = ItemList.front() ;

        // 移除
        ItemList.pop_front() ;
        TreeList.pop_front() ;

        for (list<PTreeNode>::iterator it = pNode->list.begin() ;
                it != pNode->list.end(); ++it)
        {
            if ((*it)->strComment.GetLength() > 0)
            {
                str.Format(_T("%p::%s"), (int *)(*it)->dwAddress, (*it)->strComment) ;
            }
            else
            {
                str.Format(_T("%p"), (int *)(*it)->dwAddress) ;
            }

            if ((*it)->dwSequenceCallCount > 1)
            {
                strTmp.Format(_T("        Sequence Call: %d"), (*it)->dwSequenceCallCount) ;
                str += strTmp ;
            }

            hLeft = m_Tree.InsertItem(str,0,0, hPater) ;

            // 将地址设置进去，方便跳转
            m_Tree.SetItemData(hLeft, (*it)->dwAddress) ;

            if (0 != (*it)->list.size())
            {
                ItemList.push_back(hLeft) ;
                TreeList.push_back(*it) ;
            }
        }
    }
    return TRUE ;
}

BOOL CShowInformation::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_lpLayoutSize = new  CLayout(GetSafeHwnd(), LAYOUT_SIZE);
    m_lpLayoutSize->AddWnd(GET_HWND_BY_ID(IDC_TREE));

    m_lpLayoutBottom = new CLayout(GetSafeHwnd(),LAYOUT_RIGHT | LAYOUT_BOTTOM);
    m_lpLayoutBottom->AddWnd(GET_HWND_BY_ID(IDOK));
    m_lpLayoutBottom->AddWnd(GET_HWND_BY_ID(IDCANCEL));

    GetWindowRect(&m_orignalSize) ;

    UpdateTree() ;

    this->EnableToolTips(TRUE);
    m_tt.Create(this);
    m_tt.Activate(TRUE);
    m_tt.AddTool(GetDlgItem(IDC_TREE), _T("左键选中，右键即可跳去相对应的指令处"));

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

//void CShowInformation::OnNMRDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    // TODO: 在此添加控件通知处理程序代码
//    ::MessageBox(NULL, _T("鼠标双击"), _T("Tips"), MB_OK) ;
//    *pResult = 0;
//}

void CShowInformation::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码

    HTREEITEM hItem = m_Tree.GetSelectedItem();

    if (NULL != hItem)
    {
        DWORD dwAddr = m_Tree.GetItemData(hItem) ;
        SetASMCpu(dwAddr) ;
    }
    
    *pResult = 0;
}

void CShowInformation::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (SIZE_MINIMIZED == nType)
	{
		return;
	}

	if (NULL != m_lpLayoutSize)
	{
		m_lpLayoutSize->Resize(cx, cy);
	}

    if (NULL != m_lpLayoutBottom)
    {
        m_lpLayoutBottom->Resize(cx, cy);
    }
    Invalidate(TRUE);
}

void CShowInformation::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialog::OnSizing(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
    int nOriWidth = m_orignalSize.Width();
	int nOriHeight = m_orignalSize.Height();
	switch (fwSide)
	{
	case WMSZ_TOP:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->top = pRect->bottom - nOriHeight;
		}
		break;
	case WMSZ_BOTTOM:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->bottom = pRect->top + nOriHeight;
		}
		break;
	case WMSZ_LEFT:
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->left = pRect->right - nOriWidth;
		}
		break;
	case WMSZ_RIGHT:
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->right = pRect->left + nOriWidth;
		}
		break;
	case WMSZ_TOPLEFT:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->top = pRect->bottom - nOriHeight;
		}
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->left = pRect->right - nOriWidth;
		}
		break;
	case WMSZ_BOTTOMRIGHT:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->bottom = pRect->top + nOriHeight;
		}
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->right = pRect->left + nOriWidth;
		}
		break;
	case WMSZ_TOPRIGHT:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->top = pRect->bottom - nOriHeight;
		}
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->right = pRect->left + nOriWidth;
		}
		break;
	case WMSZ_BOTTOMLEFT:
		if (pRect->bottom - pRect->top < nOriHeight)
		{
			pRect->bottom = pRect->top + nOriHeight;
		}
		if (pRect->right - pRect->left < nOriWidth)
		{
			pRect->left = pRect->right - nOriWidth;
		}
		break;
	default:
		break;
	}
}

BOOL CShowInformation::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN: //回车
            return TRUE;
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }

    m_tt.RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}
