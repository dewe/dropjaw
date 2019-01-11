// DJClusterAgent.cpp: implementation of the CDJClusterAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <float.h>
#include <afxtempl.h>
#include "Chunkify.h"
#include "DJClusterAgent.h"
#include "DJProximityMatrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
//
IMPLEMENT_DYNAMIC( CDJClusterAgent, CDJDocAgent);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJClusterAgent::CDJClusterAgent(int nClusters) 
	: m_nClusters( nClusters )
{
	ASSERT( m_nClusters > 0 && m_nClusters <= DJ_CLUSTERAGENT_MAX_CLUSTERS );

	m_nDocsInFilter = 0;
	m_nDocsInTotal = 0;
	m_bInitialCollection = TRUE;

	// For efficiency, set up the collections before start.
	m_FilterMap.InitHashTable( HashTableSize(DJ_GENRE_PALETTE_SIZE));
	m_ClusterPtrArray.SetSize( m_nClusters );

	// Initialize new set of clusters.
	for( int i=0; i<m_nClusters; i++ )
		VERIFY( m_ClusterPtrArray[i] = new CDJTCluster(TermIndexMap()) );

	// Set the message sent when agent is dead.
	m_ExitMessage = DJ_WM_CLUSTERAGENT_DEAD;
}

CDJClusterAgent::~CDJClusterAgent()
{
	// Assure the threads are dead before removing clusters.
	KillThreads();

	// Clean up, delete the clusters.
	for( int i=0; i<m_ClusterPtrArray.GetSize(); i++ )
		delete m_ClusterPtrArray[i];

	m_ClusterPtrArray.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ClusterAgentThread
//
UINT CDJClusterAgent::ClusterAgentThread(void* pParam)
{
	THREADPARMS* pThreadParms;
	CDJClusterAgent* pOwner;
	CDJDocAgent* pSource;
	CDJTPipe* pPipe;
	CDJTDocument* pDoc;
	ClusterPtrArray* pClusters;
	CDJTCluster* pTmpCluster;
	CDJTGroup* pGroup;
	CTypedPtrMap<CMapPtrToWord, CDJTGroup*, BOOL> *pFilterMap;
	int nClusters;

	// Initialize thread communication.
	pThreadParms = (THREADPARMS*) pParam;
	ASSERT( pThreadParms != NULL );

	pPipe = pThreadParms->pPipe;
	ASSERT( pPipe != NULL );

	pSource = (CDJDocAgent*) pThreadParms->pSource;
	ASSERT( pSource != NULL );
	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJDocAgent)) );

	pOwner = (CDJClusterAgent*) pThreadParms->pOwner;
	ASSERT( pOwner != NULL );
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJClusterAgent)) );

	nClusters = pOwner->m_nClusters;
	pClusters = &pOwner->m_ClusterPtrArray;
	ASSERT( pClusters != NULL );

	pFilterMap = &pOwner->m_FilterMap;
	ASSERT( pFilterMap != NULL );

	CSingleLock tmpLock( &pOwner->m_AgentLock, FALSE );
	
	// Worker-loop.
	while( pThreadParms->bContinue )
	{
		::Sleep(0);	// Nice thread.


		// Try to get document from source.
		if( !pSource->NextDocument(&pDoc) )	
		{
			// If Source is dead and empty it's time to go kill yourself.
			if( pSource->IsDead() && pSource->IsEmpty() )
			{
				if( pOwner->m_bInitialCollection )
				{
					pOwner->m_bInitialCollection = FALSE;
					HierarchClustering( &pOwner->m_InitialDocPtrs, 
										pClusters, 
										nClusters, 
										&pThreadParms->bContinue );
				}
				break;
			}

			// Source is still valid. Sleep for a while.
			::Sleep(DJ_AGENT_SLEEP);
		}

		else
		{
			tmpLock.Lock();
			pOwner->m_nDocsInTotal++;
			tmpLock.Unlock();

//			TermIndexMap()->SetIdfBaseSize( pSource->ExpectedSize() );

			// Collect first X documents for hierarchical clustering.
			if( pOwner->m_bInitialCollection )
			{
				tmpLock.Lock();
				pOwner->m_InitialDocPtrs.AddTail( pDoc );
				tmpLock.Unlock();

				int nSize = InitCollSize( pSource->ExpectedSize(), nClusters );
				if( pOwner->m_InitialDocPtrs.GetCount() >= nSize )
				{
					pOwner->m_bInitialCollection = FALSE;
					HierarchClustering( &pOwner->m_InitialDocPtrs, 
										pClusters, 
										nClusters, 
										&pThreadParms->bContinue );
				}
			}

			else
			{
				// Do Cluster assignment on document.
				pTmpCluster = AssignToCluster( pDoc, pClusters, nClusters );

				// Add document to cluster.
				VERIFY( pGroup = pTmpCluster->AddDocument(pDoc) );

				// If document is found in filter, make it available on pipe.
				BOOL bForFutureUse;
				if( pFilterMap->Lookup( pGroup, bForFutureUse ) )
				{
					while( !pPipe->Write((LPVOID*)&pDoc) && pThreadParms->bContinue )
						::Sleep(DJ_AGENT_SLEEP);

					tmpLock.Lock();
					pOwner->m_nDocsInFilter++;
					tmpLock.Unlock();
				}
				pDoc = NULL;	// Let go of the doc pointer.
			}
		}
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////////////
// HotDocumentsToPipe
//
UINT CDJClusterAgent::HotDocumentsToPipe(void *pParam)
{
	THREADPARMS* pThreadParms;
	CDJClusterAgent* pOwner;
	CDJTPipe* pPipe;

	DocumentPtrList* pHotDocumentPtrs;
	CDJTDocument* pDoc;
	POSITION pos;

	// Initialize thread communication.
	pThreadParms = (THREADPARMS*) pParam;
	ASSERT( pThreadParms != NULL );

	pPipe = pThreadParms->pPipe;
	ASSERT( pPipe != NULL );

	pOwner = (CDJClusterAgent*) pThreadParms->pOwner;
	ASSERT( pOwner != NULL );
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJClusterAgent)) );

	pHotDocumentPtrs = &pOwner->m_hotDocumentPtrs;
	ASSERT( pHotDocumentPtrs != NULL );

	// Copy all hot documents to pipe.
	pos = pHotDocumentPtrs->GetHeadPosition();
	while( pos != NULL && pThreadParms->bContinue )
	{
		pDoc = pHotDocumentPtrs->GetNext( pos );

		while( !pPipe->Write((LPVOID*)&pDoc) && pThreadParms->bContinue )
			::Sleep(DJ_AGENT_SLEEP);
	}

	// Clear hot document list.
	pHotDocumentPtrs->RemoveAll();

	return 0;
}


//////////////////////////////////////////////////////////////////////
// AssignToCluster:
// Perform assign-to-nearest clustering, based on the existing clusters and 
// the properties of the document.
//
CDJTCluster* CDJClusterAgent::AssignToCluster(CDJTDocument* pDoc, 
											  ClusterPtrArray* pClusters, 
											  int nClusters)
{
	CDJTCluster *pBestCluster = NULL;
	double fSim, fMax = -1;
	
	ASSERT( pDoc != NULL );
	
	// Major magic here.
	for( int i=0; i<nClusters; i++ )
	{
		fSim = Similarity( pDoc, pClusters->GetAt(i)->Centroid() );
		if( fSim > fMax )
		{
			fMax = fSim;
			pBestCluster = pClusters->GetAt(i);
		}

		::Sleep(0);	// Friendly thread.
	}
	
	// If no cluster showed any similarity, join two clusters.
	ASSERT( pBestCluster != NULL );
	if( pBestCluster == NULL )
		pBestCluster = FreeCluster( pClusters, nClusters );
	
	return( pBestCluster );
}


//////////////////////////////////////////////////////////////////////
// HierarchClustering:
// Perform initial hierarchical agglomerative clustering, filling the
// clusters with the initial documents, constituting the initial clusters.
// Executed only once.
//
void CDJClusterAgent::HierarchClustering(const DocumentPtrList *pDocPtrList,
										 const ClusterPtrArray *pClusters, 
										 int nReqClusters,
										 BOOL *pbContinue)
{
	int nClusters = pDocPtrList->GetCount();
	CDJProximityMatrix ProxMatrix( nClusters, Similarity );
	POSITION pos;
	CDJClusterNode *pNode, *pNode1, *pNode2;

	// Create initial clusters. One doc for each cluster.
	ASSERT( pDocPtrList->GetCount() == nClusters );
	pos = pDocPtrList->GetHeadPosition();
	while( pos != NULL )
	{
		pNode = new CDJClusterNode;
		pNode->AddDocument( pDocPtrList->GetNext(pos) );
		ProxMatrix.AddClusterLast( pNode );
	}

	// Reduce clusters by joining closest...
	while( nClusters > nReqClusters )
	{
		ProxMatrix.ClosestClusters( &pNode1, &pNode2 );

/*		// Code for single link algorithm.
		ProxMatrix.UpdateSingleLinkDists( pNode1, pNode2 );

		// Copy all docs.
		pos = pNode2->m_NodeDocPtrs.GetHeadPosition();
		while( pos != NULL )
			pNode1->AddDocument( pNode->m_NodeDocPtrs.GetNext(pos) );

		ProxMatrix.RemoveCluster( pNode2 );
		delete pNode2;	// delete unused cluster.
		nClusters--;
*/

		// Code for group average algorithm
		ProxMatrix.RemoveCluster( pNode2 );

		// Copy all docs.
		pos = pNode2->m_NodeDocPtrs.GetHeadPosition();
		while( pos != NULL )
			pNode1->AddDocument( pNode->m_NodeDocPtrs.GetNext(pos) );

		delete pNode2;	// delete unused cluster.
		nClusters--;

		ProxMatrix.UpdateGroupAverageDists( pNode1 );
	}
	
	// Copy clustered documents to the real clusters.
	for( int i=0; i<nClusters; i++ )
	{
		pNode = ProxMatrix.m_ClusterPtrs[i];
		CDJTCluster *pC = pClusters->GetAt(i);
		ASSERT( pC != NULL );

		// Copy docs.
		pos = pNode->m_NodeDocPtrs.GetHeadPosition();
		while( pos != NULL )
			pC->AddDocument( pNode->m_NodeDocPtrs.GetNext(pos) );

		delete pNode;
	}
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ConnectAgent:
//
void CDJClusterAgent::ConnectAgent(CDJAgent* pSource)
{
	AFX_THREADPROC ctrlFncts[] = { CDJClusterAgent::ClusterAgentThread };

	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJDocAgent)) );
	LaunchThreads( ctrlFncts, 1, pSource );
}


//////////////////////////////////////////////////////////////////////
// SetFilter:
// Reset the action of the agent. Before calling this function, all 
// agents after this one must be dead. This function performs the 
// following actions:
// 1.	Stop current thread.
// 2.	Reset the pipe. 
// 3.	Establish a new filter (maybe empty).
// 4.	Copy all document pointers matching the filter. 
// 6.	Start a new thread writing those pointers to the pipe.
// 7.	Start a new thread processing incoming docs.
//
// In:	A List of pointers to groups that the following agents are 
//		interested in, or NULL for no following agents.
//
void CDJClusterAgent::SetFilter(GroupPtrList* pGroupPtrs)
{
	POSITION pos;
	CDJTGroup* pGroup;
	UINT nExitMsg;
	CSingleLock tmpLock( &m_AgentLock, FALSE );
	
	// Stop current threads, reset pipe.
	nExitMsg = m_ExitMessage;
	m_ExitMessage = DJ_WM_CLUSTERAGENT_STOPPED;
	KillThreads();
	m_ExitMessage = nExitMsg;
	VERIFY( m_pPipe->Reset() );

	tmpLock.Lock();
	m_FilterMap.RemoveAll();
	m_hotDocumentPtrs.RemoveAll();

	if( pGroupPtrs != NULL )
	{
		// Set up new filter and collect all hot documents.
		pos = pGroupPtrs->GetHeadPosition();
		while( pos != NULL )
		{
			pGroup = pGroupPtrs->GetNext(pos);
			m_FilterMap.SetAt( pGroup, TRUE );
			pGroup->GetDocPtrs( &m_hotDocumentPtrs, pGroup->NumOfDocuments() );
		}
	}

	m_nDocsInFilter = m_hotDocumentPtrs.GetCount();

	// Launch new threads. If pGroupPtrs is NULL, HotDocumentsToPipe will die soon.
	AFX_THREADPROC ctrlFncts[] = {	CDJClusterAgent::ClusterAgentThread, 
									CDJClusterAgent::HotDocumentsToPipe };

	tmpLock.Unlock();
	LaunchThreads( ctrlFncts, 2, m_ThreadParms.pSource );
}


//////////////////////////////////////////////////////////////////////
// Similarity:
// Compute the measure of similarity between two documents.
// In this implementation the measure is the "cosine coefficient", [0,1].
//
double CDJClusterAgent::Similarity(CDJTDocument* pDoc1, CDJTDocument* pDoc2 )
{
	int k = 0;
	double wi, wj;
	double fVectProdSum = 0;
	double fWiSquareSum = 0, fWjSquareSum = 0;

	if( pDoc1 == pDoc2 )
		return( -1.0 );

	while( k <= pDoc1->m_nMaxIndex || k <= pDoc2->m_nMaxIndex )
	{
		if( (wi = pDoc1->WeightAt(k)) != 0 )
		{
			ASSERT( !_isnan(wi) && _finite(wi) && wi > 0 );
			fWiSquareSum += wi * wi;
		}

		if( (wj = pDoc2->WeightAt(k)) != 0 )
		{
			ASSERT( !_isnan(wj) && _finite(wj) && wj > 0);
			fWjSquareSum += wj * wj;
		}
		
		fVectProdSum += wi * wj;
		k++;
	}

/*
	while( k <= pDoc1->m_nMaxIndex || k <= pDoc2->m_nMaxIndex )
	{
		wi = pDoc1->WeightAt(k); 
		wj = pDoc2->WeightAt(k);

		ASSERT( !_isnan(wi) && _finite(wi) );
		ASSERT( !_isnan(wj) && _finite(wj) );
		ASSERT( wi >= 0 && wj >= 0 );
		
		fVectProdSum += wi * wj;
		fWiSquareSum += wi * wi;
		fWjSquareSum += wj * wj;
		k++;
	}
*/
	ASSERT( !_isnan(fVectProdSum) && _finite(fVectProdSum) && fVectProdSum >= 0 );
	ASSERT( !_isnan(fWiSquareSum) && _finite(fWiSquareSum) && fWiSquareSum >= 0 );
	ASSERT( !_isnan(fWjSquareSum) && _finite(fWjSquareSum) && fWjSquareSum >= 0 );

	::Sleep( 0 );	// Friendly thread.

	return fVectProdSum / sqrt(fWiSquareSum * fWjSquareSum);
}


//////////////////////////////////////////////////////////////////////
// ExpectedSize:
//
int CDJClusterAgent::ExpectedSize()
{
	CSingleLock tmpLock( &m_AgentLock, TRUE );

	ASSERT( m_ThreadParms.pSource != NULL );

	if( m_FilterMap.IsEmpty() )
		return m_ThreadParms.pSource->ExpectedSize();

	float fFilterFactor = (float)m_nDocsInFilter / m_nDocsInTotal;
	return (int) ceil( m_ThreadParms.pSource->ExpectedSize() * fFilterFactor );
}


//////////////////////////////////////////////////////////////////////
// InitCollSize: (static)
// Return the number of documents used for the initial clustering.
// Note minimum returned is two times the number of clusters.
//
int CDJClusterAgent::InitCollSize( int nExpected, int nClusters )
{
	int nXpct = (int) ceil( sqrt(g_InitClusterK * nExpected) );
	ASSERT( nXpct <= nExpected );
	return max( nXpct, 2 * nClusters );
}


//////////////////////////////////////////////////////////////////////
// FreeCluster: (static)
// Join the two closest clusters in order to free one cluster for new 
// documents.
//
CDJTCluster* CDJClusterAgent::FreeCluster( const ClusterPtrArray* pClusters, 
										  int nClusters)
{
	int i, j, k, nIMax, nJMax, nDocs;
	double fSim, fSimMax;
	CDJTCluster *pClust1, *pClust2;
	GroupPtrArray* pGroups;
	DocumentPtrList DocPtrs;
	POSITION listPos;

	// Find the two closest clusters.
	fSimMax = -1;
	nIMax = nJMax = -1;
	for( i=0; i<nClusters-1; i++ )
	{
		for( j=i+1; j<nClusters; j++ )
		{
			fSim = Similarity( (*pClusters)[i]->Centroid(),
							   (*pClusters)[j]->Centroid() );
			if( fSim > fSimMax )
			{
				fSimMax = fSim;
				nIMax = i;
				nJMax = j;
			}
		}
	}

	ASSERT( nIMax > -1 && nJMax > -1 && nIMax != nJMax);

	pClust1 = pClusters->GetAt(nJMax);
	pClust2 = pClusters->GetAt(nIMax);

	ASSERT( pClust1 != NULL && pClust2 != NULL );

	// Move all documents from one cluster to the other.
	pGroups = pClust1->Groups();
	for( k=0; k<pClust1->m_nGroups; k++ )
	{
		nDocs = pGroups->GetAt(k)->GetDocPtrs( &DocPtrs );
		listPos = DocPtrs.GetHeadPosition();
		while( listPos != NULL )
			pClust2->AddDocument( DocPtrs.GetNext(listPos) );
		pGroups->GetAt(k)->RemoveAll();
	}

	// Notify Easify application.
	VERIFY( PostMessage(g_hEasify, 
						DJ_WM_CLUSTERS_FUSED, 
						NULL, 
						NULL
						));

	return pClust2;
}
