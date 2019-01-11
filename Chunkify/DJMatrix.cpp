// DJMatrix.cpp: implementation of the CDJMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJMatrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJMatrix::CDJMatrix( int nSize )
{
	m_nMaxSize = nSize;
	m_ppArray = new double*[m_nMaxSize];

	for( int i=0; i<m_nMaxSize; i++ )
		m_ppArray[i] = new double[m_nMaxSize];
}

CDJMatrix::~CDJMatrix()
{

	for( int i=0; i<m_nMaxSize; i++ )
		delete[] m_ppArray[i];
	
	delete[] m_ppArray;

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Delete all values in row, col. Contract matrix, i.e. all rows 
// and cols with greater index move down one step.
//
void CDJMatrix::DeleteRowCol(int row, int col)
{
	ASSERT( row >= 0 && row < m_nMaxSize );
	ASSERT( col >= 0 && col < m_nMaxSize );

	// Delete coloumn.
	for( int i=0; i<m_nMaxSize; i++ )
		memcpy( &m_ppArray[i][col], &m_ppArray[i][col+1], (m_nMaxSize-col)*sizeof(double) );

	// Move out row down last in array...
	double *pRow = m_ppArray[row];
	memcpy( &m_ppArray[row], &m_ppArray[row+1], (m_nMaxSize-row)*sizeof(double*) );
	m_ppArray[m_nMaxSize-1] = pRow;
}


