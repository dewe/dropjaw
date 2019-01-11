// DJStatAgent.h: interface for the CDJStatAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJSTATAGENT_H__E9C2CB81_ED8D_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJSTATAGENT_H__E9C2CB81_ED8D_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "DJDocAgent.h"
#include "DJTDocument.h"
#include "DJTempFile.h"
#include "DJUrl.h"
#include "DJGStatistics.h"	// Added by ClassView
#include "DJGBiber.h"	// Added by ClassView
#include "DJTTermIndexMap.h"	// Added by ClassView


// CDJStatAgent class definition
//
class CDJStatAgent : public CDJDocAgent  
{
	DECLARE_DYNAMIC( CDJStatAgent );
public:
	// Constructors
	CDJStatAgent(BOOL bLeaveFiles = FALSE);
	virtual ~CDJStatAgent();

	// Attributes
	BOOL m_bLeaveFileOnDisk;
	DocumentPtrList m_DocumentPtrList;
	CDJGStatistics m_GStatObject;
	CDJGBiber m_BiberScoreObject;
	CDJTTermIndexMap m_TermIndexMap;

	// Operations
	static void InitTermFreqArray( CDJTDocument* pDoc,
								   CDJTTermIndexMap *pTIM);
	static void PrepareDocument(CDJStatAgent *pOwner, 
								CDJTempFile* pFile, 
								CDJTDocument* pDoc);
	void ConnectAgent(CDJAgent* pSource);

	// Implementation
	static UINT StatAgentThread(LPVOID pParam);
};

#endif // !defined(AFX_DJSTATAGENT_H__E9C2CB81_ED8D_11D0_8C2B_08002BE6493E__INCLUDED_)
