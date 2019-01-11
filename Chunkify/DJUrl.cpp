// DJUrl.cpp: implementation of the CDJUrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJUrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Access dynamic run-time class information
//
IMPLEMENT_DYNAMIC( CDJUrl, CObject);


//////////////////////////////////////////////////////////////////////
// Debugging
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CDJUrl::Dump(CDumpContext & dc)
{
	CObject::Dump( dc );
	dc << "URL:  " << m_strUrl << "\nRank: " << m_nRank << "\n";
}
#endif
