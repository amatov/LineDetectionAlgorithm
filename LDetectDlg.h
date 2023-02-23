#if !defined(AFX_LDETECTDLG_H__A661496A_2E52_4C9D_AB67_643ED213CB4C__INCLUDED_)
#define AFX_LDETECTDLG_H__A661496A_2E52_4C9D_AB67_643ED213CB4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LDetectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLDetectDlg dialog

class CLDetectDlg : public CDialog
{
// Construction
public:
	CLDetectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLDetectDlg)
	enum { IDD = IDD_LDETECT };
	BOOL	m_bmpOrNot;
	int		m_color;
	double	m_noise;
	int		m_display;
	int		m_segment;
	int		m_segThre;
	double	m_segEpsi;
	int		m_cluster;
	BOOL	m_lastStart;
	BOOL	m_tanEst;
	double	m_deltaC;
	double	m_partial;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDetectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLDetectDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LDETECTDLG_H__A661496A_2E52_4C9D_AB67_643ED213CB4C__INCLUDED_)
