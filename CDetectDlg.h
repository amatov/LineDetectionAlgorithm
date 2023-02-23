#if !defined(AFX_CDETECTDLG_H__1CC4DEE5_E208_4840_8A77_ABEFA6640C5F__INCLUDED_)
#define AFX_CDETECTDLG_H__1CC4DEE5_E208_4840_8A77_ABEFA6640C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDetectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCDetectDlg dialog

class CCDetectDlg : public CDialog
{
// Construction
public:
	CCDetectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCDetectDlg)
	enum { IDD = IDD_CDETECT };
	int		m_detection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCDetectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCDetectDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDETECTDLG_H__1CC4DEE5_E208_4840_8A77_ABEFA6640C5F__INCLUDED_)
