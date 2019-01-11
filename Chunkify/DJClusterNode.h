// DJClusterNode.h: interface for the CDJClusterNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJCLUSTERNODE_H__B7FDB7C2_0977_11D1_91D5_0060974B4CFF__INCLUDED_)
#define AFX_DJCLUSTERNODE_H__B7FDB7C2_0977_11D1_91D5_0060974B4CFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>


// Structures
//
struct TermIndexAssoc 
	{
		CString* pstrTerm;
		double fValue;
	};



// Type definitions
//
typedef CDJTDocument Centroid;


// CDJClusterNode class definition
//
class CDJClusterNode : public CObject  
{
	DECLARE_DYNAMIC( CDJClusterNode );
public:
	// Constructors
	CDJClusterNode();
	virtual ~CDJClusterNode();

	// Attributes
	int m_nDocCount;
	Centroid m_Centroid;

	// Operations
	virtual CDJTGroup* AddDocument(CDJTDocument* pDoc);
	Centroid* Centroid() { return &m_Centroid; }

	// Implementation
	CList < TermIndexAssoc, TermIndexAssoc& >m_KeywordList;
	void AddToCentroid(CDJTDocument* pDoc);
	DocumentPtrList m_NodeDocPtrs;
};

#endif // !defined(AFX_DJCLUSTERNODE_H__B7FDB7C2_0977_11D1_91D5_0060974B4CFF__INCLUDED_)
