// DJTPipe.cpp: implementation of the CDJTPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DJTPipe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJTPipe::CDJTPipe(int nSize) : m_nSize(nSize)

{
	ASSERT( nSize <= DJ_PIPE_MAX_SIZE );
	m_pListCount = new CSemaphore( nSize, nSize );
}

CDJTPipe::~CDJTPipe() 
{ 
	m_List.RemoveAll();
	delete m_pListCount;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Read/Write
//
// Return TRUE when a call is succesfull.
// Return FALSE when the pipe is either full or busy.

BOOL CDJTPipe::Read( LPVOID* pData )
{
	if( m_List.IsEmpty() )
		return FALSE;

	CSingleLock tmpLock( &m_ReadLock );
	if( !tmpLock.Lock(0) )
		return FALSE;

	*pData = (LPVOID) m_List.RemoveHead();
	m_pListCount->Unlock();
	return TRUE;
}


BOOL CDJTPipe::Write( LPVOID* pData )
{
	CSingleLock tmpLock( &m_WriteLock );
	if( !tmpLock.Lock(0) )
		return FALSE;

	if( !m_pListCount->Lock(0) )
		return FALSE;

	m_List.AddTail( (LPVOID) *pData );
	ASSERT( m_List.GetCount() <= m_nSize );
	return TRUE;
}


/////////////////////////////////////////////////////////////////////
// Reset:
//
BOOL CDJTPipe::Reset()
{
	CSingleLock tmpLock1( &m_ReadLock );
	CSingleLock tmpLock2( &m_WriteLock );

	// Wait for both read-handle and write-handle.
	if( tmpLock1.Lock(DJ_PIPE_TIMEOUT_RESET) && 
		tmpLock2.Lock(DJ_PIPE_TIMEOUT_RESET) )
	{
		// Empty internal list.
		m_List.RemoveAll();
		return TRUE;
	}

	else
		return FALSE;
}