// DJProximityMatrix.cpp: implementation of the CDJProximityMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJProximityMatrix.h"
#include <limits>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJProximityMatrix::CDJProximityMatrix( int nInitSize, pSimFctn pFunc )
	: m_Matrix( nInitSize ), m_pSimFctn( pFunc )
{
	m_nSize = 0;

	// Initialize matrix...
	for( int i=0; i<nInitSize; i++ )
	{
		for( int j=0; j<nInitSize; j++ )
			m_Matrix.SetAt(i,j,-1);
	}
}


CDJProximityMatrix::~CDJProximityMatrix()
{

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AddClusterLast:
//
void CDJProximityMatrix::AddClusterLast( CDJClusterNode *pNode )
{
	// Add to cluster array.
	ASSERT( m_nSize == m_ClusterPtrs.Add(pNode) );

	// Calculate new distances.
	Centroid *pCentroid = m_ClusterPtrs[m_nSize]->Centroid();
	for( int j=0; j<m_nSize; j++ )
	{
		double fSim = (*m_pSimFctn)( pCentroid, m_ClusterPtrs[j]->Centroid() );
		m_Matrix.SetAt( m_nSize, j, fSim );
	}

	m_nSize++;
}

//////////////////////////////////////////////////////////////////////
// UpdateGroupAverageDists:
// Update values for the cluster pNode, using averages from the 
// centroids.
void CDJProximityMatrix::UpdateGroupAverageDists( CDJClusterNode *pNode )
{
	// Find clusters index
	int index=0;
	while( pNode != m_ClusterPtrs[index] ) { index++; }

	// Calculate new distances.
	Centroid *pCentroid = m_ClusterPtrs[index]->Centroid();
	for( int j=0; j<index; j++ )
	{
		double fSim = (*m_pSimFctn)( pCentroid, m_ClusterPtrs[j]->Centroid() );
		m_Matrix.SetAt( index, j, fSim );
	}
}

//////////////////////////////////////////////////////////////////////
// UpdateSingleLinkDists:
// Update values for the cluster pNodeDest, using single ling algorithm.
//
void CDJProximityMatrix::UpdateSingleLinkDists( CDJClusterNode *pNodeDest, 
											    CDJClusterNode *pNodeSrc )
{
	// Find clusters indices.
	int nIndexDest = -1, nIndexSrc = -1;
	for( int i=0; (nIndexDest==-1 || nIndexSrc==-1) && i<m_nSize; i++ )
	{
		if( m_ClusterPtrs[i] == pNodeDest ) { nIndexDest = i; }
		else if( m_ClusterPtrs[i] == pNodeSrc ) { nIndexSrc = i; }
	}

	ASSERT( nIndexDest != -1 && nIndexSrc != -1 );

	// Calculate new distances for pNodeDest.
	for( int j=0; j<nIndexDest; j++ )
	{
		double fSim = max( m_Matrix.GetAt(nIndexDest, j), 
						   m_Matrix.GetAt(nIndexSrc, j) );
		m_Matrix.SetAt( nIndexDest, j, fSim );
	}
}

//////////////////////////////////////////////////////////////////////
// ClosestClusters:
//
double CDJProximityMatrix::ClosestClusters(CDJClusterNode **pC1, CDJClusterNode **pC2)
{
	int maxi, maxj;
	double fSim, maxSim = -numeric_limits<double>::infinity();

	// Find closest distance, i.e. greatest similarity.
	for( int i=0; i<m_nSize; i++ )
	{
		for( int j=0; j<i; j++ )
		{
			if( (fSim = m_Matrix.GetAt(i,j)) > maxSim )
			{
				maxSim = fSim;
				maxi = i;
				maxj = j;
			}
		}
	}

	*pC1 = m_ClusterPtrs.GetAt( maxj );
	*pC2 = m_ClusterPtrs.GetAt( maxi );
	
	return maxSim;
}

//////////////////////////////////////////////////////////////////////
// RemoveCluster:
//
CDJClusterNode* CDJProximityMatrix::RemoveCluster( CDJClusterNode *pNode )
{
	ASSERT( pNode != NULL );

	// Find clusters index
	int index=0;
	while( pNode != m_ClusterPtrs[index] ) { index++; }

	CDJClusterNode* pRetNode = m_ClusterPtrs.GetAt( index );
		
	m_ClusterPtrs.RemoveAt( index );
	m_Matrix.DeleteRowCol( index, index );
	m_nSize--;

	return pRetNode;
}
