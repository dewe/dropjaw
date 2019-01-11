// DJDocAgent.cpp: implementation of the CDJDocAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJDocAgent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJDocAgent, CDJAgent);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJDocAgent::CDJDocAgent()
{

}

CDJDocAgent::~CDJDocAgent()
{

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// NextDocument:
//
BOOL CDJDocAgent::NextDocument(CDJTDocument** pDocPtr)
{
	CDJTDocument* ReturnPtr;

	if( !NextDataPtr((LPVOID*)&ReturnPtr) )	// Get the pointer...
		return FALSE;

	#ifdef _DEBUG
	if( ReturnPtr != NULL )
		ASSERT( ReturnPtr->IsKindOf(RUNTIME_CLASS(CDJTDocument)) );
	#endif

	*pDocPtr = (CDJTDocument*) ReturnPtr;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// NextDocument:
//
int CDJDocAgent::ExpectedSize()
{
	ASSERT( m_ThreadParms.pSource != NULL );
	return m_ThreadParms.pSource->ExpectedSize();
}
