// DJGStatistics.h: interface for the CDJGStatistics class.
// Statistic functions for genre classification.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJGSTATISTICS_H__001D8E22_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_)
#define AFX_DJGSTATISTICS_H__001D8E22_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include "DJTDocument.h"


// Definitions
//
const char	DJ_STATAGENT_HTMLENTITY_FILENAME[] = "data\\html_ent.djs";
const int	DJ_STATAGENT_MAX_TOKENLENGTH = 50;
const UINT	DJ_STATAGENT_READBUF_SIZE = 8192;
const UINT	DJ_STATAGENT_READBUF_LIMIT = 4095;	// Approx half of readbuf size.
const UINT	DJ_STATAGENT_READLINE_SIZE = 1023;



// CDJGStatistics class definition
//
class CDJGStatistics  
{
public:
	// Construction
	CDJGStatistics();
	virtual ~CDJGStatistics();

	// Attributes
	CMapStringToString m_HtmlEntityMap;

	// Operations
	void ExtractStatistics(CStdioFile *pOpenFile, CDJTDocument *pDoc);

	// Implementation
	BOOL ReadLines(CStdioFile *pOpenFile, 
				   LPTSTR pszBuf, 
				   UINT nBufSize, 
				   BOOL bNewFile);
	void StripTags(LPTSTR pszBuf, BOOL bNewDoc);
	void CountHtmlTags(LPCTSTR pszBuf, CDJTDocument *pDoc, BOOL bReset);
	void CountWordFeatures(LPTSTR pszBuf, CDJTDocument *pDoc);

	CString PickServerFromUrl(LPCTSTR pszUrl);
	void GrabUrls(LPCTSTR pszBuf, CDJTDocument *pDoc);
	void NormalizeText(LPTSTR pszBuf);
	void CommercialImgs(LPCTSTR pszBuf, int *pCounter);
	void TagsAndComments(LPCTSTR pszBuf, int *pnCmntCnt, int *pnTagCnt, BOOL bNewDoc );
	void RmvAbbrevPeriod(LPTSTR pszBuf);
	void RplPeriodNonDec(LPTSTR pszBuf, LPCTSTR szTo);
	void SentenceCapToLwr(LPTSTR pszBuf, LPCTSTR pszSentenceBreak);
	void InitMapFromStrDataFile(LPCTSTR pszFName, 
								CMapStringToString *pMap,
								LPCTSTR pszDefault);
	void CharsInTag(LPCTSTR pszBuf, 
					LPCTSTR pszStartTag, 
					LPCTSTR pszEndTag, 
					int *pCounter, 
					BOOL *pbInTag);

};

#endif // !defined(AFX_DJGSTATISTICS_H__001D8E22_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_)
