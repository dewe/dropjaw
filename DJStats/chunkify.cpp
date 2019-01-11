// Chunkify.cpp
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////

#include "Chunkify.h"
#include <math.h>

CString g_strHomeDir = "";
CString g_strBiberFeateursFileName = "data\\featrs.djb";
CString g_strBiberNormalsFileName = "data\\normals.djb";
CString g_strDocStoplistFileName = "data\\stoplist.djd";

//////////////////////////////////////////////////////////////////////
// IsPrime:
// Test for prime. Brute force.
//
BOOL IsPrime( int n ) 
{
	int d;
	int maxd = (int)sqrt(n);
	
	if ((n % 2 == 0) && (n != 2))
		return FALSE;
	
	for( d=3; d <= maxd; d+=2 )
	{
		if (n % d == 0)
			return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// HashTableSize:
//
int HashTableSize(int nExpectedSize)
{
	int	n = (int) ceil( 1.2 * nExpectedSize );

	while( !IsPrime(n) )
		n++;

	return n;
}


//////////////////////////////////////////////////////////////////////
// RandomFloat:
//
float RandomFloat()
{
	static BOOL bSeeded = FALSE;

	if( !bSeeded )
	{
		srand( (unsigned)time( NULL ) );
		bSeeded = TRUE;
	}

	return (float) rand()/RAND_MAX;
}




