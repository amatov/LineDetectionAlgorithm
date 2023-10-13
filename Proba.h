// Proba.h : main header file for the PROBA DLL
//

#if !defined(AFX_PROBA_H__5283BFD9_7CCE_4FB8_962C_12796F6BFE89__INCLUDED_)
#define AFX_PROBA_H__5283BFD9_7CCE_4FB8_962C_12796F6BFE89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProbaApp
// See Proba.cpp for the implementation of this class
//

class CProbaApp : public CWinApp
{
public:
	CProbaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProbaApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CProbaApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROBA_H__5283BFD9_7CCE_4FB8_962C_12796F6BFE89__INCLUDED_)
