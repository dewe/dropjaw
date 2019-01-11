// DJGBiber.h: interface for the CDJGBiber class. 
// For calculationg final scores and biber dimensions.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DJGBIBER_H__001D8E21_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_)
#define AFX_DJGBIBER_H__001D8E21_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include "DJTDocument.h"


// Globals.
//
const int DJ_BIBER_NUM_OF_FEATURES = 40;
extern const char *DJ_BIBER_FEATURE_NAMES[];
const int DJ_BIBER_NUM_OF_FACTORS = 6;


// Structures
//
struct FeatureNormals {
	double mean;
	double stdev;
};

// Collections
//
typedef CList<CString, LPCTSTR> DimensionList;	// List of words, making a feature.
typedef CMap<CString, LPCTSTR, DimensionList*, DimensionList*> BiberFeatureMap;
typedef CMap<CString, LPCTSTR, FeatureNormals, FeatureNormals> FeatureNormalsMap;
typedef CMap<CString, LPCTSTR, double, double> FactorsTemplateMap;
typedef CArray<FactorsTemplateMap*, FactorsTemplateMap*> FactorsArray;


// CDJGBiber class definition
//
class CDJGBiber  
{
public:
	// Construction
	CDJGBiber();
	virtual ~CDJGBiber();

	// Operations
	void CalculateScores(CDJTDocument *pDoc);
	void EqualizeScores(CDJTDocument *pDoc);
	void CalculateGenreScores(CDJTDocument *pDoc);	

	// Attributes
	BiberFeatureMap m_BiberFeatureMap;		// Map with all Biber dimension lists.
	FeatureNormalsMap m_FeatureNormalsMap;	// All normal values for the features.
	FactorsArray m_Factors;					// All factor templates.

	// Implementation
	void NormalizeBiberScores(CDJTDocument *pDoc);
	void CalcFactorScores( CDJTDocument *pDoc );
	void InitFactorTemplates();
	void InitBiberFeatures();
	void InitFeatureNormalsMap();
	void AddDimension(BiberFeatureMap *pMap, LPCTSTR pszDimension);
	int ReadDimensionList(LPCTSTR pszFName, LPCTSTR pszDim, DimensionList *pList);
	int ReadNormalValues(LPCTSTR pszFName, FeatureNormalsMap *pMap);

	double CalcDimListScore(CDJTDocument *pDoc, DimensionList *pDimList);
	void CalcBiberFeatureMap(CDJTDocument *pDoc, BiberFeatureMap *pMap);

	double ProbGenreText( CDJTDocument *pDoc );
	double ProbGenre01(CDJTDocument * pDoc);
	double ProbGenre02(CDJTDocument * pDoc);
	double ProbGenre03(CDJTDocument * pDoc);
	double ProbGenre04(CDJTDocument * pDoc);
	double ProbGenre05(CDJTDocument * pDoc);
	double ProbGenre06(CDJTDocument * pDoc);
	double ProbGenre07(CDJTDocument * pDoc);
	double ProbGenre08(CDJTDocument * pDoc);
	double ProbGenre09(CDJTDocument * pDoc);
	double ProbGenre10(CDJTDocument * pDoc);
	double ProbGenre11(CDJTDocument * pDoc);

	double TT(CDJTDocument *pDoc);
	double CTT(CDJTDocument *pDoc);
	double LW(CDJTDocument *pDoc);
	double CPW(CDJTDocument *pDoc);
	double DIGS(CDJTDocument *pDoc);
	double WPS(CDJTDocument *pDoc);
	double CONTR(CDJTDocument *pDoc);

	double TPW(CDJTDocument *pDoc);
	double HREF(CDJTDocument *pDoc);
	double IMG(CDJTDocument *pDoc);
	double HTML(CDJTDocument *pDoc);
	double CMNTS(CDJTDocument *pDoc);
	double CMETA(CDJTDocument *pDoc);
	double CACT(CDJTDocument *pDoc);
};


#endif // !defined(AFX_DJGBIBER_H__001D8E21_1BA0_11D1_91E4_0060974B4CFF__INCLUDED_)
