// ShowInformation.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RunTracing.h"
#include "ShowInformation.h"
#include "PublicFunction.h"

// CShowInformation �Ի���

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


// CShowInformation ��Ϣ�������
BOOL CShowInformation::SetUserData(PRunData pRunData)
{
    NULLVALUE_CHECK(pRunData, SetUserData) ;
    m_pRunData = pRunData ;

    return TRUE ;
}

//******************************************************************************
// ������: UpdateTree
// Ȩ  ��: public 
// ����ֵ: ����ɹ�������TRUE�����ʧ�ܷ���FALSE
// ��  ��: ��
// ˵  ��: ����
// ��  ��:
//******************************************************************************
BOOL CShowInformation::UpdateTree()
{
    NULLVALUE_CHECK(m_pRunData, UpdateTree) ;

    m_Tree.DeleteAllItems() ;

    list<PTreeNode> TreeList ; // ���Ľڵ�
    list<HTREEITEM> ItemList ; // Tree�ؼ�
    PTreeNode pNode = NULL ;
    HTREEITEM hLeft = NULL ;
    HTREEITEM hPater = NULL ;
    CString str ;
    CString strTmp ;

    // �ж���û�и��ڵ㣬�еĻ����
    if (NULL != m_pRunData->pTreeHead)
    {
        str.Format(_T("%p"), (int *)m_pRunData->pTreeHead->dwAddress) ;
        // ����Ӹ��ڵ�
        hLeft = m_Tree.InsertItem(str, 0,0) ;
        m_Tree.SetItemData(hLeft, m_pRunData->pTreeHead->dwAddress) ;
        ItemList.push_back(hLeft) ;
        TreeList.push_back(m_pRunData->pTreeHead) ;
    }
    else
    {
        hLeft = m_Tree.InsertItem(_T("��¼Ϊ��"), 0,0) ;
    }

    while (! ItemList.empty())
    {
        // ��ջ��ȡ��
        pNode = TreeList.front() ;
        hPater = ItemList.front() ;

        // �Ƴ�
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

            // ����ַ���ý�ȥ��������ת
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

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
    m_tt.AddTool(GetDlgItem(IDC_TREE), _T("���ѡ�У��Ҽ�������ȥ���Ӧ��ָ�"));

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

//void CShowInformation::OnNMRDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    // TODO: �ڴ���ӿؼ�֪ͨ����������
//    ::MessageBox(NULL, _T("���˫��"), _T("Tips"), MB_OK) ;
//    *pResult = 0;
//}

void CShowInformation::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������

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

    // TODO: �ڴ˴������Ϣ����������
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

    // TODO: �ڴ˴������Ϣ����������
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
    // TODO: �ڴ����ר�ô����/����û���

    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN: //�س�
            return TRUE;
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }

    m_tt.RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}
