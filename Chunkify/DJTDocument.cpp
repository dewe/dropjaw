// DJTDocument.cpp: implementation of the CDJTDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h" 
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <afxinet.h>
#include "Chunkify.h"
#include "DJTDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJTDocument, CObject );


/*// Template collection helper function.
//
template <> void AFXAPI DestructElements<int>( int *pValue, int nCount )
{
    for ( int i=0; i<nCount; i++, pValue++ )
		delete pValue;
}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJTDocument::CDJTDocument()
{	 
	m_nMaxIndex = -1;

	// Initialize feature attributes
	m_nTagsTotal = 0;
	m_nCommentsTotal = 0;
	m_nHrefTags = 0;
	m_nImgTags = 0;
	m_nCommImgs = 0;
	m_nEndPTags = 0;
	m_nRepetedTags = 0;
	m_nEndHtmlTags = 0;
	m_nMetaTags = 0;
	m_nCharsInMeta = 0;
	m_nActiveTags = 0;
	m_nCharsInActive = 0;
	m_nFormTags = 0;

	m_nCharsTotal = 0;
	m_nWordsTotal = 0;
	m_nTypesTotal = 0;
	m_nLongWords = 0;
	m_nDigits = 0;
	m_nCapWords = 0;
	m_nCapTypes = 0;
	m_nContractions = 0;
	m_nSentences = 0;

	m_pCapTokenMap = new CMap<CString, LPCTSTR, int, int>;
	m_pTokenMap = new CMap<CString, LPCTSTR, int, int>;
	m_pUrlMap = new CMap<CString, LPCTSTR, int, int>;
	m_pFeatureScoreMap = new CMap<CString, LPCTSTR, double, double>;
	m_pFactorScores = new CArray<double, double>;

	m_Mendenhall.SetSize( 100 );
	m_pFactorScores->SetSize( DJ_GENRE_NUM_OF_FACTORS );

	// For efficiency, set the sizes before start.
	UINT nTFInitSize = max( DJ_DOCUMENT_TF_ARRAY_SIZE, TermIndexMap()->GetCount() );
	m_TermFreqArray.SetSize( nTFInitSize, nTFInitSize );
	m_pTokenMap->InitHashTable( HashTableSize(DJ_DOCUMENT_TOKENSIZE) );
	m_pCapTokenMap->InitHashTable( HashTableSize(DJ_DOCUMENT_CAPTOKENSIZE) );
	m_pUrlMap->InitHashTable( HashTableSize(1000) );	// Arbitrary... max num of urls in doc.

	InitGenreProbs();
}

CDJTDocument::~CDJTDocument()
{
	// Clean up, delete the "genre probs".
	for( int i=0; i<m_GenreProbPtrs.GetSize(); i++ )
	{
		if( m_GenreProbPtrs[i] != NULL )
			delete m_GenreProbPtrs[i];
	}
	m_GenreProbPtrs.RemoveAll();

	if( m_pTokenMap != NULL )
	{
		m_pTokenMap->RemoveAll();
		delete m_pTokenMap;
		m_pTokenMap = NULL;
	}
	
	if( m_pCapTokenMap != NULL )
	{
		m_pCapTokenMap->RemoveAll();
		delete m_pCapTokenMap;
		m_pCapTokenMap = NULL;
	}

	if( m_pUrlMap != NULL )
	{
		m_pUrlMap->RemoveAll();
		delete m_pUrlMap;
		m_pUrlMap = NULL;
	}

	if( m_pFeatureScoreMap != NULL )
	{
		m_pFeatureScoreMap->RemoveAll();
		delete m_pFeatureScoreMap;
		m_pFeatureScoreMap = NULL;
	}

	if( m_pFactorScores != NULL )
	{
		m_pFactorScores->RemoveAll();
		delete m_pFactorScores;
		m_pFactorScores = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// PrepareFromUrl:
// Copy the interesting data from collected in the URL.
//
void CDJTDocument::PrepareFromUrl(CDJUrl* pUrl)
{
	m_nRank = pUrl->m_nRank;
	m_strUrl = pUrl->m_strUrl;
	m_strTitle = pUrl->m_strTitle;
	m_strExtract = pUrl->m_strExtract;
	m_strSize = pUrl->m_strSize;
	m_strDate = pUrl->m_strDate;
	m_strLanguage = pUrl->m_strLang;
}


//////////////////////////////////////////////////////////////////////
// Debugging
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Dump:
//
#ifdef _DEBUG
void CDJTDocument::Dump(CDumpContext& dc) const
{
	// Call base class function first.
	CObject::Dump( dc );

	// Dump info about document.
	dc  << "Title:\t" << m_strTitle << "\n"
		<< "Rank:\t" << m_nRank << "\n"
		<< "Language:\t" << m_strLanguage << "\n"
		<< "Url:\t" << m_strUrl;

}
#endif

//////////////////////////////////////////////////////////////////////
// DumpTerms:
// Dump all words in term vector. 
//
#ifdef _DEBUG
void CDJTDocument::DumpTerms(CDumpContext& dc)
{
	CString strTmp;
	USHORT usIndex;
	CSingleLock tmpLock2( &m_TermFreqArrayLock, TRUE );
	CSingleLock tmpLock1( &TermIndexMap()->m_TIMLock, TRUE );

	dc << "\nTerms:\n";
	POSITION mapPos = TermIndexMap()->m_TheTermMap.GetStartPosition();
	while( mapPos != NULL )
	{
		TermIndexMap()->m_TheTermMap.GetNextAssoc( mapPos, strTmp, usIndex );
		if( m_TermFreqArray[usIndex] > 0 )
			dc << "\t" << m_TermFreqArray[usIndex]<< "\t" << strTmp << "\n";
	}
}
#endif


//////////////////////////////////////////////////////////////////////
// AddToTermFreqArray:
// 
double CDJTDocument::AddToTermFreqArray( double fVal, USHORT usIndex )
{
//	CSingleLock tmpLock( &m_TermFreqArrayLock, TRUE );

	ASSERT( usIndex >= 0 );
	ASSERT( !_isnan(fVal) && _finite(fVal) );

	m_nMaxIndex = max( m_nMaxIndex, usIndex );

	if( m_nMaxIndex > m_TermFreqArray.GetUpperBound() )
	{
		int nSize = m_nMaxIndex + DJ_DOCUMENT_TF_ARRAY_GROW;
		m_TermFreqArray.SetSize( nSize );
	}
	
	return m_TermFreqArray[usIndex] += fVal;
}


//////////////////////////////////////////////////////////////////////
// PrintStatistics:
//
#ifdef _DEBUG
void CDJTDocument::PrintStatistics()
{
	#ifdef _PrintStats

		// Html features.
		printf("\n");
		printf( "m_nTagsTotal\t %6d\n", m_nTagsTotal );
		printf( "m_nCommentsTotal %6d\n", m_nCommentsTotal );
		printf( "m_nHrefTags\t %6d\n", m_nHrefTags );
		printf( "m_nImgTags\t %6d\n", m_nImgTags );
		printf( "m_nCommImgs\t %6d\n", m_nCommImgs );
		printf( "m_nEndPTags\t %6d\n", m_nEndPTags );
		printf( "m_nRepetedTags\t %6d\n", m_nRepetedTags );
		printf( "m_nEndHtmlTags\t %6d\n", m_nEndHtmlTags );
		printf( "m_nMetaTags\t %6d\n", m_nMetaTags );
		printf( "m_nCharsInMeta\t %6d\n", m_nCharsInMeta );
		printf( "m_nActiveTags\t %6d\n", m_nActiveTags );
		printf( "m_nCharsInActive %6d\n", m_nCharsInActive );
		printf( "m_nFormTags\t %6d\n", m_nFormTags );
	
		// Word features.
		printf("\n");
		printf( "m_nCharsTotal\t %6d\n", m_nCharsTotal );
		printf( "m_nWordsTotal\t %6d\n", m_nWordsTotal );
		printf( "m_nTypesTotal\t %6d\n", m_nTypesTotal );
		printf( "m_nLongWords\t %6d\n", m_nLongWords );
		printf( "m_nDigits\t %6d\n", m_nDigits );
		printf( "m_nCapWords\t %6d\n", m_nCapWords );
		printf( "m_nCapTypes\t %6d\n", m_nCapTypes );
		printf( "m_nContractions\t %6d\n", m_nContractions );
		printf( "m_nSentences\t %6d\n", m_nSentences );
		printf("\n");
	
		// Mendenhall
		printf("\n");
		printf( "Mendenhall:\n" );
		int i;
		printf( "\t%5s%10s\n", "index", "count" );
		for( i=0; i<25; i++ )
			printf( "\t%5d%10d\n", i, m_Mendenhall[i] );
		printf("\n");
	
		// TypeMap
		printf( "Token types:\n" );
		CString strKey;
		int nVal;
		POSITION pos = m_pTokenMap->GetStartPosition();
		while( pos != NULL )
		{
			m_pTokenMap->GetNextAssoc( pos, strKey, nVal );
			printf( "\t%d\t%s\n", nVal ,strKey );
		}
		printf("\n");
	
		// CapTypeMap
		printf( "CapToken types:\n" );
		pos = m_pCapTokenMap->GetStartPosition();
		while( pos != NULL )
		{
			m_pCapTokenMap->GetNextAssoc( pos, strKey, nVal );
			printf( "\t%d\t%s\n", nVal ,strKey );
		}
	#endif //_PrintStats
}
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////
// ScoreAt:
//
double CDJTDocument::ScoreAt( LPCTSTR pszFeature )
{
	double fScore;
	ASSERT( pszFeature != NULL );
	VERIFY( m_pFeatureScoreMap->Lookup(pszFeature, fScore) );

	return fScore;
}

//////////////////////////////////////////////////////////////////////
// SetGenre:
// Index genres from 1 to 12, using 1 to 11 as actual genres, and 
// 12 as unclassified.
//
double CDJTDocument::SetGenre(int nGenre, double fValue)
{
	int nIndex = nGenre-1;
	ASSERT( nIndex < DJ_GENRE_PALETTE_SIZE && nIndex >= 0 );
	return m_GenreProbPtrs[nIndex]->m_fProbability = fValue;
}

//////////////////////////////////////////////////////////////////////
// InitGenreProbs:
//
void CDJTDocument::InitGenreProbs()
{
	m_GenreProbPtrs.SetSize( DJ_GENRE_PALETTE_SIZE );
	for( int i=0; i<DJ_GENRE_PALETTE_SIZE; i++)
		m_GenreProbPtrs[i] = new CDJGenreProb( &g_GenrePalette[i], 0 );
}

//////////////////////////////////////////////////////////////////////
// FreeStatsMem:
//
void CDJTDocument::FreeStatsMem()
{
	m_pTokenMap->RemoveAll();
	delete m_pTokenMap;
	m_pTokenMap = NULL;

	m_pCapTokenMap->RemoveAll();
	delete m_pCapTokenMap;
	m_pCapTokenMap = NULL;

	m_pUrlMap->RemoveAll();
	delete m_pUrlMap;
	m_pUrlMap = NULL;

	m_pFeatureScoreMap->RemoveAll();
	delete m_pFeatureScoreMap;
	m_pFeatureScoreMap = NULL;

	m_pFactorScores->RemoveAll();
	delete m_pFactorScores;
	m_pFactorScores = NULL;
}
