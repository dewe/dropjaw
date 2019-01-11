// DJMatrix.h: interface for the CDJMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJMATRIX_H__35CD7052_2829_11D1_91E4_0060974B4CFF__INCLUDED_)
#define AFX_DJMATRIX_H__35CD7052_2829_11D1_91E4_0060974B4CFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDJMatrix  
{
public:
	// Construction
	CDJMatrix(int nSize);
	virtual ~CDJMatrix();

	// Operations
	double SetAt(int row, int col, double val) { return m_ppArray[row][col] = val; };
	double GetAt(int row, int col) { return m_ppArray[row][col]; };
	void DeleteRowCol( int row, int col );

	// Implementation
	double **m_ppArray;
	int m_nMaxSize;

};


#endif // !defined(AFX_DJMATRIX_H__35CD7052_2829_11D1_91E4_0060974B4CFF__INCLUDED_)
