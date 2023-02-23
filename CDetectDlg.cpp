// CDetectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Button.h"
#include "CDetectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCDetectDlg dialog


CCDetectDlg::CCDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDetectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCDetectDlg)
	m_detection = -1;
	//}}AFX_DATA_INIT
}


void CCDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCDetectDlg)
	DDX_Radio(pDX, IDC_DETECTION, m_detection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCDetectDlg, CDialog)
	//{{AFX_MSG_MAP(CCDetectDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDetectDlg message handlers
