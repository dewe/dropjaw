// DJDocAgent.h: interface for the CDJDocAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJDOCAGENT_H__F8D725B4_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
#define AFX_DJDOCAGENT_H__F8D725B4_ED32_11D0_8C2A_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DJTDocument.h"
#include "DJAgent.h"

class CDJDocAgent : public CDJAgent  
{
	DECLARE_DYNAMIC( CDJDocAgent );
public:
	// Constructors
	CDJDocAgent();
	virtual ~CDJDocAgent();

	// Attributes

	// Operations
	BOOL NextDocument(CDJTDocument** pDocPtr);
	int ExpectedSize();
	
	// Implementation
};

#endif // !defined(AFX_DJDOCAGENT_H__F8D725B4_ED32_11D0_8C2A_08002BE6493E__INCLUDED_)
