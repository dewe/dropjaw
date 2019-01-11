// DJProximityMatrix.h: interface for the CDJProximityMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJPROXIMITYMATRIX_H__1FA4C6E1_2860_11D1_91E4_0060974B4CFF__INCLUDED_)
#define AFX_DJPROXIMITYMATRIX_H__1FA4C6E1_2860_11D1_91E4_0060974B4CFF__INCLUDED_

#include <afxtempl.h>
#include "DJMatrix.h"
#include "DJClusterNode.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Type definitions
//
typedef CArray<CDJClusterNode*, CDJClusterNode*> NodePtrArray;
typedef double (*pSimFctn)( CDJTDocument*, CDJTDocument* );


class CDJProximityMatrix  
{
public:
	// Construction
	CDJProximityMatrix(int nInitSize, pSimFctn pFunc);
	virtual ~CDJProximityMatrix();

	// Attributes
	int m_nSize;
	pSimFctn m_pSimFctn;
	NodePtrArray m_ClusterPtrs;
	
	// Operations
	void AddClusterLast( CDJClusterNode *pNode );
	double ClosestClusters( CDJClusterNode **pC1, CDJClusterNode **pC2 );
	CDJClusterNode* RemoveCluster( CDJClusterNode *pNode );
	void UpdateGroupAverageDists( CDJClusterNode *pNode );
	void UpdateSingleLinkDists( CDJClusterNode *pNodeDest, 
								CDJClusterNode *pNodeSrc );

	// Implementation
	CDJMatrix m_Matrix;
};

#endif // !defined(AFX_DJPROXIMITYMATRIX_H__1FA4C6E1_2860_11D1_91E4_0060974B4CFF__INCLUDED_)
