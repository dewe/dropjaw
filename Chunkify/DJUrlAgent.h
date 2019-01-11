// DJUrlAgent.h: interface for the CDJUrlAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJURLAGENT_H__C5E04AE7_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJURLAGENT_H__C5E04AE7_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DJAgent.h"
#include "DJUrl.h"
#include "DJFetchAgent.h"
#include "DJInternetSession.h"


// Definitions
//
const int	DJ_URLAGENT_BUF_SIZE = 4096;	// Size of intermediate buffert.
const int	DJ_URLAGENT_DEFAULT_URLS = 200;		// Default number of URLs from search.

// Internet options.
const char	DJ_URLAGENT_ID_STRING[] = "Mozilla/4.01 [en] (WinNT; I)"; // Make DJ look like Netscape.

// These time-out values has no effect what so ever...
// Bug? Microsoft don't know either...
const int	DJ_URLAGENT_CONNECT_TIMEOUT = 2000;
const int	DJ_URLAGENT_DATA_SEND_TIMEOUT = 3000;
const int	DJ_URLAGENT_DATA_RECEIVE_TIMEOUT = 5000;
const int	DJ_URLAGENT_CONNECT_RETRIES = 2;
const int	DJ_URLAGENT_CONNECT_BACKOFF = 200; 		// Time between retries.


// CDJUrlAgent class definition
//
class CDJUrlAgent : public CDJAgent  
{
	DECLARE_DYNAMIC( CDJUrlAgent );
public:
	// Constructors
	CDJUrlAgent();
	virtual ~CDJUrlAgent();

	// Attributes
	UrlPtrList m_UrlPtrs;
	CString m_strQuery;
	int m_nMaxUrls;

	// Operations
	void DoSearch(LPCTSTR szSearchString, UINT nMaxUrls = DJ_URLAGENT_DEFAULT_URLS);
	BOOL NextUrl(CDJUrl** pUrlPtr);

	// Implementation
	static UINT UrlAgentThread(LPVOID pParam);
	void ConnectAgent(CDJAgent* pSource);	// Should never be used.
	static POSITION ParseAltaVista(
							CStdioFile* pFile,	// File containing data. 
							int nBaseRank,		// The base rank of this data.
							 int nMaxUrls,		// Max number of Urls collected.
							UrlPtrList* pUrlList, 
							CCriticalSection* pCSection);
	static CDJUrl* CreateUrlFromAltaVistaBuffer(LPCTSTR pszBuf,  int nLength, int nRank);
};

#endif // !defined(AFX_DJURLAGENT_H__C5E04AE7_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
