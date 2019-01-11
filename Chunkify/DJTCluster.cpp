// DJTCluster.cpp: implementation of the CDJTCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <limits>
#include "Chunkify.h"
#include "DJTCluster.h"
#include "DJTextBufferHelpers.h"
#include "DJTTermIndexmap.h"

// include files for dummy functions
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJTCluster, CDJClusterNode );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJTCluster::CDJTCluster( CDJTTermIndexMap *pTIM ) 
	: m_nGroups( DJ_GENRE_PALETTE_SIZE )
{
	ASSERT( m_nGroups > 0 );
	
	m_pTermIndexMap = pTIM;

		// For efficiency, set the size of the collections before start.
	m_GroupPtrs.SetSize( m_nGroups );

	// Create new set of groups.
	for( int i=0; i<m_nGroups; i++ )
		VERIFY( m_GroupPtrs[i] = new CDJTGroup(&g_GenrePalette[i]) );
}

CDJTCluster::~CDJTCluster()
{
	// Clean up, delete the groups.
	for( int i=0; i<m_nGroups; i++ )
		delete m_GroupPtrs[i];

	m_GroupPtrs.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AddDocument:
// Dispatch document to appropriate group.
// Return pointer to the group added to. If failed, return NULL.
//
CDJTGroup* CDJTCluster::AddDocument(CDJTDocument* pDoc)
{
	GenrePtrList genrePtrList;
	const DJGenre* pGenre;
	CDJTGroup* pGroup;
	BOOL bReturnVal = TRUE;
	POSITION pos;

	ASSERT( pDoc != NULL );


	// Determine what group to add document to.
	GenrePolicy( &genrePtrList, pDoc );
	ASSERT( !genrePtrList.IsEmpty() );

	// Add document to appropriate groups.
	pos = genrePtrList.GetHeadPosition();
	while( pos != NULL )
	{
		pGenre = genrePtrList.GetNext( pos );
		ASSERT( pGenre->nIndex < m_nGroups );
		pGroup = m_GroupPtrs[ pGenre->nIndex ];
		if( !pGroup->AddDocument(pDoc) )
			bReturnVal = NULL;
	}

	AddToCentroid( pDoc );
	//UpdateKeywords();
	
	return pGroup;
}

//////////////////////////////////////////////////////////////////////
// ContainGroup:
//
BOOL CDJTCluster::ContainGroup(CDJTGroup * pGroup)
{
	ASSERT( pGroup != NULL );	// Don't accept null pointers.
	
	for( int i=0; i<m_nGroups; i++ )
	{
		if( m_GroupPtrs[i] == pGroup ) 
			return TRUE; 
	}
	
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// GenrePolicy:
// Sets the policy that assign a document to a set of groups,
// based on the documents "genre probabilities".
//
void CDJTCluster::GenrePolicy( GenrePtrList* pEmptyGenreList, CDJTDocument* pDoc )
{
	ASSERT( pDoc != NULL );
	GenreProbPtrArray *pGenreProbPtrs;
	CDJGenreProb *pProb;
	DJGenre* pGenre;

	/*
	// Simple policy, assign document all groups with 
	// genre probability greater than zero.
	pGenreProbPtrs = &pDoc->m_GenreProbPtrs;
	for( int i=0; i < pGenreProbPtrs->GetSize(); i++ )
	{
		pProb = pGenreProbPtrs->GetAt(i);
		ASSERT( pProb != NULL );

		if( pProb->Probability() > 0 )
		{
			pGenre = (DJGenre*) pProb->Genre();
			pEmptyGenreList->AddTail( pGenre );
		}
	}
	*/

	// Simple policy, assign document to the group with 
	// highest genre probability.
	double fPVal, fPMax = -numeric_limits<double>::infinity();
	pGenreProbPtrs = &pDoc->m_GenreProbPtrs;
	for( int i=0; i < pGenreProbPtrs->GetSize(); i++ )
	{
		pProb = pGenreProbPtrs->GetAt(i);
		ASSERT( pProb != NULL );

		if( (fPVal = pProb->Probability()) > fPMax )
		{
			fPMax = fPVal;
			pGenre = (DJGenre*) pProb->Genre();
		}
	}
	pEmptyGenreList->AddTail( pGenre );	
}


//////////////////////////////////////////////////////////////////////
// Dump:
//
#ifdef _DEBUG
void CDJTCluster::Dump(CDumpContext& dc) const
{
	// Call base class function first.
	CObject::Dump( dc );

	// Dump info about documents in cluster.
	dc << "=====================================\n";
	for( int i=0; i<m_GroupPtrs.GetSize(); i++ )
	{
		m_GroupPtrs[i]->Dump( dc );
	}
}
#endif



//////////////////////////////////////////////////////////////////////
// ClusterKeywords:
//
int CDJTCluster::ClusterKeywords(CStringArray& Strings)
{
	CList<TermIndexAssoc, TermIndexAssoc&> bestList;
	TermIndexAssoc termIndexAssoc;
	POSITION mapPos, listPos, lastPos;
	CString strTmp, *pstrTerm;
	USHORT usIndex;
	double fVal;

	CSingleLock tmpLock( &m_pTermIndexMap->m_TIMLock, TRUE );
	ASSERT( Strings.GetSize() > 0 );

	mapPos = m_pTermIndexMap->m_TheTermMap.GetStartPosition();
	while( mapPos != NULL )
	{
		m_pTermIndexMap->m_TheTermMap.GetNextAssoc( mapPos, strTmp, usIndex );

		fVal = m_Centroid.WeightAt(usIndex);	// Ordinary weights.
		//fVal = m_Centroid.TermFreqArrayAt(usIndex);	// Use only frequency.

		if( fVal > 0 && IsValidKeyword(strTmp) )	// Filter out URLs and stuff.
		{			
			if( bestList.GetCount() == Strings.GetSize() && 
				fVal >= bestList.GetTail().fValue )
			{
				delete bestList.RemoveTail().pstrTerm;	// Make som space in list.
			}
			
			if( bestList.GetCount() < Strings.GetSize() )
			{
				lastPos = listPos = bestList.GetHeadPosition();
				while( listPos != NULL ) 
				{
					lastPos = listPos;
					if( fVal <= bestList.GetNext(listPos).fValue )
						break;
				}
				
				termIndexAssoc.pstrTerm = new CString( strTmp );
				termIndexAssoc.fValue = fVal;
				
				// Add to list.
				if( lastPos != NULL )
					bestList.InsertAfter( lastPos, termIndexAssoc );
				else
					bestList.AddHead( termIndexAssoc );
			}
		}
	}

	ASSERT( bestList.GetCount() <= Strings.GetSize() );
	int index = 0;
	while( !bestList.IsEmpty() )
	{
		pstrTerm = bestList.RemoveHead().pstrTerm;
		Strings[index++] = *pstrTerm;
		delete pstrTerm;
	}

	return index;
}


//////////////////////////////////////////////////////////////////////
// IsValidKeyword:
//
BOOL CDJTCluster::IsValidKeyword( LPCTSTR pszKeyword )
{
	if( strlen(pszKeyword) < 3 ) return FALSE;		// No short words.
	if( strchr(pszKeyword, '.') ) return FALSE;		// No dots.
	if( strchr(pszKeyword, '/') ) return FALSE;		// No slashes.
	if( strchr(pszKeyword, '@') ) return FALSE;		// No email.
	if( IsNumber(pszKeyword) ) return FALSE;		// No numbers.
	if( !NextAlpha(pszKeyword) ) return FALSE;		// No words w/o alpha chars.

	return TRUE;
}
