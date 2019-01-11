// DJGenre.h: Definitions for struct DJGenre.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJGENRE_H__C5E04AE3_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
#define AFX_DJGENRE_H__C5E04AE3_EE04_11D0_8C2B_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <afxtempl.h>

// Definitions
//
const int DJ_GENRE_PALETTE_SIZE = 12;	// Total number of genres.
const int  DJ_GENRE_NUM_OF_FACTORS = 6;	// Number of factors in genredefinition.


// DJGenre definition
//
struct DJGenre 
{
	int		nIndex;			// Used for fast access when dispatching documents.
	BOOL	bEnabled;		// Enabled in user interface.
	const	char* szLabel;
	int		nUIPosition;	// Index used for positioning labels in UI.
	int		nColorIndex;	// Index used for UI.
	COLORREF crColor;
};


// Type Definitions
//
typedef CTypedPtrList<CPtrList, DJGenre*> GenrePtrList;

// Declaration of genre palette.
//
extern const DJGenre g_GenrePalette[DJ_GENRE_PALETTE_SIZE];

#endif // !defined(AFX_DJGENRE_H__C5E04AE3_EE04_11D0_8C2B_08002BE6493E__INCLUDED_)
