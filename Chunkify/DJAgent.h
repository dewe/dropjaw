// DJAgent.h: interface for the CDJAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJAGENT_H__F8D725B2_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
#define AFX_DJAGENT_H__F8D725B2_ED32_11D0_8C2A_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxmt.h>
#include <afxinet.h>
#include "DJTPipe.h"


//////////////////////////////////////////////////////////////////////
// Constants
const int DJ_AGENT_PIPESIZE = 2;			// The standard size of an internal agent pipe.
const int DJ_AGENT_KILL_TIMEOUT = 300000;	// Huge Timeout when waiting for thread. (Tune) !!!
const int DJ_AGENT_MAX_THREADS = 12;		// Max number of threads in an agent.
const int DJ_AGENT_SLEEP = 50;		 // Sleep time when failed reading/writing data. (Tune)	!!!


// Thread communication structure
//
class CDJAgent;	// Declarate class without defining it.

struct ThreadParms {
	BOOL		bContinue;	// Terminate-thread flag.
	CDJAgent*	pSource;	// Pointer to source to read from.
	CDJAgent*	pOwner;		// Pointer thread owner (for convenient access).
	CDJTPipe*	pPipe;		// Pointer to the output pipe.
};
typedef struct ThreadParms THREADPARMS;


// CDJAgent Class declaration
//
class CDJAgent : public CObject  
{
	DECLARE_DYNAMIC( CDJAgent );
public:
	// Constructors
	CDJAgent();
	virtual ~CDJAgent();

	// Attributes
	int m_nThreadPriority;	// Set before launch, else standard setting.
	UINT m_ExitMessage;		// Message to be sent when all threads are dead.
	int m_nExpectedSize;

	// Operations
	inline BOOL IsEmpty();
	inline BOOL IsDead();
	inline BOOL NextDataPtr(LPVOID* pData);
	void LaunchThreads(	AFX_THREADPROC pfnThreadProcs[], 
						int nThreads,
						CDJAgent* pSource = NULL );
	virtual void ConnectAgent(CDJAgent* pSource);
	virtual int ExpectedSize();
	void SetStopFlag() { m_ThreadParms.bContinue = FALSE; };

	// Implementation
	CCriticalSection m_AgentLock;
	HANDLE m_hWatchThread;
	void KillThreads();
	THREADPARMS m_ThreadParms;
	static UINT WatchThread(LPVOID pParam);

protected:
	HANDLE m_ThreadHandles[DJ_AGENT_MAX_THREADS];
	CDJTPipe* m_pPipe;
	BOOL m_bAllThreadsDead;
	int m_nNumOfThreads;
};


// Inline functions
//
inline	BOOL CDJAgent::IsDead() { return m_bAllThreadsDead; }

inline	BOOL CDJAgent::IsEmpty()
{ 
	ASSERT( m_pPipe != NULL ); 
	return m_pPipe->IsEmpty(); 
}

inline BOOL CDJAgent::NextDataPtr(LPVOID* pData)
{ 
	ASSERT( m_pPipe != NULL ); 
	return m_pPipe->Read(pData);
}


#endif // !defined(AFX_DJAGENT_H__F8D725B2_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
