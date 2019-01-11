// DJTPipe.h: interface for the CDJTPipe class.
//
// Non-blocking queue/pipe.
// When a read or write operation fails, the call return FALSE.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTPIPE_H__F8492F51_E641_11D0_8C27_08002BE6493E__INCLUDED_)
#define AFX_DJTPIPE_H__F8492F51_E641_11D0_8C27_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxmt.h>
#include <afxcoll.h>

// Settings
//
const int DJ_PIPE_MAX_SIZE = 255;		// Arbitrary value...
const int DJ_PIPE_TIMEOUT_RESET = 3000;	// Milliseconds.

// CDJTPipe class definition.
//
class CDJTPipe  
{
public:
	// Constructors
	CDJTPipe(int nSize);
	~CDJTPipe();

	// Attributes
	const int m_nSize;	// Max number of pointers in queue.

	// Operations
	BOOL Read(LPVOID* pData);
	BOOL Write(LPVOID* pDtata);
	BOOL Reset();
	inline BOOL IsEmpty();

	// Implementation
	CPtrList m_List;
	CCriticalSection m_WriteLock;
	CCriticalSection m_ReadLock;
	CSemaphore* m_pListCount;
};


// Inline functions
//
inline BOOL CDJTPipe::IsEmpty() { return m_List.IsEmpty(); }


#endif // !defined(AFX_DJTPIPE_H__F8492F51_E641_11D0_8C27_08002BE6493E__INCLUDED_)
