// DJGenre.cpp: deklaration and initialization of the global DJGenre struct.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJGenre.h"

/*#ifdef _TestChunkify
	#include "res\DropJawV2.rc2"
#else
	#include "..DropJawV2\res\DropJawV2.rc2"
#endif //_TestChunkify
*/

#include "..\res\DropJawV2.rc2"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Genre palette initialization
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// g_GenrePalette
// The order is important to the UI.

const DJGenre g_GenrePalette[DJ_GENRE_PALETTE_SIZE] =
{	
	{ 0,  TRUE,	 "Informal, Private",	  0, DJ_INDEX_GREEN,	DJ_GREEN_COLOR },
	{ 1,  TRUE,	 "Public, Commercial",	  0, DJ_INDEX_YELLOW,	DJ_YELLOW_COLOR },
	{ 2,  TRUE,	 "Interactive pages",	  0, DJ_INDEX_BROWN,	DJ_BROWN_COLOR },
	{ 3,  TRUE,	 "Journalistic material", 0, DJ_INDEX_PINK,		DJ_PINK_COLOR },
	{ 4,  TRUE,	 "Reports",				  0, DJ_INDEX_DBLUE,	DJ_DBLUE_COLOR },
	{ 5,  TRUE,	 "Other running text",	  0, DJ_INDEX_LBLUE,	DJ_LBLUE_COLOR },
	{ 6,  TRUE,	 "FAQs",				  0, DJ_INDEX_SEAGREEN, DJ_SEAGREEN_COLOR },
	{ 7,  TRUE,	 "Listings, Tables",	  0, DJ_INDEX_PEAGREEN, DJ_PEAGREEN_COLOR },
	{ 8,  TRUE,	 "Link Collections",	  0, DJ_INDEX_ORANGE,	DJ_ORANGE_COLOR },
	{ 9,  TRUE,	 "Discussions",			  0, DJ_INDEX_RED,		DJ_RED_COLOR },
	{ 10, FALSE, "Error Messages",		  0, DJ_INDEX_RED,		RGB(255,255,255) },
	{ 11, FALSE, "Unclassified",		  0, DJ_INDEX_RED,		RGB(255,255,255) }
};
