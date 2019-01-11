// DJUrl.h: interface for the CDJUrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJURL_H__DDBD7A72_EEC7_11D0_8C2C_08002BE6493E__INCLUDED_)
#define AFX_DJURL_H__DDBD7A72_EEC7_11D0_8C2C_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxtempl.h>

class CDJUrl : public CObject  
{
	DECLARE_DYNAMIC( CDJUrl );
public:
	// Constructors
	CDJUrl() { }
	virtual ~CDJUrl() { }
	
	// Attributes
	UINT m_nRank;
	CString m_strUrl;
	CString m_strTitle;
	CString m_strExtract;
	CString m_strSize;
	CString m_strDate;
	CString m_strLang;
	
#ifdef _DEBUG
	void Dump(CDumpContext& dc);
#endif
};

typedef CTypedPtrList<CObList, CDJUrl*> UrlPtrList;

#endif // !defined(AFX_DJURL_H__DDBD7A72_EEC7_11D0_8C2C_08002BE6493E__INCLUDED_)
