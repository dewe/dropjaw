// DJClusterNode.cpp: implementation of the CDJClusterNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJClusterNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJClusterNode, CObject );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJClusterNode::CDJClusterNode()
{
	m_nDocCount = 0;
}

CDJClusterNode::~CDJClusterNode()
{

}

//////////////////////////////////////////////////////////////////////
// AddDocument: (Overridable)
// In CDJTClusterNode this function always returns NULL.
//
CDJTGroup* CDJClusterNode::AddDocument( CDJTDocument* pDoc )
{
	ASSERT( pDoc != NULL );

	AddToCentroid( pDoc );
	m_NodeDocPtrs.AddTail( pDoc );
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// CalculateCentroid:
// Calculate the centroid based on average term frequency.
//
void CDJClusterNode::AddToCentroid( CDJTDocument* pDoc )
{
	int	i=0;
	double w, fNom, fDocWeight;
	double dLength = 1,	dSum = 0;

	for( i=0; i <= pDoc->m_nMaxIndex; i++ )
	{
		w = pDoc->TermFreqArrayAt(i);
		dSum += w * w;
	}
	
	ASSERT( dSum > 0 );
	dLength = sqrt(dSum);

	for( i=0; i <= pDoc->m_nMaxIndex; i++ )
	{
		fDocWeight = (double)(pDoc->TermFreqArrayAt(i) / dLength);	// Normalized weight;
		fNom = m_Centroid.TermFreqArrayAt(i) * m_nDocCount + fDocWeight;
		m_Centroid.AddToTermFreqArray( (double) fNom / (m_nDocCount+1), i );
	}

	// One more document added to centroid...
	m_nDocCount++;
}

