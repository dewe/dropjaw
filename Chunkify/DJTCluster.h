// DJTCluster.h: interface for the CDJTCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTCLUSTER_H__C5E04AE4_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJTCLUSTER_H__C5E04AE4_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxmt.h>
#include <afxtempl.h>
#include "DJGenre.h"
#include "DJTGroup.h"
#include "DJTDocument.h"
#include "DJClusterNode.h"
#include "DJTTermIndexMap.h"	// Added by ClassView


// CDJTCluster class definition
//
class CDJTCluster : public CDJClusterNode  
{
	DECLARE_DYNAMIC( CDJTCluster );
public:
	// Constructors
	CDJTCluster( CDJTTermIndexMap *pTIM );
	virtual ~CDJTCluster();

	// Attributes
	const int m_nGroups;
	GroupPtrArray m_GroupPtrs;
	CDJTTermIndexMap *m_pTermIndexMap;

	// Operations
	int ClusterKeywords( CStringArray& Strings );
	inline GroupPtrArray* Groups();
	CDJTGroup* AddDocument(CDJTDocument* pDoc);
	BOOL ContainGroup( CDJTGroup* pGroup );

	// Implementation
	CCriticalSection m_Lock;
	void GenrePolicy(GenrePtrList* pEmptyGenreList, CDJTDocument* pDoc);
	BOOL IsValidKeyword(LPCTSTR pszKeyword);

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};

// Inline functions
//
inline GroupPtrArray* CDJTCluster::Groups() { return &m_GroupPtrs; }


// Type definitions
//
typedef	CTypedPtrArray<CObArray, CDJTCluster*> ClusterPtrArray;


#endif // !defined(AFX_DJTCLUSTER_H__C5E04AE4_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
