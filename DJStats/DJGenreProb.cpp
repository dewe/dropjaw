// DJGenreProb.cpp: implementation of the CDJGenreProb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJGenreProb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDJGenreProb::CDJGenreProb( const DJGenre* pGenre, double fProbability )
	: m_pGenre(pGenre)
{
	m_fProbability = fProbability;
}

CDJGenreProb::~CDJGenreProb()
{

}

