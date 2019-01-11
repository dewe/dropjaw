// DJAgent.cpp: implementation of the CDJAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJAgent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJAgent, CObject);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJAgent::CDJAgent()
{
	m_nThreadPriority = THREAD_PRIORITY_NORMAL;
	m_ThreadParms.pSource = NULL;
	m_nNumOfThreads = 0;
	m_bAllThreadsDead = FALSE;	// Initially, fake a living agent.
	m_ExitMessage = NULL;
	m_nExpectedSize = -1;

	// Set up the pipe
	m_pPipe = new CDJTPipe( DJ_AGENT_PIPESIZE );
	ASSERT( m_pPipe != NULL );
}

CDJAgent::~CDJAgent()
{
	KillThreads();
	if( m_pPipe != NULL )
	{
		delete m_pPipe;
		m_pPipe = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// WatchThread
// Rises the 'AllThreadsDead'-flag when all threads have terminated.
//
UINT CDJAgent::WatchThread(void* pParam)
{
	CDJAgent* pOwner;
	DWORD dwReturn;

	ASSERT( pParam != NULL );
	pOwner = (CDJAgent*) pParam;
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJAgent)) );

	pOwner->m_bAllThreadsDead = FALSE;

	ASSERT( pOwner->m_nNumOfThreads <= MAXIMUM_WAIT_OBJECTS );

	do {
		Sleep( DJ_AGENT_SLEEP );	// Nice Agent.
		dwReturn = WaitForMultipleObjects( 
								pOwner->m_nNumOfThreads,
								pOwner->m_ThreadHandles,
								TRUE,
								INFINITE
								);
	} while( dwReturn != WAIT_OBJECT_0 );

	// Some stuff for debugging ...
	#ifdef _DEBUG
		if( dwReturn == WAIT_FAILED )
		{
			CString strTmp;
			strTmp.Format( "WatchThread: WAIT_FAILED, Error: %d", 
							GetLastError() );
			AfxMessageBox( strTmp );
		}

		for( int t=0; t<pOwner->m_nNumOfThreads; t++ )
		{
			if( dwReturn == WAIT_ABANDONED+t )
			{
				CString strTmp;
				strTmp.Format( "WatchThread: WAIT_ABANDONED+%d", t );
				AfxMessageBox( strTmp );
			}
		}

		for( t=1; t<pOwner->m_nNumOfThreads; t++ )
		{
			if( dwReturn == WAIT_OBJECT_0+t )
			{
				CString strTmp;
				strTmp.Format( "WatchThread: WAIT_OBJECT_0+%d", t );
				AfxMessageBox( strTmp );
			}
		}

	#endif

	ASSERT( dwReturn == WAIT_OBJECT_0 );
	pOwner->m_bAllThreadsDead = TRUE;

	// Notify the window
	if( pOwner->m_ExitMessage != NULL )
	{
		VERIFY( PostMessage(g_hEasify, 
							pOwner->m_ExitMessage, 
							(WPARAM)pOwner,
							NULL
							));
	}

	else
		ASSERT( FALSE );
	return 0;
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// LaunchThreads:
// Launches a set of threads using the thread control functions in 
// pfnThreadProcs. The thread object is set to survive after thread 
// termination, thus the responsibility for deletion is with 
// ~CDJAgent()/KillThreads().
//
void CDJAgent::LaunchThreads(
					AFX_THREADPROC pfnThreadProcs[],	// Vector of thread controllers.
					int nThreads,						// Number of controllers in vector.
					CDJAgent* pSource /* = NULL */)		// The feeding agent.
{
	CWinThread* pThread;
	CSingleLock tmpLock( &m_AgentLock, FALSE );
	AFX_THREADPROC pCntrlFnctn;

	ASSERT( nThreads > 0 && nThreads <= DJ_AGENT_MAX_THREADS );

	tmpLock.Lock();

	// Initialize thread parameters
	for( int i=0; i<DJ_AGENT_MAX_THREADS; i++ )
		m_ThreadHandles[i] = NULL;
	m_ThreadParms.pOwner = this;
	m_ThreadParms.pPipe = m_pPipe;
	m_ThreadParms.bContinue = TRUE;
	m_ThreadParms.pSource = pSource;

	// Assign new threads.
	for( i=0; i<nThreads; i++ )
	{
		// Check the pointer.
		pCntrlFnctn = pfnThreadProcs[i];
		ASSERT( pCntrlFnctn != NULL );

		// Launch the thread	
		pThread = AfxBeginThread(
						pCntrlFnctn, 
						&m_ThreadParms,		// pointer to the THREADPARMS struct.
						m_nThreadPriority,
						0,					// Default stacksize.
						CREATE_SUSPENDED,	// Wait to get handle.
						NULL );
		ASSERT( pThread != NULL );
		DuplicateHandle(					// Store handle in array.
						GetCurrentProcess(),
						pThread->m_hThread,
						GetCurrentProcess(),
						&m_ThreadHandles[i],
						NULL,
						TRUE,
						DUPLICATE_SAME_ACCESS );
		pThread->ResumeThread();
	}

	// Set counter.
	m_nNumOfThreads = nThreads;		

	// Launch watch thread
	pThread = AfxBeginThread( 
					CDJAgent::WatchThread, 
					this, 
					m_nThreadPriority+1,	// Higher priority than normal.
					0,
					CREATE_SUSPENDED,
					NULL );
	m_hWatchThread = pThread->m_hThread;
	pThread->ResumeThread();
	tmpLock.Unlock();
}



//////////////////////////////////////////////////////////////////////
// KillThreads:
// Kills the thread, even the hard way if needed.
//
void CDJAgent::KillThreads()
{
	DWORD dwExitCode;
	BOOL bReturn = TRUE;

	SetStopFlag();
	
	// Wait for the watch thread.
	::WaitForSingleObject( m_hWatchThread, DJ_AGENT_KILL_TIMEOUT );

	::Sleep( 200 );
	::GetExitCodeThread( m_hWatchThread, &dwExitCode );

	// Kill all threads the hard way.
	if( dwExitCode == STILL_ACTIVE )
	{
		AfxMessageBox( "Thread is not dead after timeout. It will die the nasty way..." );
		for( int i=0; i<DJ_AGENT_MAX_THREADS && m_ThreadHandles[i] != NULL; i++ )
			::TerminateThread(m_ThreadHandles[i], 4711);
	}
}


//////////////////////////////////////////////////////////////////////
// ConnectAgent (Overridable function):
// Sets up the thread with the right thread control function.
//
// Override this function to launch the customized thread  control 
// function. 
//
void CDJAgent::ConnectAgent(CDJAgent* pSource)
{
	// 'Virtual Function', no action.
	ASSERT( FALSE );
}


//////////////////////////////////////////////////////////////////////
// ExpectedSize (Overridable function):
// Returns the expected size of collected document space.
//
int CDJAgent::ExpectedSize()
{
	CSingleLock tmpLock( &m_AgentLock, TRUE );

	ASSERT( m_nExpectedSize != -1 );
	return m_nExpectedSize;
}

