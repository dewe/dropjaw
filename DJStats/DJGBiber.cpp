// DJGBiber.cpp: implementation of the CDJGBiber class.
// Calculations for filling documents with feature scores.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afx.h>
#include <float.h>
#include <afxcoll.h>
#include "DJTDocument.h"
#include "DJGBiber.h"
#include "Chunkify.h"
#include "DJGenre.h"


// Definitions
//
const char *DJ_BIBER_FEATURE_NAMES[ DJ_BIBER_NUM_OF_FEATURES ] = { 
	"ADVSUB", "AMPLIFIERS", "BECAUSE", "CACT", "CMETA", "CMNTS", "CONCESSIVE", "CONDITION", 
	"CONTR", "CPW", "CTT", "DIGS", "DOWNTONERS", "EMPHATICS", "FORMS", "HREF", "HTML", 
	"INDEF", "IT", "IMG", "IMGCOM", "LW", "NECMOD", "PLACEADV", "POSSMOD", "PREDMOD", 
	"PRIVATEVERB", "PRON1", "PRON2", "PRON3", "PUBLICVERB", "PUBLWEB", "S/A", 
	"SUASIVEVERB", "SYNTHNEG", "TIMEADV", "TPW", "TT", "WDS", "WPS" };


// Template collection helper function.
//
template <> void AFXAPI DestructElements<DimensionList*>( DimensionList** ppDimLists, int nCount )
{
    for ( int i=0; i<nCount; i++, ppDimLists++ )
		delete *ppDimLists;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJGBiber::CDJGBiber()
{
	InitFactorTemplates();
	InitBiberFeatures();
	InitFeatureNormalsMap();
}


CDJGBiber::~CDJGBiber()
{
	for( int i=0; i<DJ_BIBER_NUM_OF_FACTORS; i++ )
		delete m_Factors[i];

	m_Factors.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ReadDimensionList:
// Seek for dimension name (in capitals) and read all lines found (until
// next section) into the list. If no section was found the function returns -1,
// else it returns the number of items put into the list. Use '#' for comments.
//
int CDJGBiber::ReadDimensionList( LPCTSTR pszFName, 
								  LPCTSTR pszDim, 
								  DimensionList *pList )
{
	CString strLine, strHeader = pszDim;
	BOOL bInSection = FALSE;
	int nReturn = -1;

	strHeader = "[" + strHeader + "]";
	
	try
	{
		CStdioFile dataFile( pszFName, CFile::modeRead |
			CFile::shareDenyWrite | CFile::typeText );

		while( dataFile.ReadString(strLine) )
		{
			strLine.TrimLeft();
			strLine.TrimRight();
			
			// Look for end of section.
			if( !strLine.IsEmpty() && strLine.Find("[") > -1 )
				bInSection = FALSE;
			
			// Look for my section.
			if( !bInSection )
			{
				if( !strLine.IsEmpty() && strLine == strHeader )
				{
					bInSection = TRUE;
					nReturn = 0;
				}
			}
			
			// We are in our section.
			else
			{
				// Avoid comments and empty lines.
				if( !strLine.IsEmpty() && !(strLine.Find("#") == 0) )
				{
					pList->AddTail( strLine );
					nReturn++;
				}
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

	return nReturn;
}


//////////////////////////////////////////////////////////////////////
// AddDimension:
// Add a dimension-list to the biber feature map. 
//
void CDJGBiber::AddDimension( BiberFeatureMap *pMap, LPCTSTR pszDimension )
{
	DimensionList *pDimList = new DimensionList;

	VERIFY( 0 < ReadDimensionList( g_strHomeDir + g_strBiberFeateursFileName, 
								   pszDimension, 
								   pDimList) );
	pMap->SetAt( pszDimension, pDimList );
}


//////////////////////////////////////////////////////////////////////
// InitBiberFeatures
//
void CDJGBiber::InitBiberFeatures( )
{
	m_BiberFeatureMap.InitHashTable(HashTableSize(28));	// 20% more than 23 dimensions.

	AddDimension( &m_BiberFeatureMap, "ADVSUB" );
	AddDimension( &m_BiberFeatureMap, "AMPLIFIERS" );
	AddDimension( &m_BiberFeatureMap, "BECAUSE" );
	AddDimension( &m_BiberFeatureMap, "CONCESSIVE" );
	AddDimension( &m_BiberFeatureMap, "CONDITION" );
	AddDimension( &m_BiberFeatureMap, "DOWNTONERS" );
	AddDimension( &m_BiberFeatureMap, "EMPHATICS" );
	AddDimension( &m_BiberFeatureMap, "INDEF" );
	AddDimension( &m_BiberFeatureMap, "IT" );
	AddDimension( &m_BiberFeatureMap, "NECMOD" );
	AddDimension( &m_BiberFeatureMap, "PLACEADV" );
	AddDimension( &m_BiberFeatureMap, "POSSMOD" );
	AddDimension( &m_BiberFeatureMap, "PREDMOD" );
	AddDimension( &m_BiberFeatureMap, "PRIVATEVERB" );
	AddDimension( &m_BiberFeatureMap, "PRON1" );
	AddDimension( &m_BiberFeatureMap, "PRON2" );
	AddDimension( &m_BiberFeatureMap, "PRON3" );
	AddDimension( &m_BiberFeatureMap, "PUBLICVERB" );
	AddDimension( &m_BiberFeatureMap, "S/A" );
	AddDimension( &m_BiberFeatureMap, "SUASIVEVERB" );
	AddDimension( &m_BiberFeatureMap, "SYNTHNEG" );
	AddDimension( &m_BiberFeatureMap, "TIMEADV" );
	AddDimension( &m_BiberFeatureMap, "PUBLWEB" );	// New dimension, not original Biber.
}


//////////////////////////////////////////////////////////////////////
// InitFactorTemplates:
// 
void CDJGBiber::InitFactorTemplates()
{
	m_Factors.SetSize( DJ_BIBER_NUM_OF_FACTORS );

	// Initialize new templates
	for( int i=0; i<DJ_BIBER_NUM_OF_FACTORS; i++ )
	{
		m_Factors[i] = new FactorsTemplateMap;
		m_Factors[i]->InitHashTable( HashTableSize(15) );
	}

	// Factor template 1
	m_Factors[0]->SetAt( "PRIVATEVERB", 1 );
	m_Factors[0]->SetAt( "CONTR", 1 );
	m_Factors[0]->SetAt( "PRON2", 1 );
	m_Factors[0]->SetAt( "EMPHATICS", 1 );
	m_Factors[0]->SetAt( "PRON1", 1 );
	m_Factors[0]->SetAt( "IT", 1 );
	m_Factors[0]->SetAt( "BECAUSE", 1 );
	m_Factors[0]->SetAt( "INDEF", 1 );
	m_Factors[0]->SetAt( "AMPLIFIERS", 1 );
	m_Factors[0]->SetAt( "POSSMOD", 1 );
	m_Factors[0]->SetAt( "CPW", -1 );
	m_Factors[0]->SetAt( "TT", -1 );

	// Factor template 2
	m_Factors[1]->SetAt( "PRON3", 1 );
	m_Factors[1]->SetAt( "PUBLICVERB", 1 );
	m_Factors[1]->SetAt( "SYNTHNEG", 1 );

	// Factor template 3
	m_Factors[2]->SetAt( "TIMEADV", -1 );
	m_Factors[2]->SetAt( "PLACEADV", -1 );

	// Factor template 4
	m_Factors[3]->SetAt( "PREDMOD", 1 );
	m_Factors[3]->SetAt( "SUASIVEVERB", 1 );

	// Factor template 5
	m_Factors[4]->SetAt( "ADVSUB", 1 );

	// Factor template 6
	m_Factors[5]->SetAt( "S/A", 1 );
}


//////////////////////////////////////////////////////////////////////
// InitFeatureNormalsMap:
// 
void CDJGBiber::InitFeatureNormalsMap()
{
	m_FeatureNormalsMap.InitHashTable(HashTableSize(DJ_BIBER_NUM_OF_FEATURES));

	ReadNormalValues( g_strHomeDir + g_strBiberNormalsFileName, 
					  &m_FeatureNormalsMap );
}


//////////////////////////////////////////////////////////////////////
// ReadNormalValues:
// Read normal values into the FeatureNormals map. The file must
// be using the following line format, one dimension per line:
// <Dimension name> <mean value> <standard deviation value>
// Comments must pe preceded with a #. The function returns the number
// of read lines.
// 
int CDJGBiber::ReadNormalValues( LPCTSTR pszFName, FeatureNormalsMap *pMap )
{
	CString strLine;
	FeatureNormals FNorm;
	char szBuf[255];
	double fMean = 0, fStdev = 0;
	int nRead = 0;

	try
	{
		CStdioFile dataFile( pszFName, CFile::modeRead |
			CFile::shareDenyWrite | CFile::typeText );

		while( dataFile.ReadString(strLine) )
		{
			if( !strLine.IsEmpty() && !(strLine.Find("#") == 0) )
			{
				VERIFY( sscanf( (LPCTSTR)strLine, 
								"%s %lf %lf", 
								szBuf, &fMean, &fStdev) == 3 );
				FNorm.mean = fMean;
				FNorm.stdev = fStdev;
				pMap->SetAt( szBuf, FNorm );
				nRead++;
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

	return nRead;
}


//////////////////////////////////////////////////////////////////////
// Other Biber feaures
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// TT: Type-token-ratio.
//
double CDJGBiber::TT( CDJTDocument *pDoc )
{
	if( pDoc->m_nTypesTotal == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nWordsTotal > 0 );
		double score = (double) pDoc->m_nTypesTotal / pDoc->m_nWordsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nTypesTotal / pDoc->m_nWordsTotal;
}


//////////////////////////////////////////////////////////////////////
// CTT: Cap-type-token-ratio.
//
double CDJGBiber::CTT( CDJTDocument *pDoc )
{
	if( pDoc->m_nCapTypes == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nCapWords > 0 );
		double score = (double) pDoc->m_nCapTypes / pDoc->m_nCapWords;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nCapTypes / pDoc->m_nCapWords;
}


//////////////////////////////////////////////////////////////////////
// LW: Long-word-ratio.
//
double CDJGBiber::LW( CDJTDocument *pDoc )
{
	if( pDoc->m_nLongWords == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT(  pDoc->m_nWordsTotal > 0 );
		double score = (double) pDoc->m_nLongWords / pDoc->m_nWordsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nLongWords / pDoc->m_nWordsTotal;
}


//////////////////////////////////////////////////////////////////////
// CONTR: Contractions per word.
//
double CDJGBiber::CONTR( CDJTDocument *pDoc )
{
	if( pDoc->m_nContractions == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nWordsTotal > 0 );
		double score = (double) pDoc->m_nContractions / pDoc->m_nWordsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif

	return (double) pDoc->m_nContractions / pDoc->m_nWordsTotal;
}


//////////////////////////////////////////////////////////////////////
// CPW: Characters per word.
//
double CDJGBiber::CPW( CDJTDocument *pDoc )
{
	if( pDoc->m_nCharsTotal == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nWordsTotal > 0 );
		double score = (double) pDoc->m_nCharsTotal / pDoc->m_nWordsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif

	return (double) pDoc->m_nCharsTotal / pDoc->m_nWordsTotal;
}


//////////////////////////////////////////////////////////////////////
// DIGS: Numbers ratio.
//
double CDJGBiber::DIGS( CDJTDocument *pDoc )
{
	if( pDoc->m_nDigits == 0 ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nWordsTotal > 0 );
		double score = (double) pDoc->m_nDigits / pDoc->m_nWordsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nDigits / pDoc->m_nWordsTotal;
}


//////////////////////////////////////////////////////////////////////
// WPS: Words per sentence.
//
double CDJGBiber::WPS( CDJTDocument *pDoc )
{
	if( pDoc->m_nWordsTotal == 0 ) return 0;
	if( pDoc->m_nSentences == 0 ) return pDoc->m_nWordsTotal;
	
	#ifdef _DEBUG
		ASSERT( pDoc->m_nSentences > 0 );
		double score = (double) pDoc->m_nWordsTotal / pDoc->m_nSentences;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif
	
	return (double) pDoc->m_nWordsTotal / pDoc->m_nSentences;
}



//////////////////////////////////////////////////////////////////////
// HTML features
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// HREF: Hyperlinks per code-word, i.e. words+tags. (Href-ratio).
//
double CDJGBiber::HREF( CDJTDocument *pDoc )
{
	if( pDoc->m_nHrefTags == 0 ) return 0;

	// If the proportions are wrong, don't count... (trap for dirty HTML code)
	if( pDoc->m_nHrefTags > pDoc->m_nTagsTotal ) return 0;

	int nBase = pDoc->m_nTagsTotal + pDoc->m_nWordsTotal;
	#ifdef _DEBUG
		ASSERT( nBase > 0 );
		double score = (double) pDoc->m_nHrefTags / nBase;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nHrefTags / nBase;
}


//////////////////////////////////////////////////////////////////////
// IMG: Images per tag (Img-ratio).
//
double CDJGBiber::IMG( CDJTDocument *pDoc )
{
	if( pDoc->m_nImgTags == 0 ) return 0;

	// If the proportions are wrong, don't count... (trap for dirty HTML code)
	if( pDoc->m_nImgTags > pDoc->m_nTagsTotal ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nTagsTotal > 0 );
		double score = (double) pDoc->m_nImgTags / pDoc->m_nTagsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nImgTags / pDoc->m_nTagsTotal;
}


//////////////////////////////////////////////////////////////////////
// CMNTS: Comments per tag (Comment-ratio).
//
double CDJGBiber::CMNTS( CDJTDocument *pDoc )
{
	if( pDoc->m_nCommentsTotal == 0 ) return 0;
	if( pDoc->m_nTagsTotal == 0 ) return 0;

	#ifdef _DEBUG
		double score = (double) pDoc->m_nCommentsTotal / pDoc->m_nTagsTotal;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif

	return (double) pDoc->m_nCommentsTotal / pDoc->m_nTagsTotal;
}


//////////////////////////////////////////////////////////////////////
// CMETA: Characters per meta-tag.
//
double CDJGBiber::CMETA( CDJTDocument *pDoc )
{
	if( pDoc->m_nCharsInMeta == 0 ) return 0;

	// If the proportions are wrong, don't count... (trap for dirty HTML code)
	if( pDoc->m_nMetaTags > pDoc->m_nCharsInMeta ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nMetaTags > 0 );
		double score = (double) pDoc->m_nCharsInMeta / pDoc->m_nMetaTags;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif

	return (double) pDoc->m_nCharsInMeta / pDoc->m_nMetaTags;
}


//////////////////////////////////////////////////////////////////////
// TPW: Tags per code-word, i.e. words+tags. (Tag-ratio).
//
double CDJGBiber::TPW( CDJTDocument *pDoc )
{
	if( pDoc->m_nTagsTotal == 0 ) return 0;

	int nBase = pDoc->m_nTagsTotal + pDoc->m_nWordsTotal;
	#ifdef _DEBUG
		ASSERT( nBase > 0 );
		double score = (double) pDoc->m_nTagsTotal / nBase;
		ASSERT( !_isnan(score) && _finite(score) && score>0 && score<=1 );
	#endif

	return (double) pDoc->m_nTagsTotal / nBase;
}


//////////////////////////////////////////////////////////////////////
// HTML: Level of machine produced html-code.
//
double CDJGBiber::HTML( CDJTDocument *pDoc )
{
	double score;

	if( pDoc->m_nTagsTotal == 0 ) return 0;

	if( pDoc->m_nEndHtmlTags != 1 ) 
		return 0;	// No </HTML>, no score at all.
	else
		score = 1;	// Else, start final score at 1...

	score +=  (double) pDoc->m_nEndPTags / pDoc->m_nTagsTotal;
	score += (double) pDoc->m_nRepetedTags / pDoc->m_nTagsTotal;
	ASSERT( !_isnan(score) && _finite(score) && score>0 );
	
	return score;
}


//////////////////////////////////////////////////////////////////////
// CACT: Characters per active tag. (embed, script, applet)
//
double CDJGBiber::CACT( CDJTDocument *pDoc )
{
	if( pDoc->m_nCharsInActive == 0 ) return 0;

	// If the proportions are wrong, don't count... (trap for dirty HTML code)
	if( pDoc->m_nActiveTags > pDoc->m_nCharsInActive ) return 0;

	#ifdef _DEBUG
		ASSERT( pDoc->m_nActiveTags > 0 );
		double score = (double) pDoc->m_nCharsInActive / pDoc->m_nActiveTags;
		ASSERT( !_isnan(score) && _finite(score) && score>0 );
	#endif

	return (double) pDoc->m_nCharsInActive / pDoc->m_nActiveTags;
}



//////////////////////////////////////////////////////////////////////
// Calculating scores
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CalcDimListScore:
// Accumulate a score based on the occurrances of the words in pDimList.
//
double CDJGBiber::CalcDimListScore( CDJTDocument *pDoc, 
								    DimensionList *pDimList )
{
	POSITION listPos;
	double fAccScore = 0;
	int nDimScore; 
	CString strToken;
	
	listPos = pDimList->GetHeadPosition();
	while( listPos != NULL )
	{
		strToken = pDimList->GetNext( listPos );
		if( pDoc->m_pTokenMap->Lookup(strToken, nDimScore) )
			fAccScore += nDimScore;
	}
	
	return fAccScore;
}


//////////////////////////////////////////////////////////////////////
// CalcBiberFeatureMap:
// Process all dimension lists in the Biber feature map. Add
// scores to the documents score map.
//
void CDJGBiber::CalcBiberFeatureMap( CDJTDocument *pDoc, BiberFeatureMap *pMap )
{
	POSITION mapPos;
	CString strDimension;
	DimensionList *pDimList;
	double fDimScore;

	mapPos = pMap->GetStartPosition();
	while( mapPos != NULL )
	{
		pMap->GetNextAssoc( mapPos, strDimension, pDimList );
		fDimScore = CalcDimListScore( pDoc, pDimList );
		pDoc->m_pFeatureScoreMap->SetAt( strDimension, fDimScore );
		::Sleep(0);	// Friendly thread.
	}
}


//////////////////////////////////////////////////////////////////////
// CalculateScores:
// Add scores to the documents score map. Call only once per document.
//
void CDJGBiber::CalculateScores( CDJTDocument *pDoc )
{
	// Set the size of the documents score map hash table.
	pDoc->m_pFeatureScoreMap->InitHashTable( 
			HashTableSize(DJ_BIBER_NUM_OF_FEATURES) );

	// Calculate Biber dimensions. (Dimension lists).
	CalcBiberFeatureMap( pDoc, &m_BiberFeatureMap );
	NormalizeBiberScores( pDoc );


	// Simple measures (already normalized).
	pDoc->m_pFeatureScoreMap->SetAt( "CPW", CPW(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "CTT", CTT(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "CONTR", CONTR(pDoc) ); 
	pDoc->m_pFeatureScoreMap->SetAt( "DIGS", DIGS(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "LW", LW(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "TT", TT(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "WDS", pDoc->m_nWordsTotal );
	pDoc->m_pFeatureScoreMap->SetAt( "WPS", WPS(pDoc) );

	// HTML ratios.
	pDoc->m_pFeatureScoreMap->SetAt( "CACT", CACT(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "CMETA", CMETA(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "CMNTS", CMNTS(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "HREF", HREF(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "HTML", HTML(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "IMG", IMG(pDoc) );
	pDoc->m_pFeatureScoreMap->SetAt( "TPW", TPW(pDoc) );

	// HTML absolute values.
	pDoc->m_pFeatureScoreMap->SetAt( "IMGCOM", pDoc->m_nCommImgs );
	pDoc->m_pFeatureScoreMap->SetAt( "FORMS", pDoc->m_nFormTags );
	
	::Sleep(0);	// Friendly thread.
}


//////////////////////////////////////////////////////////////////////
// NormalizeBiberScores:
// Normalize scores using the total count of words and tags 
// as length factor.
//
void CDJGBiber::NormalizeBiberScores(CDJTDocument *pDoc)
{
	POSITION mapPos;
	CString strDim;
	double fScore;

	ASSERT( pDoc->m_nWordsTotal >= 0 );
	
	// If there is no words, there is no biber scores...
	if( pDoc->m_nWordsTotal == 0 )
		return;

	// Normalize length.
	mapPos = pDoc->m_pFeatureScoreMap->GetStartPosition();
	while( mapPos != NULL )
	{
		pDoc->m_pFeatureScoreMap->GetNextAssoc( mapPos, strDim, fScore );
		(*pDoc->m_pFeatureScoreMap)[strDim] = 1000 * (fScore  / pDoc->m_nWordsTotal);
	}
}


//////////////////////////////////////////////////////////////////////
// Equalize:
// Transform the score map to normalized deviations, using 
// normal values (i.e mean value and standard deviation).
//
void CDJGBiber::EqualizeScores(CDJTDocument *pDoc)
{
	POSITION mapPos;
	CString strDim;
	FeatureNormals FNorm;
	double fScore;

	// Normalize values in regords to the distribution.
	mapPos = pDoc->m_pFeatureScoreMap->GetStartPosition();
	while( mapPos != NULL )
	{
		pDoc->m_pFeatureScoreMap->GetNextAssoc( mapPos, strDim, fScore );
		VERIFY( m_FeatureNormalsMap.Lookup(strDim, FNorm) );
		ASSERT( FNorm.stdev > 0 );
		ASSERT( FNorm.mean >= 0 );
		ASSERT( !_isnan((fScore-FNorm.mean)/FNorm.stdev) && _finite((fScore-FNorm.mean)/FNorm.stdev) );
		(*pDoc->m_pFeatureScoreMap)[strDim] = (fScore - FNorm.mean) / FNorm.stdev;
	}
}


//////////////////////////////////////////////////////////////////////
// CalcFactorScores:
// For each FactorTemplate, calculate the resulting score, and set
// the documents FactorScores array.
//
void CDJGBiber::CalcFactorScores(CDJTDocument *pDoc)
{

	// Not in use. CalculateGenreScores, uses if/else rules instead
	// of factor scores.

/*	POSITION mapPos;
	CString strFeature;
	double fWeight, fScore;

	for( int i=0; i<DJ_BIBER_NUM_OF_FACTORS; i++ )
	{
		mapPos = m_Factors[i]->GetStartPosition();
		while( mapPos != NULL )
		{
			m_Factors[i]->GetNextAssoc( mapPos, strFeature, fWeight );
			VERIFY( pDoc->m_pFeatureScoreMap->Lookup(strFeature, fScore) );

			(*pDoc->m_pFactorScores)[i] = fScore * fWeight;
		}

		::Sleep(0);	// Friendly thread.
	}
*/

}


//////////////////////////////////////////////////////////////////////
// CalculateGenreScores:
// Assign scores to the documents GenreProb array, based on the 
// documents factor scores.
//
void CDJGBiber::CalculateGenreScores(CDJTDocument *pDoc)
{
	// Text categories
	if( ProbGenreText(pDoc) > 0 )
	{
		double fRes = 0;
		fRes += pDoc->SetGenre(  4, ProbGenre04(pDoc) );
		fRes += pDoc->SetGenre( 10, ProbGenre10(pDoc) );
		fRes += pDoc->SetGenre(  5, ProbGenre05(pDoc) );
		fRes += pDoc->SetGenre(  6, ProbGenre06(pDoc) );

		// Default genre (06).
		if( fRes == 0 )
			pDoc->SetGenre( 6, 1.0 );
	}

	// Web categories
	else
	{
		double fRes = 0;
		fRes += pDoc->SetGenre( 11, ProbGenre11(pDoc) );
		fRes += pDoc->SetGenre(  8, ProbGenre08(pDoc) );
		fRes += pDoc->SetGenre(  1, ProbGenre01(pDoc) );
		fRes += pDoc->SetGenre(  3, ProbGenre03(pDoc) );
		fRes += pDoc->SetGenre(  9, ProbGenre09(pDoc) );
		fRes += pDoc->SetGenre(  2, ProbGenre02(pDoc) );

		// Default genre (09).
		if( fRes == 0 )
			pDoc->SetGenre( 9, 1.0 );
	}
}



////////////////////////////////////////////////////////////////////////
// Rules
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// ProbGenreText:
//
double CDJGBiber::ProbGenreText(CDJTDocument * pDoc)
{
	// Rule 136:class txt  [94.2%]
	if(
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "CTT" ) <= 0			&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "SUASIVEVERB" ) <= 0 &&
		pDoc->ScoreAt( "SYNTHNEG" ) > 0
		)
		return 0.942;

	// Rule 145:class txt  [93.0%]
	if(
    	pDoc->ScoreAt( "SYNTHNEG" ) > 0		&&
    	pDoc->ScoreAt( "WDS" ) > 0
		)
		return 0.930;


	// Rule 87:class txt  [90.0%]
	if(
    	pDoc->ScoreAt( "BECAUSE" ) > 0		&&
    	pDoc->ScoreAt( "CPW" ) > 0			&&
    	pDoc->ScoreAt( "PUBLWEB" ) <= 0		&&
    	pDoc->ScoreAt( "TT" ) <= 0
		)
		return 0.900;

	// Rule 50:class txt  [89.5%]
	if(
    	pDoc->ScoreAt( "AMPLIFIERS" ) > 0	&&
    	pDoc->ScoreAt( "CMNTS" ) <= 0		&&
    	pDoc->ScoreAt( "CONDITION" ) <= 0	&&
    	pDoc->ScoreAt( "POSSMOD" ) > 0		&&
    	pDoc->ScoreAt( "PRON2" ) <= 0		&&
    	pDoc->ScoreAt( "PUBLWEB" ) <= 0
		)
		return 0.895;

	// Rule 160:class txt  [88.4%]
	if(
    	pDoc->ScoreAt( "BECAUSE" ) > 0		&&
    	pDoc->ScoreAt( "FORMS" ) <= 0		&&
    	pDoc->ScoreAt( "NECMOD" ) > 0		
		)
		return 0.884;

	// Rule 47:class txt  [87.8%]

	if(
    	pDoc->ScoreAt( "AMPLIFIERS" ) > 0	&&
    	pDoc->ScoreAt( "CMNTS" ) <= 0		&&
    	pDoc->ScoreAt( "IT" ) > 0			&&
    	pDoc->ScoreAt( "PRON2" ) <= 0		&&
    	pDoc->ScoreAt( "PUBLWEB" ) <= 0
		)
		return 0.878;

	// Rule 73:class txt  [87.4%]
	if(
    	pDoc->ScoreAt( "DIGS" ) <= 0		&&
    	pDoc->ScoreAt( "FORMS" ) <= 0		&&
    	pDoc->ScoreAt( "NECMOD" ) > 0		&&
    	pDoc->ScoreAt( "PUBLICVERB" ) > 0
		)
		return 0.874;

	// Rule 105:class txt  [87.1%]

	if(
    	pDoc->ScoreAt( "CONCESSIVE" ) > 0	&&
    	pDoc->ScoreAt( "FORMS" ) <= 0		&&
    	pDoc->ScoreAt( "TPW" ) <= 0
		)
		return 0.871;

	// Rule 175:class txt  [86.6%]
	if(
    	pDoc->ScoreAt( "BECAUSE" ) > 0		&&
    	pDoc->ScoreAt( "CACT" ) <= 0		&&
    	pDoc->ScoreAt( "PRON3" ) > 0
		)
		return 0.866;

	// Rule 3:class txt  [86.0%]
	if(
    	pDoc->ScoreAt( "CONTR" ) <= 0		&&
    	pDoc->ScoreAt( "DOWNTONERS" ) <= 0	&&
    	pDoc->ScoreAt( "HREF" ) <= 0		&&
    	pDoc->ScoreAt( "HTML" ) > 0			&&
    	pDoc->ScoreAt( "INDEF" ) <= 0		&&
    	pDoc->ScoreAt( "IMG" ) <= 0			&&
    	pDoc->ScoreAt( "LW" ) > 0			&&
    	pDoc->ScoreAt( "PLACEADV" ) <= 0	&&
    	pDoc->ScoreAt( "PUBLICVERB" ) <= 0	&&
    	pDoc->ScoreAt( "S/A" ) <= 0			&&
    	pDoc->ScoreAt( "SUASIVEVERB" ) <= 0	&&
    	pDoc->ScoreAt( "TPW" ) <= 0			&&
    	pDoc->ScoreAt( "WDS" ) <= 0
		)
		return 0.860;

	// Default, not class txt.
	return 0;
}


////////////////////////////////////////////////////////////////////////
// ProbGenreXX:
//
double CDJGBiber::ProbGenre01(CDJTDocument * pDoc)
{
	// Rule 184:class 01  [59.4%]
    if(
		pDoc->ScoreAt( "BECAUSE" ) > 0		&&
		pDoc->ScoreAt( "DIGS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) > 0		&&
		pDoc->ScoreAt( "LW" ) <= 0			&&
		pDoc->ScoreAt( "WDS" ) <= 0
		)
		return 0.594;
	
	// Rule 90:class 01  [44.2%]
	if(
		pDoc->ScoreAt( "CONCESSIVE" ) <= 0	&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "PRON3" ) > 0		&&
		pDoc->ScoreAt( "WDS" ) <= 0			&&
		pDoc->ScoreAt( "WPS" ) <= 0			
		)
		return 0.442;
	
	// Default
	return 0;
}

double CDJGBiber::ProbGenre02(CDJTDocument * pDoc)
{
	// Rule 72:class 02  [50.5%]
	if(
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "CONCESSIVE" ) <= 0	&&
		pDoc->ScoreAt( "DIGS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) <= 0		&&
		pDoc->ScoreAt( "PREDMOD" ) > 0		&&
		pDoc->ScoreAt( "PRON3" ) <= 0		&&
		pDoc->ScoreAt( "PUBLICVERB" ) <= 0	&&
		pDoc->ScoreAt( "S/A" ) <= 0			&&
		pDoc->ScoreAt( "WDS" ) <= 0
		)
		return 0.505;
	
	// Rule 122:class 02  [43.4%]
	if(
		pDoc->ScoreAt( "BECAUSE" ) <= 0		&&
		pDoc->ScoreAt( "CMETA" ) > 0		&&
		pDoc->ScoreAt( "CONCESSIVE" ) <= 0	&&
		pDoc->ScoreAt( "DIGS" ) <= 0		&&
		pDoc->ScoreAt( "NECMOD" ) <= 0		&&
		pDoc->ScoreAt( "PRON3" ) <= 0		&&
		pDoc->ScoreAt( "PUBLICVERB" ) <= 0	&&
		pDoc->ScoreAt( "SYNTHNEG" ) <= 0	&&
		pDoc->ScoreAt( "WDS" ) <= 0
		)
		return 0.434;
	
	// Rule 204:class 02  [37.1%]
	if(
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "PRON3" ) <= 0		&&
		pDoc->ScoreAt( "PUBLICVERB" ) <= 0	&&
		pDoc->ScoreAt( "SUASIVEVERB" ) > 0
		)
		return 0.371;

	// Default
	return 0;	
}

double CDJGBiber::ProbGenre03(CDJTDocument * pDoc)
{
	// Rule 208:class 03  [52.7%]
	if(
   		pDoc->ScoreAt( "FORMS" ) > 0		&&
    	pDoc->ScoreAt( "SYNTHNEG" ) <= 0
		)
		return 0.527;

	// Default
	return 0;
}

double CDJGBiber::ProbGenre04(CDJTDocument * pDoc)
{
	// Rule 87:class 04  [79.4%]
	if(
    	pDoc->ScoreAt( "CPW" ) > 0			&&
    	pDoc->ScoreAt( "PRON2" ) > 0		&&
    	pDoc->ScoreAt( "PUBLWEB" ) <= 0
		)
		return 0.794;

	// Rule 56:class 04  [75.9%]
	if(
    	pDoc->ScoreAt( "CONTR" ) > 0		&&
    	pDoc->ScoreAt( "IMG" ) > 0			&&
    	pDoc->ScoreAt( "PRON2" ) <= 0		&&
    	pDoc->ScoreAt( "TT" ) <= 0
		)
		return 0.759;

	// Rule 98:class 04  [75.8%]
	if(
    	pDoc->ScoreAt( "HREF" ) <= 0		&&
    	pDoc->ScoreAt( "TT" ) > 0			&&
    	pDoc->ScoreAt( "WPS" ) > 0
		)
		return 0.758;

	// Rule 122:class 04  [70.7%]
	if(
    	pDoc->ScoreAt( "FORMS" ) > 0		&&
    	pDoc->ScoreAt( "HREF" ) > 0
		)
		return 0.707;

	// Default
	return 0;
}

double CDJGBiber::ProbGenre05(CDJTDocument * pDoc)
{
	// Rule 24:class 05  [93.0%]
   	if(
		pDoc->ScoreAt( "CONTR" ) <= 0		&&
		pDoc->ScoreAt( "PLACEADV" ) <= 0	&&
		pDoc->ScoreAt( "PRON1" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "PUBLWEB" ) <= 0		&&
		pDoc->ScoreAt( "WDS" ) > 0
		)
		return 0.930;
	
	// Rule 36:class 05  [91.7%]
   	if(
		pDoc->ScoreAt( "CONTR" ) <= 0		&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) <= 0		&&
		pDoc->ScoreAt( "PRON1" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "SYNTHNEG" ) > 0
		)
		return 0.917;
	
	// Rule 66:class 05  [85.9%]
   	if(
		pDoc->ScoreAt( "ADVSUB" ) <= 0		&&
		pDoc->ScoreAt( "IMGCOM" ) <= 0		&&
		pDoc->ScoreAt( "PRON1" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "S/A" ) > 0			&&
		pDoc->ScoreAt( "SUASIVEVERB" ) <= 0
		)
		return 0.859;
	
	// Rule 28:class 05  [83.2%]
   	if(
		pDoc->ScoreAt( "CPW" ) > 0			&&
		pDoc->ScoreAt( "CTT" ) <= 0			&&
		pDoc->ScoreAt( "DIGS" ) <= 0		&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "IMG" ) <= 0			&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "PUBLWEB" ) <= 0		&&
		pDoc->ScoreAt( "SUASIVEVERB" ) <= 0	&&
		pDoc->ScoreAt( "TPW" ) <= 0			&&
		pDoc->ScoreAt( "TT" ) <= 0
		)
		return 0.832;
	
	// Rule 74:class 05  [78.6%]
   	if(
		pDoc->ScoreAt( "CTT" ) <= 0			&&
		pDoc->ScoreAt( "PRON1" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "SUASIVEVERB" ) > 0
		)
		return 0.786;
	
	// Rule 91:class 05  [67.8%]
   	if(
		pDoc->ScoreAt( "ADVSUB" ) <= 0		&&
		pDoc->ScoreAt( "NECMOD" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "PRON3" ) <= 0		&&
		pDoc->ScoreAt( "PUBLWEB" ) <= 0		&&
		pDoc->ScoreAt( "SUASIVEVERB" ) <= 0	&&
		pDoc->ScoreAt( "TPW" ) <= 0
		)
		return 0.678;
	
	// Default
	return 0;
}

double CDJGBiber::ProbGenre06(CDJTDocument * pDoc)
{
	
	// Rule 48:class 06  [89.1%]
   	if(
        pDoc->ScoreAt( "AMPLIFIERS" ) > -1	&&
        pDoc->ScoreAt( "CONTR" ) <= -1		&&
        pDoc->ScoreAt( "IMG" ) > -1			&&
        pDoc->ScoreAt( "PRON1" ) > -1
        )
		return 0.891;
	
	// Rule 64:class 06  [80.9%]
	if(
		pDoc->ScoreAt( "DIGS" ) <= -1		&&
        pDoc->ScoreAt( "DOWNTONERS" ) > -1	&&
        pDoc->ScoreAt( "PREDMOD" ) <= -1	&&
        pDoc->ScoreAt( "PRON1" ) <= -1		&&
        pDoc->ScoreAt( "PRON2" ) <= -1		&&
        pDoc->ScoreAt( "S/A" ) <= -1		&&
        pDoc->ScoreAt( "SUASIVEVERB" )<= -1	&&
        pDoc->ScoreAt( "SYNTHNEG" ) <= -1	&&
        pDoc->ScoreAt( "TT" ) <= -1
        )
		return 0.809;
	
	// Rule 83:class 06  [63.9%]
   	if(
        pDoc->ScoreAt( "CPW" ) <= -1		&&
        pDoc->ScoreAt( "FORMS" ) <= -1		&&
        pDoc->ScoreAt( "PRON2" ) > -1
        )
		return 0.639;
	
	// Rule 101:class 06  [55.8%]
   	if(
        pDoc->ScoreAt( "BECAUSE" ) <= -1	&&
        pDoc->ScoreAt( "IMGCOM" ) <= -1		&&
        pDoc->ScoreAt( "TPW" ) <= -1		&&
        pDoc->ScoreAt( "TT" ) > -1
        )
		return 0.558;
	
	// Default
	return 0;
}

double CDJGBiber::ProbGenre07(CDJTDocument * pDoc)
{

	// No rules for genre 07.

	// Default
	return 0;
}

double CDJGBiber::ProbGenre08(CDJTDocument * pDoc)
{
	// Rule 209:class 08  [62.1%]
   	if(
		pDoc->ScoreAt( "WDS" ) > 0
		)
		return 0.621;
	
	// Rule 128:class 08  [48.6%]
	if(
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "DIGS" ) > 0			&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) <= 0		&&
		pDoc->ScoreAt( "PLACEADV" ) > 0		&&
		pDoc->ScoreAt( "S/A" ) <= 0			&&
		pDoc->ScoreAt( "WPS" ) <= 0
		)
		return 0.486;
	
	// Rule 129:class 08  [33.6%]
	if(
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "DIGS" ) > 0			&&
		pDoc->ScoreAt( "IMGCOM" ) <= 0		&&
		pDoc->ScoreAt( "S/A" ) <= 0			&&
		pDoc->ScoreAt( "WPS" ) > 0
		)
		return 0.336;
	
	// Default
	return 0;
}

double CDJGBiber::ProbGenre09(CDJTDocument * pDoc)
{
	// Rule 7:class 09  [68.2%]
	if(
		pDoc->ScoreAt( "ADVSUB" ) <= 0		&&
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "DOWNTONERS" ) <= 0	&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) <= 0		&&
		pDoc->ScoreAt( "IMG" ) <= 0			&&
		pDoc->ScoreAt( "PRON2" ) <= 0		&&
		pDoc->ScoreAt( "PUBLICVERB" ) <= 0	&&
		pDoc->ScoreAt( "PUBLWEB" ) > 0
		)
		return 0.682;
	
/*	Do not use 195
	// Rule 195:class 09  [53.2%]
	if(
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "NECMOD" ) <= 0		&&
		pDoc->ScoreAt( "S/A" ) <= 0			&&
		pDoc->ScoreAt( "SUASIVEVERB" ) <= 0	&&
		pDoc->ScoreAt( "WPS" ) > 0
		)
		return 0.532;
*/

	// Default
	return 0;
}

double CDJGBiber::ProbGenre10(CDJTDocument * pDoc)
{
	// Rule 59:class 10  [47.7%]
	if(
		pDoc->ScoreAt( "CONTR" ) > 0		&&
		pDoc->ScoreAt( "DIGS" ) > 0			&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "PRON2" ) <= 0
		)
		return 0.477;
	
	// Default
	return 0;
}

double CDJGBiber::ProbGenre11(CDJTDocument * pDoc)
{
	// Rule 3:class 11  [90.5%]
	if(
		pDoc->ScoreAt( "CACT" ) <= 0		&&
		pDoc->ScoreAt( "EMPHATICS" ) <= 0	&&
		pDoc->ScoreAt( "FORMS" ) <= 0		&&
		pDoc->ScoreAt( "INDEF" ) <= 0		&&
		pDoc->ScoreAt( "IMG" ) <= 0			&&
		pDoc->ScoreAt( "NECMOD" ) <= 0		&&
		pDoc->ScoreAt( "PLACEADV" ) <= 0	&&
		pDoc->ScoreAt( "POSSMOD" ) <= 0		&&
		pDoc->ScoreAt( "PRIVATEVERB" ) <= 0	&&
		pDoc->ScoreAt( "PRON1" ) <= 0		&&
		pDoc->ScoreAt( "PUBLWEB" ) <= 0		&&
		pDoc->ScoreAt( "S/A" ) <= 0			&&
		pDoc->ScoreAt( "TIMEADV" ) <= 0		&&
		pDoc->ScoreAt( "TT" ) > 0			&&
		pDoc->ScoreAt( "WPS" ) <= 0
		)
		return 0.905;
	
	// Default
	return 0;
}


