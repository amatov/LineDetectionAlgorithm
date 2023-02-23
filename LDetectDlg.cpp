// LDetectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Button.h"
#include "LDetectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLDetectDlg dialog


CLDetectDlg::CLDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLDetectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLDetectDlg)
	m_bmpOrNot = FALSE;
	m_color = 0;
	m_noise = 0.0;
	m_display = -1;
	m_segment = 0;
	m_segThre = 0;
	m_segEpsi = 0.0;
	m_cluster = 0;
	m_lastStart = FALSE;
	m_tanEst = FALSE;
	m_deltaC = 0.0;
	m_partial = 0.0;
	//}}AFX_DATA_INIT
}


void CLDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLDetectDlg)
	DDX_Check(pDX, IDC_BMP_OR_NOT, m_bmpOrNot);
	DDX_Text(pDX, IDC_COLOR, m_color);
	DDX_Text(pDX, IDC_NOISE1, m_noise);
	DDX_Radio(pDX, IDC_NO_DISP, m_display);
	DDX_Text(pDX, IDC_SEGMENT, m_segment);
	DDX_Text(pDX, IDC_SEG_THRE, m_segThre);
	DDX_Text(pDX, IDC_SEG_EPSI, m_segEpsi);
	DDX_Text(pDX, IDC_CLUSTER, m_cluster);
	DDX_Check(pDX, IDC_LAST_AS_START, m_lastStart);
	DDX_Check(pDX, IDC_TAN_EST, m_tanEst);
	DDX_Text(pDX, IDC_DELTA_C_MIN, m_deltaC);
	DDX_Text(pDX, IDC_PARTIAL, m_partial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLDetectDlg, CDialog)
	//{{AFX_MSG_MAP(CLDetectDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLDetectDlg message handlers
