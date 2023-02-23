// ButtonDoc.h : interface of the CButtonDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "struct.h"


#if !defined(AFX_BUTTONDOC_H__422049A4_7AF9_4967_8B9F_1615597301B3__INCLUDED_)
#define AFX_BUTTONDOC_H__422049A4_7AF9_4967_8B9F_1615597301B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CButtonDoc : public CDocument
{
protected: // create from serialization only
	CButtonDoc();
	DECLARE_DYNCREATE(CButtonDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	Parameters params, defpars;
	CString toString();
	
// Generated message map functions
protected:
	//{{AFX_MSG(CButtonDoc)
	afx_msg void OnEditConfig();
	afx_msg void OnActionStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONDOC_H__422049A4_7AF9_4967_8B9F_1615597301B3__INCLUDED_)
