// DJInternetSession.h: interface for the CDJInternetSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJINTERNETSESSION_H__1E75E781_F3E0_11D0_8C2C_08002BE6493E__INCLUDED_)
#define AFX_DJINTERNETSESSION_H__1E75E781_F3E0_11D0_8C2C_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxinet.h>

// CDJInternetSession class definition
//
class CDJInternetSession : public CInternetSession  
{
public:
	// Constructors
	CDJInternetSession( LPCTSTR szID,	// Application id-string.
						DWORD	dwCTO,	// Connect timeout (ms).
						DWORD	dwCR,	// Number of connect retries.
						DWORD	dwCB,	// Wait between retries (ms).
						DWORD	dwDSTO,	// Data send timeout (ms).
						DWORD	dwDRTO	// Data receive timeout (ms).
						);
	virtual ~CDJInternetSession();
	
	//Overrides
	void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, 
			LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
};

#endif // !defined(AFX_DJINTERNTSESSION_H__1E75E781_F3E0_11D0_8C2C_08002BE6493E__INCLUDED_)
