// DJAltaVistaQuery.h: interface for the CDJAltaVistaQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJALTAVISTAQUERY_H__5D605D70_01BC_11D1_8C2E_08002BE6493E__INCLUDED_)
#define AFX_DJALTAVISTAQUERY_H__5D605D70_01BC_11D1_8C2E_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Definitions
//
// const char	DJ_ALTAVISTA_BASE_URL[] = "http://altavista.digital.com/cgi-bin/query";
const char	DJ_ALTAVISTA_BASE_URL[] = "http://altavista.telia.com/cgi-bin/query";
const int	DJ_ALTAVISTA_MAX_URLS =  200;		// Max retrieved Urls from AltaVista.
const int	DJ_ALTAVISTA_MAX_QUERY =  800;		// Max characters in a query.

// CDJAltaVistaQuery class declaration
//
class CDJAltaVistaQuery  
{
public:
	// Construction
	CDJAltaVistaQuery();
	virtual ~CDJAltaVistaQuery();

	// Operations
	void SetQueryString(LPTSTR pszQuery);
	BOOL GetURL(CString& strResult);
	int MaxUrls() { return DJ_ALTAVISTA_MAX_URLS; }

	// Attributes
	CString m_strBaseUrl;	// AltaVista base address.
	CString m_strQuery;		// Query string.
	int m_nStq;				// Document count.

protected:
	// Implementation
	CString m_strMss;		// Preferred lang.	(gb = english)
	CString m_strPg;		// Type of query.	(q = simple)
	CString m_strWhat;		// What to search.	(web = the web)
	CString m_strKl;		// Language.		(XX = any language)
	CString m_strFmt;		// Format.			(. = standard)
	CString m_Country;		// Country			(gb = England)
	CString m_strR;			// Ranking string. Advanced queries only.
	CString m_strD0;		// Starting date. Advanced queries only.
	CString m_strD1;		// Ending date. Advanced queries only.

};

#endif // !defined(AFX_DJALTAVISTAQUERY_H__5D605D70_01BC_11D1_8C2E_08002BE6493E__INCLUDED_)
