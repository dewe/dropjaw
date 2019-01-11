// DJGenre.cpp: deklaration and initialization of the global DJGenre struct.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chunkify.h"
#include "DJGenre.h"

#ifdef _TestChunkify
	#include "res\DropJawV2.rc2"
#else
	#include "..\DropJawv2\res\DropJawV2.rc2"
#endif //_TestChunkify

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
	{ 1,  TRUE,	 "Public, Commercial",	  1, DJ_INDEX_YELLOW,	DJ_YELLOW_COLOR },
	{ 2,  TRUE,	 "Interactive pages",	  7, DJ_INDEX_PEAGREEN,	DJ_PEAGREEN_COLOR },
	{ 3,  TRUE,	 "Journalistic material", 3, DJ_INDEX_PINK,		DJ_PINK_COLOR },
	{ 4,  TRUE,	 "Reports",				  4, DJ_INDEX_DBLUE,	DJ_DBLUE_COLOR },
	{ 5,  TRUE,	 "Other running text",	  5, DJ_INDEX_LBLUE,	DJ_LBLUE_COLOR },
	{ 6,  TRUE,	 "FAQs",				  9, DJ_INDEX_RED,		DJ_RED_COLOR },
	{ 7,  TRUE,	 "Listings, Tables",	  6, DJ_INDEX_SEAGREEN, DJ_SEAGREEN_COLOR },
	{ 8,  TRUE,	 "Link Collections",	  8, DJ_INDEX_ORANGE,	DJ_ORANGE_COLOR },
	{ 9,  TRUE,	 "Discussions",			  2, DJ_INDEX_BROWN,	DJ_BROWN_COLOR },
	{ 10, FALSE, "Error Messages",		  10, DJ_INDEX_RED,		DJ_RED_COLOR },
	{ 11, FALSE, "Unclassified",		  11, DJ_INDEX_RED,		DJ_RED_COLOR }
};

/*	{ 0,  TRUE,	 "Informal, Private",	  0, DJ_INDEX_GREEN,	DJ_GREEN_COLOR },
	{ 1,  TRUE,	 "Public, Commercial",	  1, DJ_INDEX_YELLOW,	DJ_YELLOW_COLOR },
	{ 2,  TRUE,	 "Interactive pages",	  7, DJ_INDEX_BROWN,	DJ_BROWN_COLOR },
	{ 3,  TRUE,	 "Journalistic material", 3, DJ_INDEX_PINK,		DJ_PINK_COLOR },
	{ 4,  TRUE,	 "Reports",				  4, DJ_INDEX_DBLUE,	DJ_DBLUE_COLOR },
	{ 5,  TRUE,	 "Other running text",	  5, DJ_INDEX_LBLUE,	DJ_LBLUE_COLOR },
	{ 6,  TRUE,	 "FAQs",				  9, DJ_INDEX_SEAGREEN, DJ_SEAGREEN_COLOR },
	{ 7,  TRUE,	 "Listings, Tables",	  6, DJ_INDEX_PEAGREEN, DJ_PEAGREEN_COLOR },
	{ 8,  TRUE,	 "Link Collections",	  8, DJ_INDEX_ORANGE,	DJ_ORANGE_COLOR },
	{ 9,  TRUE,	 "Discussions",			  2, DJ_INDEX_RED,		DJ_RED_COLOR },
	{ 10, FALSE, "Error Messages",		  0, DJ_INDEX_RED,		RGB(255,255,255) },
	{ 11, FALSE, "Unclassified",		  0, DJ_INDEX_RED,		RGB(255,255,255) }
*/