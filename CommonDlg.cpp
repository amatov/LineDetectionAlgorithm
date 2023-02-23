// CommonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Button.h"
#include "CommonDlg.h"
#include "ARDlg.h"
#include "LDetectDlg.h"
#include "CDetectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommonDlg dialog


CCommonDlg::CCommonDlg(Parameters &p, CWnd* pParent /*=NULL*/)
	: CDialog(CCommonDlg::IDD, pParent), params(p)
{
	//{{AFX_DATA_INIT(CCommonDlg)
	m_alphaTZ = 0.0;
	m_alpha = 0.0;
	m_cycles = 0;
	m_epsilon = 0.0;
	m_expT = FALSE;
	m_fileName = _T("");
	m_graphDisp = FALSE;
	m_autoIniT = FALSE;
	m_Tzero = 0.0;
	m_IniT = 0.0;
	m_maxNbSubS = 0;
	m_maxNbRetry = 0;
	m_detectionType = -1;
	m_STOP_CY = 0;
	m_stopCyProp = 0.0;
	m_updateTzero = 0.0;
	m_monWellCl = FALSE;
	m_drawBadS = FALSE;
	m_rnd = FALSE;
	m_retryWithL = 0;
	m_noPtsRmv = FALSE;
	m_stopTemp = 0.0;
	m_monT = FALSE;
	m_monAB = FALSE;
	m_result = _T("");
	m_nbSubS = 0;
	m_noCorr = 0.0;
	m_rndOrigin = FALSE;
	//}}AFX_DATA_INIT
}


void CCommonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonDlg)
	DDX_Text(pDX, IDC_ALPHA_TZERO, m_alphaTZ);
	DDX_Text(pDX, IDC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_CYCLES, m_cycles);
	DDX_Text(pDX, IDC_EPS, m_epsilon);
	DDX_Check(pDX, IDC_EXP_TEMP, m_expT);
	DDX_Text(pDX, IDC_FILE_NAME, m_fileName);
	DDX_Check(pDX, IDC_GRAPHICAL_DISPLAY, m_graphDisp);
	DDX_Check(pDX, IDC_AUTO_INI_TEMP, m_autoIniT);
	DDX_Text(pDX, IDC_INI_TZERO, m_Tzero);
	DDX_Text(pDX, IDC_MANU_INI_TEMP, m_IniT);
	DDX_Text(pDX, IDC_MAX_NB_SUBS, m_maxNbSubS);
	DDX_Text(pDX, IDC_NB_RETRY, m_maxNbRetry);
	DDX_CBIndex(pDX, IDC_DETECTION_TYPE, m_detectionType);
	DDX_Text(pDX, IDC_STOP_CY, m_STOP_CY);
	DDX_Text(pDX, IDC_STOP_CY_PROP, m_stopCyProp);
	DDX_Text(pDX, IDC_UPDATE_TZERO, m_updateTzero);
	DDX_Check(pDX, IDC_MON_WELL_CL, m_monWellCl);
	DDX_Check(pDX, IDC_DRAW_BAD_SUBS, m_drawBadS);
	DDX_Check(pDX, IDC_RND, m_rnd);
	DDX_Text(pDX, IDC_RETRY_WITH_LAST, m_retryWithL);
	DDX_Check(pDX, IDC_NO_PTS_RMV, m_noPtsRmv);
	DDX_Text(pDX, IDC_STOP_TEMP, m_stopTemp);
	DDX_Check(pDX, IDC_MON_T, m_monT);
	DDX_Check(pDX, IDC_MON_AB, m_monAB);
	DDX_Text(pDX, IDC_RESULT, m_result);
	DDX_Text(pDX, IDC_NB_SUBS, m_nbSubS);
	DDX_Text(pDX, IDC_NO_CORR, m_noCorr);
	DDX_Check(pDX, IDC_RND_ORIGIN, m_rndOrigin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommonDlg, CDialog)
	//{{AFX_MSG_MAP(CCommonDlg)
	ON_BN_CLICKED(IDC_B_DETECTION_SET, OnBDetectionSet)
	ON_BN_CLICKED(IDC_B_SELECT_FILE, OnBSelectFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonDlg message handlers

void CCommonDlg::OnBDetectionSet() 
{
	// TODO: Add your control notification handler code here
	
	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_DETECTION_TYPE);
        int dType = pCB->GetCurSel();
        switch(dType) {
        case CB_ERR: AfxMessageBox("Select detection type first!"); break;
        case 0: { // AR modelling                  
			CARDlg dlg;

			dlg.m_ORDER_AR = params.ORDER_AR;
			dlg.m_winnow = params.winnow;
			dlg.m_contiguous = params.contiguous;
			dlg.m_indexMin = params.indexMin;
			dlg.m_indexMax = params.indexMax;
			dlg.m_arOutFile = params.arOutFile;

			int ret = dlg.DoModal(); 
			if(ret == IDOK){
	
				params.ORDER_AR = dlg.m_ORDER_AR;
				params.winnow = dlg.m_winnow;
				params.contiguous = dlg.m_contiguous;
				params.indexMin = dlg.m_indexMin;
				params.indexMax = dlg.m_indexMax;
				params.arOutFile = dlg.m_arOutFile;
	
			}
			break;			
                        
						}
        case 1: { // Conic detection
			CCDetectDlg dlg;

			int ret = dlg.DoModal(); 
			break;
                                
                        }
        case 2: { // Line detection
			CLDetectDlg dlg;

			dlg.m_bmpOrNot = params.bmpOrNot;
			dlg.m_color = params.color;
			dlg.m_noise = params.noise;
			dlg.m_display = params.display;
			dlg.m_segment = params.segment;
			dlg.m_segThre = params.segThre;
			dlg.m_segEpsi = params.segEpsi;
			dlg.m_cluster = params.cluster;
			dlg.m_lastStart = params.lastStart;
			dlg.m_tanEst = params.tanEst;
			dlg.m_deltaC = params.deltaC;
			dlg.m_partial = params.partial;

			int ret = dlg.DoModal(); 
			if(ret == IDOK){	
				params.bmpOrNot = dlg.m_bmpOrNot;
				params.color = dlg.m_color;
				params.noise = dlg.m_noise;
				params.display = dlg.m_display;
				params.segment = dlg.m_segment;
				params.segThre = dlg.m_segThre;
				params.segEpsi = dlg.m_segEpsi;
				params.cluster = dlg.m_cluster;
				params.lastStart = dlg.m_lastStart;
				params.tanEst = dlg.m_tanEst;
				params.deltaC = dlg.m_deltaC;
				params.partial = dlg.m_partial;
			}
			break;
                                
                        }
        default: AfxMessageBox("Unknown selection!"); break; // should not happen
        }
}

void CCommonDlg::OnBSelectFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, "bmp", "*.bmp");
	if(dlg.DoModal() == IDOK) {
		m_fileName = dlg.GetPathName();
		CEdit* pE = (CEdit*) GetDlgItem(IDC_FILE_NAME);
		pE->SetWindowText((LPCTSTR) m_fileName);
	}
}
