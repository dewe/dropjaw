// DJFetchAgent.cpp: implementation of the CDJFetchAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <time.h>
#include <afxinet.h>
#include <math.h>
#include "Chunkify.h"
#include "DJFetchAgent.h"
#include "DJTextBufferHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Constants
//

#define DJ_DEBUG_LOG_FILENAME "fetchlog.txt"

#ifdef _TestChunkify
	#define DJ_DEBUG_DO_LOGGING 1
#else
	#define	DJ_DEBUG_DO_LOGGING 0	// No logging when used in Easify.
#endif // _TestChunkify



// Globals
// 
CCriticalSection g_LogFileLock;		// Locks the logfile.


// Access dynamic run-time class information
//
IMPLEMENT_DYNAMIC( CDJFetchAgent, CDJAgent);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJFetchAgent::CDJFetchAgent(int nMaxBytesRead, 
							 int nThreads,
							 LPCTSTR szTmpFilePath /* = NULL */ ) 
	: m_nThreads( nThreads )
{
	m_nFetchedDocs = 0;
	m_nNonFetchedDocs = 0;
	m_nMaxBytesRead = nMaxBytesRead;

	if( szTmpFilePath == NULL )
		m_strTmpFilePath = "";
	else 
		m_strTmpFilePath = szTmpFilePath;

	m_bDoLog = DJ_DEBUG_DO_LOGGING;
	m_pLogFile = NULL;
	if( m_bDoLog )
	{
		CSingleLock logFileLock( &g_LogFileLock, TRUE );
		m_pLogFile = new CStdioFile( DJ_DEBUG_LOG_FILENAME, 
							CFile::modeWrite | CFile::modeCreate | 
							CFile::typeText | CFile::shareDenyNone );
	}

	// Set the message sent when agent is dead.
	m_ExitMessage = DJ_WM_FETCHAGENT_DEAD;
}

CDJFetchAgent::~CDJFetchAgent()
{
	// Make sure the threads are dead before removing list and logfile.
	KillThreads();

	// Clean up.
	if( m_bDoLog )
	{
		m_pLogFile->Close();
		delete m_pLogFile;
		m_pLogFile = NULL;
	}

	// Delete all objects pointed to by m_UrlPtrs.
	CSingleLock tmpLock( &m_AgentLock, TRUE );
	POSITION pos = m_FilePtrs.GetHeadPosition();
	while( pos != NULL )
	{
		delete m_FilePtrs.GetNext( pos );
	}
	m_FilePtrs.RemoveAll();
	ASSERT( m_FilePtrs.IsEmpty() );
}


//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

UINT CDJFetchAgent::FetchAgentThread(LPVOID pParam)
{
	THREADPARMS* pThreadParms;
	CDJTPipe* pPipe;
	CDJFetchAgent* pOwner;
	CDJUrlAgent* pSource;
	CDJInternetSession* pInetSession;
	CDJUrl* pUrl = NULL;
	CStdioFile* pInetFile = NULL;
	CDJTempFile* pTmpFile = NULL;
	FilePtrList* pFilePtrList;

	// Initialize thread communication.
	pThreadParms = (THREADPARMS*) pParam;
	ASSERT( pThreadParms != NULL );

	pPipe = pThreadParms->pPipe;
	ASSERT( pPipe != NULL );

	pSource = (CDJUrlAgent*) pThreadParms->pSource;
	ASSERT( pSource != NULL );
	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJUrlAgent)) );

	pOwner = (CDJFetchAgent*) pThreadParms->pOwner;
	ASSERT( pOwner != NULL );
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJFetchAgent)) );

	pFilePtrList = &pOwner->m_FilePtrs;
	ASSERT( pFilePtrList != NULL );

	// Protect common resources.
	CSingleLock tmpLock( &pOwner->m_AgentLock, FALSE );

	try
	{
		// Set up the Internet session.
		pInetSession = new CDJInternetSession(
							DJ_FETCHAGENT_ID_STRING,
							DJ_FETCHAGENT_CONNECT_TIMEOUT, 
							DJ_FETCHAGENT_CONNECT_RETRIES, 
							DJ_FETCHAGENT_CONNECT_BACKOFF, 
							DJ_FETCHAGENT_DATA_SEND_TIMEOUT, 
							DJ_FETCHAGENT_DATA_RECEIVE_TIMEOUT
							);
		ASSERT( pInetSession != NULL );

		// Only use callback while debugging.
		#ifdef _DEBUG
			pInetSession->EnableStatusCallback(TRUE);
		#endif
	}
	catch( CInternetException* ex )
	{
		ex->ReportError();
		throw;				// Do not proceed.
	}

	// Worker-loop.
	while( pThreadParms->bContinue )
	{
		::Sleep(0);	// Nice thread.

		// Try to get Url from source.
		if( !pSource->NextUrl(&pUrl) )	
		{
			// Source is dead and empty. Time to go kill yourself.
			if( pSource->IsDead() && pSource->IsEmpty() )
				break;

			// Source is still valid. Sleep for a while.
			::Sleep(DJ_AGENT_SLEEP);
		}
		// Got Url allright...
		else
		{
			ASSERT( pUrl != NULL );

			// New temporary file (CMemoryException).
			pTmpFile = new CDJTempFile( pUrl );
			ASSERT( pTmpFile != NULL );
			tmpLock.Lock();
			pFilePtrList->AddTail( pTmpFile );
			tmpLock.Unlock();

			try
			{

				//Open Http-connection (CInternetException).
				pInetFile = NewInternetFile( pInetSession, 
											 pUrl->m_strUrl, 
											 pOwner->m_bDoLog, 
											 pOwner->m_pLogFile );

				ASSERT( pInetFile != NULL );

				// Open temporary file (CFileException).
				OpenTmpFile( pTmpFile, pOwner->m_strTmpFilePath );

				// Transfer data from internet to temp file (CInternetException).
				TransferData( pInetFile, pTmpFile, pOwner->m_nMaxBytesRead );

				// Check for empty files...
				if( pTmpFile->GetLength() == 0 ) 
				{
					TRACE( "Found empty file, throwing CInternetException\n" );
					AfxThrowInternetException( 1 );
				}
				
				// Close internet file. Don't close DJTempFile (CInternetException).
				pInetFile->Close();
				delete pInetFile;
				pInetFile = NULL;

				// Put temp file pointer on pipe, but leave file open.
				while( !pPipe->Write((LPVOID*)&pTmpFile) && pThreadParms->bContinue )
					::Sleep(DJ_AGENT_SLEEP);
				pTmpFile = NULL;
				pUrl = NULL;

				// Do some statistics...
				if( pThreadParms->bContinue )
				{
					tmpLock.Lock();
					pOwner->m_nFetchedDocs++;
					tmpLock.Unlock();
				}
			}
			catch( CInternetException* pEx )
			{
				#ifdef _DEBUG
				// Write to logfile
				if( pOwner->m_bDoLog )
				{
					CString strLog;
					char szErr[1024];
					pEx->GetErrorMessage(szErr, 1024);
					strLog.Format( "%s, -, %s", pUrl->m_strUrl, szErr );
					WriteLog(strLog, pOwner->m_pLogFile);
					TRACE( "Error: %s", szErr );
				}
				#endif

				// Clean up befor proceeding.
				pEx->Delete();
				CleanUp(pInetFile, pTmpFile );

				// Do some statistics...
				tmpLock.Lock();
				pOwner->m_nNonFetchedDocs++;
				tmpLock.Unlock();
			}
			catch( CFileException* pEx )
			{
				// Couldn't open or write to temp file.
				pEx->ReportError();
				pEx->Delete();				
				CleanUp(pInetFile, pTmpFile );
				pThreadParms->bContinue = FALSE;
				break;
			}
		}
	} /* worker loop */

	pInetSession->Close();
	delete pInetSession;

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ConnectAgent
//
void CDJFetchAgent::ConnectAgent(CDJAgent* pSource)
{
	AFX_THREADPROC *ctrlFncts = new AFX_THREADPROC[ m_nThreads ] ;
	int numOfCtrlFnctns = m_nThreads;

	// Fill the vector.
	for( int i=0; i<numOfCtrlFnctns; i++ )
		ctrlFncts[i] = CDJFetchAgent::FetchAgentThread;

 	ASSERT( pSource->IsKindOf(RUNTIME_CLASS(CDJUrlAgent)) );
	LaunchThreads( ctrlFncts, 
				   numOfCtrlFnctns, 
				   pSource );

	delete[] ctrlFncts;
}


//////////////////////////////////////////////////////////////////////
// OpenTmpFile
//
CString CDJFetchAgent::OpenTmpFile(CStdioFile* pFile, LPCTSTR pTmpPath)
{
	char szTmpFilePath[MAX_PATH-12];	// Buffer for path to temp directory.
	char szTmpFileName[MAX_PATH];		// Buffer for name of temporary file.
	int nReturn;
	UINT nOpenFlags =
			CFile::modeReadWrite |	
			CFile::shareDenyWrite |
			CFile::typeText;			// Special carriage return processing.
	CFileException fEx;
	static CCriticalSection nameLock;
	
	if ( strcmp(pTmpPath, "") == 0 )
	{
		// Get system temp path.
		nReturn = ::GetTempPath( (DWORD)MAX_PATH-12, szTmpFilePath );
		ASSERT( nReturn != 0 );
		ASSERT( nReturn <= MAX_PATH-12 );	// The function needs a bigger buffer.
	}
	else
	{
		// Use provided temp path.
		ASSERT( strlen(pTmpPath) <= MAX_PATH-12 );
		strcpy( szTmpFilePath, pTmpPath );
	}
	
	// Create name for temporary file.
	nameLock.Lock();	// Avoid conflicts of naming in different threads.
	nReturn = ::GetTempFileName ( 
					szTmpFilePath,		// Path to temp directory.
					DJ_TMPFILE_PREFIX,	// Prefix for temporary file.
					0,					// 
					szTmpFileName		// Final file name.
					);
	nameLock.Unlock();
	ASSERT( nReturn != 0 );
	
	// Open file.
	if( !pFile->Open(szTmpFileName, nOpenFlags, &fEx) )
	{
		fEx.ReportError();
		AfxThrowFileException( fEx.m_cause, fEx.m_lOsError,szTmpFileName );
	}
	return CString( szTmpFileName );
}


//////////////////////////////////////////////////////////////////////
//NextFile
//
BOOL CDJFetchAgent::NextFile(CDJTempFile** pFilePtr)
{
	CDJTempFile* ReturnPtr;

	if( !NextDataPtr((LPVOID*)&ReturnPtr) )	// Get the pointer...
		return FALSE;

	#ifdef _DEBUG
	if( ReturnPtr != NULL )
		ASSERT( ReturnPtr->IsKindOf(RUNTIME_CLASS(CDJTempFile)) );
	#endif
	
	*pFilePtr = (CDJTempFile*) ReturnPtr;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// TransferData:
//
int CDJFetchAgent::TransferData( CStdioFile *pInetFile, 
								 CDJTempFile *pTmpFile, 
								 int nMaxBytes)
{
	char buf[DJ_FETCHAGENT_BUF_SIZE+1];
	int nRead, nReadTotal = 0;

	int nExpectedBytes = min( (int)pInetFile->GetLength(), nMaxBytes );
	CString *pstrUrl = &pTmpFile->m_pUrl->m_strUrl;

	VERIFY( SendMessage(g_hEasify,
						DJ_WM_FETCHAGENT_FILE, 
						(WPARAM) pstrUrl, 
						(LPARAM) nExpectedBytes ));

	// Transfer data from internet to temp file (CInternetException).
	while( (nRead = pInetFile->Read(buf, DJ_FETCHAGENT_BUF_SIZE)) &&
				(nReadTotal += nRead) < nMaxBytes)
	{
		buf[nRead] = '\0';
		Replace( buf, "\n\r", "\n" );	// remove strange CRs.
		Replace( buf, "\r", "\n" );

		pTmpFile->WriteString( buf );

		/* Easify doesn't process this message 
		VERIFY( PostMessage(g_hEasify,
							DJ_WM_FETCHAGENT_READ, 
							(WPARAM) (100*nReadTotal)/nExpectedBytes, 
							NULL
							));
		*/

		::Sleep(0);	// Nice thread.
	}

	return nReadTotal;
}


//////////////////////////////////////////////////////////////////////
// CleanUp
//
void CDJFetchAgent::CleanUp(CStdioFile* pIfile, CStdioFile* pTfile)
{
	if (pIfile != NULL) 
	{
		pIfile->Abort();
		delete pIfile;
		pIfile = NULL;
	}
	if (pTfile != NULL) 
	{
		pTfile->Abort();
		CString strPath = pTfile->GetFilePath();
		pTfile = NULL;				
	}	
}

//////////////////////////////////////////////////////////////////////
// WriteLog
//
void CDJFetchAgent::WriteLog(LPCTSTR szLog, CStdioFile* pFile)
{
	CSingleLock logFileLock( &g_LogFileLock, TRUE );
	try
	{
		ASSERT( pFile != NULL );
		pFile->WriteString( szLog );
	}
	catch( CFileException* pEx )
	{
		pEx->ReportError();
		throw;	// Handle over to other catch block...
	}
}


//////////////////////////////////////////////////////////////////////
// NewInternetFile
//
CStdioFile* CDJFetchAgent::NewInternetFile( CDJInternetSession* pSession, 
											LPCTSTR szUrl,
											BOOL bWriteToLog /* = FALSE */,
											CStdioFile* pLogFile /* = NULL */ )
{
	clock_t cl1 = 0;
	clock_t cl2 = 0;
	CStdioFile* pInetFile;

	ASSERT( !(bWriteToLog && pLogFile==NULL) );

	cl1 = clock();
	pInetFile = pSession->OpenURL( szUrl, 1, INTERNET_FLAG_TRANSFER_ASCII | 
												INTERNET_FLAG_EXISTING_CONNECT );
	cl2 = clock();
	ASSERT( pInetFile != NULL );

	if( bWriteToLog )
	{
		CString strLog;
		strLog.Format( "%s, %f\n", szUrl, (double)(cl2-cl1)/CLOCKS_PER_SEC );
		WriteLog(strLog, pLogFile);
	}

	return pInetFile;
}


//////////////////////////////////////////////////////////////////////
// ExpectedSize (Overridable function):
// Returns the expected size of collected document space.
//
int CDJFetchAgent::ExpectedSize()
{
	CSingleLock tmpLock( &m_AgentLock, TRUE );

	ASSERT( m_ThreadParms.pSource != NULL );
	float fSuccessRate = (float)m_nFetchedDocs / (m_nFetchedDocs+m_nNonFetchedDocs);
	return (int) ceil( m_ThreadParms.pSource->ExpectedSize() * fSuccessRate );
}

