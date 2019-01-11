// DJTTermIndexMap.h: interface for the CDJTermIndexMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJTERMINDEXMAP_H__F7C02A91_0766_11D1_91D5_0060974B4CFF__INCLUDED_)
#define AFX_DJTERMINDEXMAP_H__F7C02A91_0766_11D1_91D5_0060974B4CFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// Includes
//
#include <afxtempl.h>
#include <math.h>



// Type definitions
//
typedef CArray<double, double> TermFreqArray;


// CDJTTermIndexMap class definition
//
class CDJTTermIndexMap 
{
public:
	// Construction
	CDJTTermIndexMap();
	virtual ~CDJTTermIndexMap();

	// Attributes
	CCriticalSection m_TIMLock;

	// Operations
	double Idf( int index );
	int GetCount() const { return m_TheTermMap.GetCount(); }
	BOOL AddTerm( LPCTSTR pszKey, USHORT* pusRetIndex );
	void SetIdfBaseSize(int nBase);
	SHORT UpdateIdf(USHORT usIndex, SHORT sValToAdd);

	// Implementation
	int m_nIdfBaseSize;
	UINT m_nMaxIndex;
	void InitStopListMap(LPCTSTR pszFileName);

	CArray< SHORT, SHORT > m_TheIdfArray;
	CMap< CString, LPCTSTR, USHORT, USHORT > m_TheTermMap;
	CMap< CString, LPCTSTR, BOOL, BOOL > m_TheStopListMap;

#ifdef _DEBUG
	void DumpIdfValues(CDumpContext& dc);
#endif
};


// Inline functions

inline void CDJTTermIndexMap::SetIdfBaseSize(int nBase) 
{
	CSingleLock tmpLock( &m_TIMLock, TRUE );
	m_nIdfBaseSize = nBase; 
}


// Global accessors.
//
CDJTTermIndexMap *TermIndexMap();
void SetTermIndexMap(CDJTTermIndexMap *pTermIndexMap);


#endif // !defined(AFX_DJTERMINDEXMAP_H__F7C02A91_0766_11D1_91D5_0060974B4CFF__INCLUDED_)

