# Microsoft Developer Studio Project File - Name="Chunkify" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Chunkify - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Chunkify.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chunkify.mak" CFG="Chunkify - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chunkify - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Chunkify - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chunkify - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\Users\Dewe\Chunkify\Release"
# PROP Intermediate_Dir "C:\Users\Dewe\Chunkify\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_TestChunkify" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 "C:\Program Files\DevStudio\VC\lib\wininet.lib" /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Chunkify - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\Users\Chunkify\Debug"
# PROP Intermediate_Dir "C:\Users\Chunkify\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_TestChunkify" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x41d /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Chunkify - Win32 Release"
# Name "Chunkify - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Chunkify.cpp
# End Source File
# Begin Source File

SOURCE=.\Chunkify.rc

!IF  "$(CFG)" == "Chunkify - Win32 Release"

!ELSEIF  "$(CFG)" == "Chunkify - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChunkifyApp.cpp
# End Source File
# Begin Source File

SOURCE=.\ChunkifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DJAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\DJAltaVistaQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\DJClusterAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\DJClusterNode.cpp
# End Source File
# Begin Source File

SOURCE=.\DJDocAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\DJFetchAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\DJGBiber.cpp
# End Source File
# Begin Source File

SOURCE=.\DJGenre.cpp
# End Source File
# Begin Source File

SOURCE=.\DJGenreProb.cpp
# End Source File
# Begin Source File

SOURCE=.\DJGStatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\DJInternetSession.cpp
# End Source File
# Begin Source File

SOURCE=.\DJMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\DJProximityMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\DJStatAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTempFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTextBufferHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTPipe.cpp
# End Source File
# Begin Source File

SOURCE=.\DJTTermIndexMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DJUrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DJUrlAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Chunkify.h
# End Source File
# Begin Source File

SOURCE=.\ChunkifyApp.h
# End Source File
# Begin Source File

SOURCE=.\ChunkifyDlg.h
# End Source File
# Begin Source File

SOURCE=.\DJAgent.h
# End Source File
# Begin Source File

SOURCE=.\DJAltaVistaQuery.h
# End Source File
# Begin Source File

SOURCE=.\DJClusterAgent.h
# End Source File
# Begin Source File

SOURCE=.\DJClusterNode.h
# End Source File
# Begin Source File

SOURCE=.\DJDocAgent.h
# End Source File
# Begin Source File

SOURCE=.\DJFetchAgent.h
# End Source File
# Begin Source File

SOURCE=.\DJGBiber.h
# End Source File
# Begin Source File

SOURCE=.\DJGenre.h
# End Source File
# Begin Source File

SOURCE=.\DJGenreProb.h
# End Source File
# Begin Source File

SOURCE=.\DJGStatistics.h
# End Source File
# Begin Source File

SOURCE=.\DJInternetSession.h
# End Source File
# Begin Source File

SOURCE=.\DJMatrix.h
# End Source File
# Begin Source File

SOURCE=.\DJProximityMatrix.h
# End Source File
# Begin Source File

SOURCE=.\DJStatAgent.h
# End Source File
# Begin Source File

SOURCE=.\DJTCluster.h
# End Source File
# Begin Source File

SOURCE=.\DJTDocument.h
# End Source File
# Begin Source File

SOURCE=.\DJTempFile.h
# End Source File
# Begin Source File

SOURCE=.\DJTextBufferHelpers.h
# End Source File
# Begin Source File

SOURCE=.\DJTGroup.h
# End Source File
# Begin Source File

SOURCE=.\DJTPipe.h
# End Source File
# Begin Source File

SOURCE=.\DJTTermIndexMap.h
# End Source File
# Begin Source File

SOURCE=.\DJUrl.h
# End Source File
# Begin Source File

SOURCE=.\DJUrlAgent.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Chunkify.ico
# End Source File
# Begin Source File

SOURCE=.\res\Chunkify.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DropJawv2.rc2
# End Source File
# End Group
# Begin Source File

SOURCE="Z:\Mina Dokument\Minnesanteckningar.doc"
# End Source File
# Begin Source File

SOURCE=.\Slask.txt
# End Source File
# Begin Source File

SOURCE=.\ToDo_list.txt
# End Source File
# End Target
# End Project
