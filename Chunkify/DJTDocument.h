// DJTDocument.h: interface for the CDJTDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
#define AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxmt.h>
#include <float.h>
#include <afxtempl.h>
#include "DJGenre.h"
#include "DJGenreProb.h"
#include "DJTempFile.h"
#include "DJTTermIndexMap.h"


// Definitions
//
const UINT DJ_DOCUMENT_TF_ARRAY_SIZE = 20000;	// Max expected size of term vector. (Tune)
const UINT DJ_DOCUMENT_TF_ARRAY_GROW = 2000;	// Added Size when growing term freq array. (Tune)
const UINT DJ_DOCUMENT_BUF_SIZE = 8192;			// Size of read buffer in 'ExtractStatistics()'.
const int DJ_DOCUMENT_MAXWORD = 1024;			// Maximum size of one word when counting...
const int DJ_DOCUMENT_TOKENSIZE = 7000;			// Max expected number of tokens.
const int DJ_DOCUMENT_CAPTOKENSIZE = 500;		// Max expected number of capital tokens.

// CDJTDocument class declaration
//
class CDJTDocument : public CObject  
{
	DECLARE_DYNAMIC( CDJTDocument );
public:
	void FreeStatsMem();
	// Constructors
	CDJTDocument();
	virtual ~CDJTDocument();

	// Attributes
	UINT m_nRank;
	CString m_strUrl;
	CString m_strTitle;
	CString m_strExtract;
	CString m_strSize;
	CString m_strDate;
	CString m_strLanguage;

	// Html features
	int m_nTagsTotal;		// Total # of tags in doc.
	int m_nCommentsTotal;
	int m_nHrefTags;		// # of <HREF ...> tags.
	int m_nImgTags;			// # of <IMG ...> tags.
	int m_nCommImgs;		// # of commercial images.
	int m_nEndPTags;		// # of </P> tags.
	int m_nRepetedTags;		// # of <P>, <BR> or &nbsp; in sequence.
	int m_nEndHtmlTags;		// # of </HTML> tags.
	int m_nMetaTags;		// # of <META ...> tags.
	int m_nCharsInMeta;		// # of words in meta tags.
	int m_nActiveTags;		// # of <EMBED>, <APPLET> and <SCRIPT> tags.
	int m_nCharsInActive;	// # of words in active tags.
	int m_nFormTags;		// # of <FORM ..> tags.

	// Word features.
	int m_nCharsTotal;		// Total # of characters in doc.
	int m_nWordsTotal;		// Total # of words in doc.
	int m_nTypesTotal;		// Total # of different word types.
	int m_nLongWords;		// # of long words (more than 7 character).
	int m_nDigits;			// # of digits.
	int m_nCapWords;		// # of words beginning with capital char.
	int m_nCapTypes;		// # of different words beginning w cap.
	int m_nContractions;	// # of contractions (i.e. # of ' ).
	int m_nSentences;		// # of sentences.
	CArray<int, int> m_Mendenhall;	// # of words of length n. n=1,2,3...

	CMap<CString, LPCTSTR, int, int> *m_pCapTokenMap;	// Cap words.
	CMap<CString, LPCTSTR, int, int> *m_pTokenMap;	// All tokens.
	CMap<CString, LPCTSTR, int, int> *m_pUrlMap;	// All found URLs.
	CMap<CString, LPCTSTR, double, double> *m_pFeatureScoreMap;
	CArray<double, double> *m_pFactorScores;

	// Operations
	double ScoreAt( LPCTSTR pszFeature );
	double SetGenre( int nGenre, double fValue );
	void InitGenreProbs();
	UINT Rank() { return m_nRank; }
	const CString& Url() { return m_strUrl; }
	const CString& Title() { return m_strTitle; }
	const CString& Extract() { return m_strExtract; }
	const CString& Lang() { return m_strLanguage; }
	const CString& Size() { return m_strSize; }
	const CString& Date() { return m_strDate; }

	void PrepareFromUrl(CDJUrl* pUrl);

	// Termlist operations
	double TermFreqArrayAt( USHORT usIndex );
	double AddToTermFreqArray(double fVal, USHORT usIndex);
	double WeightAt(int index);

	// Implementation (termlist)
	static CString PickServerFromUrl(LPCTSTR pszUrl);
	int m_nMaxIndex;
	CCriticalSection m_TermFreqArrayLock;

	// Debugging stuff...
	#ifdef _DEBUG
		void PrintStatistics();
		void Dump(CDumpContext& dc) const;
		void DumpTerms(CDumpContext& dc);
	#endif

	// Implementation
	CCriticalSection m_Lock;
	GenreProbPtrArray m_GenreProbPtrs;
protected:
	TermFreqArray m_TermFreqArray;
};


// Inline functions
//
inline double CDJTDocument::TermFreqArrayAt( USHORT usIndex )
{
	if( usIndex > m_nMaxIndex ) return 0;

	ASSERT( m_TermFreqArray[usIndex] >= 0 );
	return m_TermFreqArray[usIndex];
}

inline double CDJTDocument::WeightAt(int index)
{
	if( index > m_nMaxIndex ) return 0;
	if( TermFreqArrayAt(index) == 0 ) return 0;

	// Look for NaN and infinite numbers (debug)...
	ASSERT( !_isnan(TermFreqArrayAt(index)) && _finite(TermFreqArrayAt(index)) );
	ASSERT( !_isnan(TermIndexMap()->Idf(index)) && _finite(TermIndexMap()->Idf(index)) );

	return (double) TermFreqArrayAt(index) * TermIndexMap()->Idf(index);
}


// Type definitions
//
typedef CTypedPtrList<CObList, CDJTDocument*> DocumentPtrList;
typedef CTypedPtrArray<CObArray, CDJTDocument*> DocumentPtrArray;


#endif // !defined(AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
