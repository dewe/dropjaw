// DJAltaVistaQuery.cpp: implementation of the CDJAltaVistaQuery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJAltaVistaQuery.h"
#include "DJUrlAgent.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJAltaVistaQuery::CDJAltaVistaQuery()
{
	m_strBaseUrl = DJ_ALTAVISTA_BASE_URL;
	m_strQuery = "";
	m_nStq = -10;		// Negative before first call to GetURL().

	// Static settings...
	m_strMss = "gb";	// Preferred lang.	(gb = english)
	m_strPg = "q";		// Simple query.
	m_strWhat = "web";	// Search the web.
	m_strKl = "";		// Search for any language.	
	m_strFmt = "";		// Standard format.
	m_Country = "gb";	// Country.
}

CDJAltaVistaQuery::~CDJAltaVistaQuery()
{

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// SetQueryString
// Encode characters to escape sequences, and set the member variable.
// Reset the document counter.
//
void CDJAltaVistaQuery::SetQueryString( LPTSTR pszQuery )
{
	DWORD nLength = DJ_ALTAVISTA_MAX_QUERY+1;
	const int nBufSize = 3*DJ_ALTAVISTA_MAX_QUERY;
	char szBuf[ nBufSize+1 ];
	char *pszWorkBuf, *pszSubstring, *pszBreak;
	char szSpecialChars[] = ":\\ +/?";
	CString strRepl;

	m_strQuery.Empty();
	pszSubstring = pszWorkBuf = _strdup( pszQuery );

	// Look for characters needing special treatment.
	while( pszBreak = strpbrk(pszSubstring, szSpecialChars) )
	{
		switch( *pszBreak )
		{
		case ' ':
			strRepl = "+";		// Space
			break;

		case '+':
			strRepl = "%2B";	// "+"
			break;

		case '/':
			strRepl = "%2F";	// "/"
			break;

		case ':':
			strRepl = "%3A";	// ":"
			break;

		case '?':
			strRepl = "%3F";	// "?"
			break;

		case '\\':
			strRepl = "%5C";	// "\"
			break;

		default:
			ASSERT( FALSE );	// Shouldn't get here.
			break;
		}

		*pszBreak = '\0';

		if( strlen(pszSubstring) > 0 )
			VERIFY( InternetCanonicalizeUrl(pszSubstring, szBuf, &nLength, NULL) );
		ASSERT( strcmp(szBuf, "/") != 0 );
		m_strQuery += szBuf + strRepl;

		*szBuf = '\0';
		nLength = nBufSize;
		pszSubstring = pszBreak + 1;
	}

	// Catch the last substring of the query;
	if( strlen(pszSubstring) > 0 )
		VERIFY( InternetCanonicalizeUrl(pszSubstring, szBuf, &nLength, NULL) );
	ASSERT( strcmp(szBuf, "/") != 0 );
	m_strQuery += szBuf;

	free( pszWorkBuf );	// Clean up.
}



//////////////////////////////////////////////////////////////////////
// GetURL:
// Compose a proper URL to feed AltaVista. Returns true while 
// document count is less than DJ_ALTAVISTA_MAX_URLS.
//
// Constructed for swedish altavista mirror. 1997-08-12/Dewe
//
BOOL CDJAltaVistaQuery::GetURL( CString& strResult )
{
	m_nStq += 10;

	if( m_nStq < DJ_ALTAVISTA_MAX_URLS )
	{
		CString strUrl;
		CString strStq;
		
		ASSERT( !m_strBaseUrl.IsEmpty() );
		ASSERT( !m_strQuery.IsEmpty() );
		
		// Make the document counter string.
		strStq.Format("%d", m_nStq);
		
		// Start out with the URL to the AltaVista CGI-script.
		strUrl = m_strBaseUrl;
		
		// Add all the required parts of the GET.
		if( m_strPg.CompareNoCase("q") == 0 )
		{
			strUrl += "?mss=" + m_strMss;	// Pref. language.
			strUrl += "/search";			// search or refine?
			strUrl += "&pg=" + m_strPg;		// Type of query.
			strUrl += "&what=" + m_strWhat;	// Search the web.
			strUrl += "&fmt=" + m_strFmt;	// Format.
			strUrl += "&kl=" + m_strKl;		// Language.
			strUrl += "&q=" + m_strQuery;	// Query string.
			strUrl += "&stq=" + strStq;		// Document count.
		}
		else
		{
			ASSERT( FALSE );		// Do not allow advanced queries yet...
		}
		
		strResult = strUrl;
		return TRUE;		
	}
	
	else
		return FALSE;
}



