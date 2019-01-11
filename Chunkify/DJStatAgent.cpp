// DJStatAgent.cpp: implementation of the CDJStatAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <locale.h>
#include "Chunkify.h"
#include "DJStatAgent.h"
#include "DJFetchAgent.h"
#include "DJTempFile.h"
#include "DJTDocument.h"
#include "DJGStatistics.h"
#include "DJGBiber.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJStatAgent, CDJDocAgent);




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDJStatAgent::CDJStatAgent( BOOL bLeaveFiles /* = FALSE */ )
	: m_bLeaveFileOnDisk(bLeaveFiles)
{
	// Make my TermIndexMap available to the public.
	SetTermIndexMap( &m_TermIndexMap );

	// Set the message sent when agent is dead.
	m_ExitMessage = DJ_WM_STATAGENT_DEAD;
}


CDJStatAgent::~CDJStatAgent()
{
	// Kill threads before removing the document list.
	KillThreads();

	// Delete all objects pointed to by m_DocumentPtrList.
	// Protect common resources.
	CSingleLock tmpLock( &m_AgentLock, TRUE );
	POSITION pos = m_DocumentPtrList.GetHeadPosition();
	while( pos != NULL )
	{
		delete m_DocumentPtrList.GetNext( pos );
	}
	m_DocumentPtrList.RemoveAll();
	ASSERT( m_DocumentPtrList.IsEmpty() );
}

//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

UINT CDJStatAgent::StatAgentThread(LPVOID pParam)
{
	THREADPARMS* pThreadParms;
	CDJFetchAgent* pSource;
	CDJStatAgent* pOwner;
	DocumentPtrList* pDocumentPtrList;
	CDJTPipe* pPipe;
	CDJTempFile* pFile;
	CDJTDocument* pDoc;

	// Initialize thread communication.
	pThreadParms = (THREADPARMS*) pParam;
	ASSERT( pThreadParms != NULL );

	pPipe = pThreadParms->pPipe;
	ASSERT( pPipe != NULL );

	pSource = (CDJFetchAgent*) pThreadParms->pSource;
	ASSERT( pSource != NULL );
	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJFetchAgent)) );

	pOwner = (CDJStatAgent*) pThreadParms->pOwner;
	ASSERT( pOwner != NULL );
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJStatAgent)) );

	pDocumentPtrList = &pOwner->m_DocumentPtrList;
	ASSERT( pDocumentPtrList != NULL );
	
	// Protect common resources.
	CSingleLock tmpLock( &pOwner->m_AgentLock, FALSE );

	// Worker-loop.
	while( pThreadParms->bContinue )
	{
		::Sleep(0);	// Nice thread.

		// Try to get file pointer from source.
		if( !pSource->NextFile(&pFile) )
		{
			// Source is dead and empty. Time to go kill yourself.
			if( pSource->IsDead() && pSource->IsEmpty() )
				break;

			// Source is still valid. Sleep for a while.
			::Sleep(DJ_AGENT_SLEEP);
		}
		// Got file pointer allright...
		else
		{
			// Process file and create new document.
			try 
			{	
				// Rewind  to beginning of file.
				ASSERT( pFile->IsKindOf(RUNTIME_CLASS(CDJTempFile)) );
				pFile->SeekToBegin();
				
				// Create new document.
				pDoc = new CDJTDocument();	
				tmpLock.Lock();
				pDocumentPtrList->AddTail( pDoc );
				tmpLock.Unlock();
				
				// Perform statistic operations.
				PrepareDocument( pOwner, pFile, pDoc );
				
				// Close, and check for remove of the temporary file.
				pFile->Close();
				if( !pOwner->m_bLeaveFileOnDisk )
					CFile::Remove( pFile->GetFilePath() );
				pFile = NULL;

				// Put document on pipe.
				while( !pPipe->Write((LPVOID*)&pDoc) && pThreadParms->bContinue )
					::Sleep(DJ_AGENT_SLEEP);

				pDoc = NULL;
			}
			catch( CException* e )	// Oops, trouble!
			{
				e->ReportError();
				e->Delete();
				if( pFile != NULL )
				{
					pFile->Abort();
					pFile = NULL;
				}
				pThreadParms->bContinue = FALSE;
				break;
			}
		}
	}	/* end worker-loop */

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CDJStatAgent::ConnectAgent(CDJAgent* pSource)
{
	AFX_THREADPROC ctrlFncts[] = { CDJStatAgent::StatAgentThread };

 	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJFetchAgent)) );
	LaunchThreads( ctrlFncts, 1, pSource);
}



//////////////////////////////////////////////////////////////////////
// PrepareDocument:
// Perform all preparation (i.e. Statistics extraction) to create a
// proper document representation.
//
void CDJStatAgent::PrepareDocument( CDJStatAgent *pOwner,
								    CDJTempFile *pFile, 
								    CDJTDocument *pDoc)
{
	CString strLanguage;
	CString strLocaleType;

	// Set language.
	strLanguage = pFile->m_pUrl->m_strLang;
	strLanguage.MakeLower();

	// Set appropriate character set for dokument.
	strLocaleType = setlocale( LC_CTYPE, NULL );
	setlocale( LC_CTYPE, (LPCTSTR)strLanguage );

	// Do the statistic stuff.
	pDoc->PrepareFromUrl( pFile->m_pUrl );	// Collect basic data from Url.
	pOwner->m_GStatObject.ExtractStatistics( pFile, pDoc );	// Fill doc with statistics.
	InitTermFreqArray( pDoc, &pOwner->m_TermIndexMap );		// From collected stats, fill array.

	// Genre categorization
	pOwner->m_BiberScoreObject.CalculateScores( pDoc );
	pOwner->m_BiberScoreObject.EqualizeScores( pDoc );
	pOwner->m_BiberScoreObject.CalculateGenreScores( pDoc );

	// Free some memory not used anymore.
	pDoc->FreeStatsMem();

	// Reset codepage.
	setlocale( LC_CTYPE, (LPCTSTR)strLocaleType );

	#ifdef _DumpTerms
		pDoc->DumpTerms( afxDump );
	#endif

	::Sleep(0);	// Friendly thread.
}

//////////////////////////////////////////////////////////////////////
// InitTermFreqArray:
// Use the data collected in ExtractStatistics function to fill
// the local TermFreqArray and TermIndexMap. (In preparation for
// clustering procedures).
//
void CDJStatAgent::InitTermFreqArray(CDJTDocument* pDoc, CDJTTermIndexMap *pTIM)
{
	POSITION pos;
	CString strToken;
	USHORT usIndex;
	int nCount;

	ASSERT( !pDoc->m_pTokenMap->IsEmpty() || !pDoc->m_pUrlMap->IsEmpty() );

	// URLs from tags and text.
	pos = pDoc->m_pUrlMap->GetStartPosition();
	while( pos != NULL )
	{
		pDoc->m_pUrlMap->GetNextAssoc( pos, strToken, nCount );
		ASSERT( nCount > 0 );
		if( pTIM->AddTerm((LPCTSTR)strToken, &usIndex) )
		{
			ASSERT( usIndex >= 0 );
			pDoc->AddToTermFreqArray( nCount, usIndex );	
		}
	}

	// Tokens from text
	pos = pDoc->m_pTokenMap->GetStartPosition();
	while( pos != NULL )
	{
		pDoc->m_pTokenMap->GetNextAssoc( pos, strToken, nCount );
		ASSERT( nCount > 0 );

		if( pTIM->AddTerm((LPCTSTR)strToken, &usIndex) )
		{
			ASSERT( usIndex >= 0 );
			pDoc->AddToTermFreqArray( nCount, usIndex );	
		}

		::Sleep(0);	// Friendly thread.
	}
}
