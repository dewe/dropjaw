// DJGStatistics.cpp: implementation of the CDJGStatistics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afx.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>
#include <afxcoll.h>
#include <afxinet.h>
#include "DJTextBufferHelpers.h"
#include "DJGStatistics.h"
#include "DJTDocument.h"
#include "Chunkify.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJGStatistics::CDJGStatistics()
{
	m_HtmlEntityMap.InitHashTable( HashTableSize(300) );

	InitMapFromStrDataFile( g_strHomeDir + DJ_STATAGENT_HTMLENTITY_FILENAME,
								&m_HtmlEntityMap, " " );
}

CDJGStatistics::~CDJGStatistics()
{

}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// ExtractStatistics:
// Read data from file and fill document with extracted statistics.
// Most of this function handles statistics for genre classification,
// but you can find some URL grabbing as well. Call only once per 
// document.
//
void CDJGStatistics::ExtractStatistics( CStdioFile *pOpenFile, 
									    CDJTDocument *pDoc )
{
	char szBuf[ DJ_STATAGENT_READBUF_SIZE ];
	BOOL bNewDoc = TRUE;

	#ifdef _PrintTextStats
		double nBufSizePre=0, nBufSizePost=0;
		int nBufs=0, nLBreak=0, nCRet=0;
	#endif //_PrintTextStats

	while( ReadLines(pOpenFile, szBuf, DJ_STATAGENT_READBUF_LIMIT, bNewDoc) )
	{
		// For debugging...
		#ifdef _PrintTextStats
			nBufs++;
	
			LPCTSTR pszStart = szBuf;
			while( pszStart = strchr(pszStart, '\n') )
			{	nLBreak++;	pszStart++;	}
	
			pszStart = szBuf;
			while( pszStart = strchr(pszStart, '\r') ) 
			{	nCRet++;	pszStart++;	}

			nBufSizePre += strlen( szBuf );
		#endif //_PrintStats

		// Actual work done here.
		GrabUrls( szBuf, pDoc );	// Add all found urls to documents UrlMap.
		CountHtmlTags( szBuf, pDoc, bNewDoc );		// Count html features.
		StripTags( szBuf, bNewDoc );	// Remove all text between < and >.
		NormalizeText( szBuf );		// Prepare for counting text features.

		// More debugging...
		#ifdef _PrintTextStats
			nBufSizePost += strlen( szBuf );
		#endif //_PrintStats

		#ifdef _PrintNormalizedText
			cout << szBuf << flush;
		#endif //_PrintNormalizedText

		// And more work.
		CountWordFeatures( szBuf, pDoc );

		bNewDoc = FALSE;
	}

	#ifdef _PrintTextStats
		printf("#EOF\n");
		printf("\nProcessed %d buffers\n", nBufs);
		printf("%d linebreaks.\n", nLBreak);
		printf("%d carriage returns.\n", nCRet);
		printf("Buffer %.2f percent of original size.\n", 
			(100 * nBufSizePost / nBufSizePre) );
	#endif //_PrintTextStats

	::Sleep(0);	// Friendly thread.
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// ReadLines:
// Read linewise into a buffer.
//
BOOL CDJGStatistics::ReadLines( CStdioFile *pOpenFile, 
							    LPTSTR pszBuf, 
								UINT nBufSize,
								BOOL bNewFile )
{
	static char szLine[ DJ_STATAGENT_READLINE_SIZE ];
	static char *pszRet;
	UINT nSpaceLeft = nBufSize;
	BOOL bReturn = FALSE;
	
	// New file? Re-initialize...
	if( bNewFile )
	{
		szLine[0] = '\0';
		pszRet = szLine;
	}

	// Reset big buffer, in preparation for strcat...
	pszBuf[0] = '\0';
	
	// If line buffer empty - read new line.
	if( pszRet != NULL && strlen(szLine) == 0 )
		pszRet = pOpenFile->ReadString(szLine, DJ_STATAGENT_READLINE_SIZE);
	
	// Read linewise until not enough space in buffer.
	while( pszRet != NULL && strlen(szLine) < nSpaceLeft )
	{
		bReturn = TRUE;
		strcat( pszBuf, szLine );
		nSpaceLeft -= strlen(szLine);
		pszRet = pOpenFile->ReadString(szLine, DJ_STATAGENT_READLINE_SIZE);
	}
	
	return bReturn;
}


//////////////////////////////////////////////////////////////////////
// StripTags:
// Rips through a buffer and replaces HTML tags with space. Remember
// its state between buffers.
//
void CDJGStatistics::StripTags( LPTSTR pszBuf, BOOL bNewDoc )
{
	LPTSTR pszEnd, pszBegin = pszBuf;
	static BOOL bInTag = FALSE;

	// New document? Re-initialize...
	if( bNewDoc )
		bInTag = FALSE;

	// Process buffer.
	if( bInTag )
	{
		pszEnd = strchr( pszBegin, '>' );
		if( pszEnd == NULL )
		{
			*pszBegin = '\0';	// No end of tag, whole buffer is in tag.
			return;
		}
		else
		{
			*pszEnd = ' ';
			memmove( pszBegin, pszEnd, strlen(pszEnd)+1 );
			bInTag = FALSE;
		}
	}

	while( pszBegin = strchr(pszBegin, '<') )
	{
		bInTag = TRUE;
		pszEnd = strchr( pszBegin, '>' );
		if( pszEnd == NULL )
		{
			*pszBegin = NULL;	// Rest of buffer is in tag.
			break;
		}
		else
		{
			*pszEnd = ' ';
			memmove( pszBegin, pszEnd, strlen(pszEnd)+1 );
			bInTag = FALSE;
		}
	}

	::Sleep(0);	// Friendly thread.
}


//////////////////////////////////////////////////////////////////////
// CountHtmlTags:
//
void CDJGStatistics::CountHtmlTags( LPCTSTR pszBuf, CDJTDocument *pDoc, BOOL bReset )
{
	LPTSTR pszLwrBuf;
	static BOOL bInEmbed = FALSE;
	static BOOL bInApplet = FALSE;
	static BOOL bInScript = FALSE;
	static BOOL bInMeta = FALSE;
	static CDJTDocument *pLastDoc = NULL;

	// New document? Re-initialize...
	if( bReset )
	{
		pLastDoc = pDoc;
		bInEmbed = FALSE;
		bInApplet = FALSE;
		bInScript = FALSE;
		bInMeta = FALSE;
	}

	// Duplicate buffer, and convert to lower case.
	pszLwrBuf = _strdup(pszBuf);
	if( pszLwrBuf == NULL ) AfxThrowMemoryException();
	pszLwrBuf = _strlwr(pszLwrBuf);

	// Do the counting stuff...
	TagsAndComments( pszLwrBuf, 
					 &pDoc->m_nTagsTotal, 
					 &pDoc->m_nCommentsTotal,
					 bReset );

	AddToCounter( pszLwrBuf, "<a href", &pDoc->m_nHrefTags );
	AddToCounter( pszLwrBuf, "<img", &pDoc->m_nImgTags );
	AddToCounter( pszLwrBuf, "</p", &pDoc->m_nEndPTags );
	AddToCounter( pszLwrBuf, "</html", &pDoc->m_nEndHtmlTags );
	AddToCounter( pszLwrBuf, "<meta", &pDoc->m_nMetaTags );
	AddToCounter( pszLwrBuf, "<embed", &pDoc->m_nActiveTags );
	AddToCounter( pszLwrBuf, "<applet", &pDoc->m_nActiveTags );
	AddToCounter( pszLwrBuf, "<script", &pDoc->m_nActiveTags );
	AddToCounter( pszLwrBuf, "<form", &pDoc->m_nFormTags );

	AddToCounter( pszLwrBuf, "<br><br>", &pDoc->m_nRepetedTags );
	AddToCounter( pszLwrBuf, "&nbsp;&nbsp;", &pDoc->m_nRepetedTags );

	CommercialImgs( pszLwrBuf, &pDoc->m_nCommImgs );

	CharsInTag( pszLwrBuf, "<embed",  "</embed",  &pDoc->m_nCharsInActive, &bInEmbed );
	CharsInTag( pszLwrBuf, "<applet", "</applet", &pDoc->m_nCharsInActive, &bInApplet );
	CharsInTag( pszLwrBuf, "<script", "</script", &pDoc->m_nCharsInActive, &bInScript );
	CharsInTag( pszLwrBuf, "<meta", ">", &pDoc->m_nCharsInMeta, &bInMeta );

	// Clean up.
	free( pszLwrBuf );

	::Sleep(0);	// Friendly thread.
}


////////////////////////////////////////////////////////////////////////
// CountWordFeatures:
//
void CDJGStatistics::CountWordFeatures( LPTSTR pszBuf, CDJTDocument *pDoc )
{
	LPTSTR pszToken, pszRestBuf = pszBuf;
	char separators[] = "\x020\t\n\r\f\v";
	int nTokLength;

	pszToken = strtok( pszBuf, separators );
	while( pszToken != NULL )
	{
		if( strcmp(pszToken, ";") == 0 )
			pDoc->m_nSentences++;
		
		else
		{
			nTokLength = strlen( pszToken );
			if( nTokLength <= DJ_STATAGENT_MAX_TOKENLENGTH )
				pDoc->m_Mendenhall[nTokLength]++;

			if( nTokLength > 7 ) 
				pDoc->m_nLongWords++;

			if( IsNumber(pszToken) )		// Number?
				pDoc->m_nDigits++;

			if( strchr(pszToken, '\'') )	// Contraction?
				pDoc->m_nContractions++;

			if( isupper((unsigned char)*pszToken) )
			{
				(*pDoc->m_pCapTokenMap)[pszToken]++;
				pDoc->m_nCapWords++;
			}

			pDoc->m_nCharsTotal += nTokLength;
			(*pDoc->m_pTokenMap)[_strlwr(pszToken)]++;
			pDoc->m_nWordsTotal++;
		}

		// Read next token.
		pszToken = strtok( NULL, separators );

	}

	pDoc->m_nTypesTotal = pDoc->m_pTokenMap->GetCount();
	pDoc->m_nCapTypes = pDoc->m_pCapTokenMap->GetCount();

	::Sleep(0);	// Friendly thread.
}


//////////////////////////////////////////////////////////////////////
// PickServerFromUrl:
// 
CString CDJGStatistics::PickServerFromUrl( LPCTSTR pszUrl )
{
	BOOL bReturn;
	DWORD dwServiceType;
	CString strServer;
	CString strObject;
	INTERNET_PORT nPort;

	bReturn = AfxParseURL(pszUrl, dwServiceType, strServer, strObject, nPort);

	if( bReturn == FALSE )
		strServer.Empty();

	return strServer;
}


//////////////////////////////////////////////////////////////////////
// GrabUrls:
// Seek for URLs in buffer and add them to document. Add documents 
// own URL too.
//
void CDJGStatistics::GrabUrls( LPCTSTR pszBuf, CDJTDocument *pDoc )
{
	LPTSTR pszLwrBuf, pszNext, pszUrl;
	CString strServer, strUrl;
	char szUrlBreaks[] = "\">< \t\r\n";
	
	// Duplicate buffer, and convert to lower case.
	pszLwrBuf = _strdup(pszBuf);
	if( pszLwrBuf == NULL ) AfxThrowMemoryException();
	pszLwrBuf = _strlwr(pszLwrBuf);

	pszNext = pszLwrBuf;
	while( pszNext != NULL && (pszUrl = strstr(pszNext, "href=\"")) )
	{
		pszUrl = strchr( pszUrl, '\"' );
		if( pszNext = strchr(++pszUrl, '\"') )
		{
			*pszNext++ = '\0';
			strServer = PickServerFromUrl( pszUrl );
			(*pDoc->m_pUrlMap)[pszUrl]++;
			if( !strServer.IsEmpty() )	// Valid Url?
				(*pDoc->m_pUrlMap)[(LPCTSTR)strServer]++;
		}
	}

	// Add documents own URL.
	ASSERT( !pDoc->m_strUrl.IsEmpty() );
	strUrl = pDoc->m_strUrl;
	strServer = PickServerFromUrl( (LPCTSTR)strUrl );
	(*pDoc->m_pUrlMap)[(LPCTSTR)strUrl]++;
	(*pDoc->m_pUrlMap)[(LPCTSTR)strServer]++;

	ASSERT( !pDoc->m_pUrlMap->IsEmpty() );

	// Clean up.
	free( pszLwrBuf );
	::Sleep(0);	// Friendly thread.
}


//////////////////////////////////////////////////////////////////////
// InitMapFromStrDataFile:
// Read pair of strings 'key value' into a Map.
//
void CDJGStatistics::InitMapFromStrDataFile(LPCTSTR pszFName, 
							CMapStringToString *pMap,
							LPCTSTR pszDefault )
{
	try
	{
		int i;
		CString strData, strKey, strValue;
		CStdioFile dataFile( pszFName, CFile::modeRead |
			CFile::shareDenyWrite | CFile::typeText );

		while( dataFile.ReadString(strData) )
		{
			strData.TrimLeft();

			if( !strData.IsEmpty() && (strData[0] != '#') )
			{
				i = strData.FindOneOf( " \t" );
				if( i < 0 )		// No value, use default.
				{
					strKey = strData;
					strValue = pszDefault;
				}
				else
				{
					strKey = strData.Left(i);
					strData = strData.Mid(i);
					strData.TrimLeft();

					if( strData.IsEmpty() )	// No value, use default
						strValue = pszDefault;

					i = strData.FindOneOf( " \t" );
					if( i >= 0 )
						strValue = strData.Left(i);
					else
						strValue = strData;
	
				}

				pMap->SetAt( strKey, strValue );
			}
		}
	}

	catch( CFileException *pEx )
	{
		#ifndef _CONSOLE
			pEx->ReportError();
		#else
			printf( "\nError while reading %s\n", pEx->m_strFileName );
		#endif

		throw;
	}

	ASSERT( !pMap->IsEmpty() );	// There has been things added to table, has it?
}


//////////////////////////////////////////////////////////////////////
// CharsInTag:
//
void CDJGStatistics::CharsInTag(LPCTSTR pszBuf, 
								LPCTSTR pszStartTag, 
								LPCTSTR pszEndTag, 
								int *pCounter, 
								BOOL *pbInTag )
{
	LPCTSTR pszEnd, pszBegin = pszBuf;

	if( *pbInTag )
	{
		pszEnd = strstr( pszBegin, pszEndTag );
		if( pszEnd == NULL ) 
		{
			*pCounter += strlen(pszBegin);
			return;
		}
		*pbInTag = FALSE;
		*pCounter += pszEnd - pszBegin;
		pszBegin = pszEnd;
	}

	while( pszBegin = strstr(pszBegin, pszStartTag) )
	{
		*pbInTag = TRUE;

		pszEnd = strstr( pszBegin, pszEndTag );
		if( pszEnd == NULL ) 
		{
			*pCounter += strlen(pszBegin) - 1;
			break; 
		}
		*pbInTag = FALSE;
		*pCounter += pszEnd - pszBegin - 1;
		pszBegin = pszEnd;
	}
}


//////////////////////////////////////////////////////////////////////
// CommercialImgs:
//
void CDJGStatistics::CommercialImgs( LPCTSTR pszBuf, int *pCounter )
{
	LPCTSTR pszBegin = pszBuf;
	LPCTSTR pszEnd, pszWidth, pszHeight;
	int nW, nH;

	while( pszBegin = strstr(pszBegin, "<img") )
	{
		pszEnd = strchr( pszBegin, '>' );
		if( pszEnd == NULL ) break;

		pszWidth = strstr( pszBegin, "width=" );
		pszHeight = strstr( pszBegin, "height=" );

		if( pszWidth != NULL && pszHeight != NULL &&
			pszWidth < pszEnd && pszHeight < pszEnd )
		{
			if( (sscanf(pszWidth, "width=%d", &nW) > 0 
				|| sscanf(pszWidth, "width=\"%d", &nW) > 0) && 
				(sscanf(pszHeight, "height=%d", &nH) > 0 
				|| sscanf(pszHeight, "height=\"%d", &nH) > 0) )
			{
				// Is 400 <= width <= 550 and 50 <= height <= 70?
				if( (400<=nW && nW<=550) && (50<=nH && nH<=70) ) 
					(*pCounter)++;
			}
		}

		pszBegin++;
	}
}


//////////////////////////////////////////////////////////////////////
// TagsAndComments:
//
void CDJGStatistics::TagsAndComments( LPCTSTR pszBuf, 
									  int *pnTagCnt, 
									  int *pnCmntCnt,
									  BOOL bNewDoc)
{
	LPCTSTR pszTmp, pszEnd, pszBegin = pszBuf;
	static BOOL bInComment = FALSE;
	static BOOL bInTag = FALSE;

	// New document? Re-initialize...
	if( bNewDoc )
	{
		bInComment = FALSE;
		bInTag = FALSE;
	}

	if( bInComment )
	{
		pszEnd = strstr( pszBegin, "-->" );
		if( pszEnd == NULL)
			return;	// Comment did not end in buffer. Break.
		bInComment = FALSE;
		(*pnCmntCnt)++;
		pszBegin = pszEnd;
	}
	else if( bInTag )
	{
		pszEnd = strchr( pszBegin, '>' );
		if( pszEnd == NULL ) 
			return;  // Broken tag, don't count.
		bInTag = FALSE;
		(*pnTagCnt)++;
		pszBegin = pszEnd;
	}

	while( pszBegin = strchr(pszBegin, '<') )
	{
		// Check for comment.
		pszTmp = strstr( pszBegin, "<!--" );
		if( pszTmp == pszBegin )
		{
			bInComment = TRUE;

			pszEnd = strstr( pszBegin, "-->" );
			if( pszEnd == NULL)
				break;	// Comment did not end in buffer. Break.
			bInComment = FALSE;
			(*pnCmntCnt)++;
		}
		else	// In tag.
		{
			bInTag = TRUE;

			pszEnd = strchr( pszBegin, '>' );
			if( pszEnd == NULL ) 
				break;  // Broken tag, don't count.
			bInTag = FALSE;
			(*pnTagCnt)++;
		}
		pszBegin = pszEnd;

	}
}


////////////////////////////////////////////////////////////////////////
// RmvAbbrevPeriod:
// Remove all periods used in abbreviations, i.e. "<dot><alpha>" or
// "<dot><space><lowercase>". As a side-effect (ouch!) other 
// character combinations will be reduced as well. For example, 
// when next alpha is lower case, three periods (...) will be reduced 
// to two periods (..). 
//
// If used, this function must be run before SentenceCapToLwr, else it 
// will remove all sentence breaking periods as well.
// 
void CDJGStatistics::RmvAbbrevPeriod( LPTSTR pszBuf )
{
	LPTSTR pszSource, pszDot = pszBuf;
	unsigned char *pcNextNonSpace, *pcNext;

	while( pszDot = strstr(pszDot, ".") )
	{
		pcNext = (unsigned char*) pszDot+1;
		pcNextNonSpace = (unsigned char*) NextNonSpace( pszDot+1 );

		// Next character is an alpha. Remove dot.
		if( isalpha(*pcNext) )
		{
			pszSource = pszDot+1;
			memmove( pszDot, pszSource, strlen(pszSource)+1 );
		}

		// Next found alphanum is lower case. Remove dot.
		else if( isspace(*pcNext) 
				 && (pcNextNonSpace != NULL) 
				 && islower(*pcNextNonSpace) )
		{
			pszSource = pszDot+1;
			memmove( pszDot, pszSource, strlen(pszSource)+1 );
		}

		pszDot++;
	}
}


////////////////////////////////////////////////////////////////////////
// RplPeriodNonDec:
// Replace all periods but decimail periods with the given string.
// 
void CDJGStatistics::RplPeriodNonDec( LPTSTR pszBuf, LPCTSTR szTo )
{
	LPTSTR pRest, pNewEnd, pszDot;
	unsigned char *pcNext;

	pszDot = pszBuf;
	while( pszDot = strstr(pszDot, ".") )
	{
		pcNext = (unsigned char*) pszDot+1;

		// Next char is a digit. Replace period.
		if( !isdigit(*pcNext) )
		{
			pRest = pszDot+1; 
			pNewEnd = pszDot + strlen( szTo );
			memmove( pNewEnd, pRest, strlen(pRest)+1 );
			memmove( pszDot, szTo, strlen(szTo) );
		}

		pszDot++;
	}
}


////////////////////////////////////////////////////////////////////////
// SentenceCapToLwr:
//
// If a sentence, or buffer, starts with a name, that name will also be
// converted to lower case. Though luck.
// 
void CDJGStatistics::SentenceCapToLwr( LPTSTR pszBuf, 
									   LPCTSTR pszSentenceBreak )
{
	LPTSTR pszHit = pszBuf;
	unsigned char *pcNextAlfa;

	// Fix sentence-capitals first in buffer.
	if( pcNextAlfa = (unsigned char*)NextAlphaNum(pszBuf) )
		*pcNextAlfa = tolower( *pcNextAlfa );

	while( pszHit = strstr(pszHit, pszSentenceBreak) )
	{
		// Convert next char to lower case.
		if( pcNextAlfa = (unsigned char*)NextAlphaNum(pszHit+2) )
			*pcNextAlfa = tolower( *pcNextAlfa );

		pszHit++;
	}
}


////////////////////////////////////////////////////////////////////////
// NormalizeText:
//
void CDJGStatistics::NormalizeText( LPTSTR pszBuf )
{
	// Replace Html entities (i.e &nbsp; etc).
	ReplaceAll( pszBuf, &m_HtmlEntityMap );	

	// Replace nasty linebreaks.
	Replace( pszBuf, "\n\r", "\n" );
	Replace( pszBuf, "\n\n", "\n" );
	Replace( pszBuf, "\r", "\n" );

	// Join hyphenated words at line breaks.
	Replace( pszBuf, "-\n", "-" );

	// Replace punctuation (lesser).
	Replace( pszBuf, "(", " " );
	Replace( pszBuf, ")", " " );
	Replace( pszBuf, "{", " " );
	Replace( pszBuf, "}", " " );
	Replace( pszBuf, "[", " " );
	Replace( pszBuf, "]", " " );
	Replace( pszBuf, ",", " " );
	Replace( pszBuf, "\"", " " );
	Replace( pszBuf, "<", " " );
	Replace( pszBuf, ">", " " );
	
	ASSERT( !strstr(pszBuf, "(") );
	ASSERT( !strstr(pszBuf, ")") );
	ASSERT( !strstr(pszBuf, "{") );
	ASSERT( !strstr(pszBuf, "}") );
	ASSERT( !strstr(pszBuf, "[") );
	ASSERT( !strstr(pszBuf, "]") );
	ASSERT( !strstr(pszBuf, "\""));
	ASSERT( !strstr(pszBuf, "<") );
	ASSERT( !strstr(pszBuf, ">") );

	// Replace punctuation (sentence break).
	Replace( pszBuf, ";", " ; " );
	Replace( pszBuf, ":", " ; " );
	Replace( pszBuf, "!", " ; " );
	Replace( pszBuf, "?", " ; " );
	Replace( pszBuf, "...", " ; " );	// New addition to Jussis normalization.
	
	// Remove period when used in abbrievations.
	RmvAbbrevPeriod( pszBuf );

	// Replace all periods but decimal periods.
	RplPeriodNonDec( pszBuf, " ; " );

	// Convert sentence initial capital to lower case.
	SentenceCapToLwr( pszBuf, ";" );

	::Sleep(0);	// Friendly thread.
}


