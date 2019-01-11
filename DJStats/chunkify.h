// Chunkify.h
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>

extern CString g_strHomeDir;
extern CString g_strBiberFeateursFileName;
extern CString g_strBiberNormalsFileName;
extern CString g_strDocStoplistFileName;

int HashTableSize(int nExpectedSize);
float RandomFloat();
