// DJInternetSession.cpp: implementation of the CDJInternetSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJInternetSession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJInternetSession::CDJInternetSession( LPCTSTR szID, DWORD dwCTO, DWORD dwCR, 
									    DWORD dwCB, DWORD dwDSTO, DWORD dwDRTO )
	: CInternetSession( 
			szID,							// Application id-string.
			1,								// Standard context.
			INTERNET_OPEN_TYPE_PRECONFIG,	// Type of internet access.
			NULL,
			NULL, 
			NULL							// No optional flags.
			) // base class initialized.
{
	// Set the parameters for this session. 
	VERIFY( SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, dwCTO) );
	VERIFY( SetOption( INTERNET_OPTION_CONNECT_TIMEOUT, dwCTO ));
	VERIFY( SetOption( INTERNET_OPTION_CONNECT_RETRIES, dwCR ));
	VERIFY( SetOption( INTERNET_OPTION_CONNECT_BACKOFF, dwCB ));	
	VERIFY( SetOption( INTERNET_OPTION_DATA_SEND_TIMEOUT, dwDSTO ));
	VERIFY( SetOption( INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, dwDRTO ));
}

CDJInternetSession::~CDJInternetSession()
{

}


//////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////

void CDJInternetSession::OnStatusCallback(
							DWORD dwContext,
							DWORD dwInternetStatus, 
							LPVOID lpvStatusInformation,  
							DWORD dwStatusInformationLength)
{
//	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch( dwInternetStatus )
	{
	case INTERNET_STATUS_RESOLVING_NAME: 
		TRACE( "INTERNET_STATUS_RESOLVING_NAME\n" );
		break;
	case INTERNET_STATUS_NAME_RESOLVED: 
		TRACE( "INTERNET_STATUS_NAME_RESOLVED\n" );
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER: 
		TRACE( "INTERNET_STATUS_CONNECTING_TO_SERVER\n" );
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER: 
		TRACE( "INTERNET_STATUS_CONNECTED_TO_SERVER\n" );
		break;
	case INTERNET_STATUS_SENDING_REQUEST:
		TRACE( "INTERNET_STATUS_SENDING_REQUEST\n" );
		break;
	case INTERNET_STATUS_REQUEST_SENT:
		TRACE( "INTERNET_STATUS_REQUEST_SENT\n" );
		break;
	case INTERNET_STATUS_RECEIVING_RESPONSE:
//		TRACE( "INTERNET_STATUS_RECEIVING_RESPONSE\n" );
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
//		TRACE( "INTERNET_STATUS_RESPONSE_RECEIVED\n" );
		break;
	case INTERNET_STATUS_CLOSING_CONNECTION:
//		TRACE( "INTERNET_STATUS_CLOSING_CONNECTION\n" );
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED:
//		TRACE( "INTERNET_STATUS_CONNECTION_CLOSED\n" );
		break;
	case INTERNET_STATUS_HANDLE_CREATED:
		TRACE( "INTERNET_STATUS_HANDLE_CREATED\n" );
		break;
	case INTERNET_STATUS_HANDLE_CLOSING:
//		TRACE( "INTERNET_STATUS_HANDLE_CLOSING\n" );
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		TRACE( "INTERNET_STATUS_REQUEST_COMPLETE\n" );
		break;
	default:
		TRACE( "*** Unknown internet status...\n" );
	}
}

