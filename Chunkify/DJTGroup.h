// DJTGroup.h: interface for the CDJTGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTGROUP_H__C5E04AE8_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJTGROUP_H__C5E04AE8_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxmt.h>
#include <afxtempl.h>
#include "DJTDocument.h"
#include "DJGenre.h"

// Definitions
//
const UINT DJ_GROUP_MAX_DOCUMENTS = 32767; // Max number of documents allowed in a group.


// CDJTGroup class definition
//
class CDJTGroup : public CObject  
{
	DECLARE_DYNAMIC( CDJTGroup );
public:
	// Constructors
	CDJTGroup(const DJGenre* pGenre);
	virtual ~CDJTGroup();

	// Attributes
	const DJGenre* m_pGenre;

	// Operations
	inline const DJGenre* Genre();
	inline int NumOfDocuments();
	int GetDocPtrs(DocumentPtrList* pDocPtrs, int nDocs=0);
	BOOL AddDocument(CDJTDocument* pDoc);
	void RemoveAll();

	// Implementation
	CCriticalSection m_Lock;
	DocumentPtrList m_DocumentPtrs;

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};


// Inline functions
//
inline const DJGenre* CDJTGroup::Genre() {	return m_pGenre; }

inline int CDJTGroup::NumOfDocuments()
{
	CSingleLock tmpLock( &m_Lock, TRUE );
	return m_DocumentPtrs.GetCount();
}


// Type definitions
//
typedef CTypedPtrArray<CObArray, CDJTGroup*> GroupPtrArray;
typedef CTypedPtrList<CObList, CDJTGroup*> GroupPtrList;

#endif // !defined(AFX_DJTGROUP_H__C5E04AE8_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
