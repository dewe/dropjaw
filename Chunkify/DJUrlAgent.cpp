// DJUrlAgent.cpp: implementation of the CDJUrlAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <afxinet.h>
#include "Chunkify.h"
#include "DJUrlAgent.h"
#include "DJAltaVistaQuery.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Access dynamic run-time class information
IMPLEMENT_DYNAMIC( CDJUrlAgent, CDJAgent);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJUrlAgent::CDJUrlAgent()
{
	// Set the message sent when agent is dead.
	m_ExitMessage = DJ_WM_URLAGENT_DEAD;
}

CDJUrlAgent::~CDJUrlAgent()
{
	// Make sure the threads are dead before removing the list.
	KillThreads();

	// Delete all objects pointed to by m_UrlPtrs.
	CSingleLock tmpLock( &m_AgentLock, TRUE );
	POSITION pos = m_UrlPtrs.GetHeadPosition();
	while( pos != NULL )
	{
		delete m_UrlPtrs.GetNext( pos );
	}
	m_UrlPtrs.RemoveAll();
	ASSERT( m_UrlPtrs.IsEmpty() );
}


//////////////////////////////////////////////////////////////////////
// Threads
//////////////////////////////////////////////////////////////////////

UINT CDJUrlAgent::UrlAgentThread(LPVOID pParam)
{
	THREADPARMS* pThreadParms;
	CDJTPipe* pPipe;
	CDJUrlAgent* pOwner;
	CDJUrl* pUrl;
	UrlPtrList* pUrlPtrList;
	CString* pstrQuery;
	int nMaxUrls, nExpect;

	CDJInternetSession* pInetSession;
	CStdioFile* pInetFile = NULL;
	CDJAltaVistaQuery AVQuery;
	CString strUrl;				// String from AltaVistaQuery.
	int nCollected = 0;			// Number of collected URLs in ParseAltaVista.
	POSITION listPos;			// Last known position in list.

	// Initialize thread communication.
	pThreadParms = (THREADPARMS*) pParam;
	ASSERT( pThreadParms != NULL );

	pPipe = pThreadParms->pPipe;
	ASSERT( pPipe != NULL );

	pOwner = (CDJUrlAgent*) pThreadParms->pOwner;
	ASSERT( pOwner != NULL );
	ASSERT( pOwner->IsKindOf(RUNTIME_CLASS(CDJUrlAgent)) );
	
	pUrlPtrList = &pOwner->m_UrlPtrs;
	ASSERT( pUrlPtrList != NULL );

	pstrQuery = &pOwner->m_strQuery;
	ASSERT( pstrQuery != NULL );

	CSingleLock tmpLock( &pOwner->m_AgentLock, FALSE );
	nMaxUrls = pOwner->m_nMaxUrls;

	// Set up the query
	AVQuery.SetQueryString( pstrQuery->GetBuffer(0) );

	// Tell everyone about what to expect.
	nExpect = min( AVQuery.MaxUrls(), nMaxUrls );
	tmpLock.Lock();
	pOwner->m_nExpectedSize = nExpect;
	tmpLock.Unlock();
	VERIFY( PostMessage(g_hEasify, 
						DJ_WM_URLAGENT_URLS, 
						(WPARAM)nExpect,
						NULL
						));


	// Set up the Internet session.
	try
	{
		pInetSession = new CDJInternetSession(
							DJ_URLAGENT_ID_STRING,
							DJ_URLAGENT_CONNECT_TIMEOUT, 
							DJ_URLAGENT_CONNECT_RETRIES, 
							DJ_URLAGENT_CONNECT_BACKOFF, 
							DJ_URLAGENT_DATA_SEND_TIMEOUT, 
							DJ_URLAGENT_DATA_RECEIVE_TIMEOUT
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
		throw;				// Do not proceed without a session.
	}

	// Collect URLs 'en masse'.
	while( AVQuery.GetURL(strUrl) && pThreadParms->bContinue )
	{
		::Sleep(0);		// Nice Thread

		try
		{
			//Open Http-connection. <CInternetException>
			pInetFile = pInetSession->OpenURL( strUrl, 1, INTERNET_FLAG_TRANSFER_ASCII | 
														  INTERNET_FLAG_EXISTING_CONNECT ); 
			ASSERT( pInetFile != NULL );
			// Get all URLs, fill the list. <CInternetException>
			listPos = ParseAltaVista(
								pInetFile, 
								AVQuery.m_nStq,
								nMaxUrls,
								pUrlPtrList, 
								&pOwner->m_AgentLock);

			// Close internet file. <CInternetException>
			pInetFile->Close();
			delete pInetFile;
			pInetFile = NULL;

			if( listPos == NULL )
			{
				// No more URLs collected. Quit.
				pThreadParms->bContinue = FALSE;
				break;
			}
			else
			{
				// Write all URLs to the pipe.
				while( listPos != NULL )
				{
					pUrl = pUrlPtrList->GetNext( listPos );
					while( !pPipe->Write((LPVOID*)&pUrl) && pThreadParms->bContinue)
						::Sleep(DJ_AGENT_SLEEP);
				}
				pUrl = NULL;	// Let go of the pointer.
			}
		}
		catch( CInternetException* pEx )
		{
			#ifdef _DEBUG
			char szErr[1024];
			pEx->GetErrorMessage(szErr, 1024);
			TRACE( "CInternetException in UrlAgent: %s\n", szErr );

			pEx->ReportError();		
			#endif

			// Clean up before proceeding.
			pEx->Delete();
			if( pInetFile != NULL )
			{
				pInetFile->Abort();
				delete pInetFile;
				pInetFile = NULL;		
			}
		}

	}

	// Update everyone on the expactations...
	nExpect = min( AVQuery.MaxUrls(), nMaxUrls );
	tmpLock.Lock();
	pOwner->m_nExpectedSize = nExpect;
	tmpLock.Unlock();
	VERIFY( PostMessage(g_hEasify, 
						DJ_WM_URLAGENT_URLS, 
						(WPARAM)nExpect, 
						NULL
						));

	pInetSession->Close();
	delete pInetSession;

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ConnectAgent:
//
void CDJUrlAgent::ConnectAgent(CDJAgent *pSource)
{
	// Should never be called, there is no Agent 
	// before UrlAgent to make a connection to.
	ASSERT( FALSE );
}


//////////////////////////////////////////////////////////////////////
// NextUrl:
//
BOOL CDJUrlAgent::NextUrl(CDJUrl** pUrlPtr)
{
	CDJUrl* ReturnPtr;

	if( !NextDataPtr((LPVOID*)&ReturnPtr) )	// Get the pointer...
		return FALSE;

	if( ReturnPtr != NULL )
		ASSERT( ReturnPtr->IsKindOf(RUNTIME_CLASS(CDJUrl)) );
	*pUrlPtr = (CDJUrl*) ReturnPtr;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// DoSearch:
//
void CDJUrlAgent::DoSearch(LPCTSTR szSearchString, 
						   UINT nMaxUrls /* = DJ_URLAGENT_DEFAULT_URLS */ )
{
	// One thread for each searchengine?
	AFX_THREADPROC ctrlFncts[] = { CDJUrlAgent::UrlAgentThread };

	ASSERT( szSearchString != NULL );
	m_strQuery = szSearchString;

	ASSERT( nMaxUrls > 0 );
	m_nExpectedSize = m_nMaxUrls = nMaxUrls;
	
	LaunchThreads( ctrlFncts, 1 );
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ParseAltaVista: (static)
// Extract information from the AltaVista search result, put new URLs 
// in the provided  list.
//
// In:	pointer to internet file, 
//		pointer to list storage for urls, 
//		pointer to sync obect for the list.
//
// Return: position in list of first collected URL. NULL if no URLs collected.
//
POSITION CDJUrlAgent::ParseAltaVista(CStdioFile* pFile, 
									 int nBaseRank,		// The base rank of this data.
									 int nMaxUrls,		// Max number of Urls collected.
									 UrlPtrList *pUrlList, 
									 CCriticalSection* pCSection)
{
	int nRead;
	BOOL bInData = FALSE;
	char buf[ DJ_URLAGENT_BUF_SIZE+1 ];
	char workBuf[ DJ_URLAGENT_BUF_SIZE+1 ];
	char dataBuf[ 10*DJ_URLAGENT_BUF_SIZE ] = "";	// Initialize empty data buffer.
	char* pDataBegin = NULL;
	char* pDataEnd = NULL;

	// Protect common resources.
	CSingleLock tmpLock( pCSection, FALSE );

	// Read all the interesting data into one data buffer.
	while( nRead = pFile->Read(buf, DJ_URLAGENT_BUF_SIZE) )
	{
		buf[nRead] = '\0';			// Make the buffer a valid C-string.
		strcpy( workBuf, buf );		// Copy buffer.

		if( (pDataBegin = strstr(workBuf, "<dl>")) ||
			(pDataBegin = strstr(workBuf, "<DL>")) )
			bInData = TRUE;
		if( (pDataEnd = strstr(workBuf, "</dl>")) ||
			(pDataEnd = strstr(workBuf, "</DL>")) ) 
			bInData = FALSE;

		if( pDataBegin && !pDataEnd )		// End of buffer contains data.
		{
			strcat( dataBuf, pDataBegin );
		}

		else if( !pDataBegin && pDataEnd )	// Beginning of buffer contains data.
		{
			*pDataEnd = '\0';
			strcat( dataBuf, workBuf );	
		}

		else if( pDataBegin && pDataEnd )	// Middle part of buffer contains data.	
		{
			*pDataEnd = '\0';
			strcat( dataBuf, pDataBegin );	
		}

		else if( bInData )					// All buffer contains data.
		{
			strcat( dataBuf, workBuf );
		}
	}

	// Make something out of the collected data.

	char* pNextData = dataBuf;
	int nRank = nBaseRank;
//	int nRankOffset = 0;	// offset from base rank for data chunk.
	CDJUrl* pUrl;
	POSITION pos;
	POSITION firstPos = NULL;

	while( (pDataBegin = strstr(pNextData, "<dt>")) && nRank < nMaxUrls )
	{
		// Find one chunk of data, or break.
		if( !(pDataEnd = strstr(pDataBegin, "</cite><br>")) )
			break;

		*pDataEnd = '\0';
		pNextData = pDataEnd + 1;

		// Add URL to the list.
//		nRank = nBaseRank + nRankOffset++;
		VERIFY( pUrl = CreateUrlFromAltaVistaBuffer(pDataBegin, 
													strlen(pDataBegin), 
													nRank++) );
		tmpLock.Lock();	
		pos = pUrlList->AddTail( pUrl );
		tmpLock.Unlock();

		// Remember the postition in list of the first URL.
		if( firstPos == NULL )
			firstPos = pos;
	}

	return firstPos;
}


//////////////////////////////////////////////////////////////////////
// CreateUrlFromAltaVistaBuffer
// Collect data from the buffer.
// In:	zero-delimited buffer representing one chunk of information,
//		ready to be parsed.
// Return: a pointer to a Url created with 'new'.
//
CDJUrl* CDJUrlAgent::CreateUrlFromAltaVistaBuffer(LPCTSTR pszBuf, 
												  int nLength, 
												  int nRank)
{
	char* workBuf = new char[nLength+1];
	CDJUrl* pUrl = NULL;

	char emptyString[] = "";

	char* pszUrl = emptyString;
	char* pszTitle = emptyString;
	char* pszExtract = emptyString;
	char* pszSize = emptyString;
	char* pszDate = emptyString;
	char* pszLang = emptyString;
	char* pEnd;


	// Copy to working buffer.
	strcpy( workBuf, pszBuf );

	VERIFY( pszUrl = strstr( workBuf, "<a href=\"" ));
	pszUrl += strlen( "<a href=\"" );
	pEnd = strstr( pszUrl, "\">" );
	if( !pEnd )
		pEnd = pszUrl;
	*pEnd++ = '\0';

	// Find Title.
	VERIFY( pszTitle = strstr( pEnd, "<strong>" ));
	pszTitle += strlen( "<strong>" );
	pEnd = strstr( pszTitle, "</strong>" );
	if( !pEnd )
		pEnd = pszTitle;
	*pEnd++ = '\0';

	// Find Extract.
	VERIFY( pszExtract = strstr( pEnd, "<dd><font size=-1>" ));
	pszExtract += strlen( "<dd><font size=-1>" );
	pEnd = strstr( pszExtract, "</font><br>" );
	if( !pEnd )
		pEnd = pszExtract;
	*pEnd++ = '\0';

	// Find Size.
	VERIFY( pszSize = strstr( pEnd, "</a> - size " ));
	pszSize += strlen( "</a> - size " );
	pEnd = strstr( pszSize, " - " );
	if( !pEnd )
		pEnd = pszSize;
	*pEnd++ = '\0';

	// Find Date.
	VERIFY( pszDate = strstr( pEnd, "- " ));
	pszDate += strlen( "-  " );
	pEnd = strstr( pszDate, " - " ); 
	if( !pEnd )
		pEnd = pszDate;
	*pEnd++ = '\0';

	// Find Language.
	pszLang = strstr( pEnd, "- " );
	pszLang += strlen( "- " );
	pEnd = strstr( pszLang, "</font>" );
	if( !pEnd )
		pEnd = pszLang;
	*pEnd++ = '\0';

	// Create new Url.
	pUrl = new CDJUrl();
	pUrl->m_nRank = nRank;
	pUrl->m_strUrl = pszUrl;
	pUrl->m_strTitle = pszTitle;
	pUrl->m_strExtract = pszExtract;
	pUrl->m_strSize = pszSize;
	pUrl->m_strDate = pszDate;
	pUrl->m_strLang = pszLang;

	delete[] workBuf;	// Clean up.

	return pUrl;
}
