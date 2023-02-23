#if !defined(AFX_ARDLG_H__60E49A71_2D79_4453_A2FB_DB18BEA012FB__INCLUDED_)
#define AFX_ARDLG_H__60E49A71_2D79_4453_A2FB_DB18BEA012FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ARDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CARDlg dialog

class CARDlg : public CDialog
{
// Construction
public:
	CARDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CARDlg)
	enum { IDD = IDD_AR };
	int		m_ORDER_AR;
	BOOL	m_winnow;
	int		m_contiguous;
	int		m_indexMin;
	int		m_indexMax;
	CString	m_arOutFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CARDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARDLG_H__60E49A71_2D79_4453_A2FB_DB18BEA012FB__INCLUDED_)
