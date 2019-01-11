// DJTempFile.h: interface for the CDJTempFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTEMPFILE_H__A7A83852_F396_11D0_8C2C_08002BE6493E__INCLUDED_)
#define AFX_DJTEMPFILE_H__A7A83852_F396_11D0_8C2C_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxtempl.h>
#include "DJUrl.h"

// CDJTempFile class definition.
//
class CDJTempFile : public CStdioFile  
{
	DECLARE_DYNAMIC( CDJTempFile );
public:
	// Constructors
	CDJTempFile(CDJUrl* pUrl) {	m_pUrl = pUrl; }
	virtual ~CDJTempFile() { };

	// Attributes
	CDJUrl* m_pUrl;

};

typedef CTypedPtrList<CObList, CDJTempFile*> FilePtrList;

#endif // !defined(AFX_DJTEMPFILE_H__A7A83852_F396_11D0_8C2C_08002BE6493E__INCLUDED_)
