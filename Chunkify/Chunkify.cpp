// Chunkify.cpp : API for the chunkify module...
//

#include "stdafx.h"
#include "Chunkify.h"
#include <math.h>
#include <stdlib.h>

#include "DJUrlAgent.h"
#include "DJFetchAgent.h"
#include "DJStatAgent.h"
#include "DJClusterAgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Thread controlling function.
UINT KillChunkifyThread(void* pParam);


//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

// These variables should be initialized from Easify.
#ifndef _TestChunkify
	CString g_strHomeDir;			// Home directory, base for subdirs.
	CString g_strBiberFeateursFileName;	// File for biber features list.
	CString g_strBiberNormalsFileName;	// File for normal values.
	CString g_strDocStoplistFileName;	// File with stoplist.
	double	g_InitClusterK;			// Coefficient for the number of documents
#else								// in initial hierarchical clustering.

// Initializations are here just to make it work in the Chunkify
// test application. 
#ifdef _TestChunkify
	CString g_strHomeDir = "c:\\users\\";
	CString g_strBiberFeateursFileName = "data\\featrs.djb";
	CString g_strBiberNormalsFileName = "data\\normals.djb";
	CString g_strDocStoplistFileName = "data\\stoplist.djd";
	double g_InitClusterK = 2.0;
#endif	//#ifdef 

#endif	//#ifndef,#else

HWND	g_hEasify;				// Handle to receiving window in Easify .


//////////////////////////////////////////////////////////////////////
// Chunkify API calls.
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AgentStruct:
//
struct AgentsStruct {
	CDJUrlAgent* pUrlAgent;
	CDJFetchAgent* pFetchAgent;
	CDJStatAgent* pStatAgent;
	CDJClusterAgent* pClusterAgent;
} g_ChunkifyAgents = {NULL,NULL,NULL,NULL/*,NULL*/};


//////////////////////////////////////////////////////////////////////
// DJCreateChunkify:
// Set up and initialize a new 'Chunkify'. 
// Returns a pointer to the first ClusterAgent to be used by the UI.
//
CDJClusterAgent* DJCreateChunkify( CWnd *pEasifyWnd,
								   int nFetchThreads, 
								   int nClusters, 
								   int nMaxFileSize,
								   BOOL bSaveTmpFiles, /* = FALSE */
								   LPCTSTR pszTmpDirectory /* = NULL */ )
{
	ASSERT( nFetchThreads <= DJ_AGENT_MAX_THREADS );
	ASSERT( nClusters <= DJ_AGENT_MAX_THREADS );

	// Set global window pointer.
	ASSERT( pEasifyWnd != NULL );
	ASSERT_VALID( pEasifyWnd );
	g_hEasify = pEasifyWnd->m_hWnd;

	// Create Agents.
	try
	{
		g_ChunkifyAgents.pUrlAgent = new CDJUrlAgent();
		g_ChunkifyAgents.pFetchAgent= new CDJFetchAgent( nMaxFileSize, 
											nFetchThreads, pszTmpDirectory );
		g_ChunkifyAgents.pStatAgent = new CDJStatAgent( bSaveTmpFiles );
		g_ChunkifyAgents.pClusterAgent = new CDJClusterAgent(nClusters);
								
	}
	catch(CException* pEx)
	{
		pEx->ReportError();
		throw;	// Let the exception be handled by outer catch block.
	}

	// Double check.
	ASSERT( g_ChunkifyAgents.pClusterAgent != NULL );
	ASSERT( g_ChunkifyAgents.pStatAgent != NULL );
	ASSERT( g_ChunkifyAgents.pFetchAgent != NULL );
	ASSERT( g_ChunkifyAgents.pUrlAgent != NULL );

	return g_ChunkifyAgents.pClusterAgent;
}


//////////////////////////////////////////////////////////////////////
// DJStartChunkify
// Run chunkify, using the query provided in 'szSearchString'.
//
CDJClusterAgent* DJStartChunkify( LPCTSTR szSearchString, int nUrls )
{
	ASSERT( szSearchString != NULL );

	ASSERT( g_ChunkifyAgents.pClusterAgent != NULL );
	ASSERT( g_ChunkifyAgents.pStatAgent != NULL );
	ASSERT( g_ChunkifyAgents.pFetchAgent != NULL );
	ASSERT( g_ChunkifyAgents.pUrlAgent != NULL );

	// Start it up, boy!
	g_ChunkifyAgents.pClusterAgent->ConnectAgent( g_ChunkifyAgents.pStatAgent );
	g_ChunkifyAgents.pStatAgent->ConnectAgent( g_ChunkifyAgents.pFetchAgent );
	g_ChunkifyAgents.pFetchAgent->ConnectAgent( g_ChunkifyAgents.pUrlAgent );
	g_ChunkifyAgents.pUrlAgent->DoSearch( szSearchString, nUrls );

	return g_ChunkifyAgents.pClusterAgent;
}



//////////////////////////////////////////////////////////////////////
// DJSetupFilterFromTo:
// Set up a filter between the source and the destination. 
// Before calling this function, make sure no other agents are calling
// the source agent, or else some documents might get lost. The destination 
// agent must be a fresh, newly created object, not yet running.
//
void DJSetupFilterFromTo(
					CDJClusterAgent* pSourceAgent,		// Agent that provides the docs.
					GroupPtrList* pFilter,				// Groups we are interested in.
					CDJClusterAgent* pDestinationAgent)	// Agent that receives the docs.
{
	ASSERT( pSourceAgent != NULL );
	ASSERT( pFilter != NULL );
	ASSERT( pDestinationAgent != NULL );

	ASSERT( !pFilter->IsEmpty() );			// Empty filter is useless.
	pSourceAgent->SetFilter( pFilter );

	pDestinationAgent->ConnectAgent( pSourceAgent );
}



//////////////////////////////////////////////////////////////////////
// DJDeleteChunkify:
//
void DJDeleteChunkify()
{
	AgentsStruct *pAgents = new AgentsStruct;

	// Copy pointers, and check for null pointers.
	VERIFY( pAgents->pUrlAgent = g_ChunkifyAgents.pUrlAgent );
	VERIFY( pAgents->pFetchAgent = g_ChunkifyAgents.pFetchAgent );
	VERIFY( pAgents->pStatAgent = g_ChunkifyAgents.pStatAgent );
	VERIFY( pAgents->pClusterAgent = g_ChunkifyAgents.pClusterAgent );

	// Start thread.
	AfxBeginThread( KillChunkifyThread, pAgents );

	// Reset Agents structure.
	g_ChunkifyAgents.pUrlAgent = NULL;
	g_ChunkifyAgents.pFetchAgent = NULL;
	g_ChunkifyAgents.pStatAgent = NULL;
	g_ChunkifyAgents.pClusterAgent = NULL;
}


//////////////////////////////////////////////////////////////////////
// DJStopChunkify:
//
void DJStopChunkify()
{
	ASSERT( g_ChunkifyAgents.pClusterAgent != NULL );
	ASSERT( g_ChunkifyAgents.pStatAgent != NULL );
	ASSERT( g_ChunkifyAgents.pFetchAgent != NULL );
	ASSERT( g_ChunkifyAgents.pUrlAgent != NULL );

	g_ChunkifyAgents.pClusterAgent->SetStopFlag();
	g_ChunkifyAgents.pStatAgent->SetStopFlag();
	g_ChunkifyAgents.pFetchAgent->SetStopFlag();
	g_ChunkifyAgents.pUrlAgent->SetStopFlag();
}




//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// KillChunkifyThread:
// Kill each agent in chunkify, in an asynchronous mode...
//
UINT KillChunkifyThread(void* pParam)
{
	ASSERT( pParam != NULL );
	AgentsStruct *pAgents = (AgentsStruct*) pParam;

	// Remove all agents.
	delete pAgents->pClusterAgent;
	pAgents->pClusterAgent = NULL;

	delete pAgents->pStatAgent;
	pAgents->pStatAgent = NULL;

	delete pAgents->pFetchAgent;
	pAgents->pFetchAgent = NULL;

	delete pAgents->pUrlAgent;
	pAgents->pUrlAgent = NULL;

	// Clean up.
	delete pAgents;

	// Notify Easify application.
	VERIFY( PostMessage(g_hEasify, 
						DJ_WM_ALL_AGENTS_DELETED, 
						NULL, 
						NULL
						));

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Global helpers
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// IsPrime:
// Test for prime. Brute force.
//
BOOL IsPrime( int n ) 
{
	int d;
	int maxd = (int)sqrt(n);
	
	if ((n % 2 == 0) && (n != 2))
		return FALSE;
	
	for( d=3; d <= maxd; d+=2 )
	{
		if (n % d == 0)
			return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// HashTableSize:
//
int HashTableSize(int nExpectedSize)
{
	int	n = (int) ceil( 1.2 * nExpectedSize );

	while( !IsPrime(n) )
		n++;

	return n;
}


//////////////////////////////////////////////////////////////////////
// RandomFloat:
//
float RandomFloat()
{
	static BOOL bSeeded = FALSE;

	if( !bSeeded )
	{
		srand( (unsigned)time( NULL ) );
		bSeeded = TRUE;
	}

	return (float) rand()/RAND_MAX;
}
