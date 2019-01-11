// DJGenreProb.h: interface for the CDJGenreProb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJGENREPROB_H__F380D653_F1E1_11D0_8C2C_08002BE6493E__INCLUDED_)
#define AFX_DJGENREPROB_H__F380D653_F1E1_11D0_8C2C_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "DJGenre.h"

//////////////////////////////////////////////////////////////////////
// CDJGenreProb class definition

class CDJGenreProb  
{
public:
	// Constructors
	CDJGenreProb( const DJGenre* pGenre, double fProbability );
	virtual ~CDJGenreProb();

	// Attributes
	const DJGenre* m_pGenre;
	double m_fProbability;

	// Operations
	const DJGenre* Genre() { return m_pGenre; }
	double Probability() { return m_fProbability; }
};


//////////////////////////////////////////////////////////////////////
// Type definitions

typedef CTypedPtrArray<CPtrArray, CDJGenreProb*> GenreProbPtrArray;

#endif // !defined(AFX_DJGENREPROB_H__F380D653_F1E1_11D0_8C2C_08002BE6493E__INCLUDED_)
