// Chunkify.h : header file for the CHUNKIFY module.
//

#if !defined(__CHUNKIFY_H__INCLUDED_)
#define __CHUNKIFY_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifdef _TestChunkify
	// Include for Chunkify test application.
	#include "chunkifyApp.h"
#else
	// Include for Dropjaw application.
	#include "..\DropJawV2\DropJawV2.h"

#endif //_TestChunkify

// Includes for Chunkify module...
//
#include "DJClusterAgent.h"
#include "DJGenre.h"
#include "DJTCluster.h"
#include "DJTGroup.h"
#include "DJTDocument.h"
#include "DJTTermIndexMap.h"

// Globals
//
// Initialized by Easify.
extern double g_InitClusterK;
extern CString g_strHomeDir;
extern CString g_strBiberFeateursFileName;
extern CString g_strBiberNormalsFileName;
extern CString g_strDocStoplistFileName;

// Initialized by Chunkify.
extern HWND g_hEasify;


// Chunkify messages
//
#define	DJ_WM_GROUPUPDATE			WM_USER+1
#define DJ_WM_FETCHAGENT_READ		WM_USER+2
#define DJ_WM_FETCHAGENT_FILE		WM_USER+3
#define DJ_WM_URLAGENT_URLS			WM_USER+4
#define DJ_WM_CLUSTERS_FUSED		WM_USER+5

#define DJ_WM_CLUSTERAGENT_STOPPED	WM_USER+11

#define DJ_WM_URLAGENT_DEAD			WM_USER+21
#define DJ_WM_FETCHAGENT_DEAD		WM_USER+22
#define DJ_WM_STATAGENT_DEAD		WM_USER+23
#define DJ_WM_CLUSTERAGENT_DEAD		WM_USER+24

#define DJ_WM_ALL_AGENTS_DELETED	WM_USER+31


// API functions
//
CDJClusterAgent*	DJCreateChunkify(CWnd *pEasifyWnd,		// Pointer to main appl. window.
									 int nFetchThreads,		// Number of fetching threads.
									 int nClusters,			// Clusters in first search.
									 int nMaxFileSize,		// Max bytes per file read from internet.
									 BOOL bSaveTmpFiles = FALSE,	// Leave tmp files on disk?
									 LPCTSTR pszTmpDirectory = NULL );	// NULL = system tmp dir.
CDJClusterAgent*	DJStartChunkify(LPCTSTR szSearchString, int nMaxUrls);
void				DJDeleteChunkify();
void				DJStopChunkify();
void				DJSetupFilterFromTo(CDJClusterAgent* pSourceAgent,
										GroupPtrList* pFilter,
										CDJClusterAgent* pDestinationAgent );

// Global help functions
//
int HashTableSize(int nExpectedSize);
float RandomFloat();


#endif // !defined(__CHUNKIFY_H__INCLUDED_)
