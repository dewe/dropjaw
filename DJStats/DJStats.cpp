// DJStats.cpp
//
////////////////////////////////////////////////////////////////////////

#include <afx.h>
#include <afxwin.h>
#include <stdio.h>
#include <iostream.h>
#include <math.h>
#include <limits>
#include "DJGStatistics.h"
#include "DJGBiber.h"
#include "DJTDocument.h"
#include "chunkify.h"


using namespace std;

////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////

// List of wanted factors.
//
const int g_nFactorSize = DJ_BIBER_NUM_OF_FEATURES;
char **g_FactorVect = (char **) DJ_BIBER_FEATURE_NAMES;


// Defaults
//
BOOL g_bOutputFeatureValues = FALSE;		// Do not output stats for every file.
BOOL g_bOutputMaxMin = FALSE;			// No output of max/min-values.
BOOL g_bQuiet = FALSE;					// No output of mean and standard deviation.
BOOL g_bNormalizedDeviation = FALSE;	// No equalization.
BOOL g_bTokenLists = FALSE;				// Output all lists.
BOOL g_bCounters = FALSE;				// Output basic document counters.
BOOL g_bInitHashTables = TRUE;			// Initiate hash tables to some great value.
BOOL g_bOutputFactors = FALSE;			// Don't output factors values (only if g_bNormalizedDeviation).
BOOL g_bOutputGenres = FALSE;			// Don't output genre assignments;
int g_nDecimals = 6;					// Decimals in printed floats.

// Variables
//
char *g_pszFileList = NULL;
double x2[g_nFactorSize];	// Vector with sums of squares (for stdev).
double x[g_nFactorSize];	// Vector with sums (for stdev and average).
double max[g_nFactorSize];
double min[g_nFactorSize];
int g_nDocCount;



////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// DisplayBanner:
//
void DisplayBanner()
{
	cerr << "\nDJSTATS - extract statistics from a set of html-files." << endl;
	cerr << "Version 1.0 - Dewe" << endl;
	cerr << endl;
}


////////////////////////////////////////////////////////////////////////
// DisplayUsage:
//
void DisplayUsage()
{
	cerr << "Usage: DJStats [-p decimals] [-q] [-m] [-s] [-e] [-g] [-dc] [-dl] [-dh]\n";
	cerr << "               [drive:][path][filename]" << endl;
	cerr << endl;


	cerr << "Options:" << endl;
	cerr << "    -p decimals   Precision: number of decimals in output." << endl;
	cerr << "    -m            Output min- and max-values." <<  endl;
	cerr << "    -q            Quiet: do not output mean- and standard deviation." << endl;
	cerr << "    -s            Output document stats linewise for every document." <<  endl;
	cerr << "    -e            Equalize: normalize distribution." << endl;
//	cerr << "    -f            Output factor values (only valid if -e)." << endl;
	cerr << "    -g            Output genre assignments (only valid if -e)." << endl;
	cerr << "    -?            Show this help text." << endl;
	cerr << endl;

	cerr << "    [drive:][path][filename]" << endl;
	cerr << "                  Specifies path to a list of files to be processed." << endl;
	cerr << "                  If no value given, DJStats will read from standard" << endl;
	cerr << "                  input, useful when piping from DIR /B." << endl;
	cerr << endl;

	#ifdef _DEBUG
	cerr << "Debug options:" << endl;
	cerr << "    -dc           Output document stat counters." <<  endl;
	cerr << "    -dl           Output document token lists." <<  endl;
	cerr << "    -dh           Do not initialize document hash tables." << endl;
	cerr << endl;
	#endif
}


////////////////////////////////////////////////////////////////////////
// ParseOptions:
//
BOOL ParseOptions( int argc, char* argv[] )
{
	int nIndex;

	for( nIndex=1; nIndex<argc; nIndex++ )
	{
		// Option or filelist?
		if( *argv[nIndex] == '-' || *argv[nIndex] == '/' )
		{
			if( argv[nIndex][1] == '?' )
				return FALSE;

			else if( argv[nIndex][1] == 'P' || argv[nIndex][1] == 'p' )
				g_nDecimals = (int) atoi( argv[++nIndex] );
			else if( argv[nIndex][1] == 'Q' || argv[nIndex][1] == 'q' )
				g_bQuiet = TRUE;
			else if( argv[nIndex][1] == 'M' || argv[nIndex][1] == 'm' )
				g_bOutputMaxMin = TRUE;
			else if( argv[nIndex][1] == 'S' || argv[nIndex][1] == 's' )
				g_bOutputFeatureValues = TRUE;
			else if( argv[nIndex][1] == 'E' || argv[nIndex][1] == 'e' )
				g_bNormalizedDeviation = TRUE;
			else if( argv[nIndex][1] == 'F' || argv[nIndex][1] == 'f' )
				g_bOutputFactors = TRUE;
			else if( argv[nIndex][1] == 'G' || argv[nIndex][1] == 'g' )
				g_bOutputGenres = TRUE;

			else if( argv[nIndex][1] == 'D' || argv[nIndex][1] == 'd' )
			{
				if( argv[nIndex][2] == 'C' || argv[nIndex][2] == 'c' )
					g_bCounters = TRUE;
				else if( argv[nIndex][2] == 'L' || argv[nIndex][2] == 'l' )
					g_bTokenLists = TRUE;
				else if( argv[nIndex][2] == 'H' || argv[nIndex][2] == 'h' )
					g_bInitHashTables = FALSE;
			}

			else
			{
				cerr << "Error: unrecognized option: " << argv[nIndex] << endl << endl;
				return FALSE;
			}
		}
		else
		{
			// Can't have more than one filelist.
			if( g_pszFileList != NULL )
			{
				cerr << "Error: can not specify more than one filelist!" << endl;
				return FALSE;
			}
			else
				g_pszFileList = argv[nIndex];
		}
	}

	// Check for dependencies.
	if( g_bOutputFactors )
		g_bOutputFactors = g_bNormalizedDeviation;
	if( g_bOutputGenres )
		g_bOutputGenres = g_bNormalizedDeviation;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////
// AddToStats:
//
void AddToStats( CDJTDocument *pDoc )
{
	static BOOL bStatsInitialized = FALSE;
	double fScore;
	int i;

	// Initialize statistics.
	if( !bStatsInitialized )
	{
		for( i=0; i<g_nFactorSize; i++ ) { 
			x[i] = x2[i] = 0;
			min[i] = numeric_limits<double>::infinity();
			max[i] = -numeric_limits<double>::infinity();
		}
		g_nDocCount = 0;
		bStatsInitialized = TRUE;
	}

	// Add values...
	for( i=0; i<g_nFactorSize; i++ )
	{
		VERIFY( pDoc->m_pFeatureScoreMap->Lookup(g_FactorVect[i], fScore) );

		x[i] += fScore;
		x2[i] += fScore * fScore;

		if( g_bOutputMaxMin )
		{
			if( fScore < min[i] ) min[i] = fScore;
			if( fScore > max[i] ) max[i] = fScore;
		}
	}
	
	g_nDocCount++;
}


////////////////////////////////////////////////////////////////////////
// OutputHeader:
//
void OutputHeader( CStdioFile *pOFile )
{
	// Date and time.
	CString strTmp, strInput = g_pszFileList;
	if( strInput.IsEmpty() ) strInput = "stdin";

	CTime theTime = CTime::GetCurrentTime();

	pOFile->WriteString( "#" + strInput + ": " );
	pOFile->WriteString( theTime.Format("%Y-%m-%d, %X") +"\n" );

	if( g_bOutputFeatureValues || g_bOutputFactors )
	{
		pOFile->WriteString( "#" );

		// Feature names.
		if( g_bOutputFeatureValues )
		{
			for( int i=0; i<g_nFactorSize; i++ )
			{
				pOFile->WriteString( g_FactorVect[i] );
				pOFile->WriteString( " " );
			}
		}
		
		// Factor names.
		if( g_bOutputFactors )
		{
			for( int i=0; i<DJ_BIBER_NUM_OF_FACTORS; i++ )
			{
				strTmp.Format( "Factor%d ", i );
				pOFile->WriteString( strTmp );
			}
		}
		
		pOFile->WriteString( "\n" );
	}

}


////////////////////////////////////////////////////////////////////////
// OutputFeatureValues:
//
void OutputFeatureValues( CStdioFile *pOFile, CDJTDocument *pDoc )
{
	double fScore;
	CString strScore;
	
	// Feature values
	if( g_bOutputFeatureValues )
	{	
		for( int i=0; i<g_nFactorSize; i++ )
		{
			VERIFY( pDoc->m_pFeatureScoreMap->Lookup(g_FactorVect[i], fScore) );
			strScore.Format( "%.*lf ", g_nDecimals, fScore );
			pOFile->WriteString( strScore );
		}
	}
	
	// Factor values
	if( g_bOutputFactors )
	{
		for( int i=0; i<DJ_BIBER_NUM_OF_FACTORS; i++ )
		{
			strScore.Format( "%.*lf ", g_nDecimals, (*pDoc->m_pFactorScores)[i] );
			pOFile->WriteString( strScore );
		}
	}
	
	pOFile->WriteString( "\n" );
}



////////////////////////////////////////////////////////////////////////
// OutputGenres:
//
void OutputGenres( CStdioFile *pOFile, CDJTDocument *pDoc )
{
	CString strGenre;
	double fProb;
	GenreProbPtrArray  *pGenreProbPtrs = &pDoc->m_GenreProbPtrs;

	for( int i=0; i<pGenreProbPtrs->GetSize(); i++ )
	{
		if( (fProb = (*pGenreProbPtrs)[i]->Probability()) > 0 )
		{
			strGenre.Format( "(%02d, %.2f) ", i+1, fProb );
			pOFile->WriteString( strGenre );
		}
	}
	
	pOFile->WriteString( "\n" );
}



////////////////////////////////////////////////////////////////////////
// OutputStats:
// Calculate statistics and print to file.
//
void OutputStats( CStdioFile *pOFile )
{
	CString strStat, strCount, strMaxMin;
	double mean, stdev, fQ;
	int n = g_nDocCount;

	// Header
	strCount.Format( "#%d documents.\n", n );
	pOFile->WriteString( strCount );
	pOFile->WriteString( "#VARIABLE MEAN STDEV" );
	if( g_bOutputMaxMin ) pOFile->WriteString( " MIN MAX" );
	pOFile->WriteString( "\n" );

	// Stats
	for( int i=0; i<g_nFactorSize; i++ )
	{
		mean = x[i] / g_nDocCount;
		ASSERT( !_isnan(mean) && _finite(mean) );

		fQ = x2[i] - (x[i]*x[i] / n);
		if( fQ < 0 ) fQ = 0;	// Check for small negative numbers.

		stdev = (n==1) ? 0 : sqrt( fQ / (n-1) );
		ASSERT( !_isnan(stdev) && _finite(stdev) );

		strStat.Format( "%s %.*lf %.*lf", g_FactorVect[i],  
											g_nDecimals, mean, 
											g_nDecimals, stdev );

		if( g_bOutputMaxMin )
		{
			strMaxMin.Format( " %.*lf %.*lf", g_nDecimals, min[i],
											g_nDecimals, max[i] );
			strStat += strMaxMin;
		}

		pOFile->WriteString( strStat );
		pOFile->WriteString( "\n" );
	}
}



////////////////////////////////////////////////////////////////////////
// ProcessData:
//
void ProcessData( CStdioFile *pListFile, CStdioFile *pOutputFile )
{
	CDJGBiber BiberScores;
	CDJGStatistics GStatistics;
	CString strFName;
	clock_t start, stop;

	OutputHeader( pOutputFile );
	cerr << endl;
	start = clock();

	while( pListFile->ReadString(strFName) )
	{
		CDJTDocument *pDoc = new CDJTDocument;
		pDoc->m_strUrl = strFName;

		// For better performance.
		if( g_bInitHashTables )
		{
			pDoc->m_pFeatureScoreMap->InitHashTable( HashTableSize(g_nFactorSize) );
			pDoc->m_pTokenMap->InitHashTable( HashTableSize(20000) );
			pDoc->m_pCapTokenMap->InitHashTable( HashTableSize(10000) );
		}

		try
		{
			CStdioFile DataFile( (LPCTSTR) strFName, 
				CFile::modeRead | CFile::shareDenyWrite | CFile::typeText);

			if( !(g_bOutputFeatureValues || g_bOutputFactors) )
				cerr << ".";

			GStatistics.ExtractStatistics( &DataFile, pDoc );
			BiberScores.CalculateScores( pDoc );
	
			// Compute deviation. (equalization?)
			if( g_bNormalizedDeviation )
			{
				BiberScores.EqualizeScores( pDoc );
				// BiberScores.CalcFactorScores( pDoc );		// Not in use.
				BiberScores.CalculateGenreScores( pDoc );		// Set genre scores...
			}
	
			// Save values for min, max, average and standard deviation.
			if( !g_bQuiet )
				AddToStats( pDoc );

			// Output values to statistics...
			if( g_bOutputFeatureValues || g_bOutputFactors )
				OutputFeatureValues( pOutputFile, pDoc );

			if( g_bOutputGenres )
				OutputGenres( pOutputFile, pDoc );
	
			#ifdef _DEBUG
				if( g_bCounters) 
					pDoc->PrintStatistics(); // define _PrintStats
				if( g_bTokenLists) 
					pDoc->PrintLists();		 // define _PrintLists
			#endif //_DEBUG

			pDoc->FreeStatsMem();
			delete pDoc;
		}
		catch( CException )
		{
			delete pDoc;
			throw;
		}
	}

	stop = clock();
	cerr << endl;

	if( !g_bQuiet )
		OutputStats( pOutputFile );

	fprintf( stderr, "\n\nTid: %.1f sekunder\n\n", (double)(stop - start)/CLOCKS_PER_SEC );
}


////////////////////////////////////////////////////////////////////////
// main:
//
void main(int argc, char *argv[])
{
	int nRetCode = 0;
	CStdioFile *pListFile = NULL, *pOutputFile = NULL;

	ios::sync_with_stdio();	// syncronize i/o streams and standard i/o.

	DisplayBanner();

    if( !AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0) )
    {
        cerr << "\nMFC failed to initialize!\n" << endl;
        exit(1);
    }

	if( !ParseOptions(argc, argv) )
	{
		DisplayUsage();
		exit(1);
	}

	if( g_strHomeDir == "" )
	{
		g_strHomeDir = argv[0];
		int nPath = g_strHomeDir.ReverseFind( '\\' );
		ASSERT( nPath != -1 );
		g_strHomeDir = g_strHomeDir.Left( nPath+1 );
	}

	try
	{
		// Open output.
		pOutputFile = new CStdioFile( stdout );

		// Open input.
		if( g_pszFileList == NULL )
			pListFile = new CStdioFile( stdin );
		else
			pListFile = new CStdioFile( g_pszFileList, CFile::modeRead | 
										CFile::shareDenyWrite | CFile::typeText );

		// Do the actual stuff...
		ProcessData( pListFile, pOutputFile );

		pListFile->Close();
		pOutputFile->Close();
	}
	catch( CFileException *pEx )
	{
		char sz[1024];

		cerr << "\nError: ";
		if (pEx->GetErrorMessage(sz, 1024))
			cerr << sz << endl;
		else
			cerr << "\nNo error message was available.\n";
		cerr << endl;

		pEx->Delete();

		if( pOutputFile ) pOutputFile->Abort();
		if( pListFile ) pListFile->Abort();
		nRetCode = 1;
	}

	if( pListFile ) delete pListFile;
	if( pOutputFile ) delete pOutputFile;

	exit(nRetCode);
}