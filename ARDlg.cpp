// ARDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Button.h"
#include "ARDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CARDlg dialog


CARDlg::CARDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CARDlg)
	m_ORDER_AR = 0;
	m_winnow = FALSE;
	m_contiguous = 0;
	m_indexMin = 0;
	m_indexMax = 0;
	m_arOutFile = _T("");
	//}}AFX_DATA_INIT
}


void CARDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CARDlg)
	DDX_Text(pDX, IDC_ORDER_AR, m_ORDER_AR);
	DDX_Check(pDX, IDC_WINNOW, m_winnow);
	DDX_Text(pDX, IDC_CONTIGUOUS, m_contiguous);
	DDX_Text(pDX, IDC_INDEX_MIN, m_indexMin);
	DDX_Text(pDX, IDC_INDEX_MAX, m_indexMax);
	DDX_Text(pDX, IDC_AR_OUT_FILE, m_arOutFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CARDlg, CDialog)
	//{{AFX_MSG_MAP(CARDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARDlg message handlers
