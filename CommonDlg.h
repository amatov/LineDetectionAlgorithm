#if !defined(AFX_COMMONDLG_H__6BD2628A_DF05_4C08_9FE4_B467169965AA__INCLUDED_)
#define AFX_COMMONDLG_H__6BD2628A_DF05_4C08_9FE4_B467169965AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommonDlg.h : header file
//
#include "struct.h"
/////////////////////////////////////////////////////////////////////////////
// CCommonDlg dialog

class CCommonDlg : public CDialog
{
// Construction
public:
	CCommonDlg(Parameters &p, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCommonDlg)
	enum { IDD = IDD_COMMON };
	double	m_alphaTZ;
	double	m_alpha;
	int		m_cycles;
	double	m_epsilon;
	BOOL	m_expT;
	CString	m_fileName;
	BOOL	m_graphDisp;
	BOOL	m_autoIniT;
	double	m_Tzero;
	double	m_IniT;
	int		m_maxNbSubS;
	int		m_maxNbRetry;
	int		m_detectionType;
	int		m_STOP_CY;
	double	m_stopCyProp;
	double	m_updateTzero;
	BOOL	m_monWellCl;
	BOOL	m_drawBadS;
	BOOL	m_rnd;
	int		m_retryWithL;
	BOOL	m_noPtsRmv;
	double	m_stopTemp;
	BOOL	m_monT;
	BOOL	m_monAB;
	CString	m_result;
	int		m_nbSubS;
	double	m_noCorr;
	BOOL	m_rndOrigin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	Parameters &params;

	// Generated message map functions
	//{{AFX_MSG(CCommonDlg)
	afx_msg void OnBDetectionSet();
	afx_msg void OnBSelectFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMONDLG_H__6BD2628A_DF05_4C08_9FE4_B467169965AA__INCLUDED_)
