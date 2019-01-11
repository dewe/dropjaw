// DJTGroup.cpp: implementation of the CDJTGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJTGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJTGroup, CObject );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJTGroup::CDJTGroup(const DJGenre* pGenre) 
	: m_pGenre( pGenre )	// initialize const member.
{

}

CDJTGroup::~CDJTGroup()
{

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AddDocument:
// Add a document pointer to the group, and post a message to
// the pWnd window. If max num of docs is exceeded, return FALSE.

BOOL CDJTGroup::AddDocument(CDJTDocument* pDoc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( pDoc != NULL );

	if( NumOfDocuments() > DJ_GROUP_MAX_DOCUMENTS )
		return FALSE;

	try
	{
		CSingleLock tmpLock( &m_Lock, TRUE );
		// Add document to the end of array. Grow array if necessary.
		m_DocumentPtrs.AddTail( pDoc );
		 
	}
	catch( CException* e )
	{
		e->Delete();		// Clean up, exit function with error.
		return FALSE;
	}

	// Notify the window --> added one document.
	VERIFY( PostMessage(g_hEasify, DJ_WM_GROUPUPDATE, (WPARAM)this, (LPARAM)1) );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GetDocPtrs:
// Add the 'nDocs' first document pointers to the tail of the list 
// pDocPtrs.
// 
// If the group contains equal or less than nDoc documents, all 
// documents will be in pDocPtrs. Function returns the number
// of documents delivered.

int CDJTGroup::GetDocPtrs(DocumentPtrList* pDocPtrs, int nDocs /* = 0 */)
{
	ASSERT( pDocPtrs != NULL );
	int nMax = nDocs;
	CSingleLock tmpLock( &m_Lock, TRUE );

	if( nDocs == 0 )
		nMax = m_DocumentPtrs.GetCount();

	POSITION pos = m_DocumentPtrs.GetHeadPosition();
	for( int i=0; (i<nMax)&&(pos!=NULL); i++ )
	{
		pDocPtrs->AddTail( m_DocumentPtrs.GetNext(pos) );
	}
	return i;	// Number of pointers copied.
}


//////////////////////////////////////////////////////////////////////
// Dump:
//
#ifdef _DEBUG
void CDJTGroup::Dump(CDumpContext& dc) const
{
	// Call base class function first.
	CObject::Dump( dc );

	// Dump info about document.
	dc << "Genre:\t" << m_pGenre->szLabel << "\n"; 
	dc << m_DocumentPtrs.GetCount() << " document(s)\n";
}
#endif


//////////////////////////////////////////////////////////////////////
// RemoveAll:
// Remove all dokument pointers and send message to Easify.
//
void CDJTGroup::RemoveAll()
{
	int nSize;
	CSingleLock tmpLock( &m_Lock, TRUE );

	nSize = m_DocumentPtrs.GetCount();
	if( nSize != NULL )
	{
		m_DocumentPtrs.RemoveAll();
		VERIFY( PostMessage(g_hEasify, 
							DJ_WM_GROUPUPDATE, 
							(WPARAM)this, 
							(LPARAM) -nSize
							));
	}
}
