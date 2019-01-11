// DJTDocument.h: interface for the CDJTDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
#define AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxtempl.h>
#include "DJGenreProb.h"


// CDJTDocument class declaration
//
class CDJTDocument : public CObject  
{
	DECLARE_DYNAMIC( CDJTDocument );
public:
	void InitGenreProbs();
	// Constructors
	CDJTDocument();
	virtual ~CDJTDocument();

	CString m_strUrl;

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

	GenreProbPtrArray m_GenreProbPtrs;

	// Operations
	double ScoreAt( LPCTSTR pszFeature );
	double SetGenre( int nGenre, double fValue );
	void FreeStatsMem();

	// Debugging
	#ifdef _DEBUG
		void PrintStatistics();
		void PrintLists();
	#endif //_DEBUG

};


#endif // !defined(AFX_DJTDOCUMENT_H__F8D725B0_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
