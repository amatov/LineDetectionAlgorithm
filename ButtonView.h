// ButtonView.h : interface of the CButtonView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUTTONVIEW_H__FAC507A0_6D42_4E30_9802_8B9423787669__INCLUDED_)
#define AFX_BUTTONVIEW_H__FAC507A0_6D42_4E30_9802_8B9423787669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CButtonView : public CView
{
protected: // create from serialization only
	CButtonView();
	DECLARE_DYNCREATE(CButtonView)

// Attributes
public:
	CButtonDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CButtonView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ButtonView.cpp
inline CButtonDoc* CButtonView::GetDocument()
   { return (CButtonDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONVIEW_H__FAC507A0_6D42_4E30_9802_8B9423787669__INCLUDED_)
