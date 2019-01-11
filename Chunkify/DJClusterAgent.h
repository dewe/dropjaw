// DJClusterAgent.h: interface for the CDJClusterAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJCLUSTERAGENT_H__C5E04AE2_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJCLUSTERAGENT_H__C5E04AE2_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "DJDocAgent.h"
#include "DJTCluster.h"
#include "DJTDocument.h"


// Constants
//
const int DJ_CLUSTERAGENT_MAX_CLUSTERS = 20;


// CDJClusterAgent class definition
//
class CDJClusterAgent : public CDJDocAgent  
{
	DECLARE_DYNAMIC( CDJClusterAgent );
public:
	// Constructors
	CDJClusterAgent(int nClusters);
	virtual ~CDJClusterAgent();

	// Attributes
	int m_nClusters;
	int m_nDocsInFilter;
	int m_nDocsInTotal;

	// Operations
	inline ClusterPtrArray* Clusters();
	void ConnectAgent(CDJAgent* pSource);
	void SetFilter( GroupPtrList* pGroupPtrs );
	int ExpectedSize();

	// Static operations
	static double Similarity( CDJTDocument* pDoc1, CDJTDocument* pDoc2 );

	// Implementation
	BOOL m_bInitialCollection;
	DocumentPtrList m_InitialDocPtrs;
	ClusterPtrArray m_ClusterPtrArray;
	DocumentPtrList m_hotDocumentPtrs;
	CTypedPtrMap< CMapPtrToWord, CDJTGroup*, BOOL> m_FilterMap;

	static int InitCollSize(int nExpected, int nClusters);
	static UINT HotDocumentsToPipe(LPVOID pParam);
	static UINT ClusterAgentThread(LPVOID pParam);
	static void HierarchClustering(const DocumentPtrList* pDocPtrs, 
								   const ClusterPtrArray* pClusters, 
								   int nClusters,
								   BOOL *pbContinue);
	static CDJTCluster* AssignToCluster(CDJTDocument* pDoc, 
										ClusterPtrArray* pClusters, 
										int nClusters);
	static CDJTCluster* FreeCluster(const ClusterPtrArray* pClusters, 
								   int nClusters);
};


// Inline functions
//
inline ClusterPtrArray* CDJClusterAgent::Clusters() { return &m_ClusterPtrArray; }


// Type definitions
//
typedef CTypedPtrList<CObList, CDJClusterAgent*> ClusterAgentPtrList;


#endif // !defined(AFX_DJCLUSTERAGENT_H__C5E04AE2_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
