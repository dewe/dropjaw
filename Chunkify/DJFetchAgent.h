// DJFetchAgent.h: interface for the CDJFetchAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJFETCHAGENT_H__C5E04AE6_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJFETCHAGENT_H__C5E04AE6_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxmt.h>
#include <afxinet.h>
#include "DJAgent.h"
#include "DJTempFile.h"
#include "DJUrlAgent.h"
#include "DJUrl.h"
#include "DJInternetSession.h"

// Definitions
//
const char	DJ_TMPFILE_PREFIX[] = "DJ_";	// Three character prefix for temp files.
const int	DJ_FETCHAGENT_BUF_SIZE = 4096;	// Size of intermediate buffert.

// Internet options 
const char	DJ_FETCHAGENT_ID_STRING[] = "DropJaw v0.7"; // May be altered to hide DJ.

// These time-out values has no effect what so ever...
// Bug? Microsoft don't know either...
const int	DJ_FETCHAGENT_CONNECT_TIMEOUT = 2000;
const int	DJ_FETCHAGENT_CONNECT_RETRIES = 2;
const int	DJ_FETCHAGENT_DATA_SEND_TIMEOUT = 3000;
const int	DJ_FETCHAGENT_DATA_RECEIVE_TIMEOUT = 5000;
const int	DJ_FETCHAGENT_CONNECT_BACKOFF = 200; 		// Time between retries.


// CDJFetchAgent class definition
//
class CDJFetchAgent : public CDJAgent  
{
	DECLARE_DYNAMIC( CDJFetchAgent );
public:
	// Constructors
	CDJFetchAgent( int nMaxFileSize,
				   int nThreads,
				   LPCTSTR szTmpFilePath = NULL );
	virtual ~CDJFetchAgent();

	// Attributes
	BOOL m_bDoLog;
	CStdioFile* m_pLogFile;
	CString m_strTmpFilePath;
	FilePtrList m_FilePtrs;
	int m_nFetchedDocs;
	int m_nNonFetchedDocs;
	int m_nMaxBytesRead;
	const int m_nThreads;

	// Operations
	void ConnectAgent(CDJAgent* pSource);
	BOOL NextFile(CDJTempFile** pFilePtr);
	int ExpectedSize();

	// Implementation
	static int TransferData(CStdioFile *pInetFile, 
							CDJTempFile *pTmpFile,
							int nMaxBytes);
	static void WriteLog(LPCTSTR szLog, CStdioFile* pFile);
	static UINT FetchAgentThread(LPVOID pParam);
	static CString OpenTmpFile(CStdioFile* pFile, LPCTSTR pTmpPath);
	static void CleanUp(CStdioFile* pIfile, CStdioFile* pTfile);
	static CStdioFile* NewInternetFile( CDJInternetSession* pSession, 
										LPCTSTR szUrl,
										BOOL bWriteToLog = FALSE,
										CStdioFile* pLogFile = NULL );
};

#endif // !defined(AFX_DJFETCHAGENT_H__C5E04AE6_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
