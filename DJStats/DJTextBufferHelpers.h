// DJTextBufferHelpers.h: headerfile for text buffer help functions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DJTEXTBUFFERHELPERS_H__
#define __DJTEXTBUFFERHELPERS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <ctype.h>
#include <afxtempl.h>


void AddToCounter(LPCTSTR pszBuf, LPCTSTR pszTag, int *pCounter);
int Replace( LPTSTR pszBuf, LPCTSTR szFrom, LPCTSTR szTo );
int ReplaceAll( LPTSTR pszBuf, CMapStringToString *pMap );
LPTSTR NextToken( LPTSTR *ppszBuf );
BOOL IsNumber( LPCTSTR pszToken );
LPCTSTR NextCharSatisfying( LPCTSTR pszBuf, int (*testfunc)(int) );


////////////////////////////////////////////////////////////////////////
// NextAlpha:
//
inline LPCTSTR NextAlpha( LPCTSTR pszBuf )
{
	return NextCharSatisfying( pszBuf, &isalpha );
}

////////////////////////////////////////////////////////////////////////
// NextAlphaNum:
//
inline LPCTSTR NextAlphaNum( LPCTSTR pszBuf )
{
	return NextCharSatisfying( pszBuf, &isalnum );
}

////////////////////////////////////////////////////////////////////////
// NextSpace:
// 
inline LPCTSTR NextSpace( LPCTSTR pszBuf )
{
	return NextCharSatisfying( pszBuf, &isspace );
}

////////////////////////////////////////////////////////////////////////
// NextNonSpace:
// 
inline LPCTSTR NextNonSpace( LPCTSTR pszBuf )
{
	return NextCharSatisfying( pszBuf, &isgraph );
}

#endif //__DJTEXTBUFFERHELPERS_H__