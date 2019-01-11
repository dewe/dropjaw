// DJTextBufferHelpers.cpp: implementation of text buffer help functions.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DJTextBufferHelpers.h"
#include <ctype.h>
#include <string.h>
#include <afxtempl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// AddToCounter:
// Count the number of occurence of pszTag in pszBuf. Add to counter.
//
void AddToCounter(LPCTSTR pszBuf, LPCTSTR pszTag, int *pCounter)
{
	LPCTSTR pszHit = pszBuf;

	while( pszHit = strstr(pszHit, pszTag) )
	{
		(*pCounter)++;
		pszHit++;
	}
}


//////////////////////////////////////////////////////////////////////
// Replace: 
// Replace every occurance of szFrom with szTo. Return the 
// number of replacements that have been done. Assume there 
// is enough space in pszBuf.
//
int Replace( LPTSTR pszBuf, LPCTSTR szFrom, LPCTSTR szTo )
{
	LPTSTR pszHit, pRest, pNewEnd;
	int nReplaced = 0;

	pszHit = pszBuf;
	while( pszHit = strstr(pszHit, szFrom) )
	{
		pRest = pszHit + strlen(szFrom); 
		pNewEnd = pszHit + strlen(szTo);
		memmove( pNewEnd, pRest, strlen(pRest)+1 );
		memmove( pszHit, szTo, strlen(szTo) );
		pszHit = pNewEnd;
		nReplaced++;
	}
	return nReplaced;
}


//////////////////////////////////////////////////////////////////////
// ReplaceAll: 
// For every key-value pair of strings in *pMap, replace occurances 
// of keystring with the value-string. Return the number of 
// replacements that have been done.Assume there is enough space 
// in pszBuf.
//
int ReplaceAll( LPTSTR pszBuf, CMapStringToString *pMap )
{
	CString strKey, strValue;
	POSITION mapPos;
	int nReplaced = 0;

	mapPos = pMap->GetStartPosition();
	while( mapPos != NULL )
	{
		pMap->GetNextAssoc( mapPos, strKey, strValue );  
		nReplaced += Replace( pszBuf, (LPCTSTR) strKey, (LPCTSTR) strValue ); 
	}

	return nReplaced;
}


////////////////////////////////////////////////////////////////////////
// NextCharSatisfying:
// Return pointer to first character in pszBuf that satisfy the 
// criterion of testfunc. If not found, return NULL.
// 
LPCTSTR NextCharSatisfying( LPCTSTR pszBuf, int (*testfunc)(int) )
{
	for( const char *pc = pszBuf; *pc != '\0'; pc++ ) 
	{
		if( (*testfunc)((unsigned char)*pc) ) 
			return pc;
	}
	
	return NULL;
}


////////////////////////////////////////////////////////////////////////
// NextToken:
// Returns pointer to the first character in next token. 
// A pointer to the rest of the buffer will be stored 
// in *pszBuf. If no token is found (i.e. empty line) 
// a NULL will be returned.
//
// Note:
// Terminates the buffer with a NULL after the first token 
// (i.e. at the first space after token) .
//
LPTSTR NextToken( LPTSTR *ppszBuf )
{
	LPTSTR pToken, pEnd;

	if( pToken = (char*) NextNonSpace(*ppszBuf) )
	{
		if( pEnd = (char*) NextSpace(pToken) )
		{
			*pEnd = '\0';
			*ppszBuf = pEnd+1;
		}
		else
			*ppszBuf = pToken+strlen(pToken);
	}

	return pToken;
}


////////////////////////////////////////////////////////////////////////
// IsNumber:
// Return TRUE if _all_ characters in buffer ar digits or decimal dot.
//
BOOL IsNumber( LPCTSTR pszToken )
{
	LPCTSTR pcChar = pszToken;

	while( *pcChar != '\0' )
	{
		if( !isdigit((unsigned char)*pcChar) && *pcChar != '.' ) 
			return FALSE;
		pcChar++;
	}

	return TRUE;
}




