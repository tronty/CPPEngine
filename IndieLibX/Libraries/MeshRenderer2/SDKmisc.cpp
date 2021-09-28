//--------------------------------------------------------------------------------------
// File: SDKmisc.cpp
//
// Various helper functionality that is shared between SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#include <Framework3/IRenderer.h>
//include "dxut.h"
#include "SDKmisc.h"
//include "DXUTres.h"
#undef min // use __min instead
#undef max // use __max instead

//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------
bool DXUTFindMediaSearchTypicalDirs(  char** strSearchPath, 
                                     int cchSearch, 
                                     const char* strLeaf, 
                                     char** strExePath,
                                     char** strExeName );
bool DXUTFindMediaSearchParentDirs(  char** strSearchPath, 
                                    int cchSearch, 
                                    char** strStartAt, 
                                    char** strLeafName );

//--------------------------------------------------------------------------------------
// Returns pointer to static media search buffer
//--------------------------------------------------------------------------------------
char** DXUTMediaSearchPath()
{
    static char* s_strMediaSearchPath[MAX_PATH] = {0};
    return s_strMediaSearchPath;

}

//--------------------------------------------------------------------------------------
const char* DXUTGetMediaSearchPath()
{
    return DXUTMediaSearchPath();
}


//--------------------------------------------------------------------------------------
HRESULT DXUTSetMediaSearchPath( const char* strPath )
{
    HRESULT hr = S_OK;

    char** s_strSearchPath = DXUTMediaSearchPath();

    hr = wcscpy_s( s_strSearchPath, MAX_PATH, strPath );
    if( SUCCEEDED( hr ) )
    {
        // append slash if needed
        size_t ch;
        ch = wcslen( s_strSearchPath);
        if( SUCCEEDED( hr ) && s_strSearchPath[ch - 1] != L'\\' )
        {
            hr = wcscat_s( s_strSearchPath, MAX_PATH, L"\\" );
        }
    }

    return hr;
}


//--------------------------------------------------------------------------------------
// Tries to find the location of a SDK media file
//       cchDest is the size in WCHARs of strDestPath.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds.
//--------------------------------------------------------------------------------------
HRESULT DXUTFindDXSDKMediaFileCch( char** strDestPath, int cchDest, 
                                          const char* strFilename )
{
    bool bFound;
    char* strSearchFor[MAX_PATH];

    if( NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10 )
        return E_INVALIDARG;

    // Get the exe name, and exe path
    char* strExePath[MAX_PATH] = {0};
    char* strExeName[MAX_PATH] = {0};
    char** strLastSlash = NULL;
    GetModuleFileName( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH - 1] = 0;
    strLastSlash = wcsrchr( strExePath, TEXT( '\\' ) );
    if( strLastSlash )
    {
        wcscpy_s( strExeName, MAX_PATH, &strLastSlash[1] );

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = wcsrchr( strExeName, TEXT( '.' ) );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    // Typical directories:
    //      .\
    //      ..\
    //      ..\..\
    //      %EXE_DIR%\
    //      %EXE_DIR%\..\
    //      %EXE_DIR%\..\..\
    //      %EXE_DIR%\..\%EXE_NAME%
    //      %EXE_DIR%\..\..\%EXE_NAME%

    // Typical directory search
    bFound = DXUTFindMediaSearchTypicalDirs( strDestPath, cchDest, strFilename, strExePath, strExeName );
    if( bFound )
        return S_OK;

    // Typical directory search again, but also look in a subdir called "\media\" 
    swprintf_s( strSearchFor, MAX_PATH, L"media\\%s", strFilename );
    bFound = DXUTFindMediaSearchTypicalDirs( strDestPath, cchDest, strSearchFor, strExePath, strExeName );
    if( bFound )
        return S_OK;

    char* strLeafName[MAX_PATH] = {0};

    // Search all parent directories starting at .\ and using strFilename as the leaf name
    wcscpy_s( strLeafName, MAX_PATH, strFilename );
    bFound = DXUTFindMediaSearchParentDirs( strDestPath, cchDest, L".", strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at the exe's dir and using strFilename as the leaf name
    bFound = DXUTFindMediaSearchParentDirs( strDestPath, cchDest, strExePath, strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at .\ and using "media\strFilename" as the leaf name
    swprintf_s( strLeafName, MAX_PATH, L"media\\%s", strFilename );
    bFound = DXUTFindMediaSearchParentDirs( strDestPath, cchDest, L".", strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at the exe's dir and using "media\strFilename" as the leaf name
    bFound = DXUTFindMediaSearchParentDirs( strDestPath, cchDest, strExePath, strLeafName );
    if( bFound )
        return S_OK;

    // On failure, return the file as the path but also return an error code
    wcscpy_s( strDestPath, cchDest, strFilename );

    return DXUTERR_MEDIANOTFOUND;
}


//--------------------------------------------------------------------------------------
// Search a set of typical directories
//--------------------------------------------------------------------------------------
bool DXUTFindMediaSearchTypicalDirs( char** strSearchPath, int cchSearch, const char* strLeaf,
                                     char** strExePath, char** strExeName )
{
    // Typical directories:
    //      .\
    //      ..\
    //      ..\..\
    //      %EXE_DIR%\
    //      %EXE_DIR%\..\
    //      %EXE_DIR%\..\..\
    //      %EXE_DIR%\..\%EXE_NAME%
    //      %EXE_DIR%\..\..\%EXE_NAME%
    //      DXSDK media path

    // Search in .\  
    wcscpy_s( strSearchPath, cchSearch, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\  
    swprintf_s( strSearchPath, cchSearch, L"..\\%s", strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\..\ 
    swprintf_s( strSearchPath, cchSearch, L"..\\..\\%s", strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\..\ 
    swprintf_s( strSearchPath, cchSearch, L"..\\..\\%s", strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\ 
    swprintf_s( strSearchPath, cchSearch, L"%s\\%s", strExePath, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\..\ 
    swprintf_s( strSearchPath, cchSearch, L"%s\\..\\%s", strExePath, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\..\..\ 
    swprintf_s( strSearchPath, cchSearch, L"%s\\..\\..\\%s", strExePath, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in "%EXE_DIR%\..\%EXE_NAME%\".  This matches the DirectX SDK layout
    swprintf_s( strSearchPath, cchSearch, L"%s\\..\\%s\\%s", strExePath, strExeName, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in "%EXE_DIR%\..\..\%EXE_NAME%\".  This matches the DirectX SDK layout
    swprintf_s( strSearchPath, cchSearch, L"%s\\..\\..\\%s\\%s", strExePath, strExeName, strLeaf );
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in media search dir 
    char** s_strSearchPath = DXUTMediaSearchPath();
    if( s_strSearchPath[0] != 0 )
    {
        swprintf_s( strSearchPath, cchSearch, L"%s%s", s_strSearchPath, strLeaf );
        if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
            return true;
    }

    return false;
}



//--------------------------------------------------------------------------------------
// Search parent directories starting at strStartAt, and appending strLeafName
// at each parent directory.  It stops at the root directory.
//--------------------------------------------------------------------------------------
bool DXUTFindMediaSearchParentDirs( char** strSearchPath, int cchSearch, char** strStartAt,
                                    char** strLeafName )
{
    char* strFullPath[MAX_PATH] = {0};
    char* strFullFileName[MAX_PATH] = {0};
    char* strSearch[MAX_PATH] = {0};
    char** strFilePart = NULL;

    GetFullPathName( strStartAt, MAX_PATH, strFullPath, &strFilePart );
    if( strFilePart == NULL )
        return false;

    while( strFilePart != NULL && *strFilePart != '\0' )
    {
        swprintf_s( strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName );
        if( GetFileAttributes( strFullFileName ) != 0xFFFFFFFF )
        {
            wcscpy_s( strSearchPath, cchSearch, strFullFileName );
            return true;
        }

        swprintf_s( strSearch, MAX_PATH, L"%s\\..", strFullPath );
        GetFullPathName( strSearch, MAX_PATH, strFullPath, &strFilePart );
    }

    return false;
}


//--------------------------------------------------------------------------------------
// CDXUTResourceCache
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CDXUTResourceCache::~CDXUTResourceCache()
{

    m_TextureCache.RemoveAll();
    m_EffectCache.RemoveAll();
    m_FontCache.RemoveAll();
}

//--------------------------------------------------------------------------------------
// Desc: Returns a view matrix for rendering to a face of a cubemap.
//--------------------------------------------------------------------------------------
D3DXFROMWINEMATRIX DXUTGetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXFROMWINEVECTOR3 vEyePt = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXFROMWINEVECTOR3 vLookDir;
    D3DXFROMWINEVECTOR3 vUpDir;

    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXFROMWINEVECTOR3( -1.0f, 0.0f, 0.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXFROMWINEVECTOR3( 0.0f, -1.0f, 0.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXFROMWINEVECTOR3( 0.0f, 0.0f, -1.0f );
            vUpDir = D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIXA16 mView;
    D3DXMatrixLookAtLH( &mView, &vEyePt, &vLookDir, &vUpDir );
    return mView;
}

