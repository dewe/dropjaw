// DJTTermIndexMap.cpp: implementation of the CDJTermIndexMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxtempl.h>
#include "Chunkify.h"
#include "DJTTermIndexMap.h"
#include "DJTDocument.h"
#include "DJUrlAgent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Globals
//
CDJTTermIndexMap *g_pTermIndexMap;


//////////////////////////////////////////////////////////////////////
// Global Accessors
//////////////////////////////////////////////////////////////////////

CDJTTermIndexMap *TermIndexMap() { return g_pTermIndexMap; };

void SetTermIndexMap(CDJTTermIndexMap *pTermIndexMap) 
{ 
	ASSERT(pTermIndexMap != NULL);
	g_pTermIndexMap = pTermIndexMap;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJTTermIndexMap::CDJTTermIndexMap()
{
	m_nMaxIndex = 0;
	m_nIdfBaseSize = DJ_URLAGENT_DEFAULT_URLS;
	InitStopListMap( g_strHomeDir + g_strDocStoplistFileName );
	m_TheTermMap.InitHashTable( HashTableSize(DJ_DOCUMENT_TF_ARRAY_SIZE) );
	m_TheIdfArray.SetSize( DJ_DOCUMENT_TF_ARRAY_SIZE, 10000 );
}

CDJTTermIndexMap::~CDJTTermIndexMap()
{
	m_TheTermMap.RemoveAll();
	m_TheStopListMap.RemoveAll();
	m_TheIdfArray.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AddTerm:
// Add the term in pszKey to the common term map. If the term is 
// already in the map no term will be added (well, it's a map, 
// after all!), and the Idf-array will be updated. If term is 
// a valid term (i.e. not in stop-list) the function return TRUE. 
// If the term is in the stoplist, the term will not be added to 
// the map, and the function return FALSE.
//
// When function exits, the index for the 'pszKey' is stored 
// at the address of pusRetIndex.
//
// This function shall only be called once per term and document,
// beacause it updates the Idf-array, which shall only be done
// once per document for every used term.
//
BOOL CDJTTermIndexMap::AddTerm(LPCTSTR pszKey, USHORT* pusRetIndex)
{
	USHORT usIndex = 0;
	BOOL bForFutureUse;	// Dummy variable, maybe useful in future.
	CSingleLock tmpLock( &m_TIMLock, FALSE );

	// Refuse to add empty strings.
	if( strlen(pszKey) == 0 )
	{
		return FALSE;
	}

	// Check if pszKey is in the stop list. If so return FALSE.
	if( m_TheStopListMap.Lookup(pszKey, bForFutureUse) )
		return FALSE;

	// Lookup possible, previously stored, index. Get/Set the index.
	tmpLock.Lock();
	if( !m_TheTermMap.Lookup(pszKey, usIndex) )
	{
		usIndex = m_nMaxIndex++;
		m_TheTermMap.SetAt( pszKey, usIndex );
	}

	// Check for limit of USHORT.
	if( m_nMaxIndex >= USHRT_MAX )
	{
		AfxMessageBox( "Too many words in term list! Throwing Exception...",
						MB_ICONSTOP );
		AfxThrowUserException();	
	}

	// Update idf-array. Store index and exit.
	UpdateIdf( usIndex, 1 );
	*pusRetIndex = usIndex;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// InitStopListMap:
// Init the stoplist, add words from the file to the stoplistmap.
//
void CDJTTermIndexMap::InitStopListMap( LPCTSTR pszFileName )
{
	try
	{
		int nMapSize;
		CList< CString, CString& > TermList;
		POSITION pos;
		CString strTerm;
		CStdioFile file( pszFileName, 
			CFile::modeRead /*|| CFile::typeText || CFile::shareDenyNone*/ );

		while( file.ReadString(strTerm) )
			TermList.AddTail( strTerm );

		nMapSize = m_TheStopListMap.GetCount() + TermList.GetCount();
		nMapSize = HashTableSize(nMapSize);
		m_TheStopListMap.InitHashTable( nMapSize );

		pos = TermList.GetHeadPosition();
		while( pos != NULL )
			m_TheStopListMap.SetAt( TermList.GetNext(pos), TRUE );

		file.Close();
	}
	catch( CFileException *pEx)
	{
		pEx->ReportError();
		throw;
	}
}


//////////////////////////////////////////////////////////////////////
// UpdateIdf:
//
SHORT CDJTTermIndexMap::UpdateIdf(USHORT usIndex, SHORT sValToAdd)
{
	if( usIndex > m_TheIdfArray.GetUpperBound() )
		m_TheIdfArray.SetSize( m_nMaxIndex );

	return m_TheIdfArray[usIndex] += sValToAdd;
}


//////////////////////////////////////////////////////////////////////
// Idf:
// Returns idf = log( N / n ), where N is provided by 'm_nIdfBaseSize'
// and n is read from TheIdfArray at 'index'. 
// 'm_nIdfBaseSize' may be set before calling this function.
//
double CDJTTermIndexMap::Idf(int index)
{
	CSingleLock tmpLock( &m_TIMLock, TRUE );

	ASSERT( index >= 0 && index < m_TheIdfArray.GetSize() );	
	ASSERT( m_TheIdfArray[index] > 0 );
	ASSERT( m_nIdfBaseSize >= m_TheIdfArray[index] );

	#ifdef _DEBUG
		double idf = log( (double)m_nIdfBaseSize / m_TheIdfArray[index] );
		ASSERT( idf > 0 );
		ASSERT( !_isnan(idf) && _finite(idf) );
	#endif

	return (double) log( (double)m_nIdfBaseSize / m_TheIdfArray[index] );
}


//////////////////////////////////////////////////////////////////////
// DumpIdfValues:
// 
#ifdef _DEBUG
void CDJTTermIndexMap::DumpIdfValues(CDumpContext& dc)
{
	CString strTmp;
	USHORT usIndex;
	CSingleLock tmpLock1( &m_TIMLock, TRUE );

	dc << "\nIdf Values:\n";
	POSITION mapPos = m_TheTermMap.GetStartPosition();
	while( mapPos != NULL )
	{
		m_TheTermMap.GetNextAssoc( mapPos, strTmp, usIndex );
		dc << "\t" << Idf(usIndex)<< "\t" << strTmp << "\n";
	}
}
#endif
