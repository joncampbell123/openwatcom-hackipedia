/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Dynamic DBCS (Japanese) dialogs.
*
****************************************************************************/


#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include "jdlg.h"
#include "mem.h"

#ifndef MB_ERR_INVALID_CHARS
#define MB_ERR_INVALID_CHARS 0x00000000
#endif

#ifdef __NT__
#define ADJUST_BLOCKLEN( a ) a = (((a)+3) & ~3)
#else
#define ADJUST_BLOCKLEN( a )
#endif

#if defined(__NT__)
    #pragma pack(2);
#endif
typedef struct {
    long        dtStyle;
#ifdef __NT__
    DWORD       dtExtendedStyle;
    WORD        dtItemCount;
#else
    BYTE        dtItemCount;
#endif
    short       dtX;
    short       dtY;
    short       dtCX;
    short       dtCY;
    //char      dtMenuName[];
    //char      dtClassName[];
    //char      dtCaptionText[];
} _DLGTEMPLATE;

typedef struct {
    short       PointSize;
    //char      szTypeFace[];
} FONTINFO;

static BYTE     *JFontInfo = NULL;
static int      JFontInfoLen = 0;


#if defined(__NT__)
static BOOL mbcs2unicode( char *src, LPWSTR *dest, int *len )
/***********************************************************/
{
    LPWSTR      new;
    int         len1, len2;

    len1 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                src, -1, NULL, 0 );

    if( len1 == 0 || len1 == ERROR_NO_UNICODE_TRANSLATION ) {
        return( FALSE );
    }

    new = MemAlloc( len1 * sizeof( WCHAR ) );
    if( new == NULL ) {
        return( FALSE );
    }

    len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                src, -1, new, len1 );
    if( len2 != len1 ) {
        MemFree( new );
        return( FALSE );
    }

    *dest = new;
    *len = len1;

    return( TRUE );
}

#endif

static BOOL createFontInfoData( char *typeface, short pointsize,
                                BYTE **fidata, int *size )
/****************************/
{
    BYTE        *data;
    int         slen;

    #if defined(__NT__)
        data = NULL;
        {
            LPWSTR      unitypeface;
            if( mbcs2unicode( typeface, &unitypeface, &slen ) ) {
                slen *= sizeof(WCHAR);
                data = (BYTE *)MemAlloc( sizeof( short ) + slen );
                if( data != NULL ) {
                    *((short *)data) = pointsize;
                    memcpy( data+sizeof(short), unitypeface, slen );
                }
            }
        }
    #else
        slen = strlen( typeface ) + 1;
        data = (BYTE *)MemAlloc( sizeof( short ) + slen );
        if( data != NULL ) {
            *((short *)data) = pointsize;
            memcpy( data+sizeof(short), typeface, slen );
        }
    #endif

    if( data == NULL ) {
        return( FALSE );
    }

    *fidata = data;
    *size = slen + sizeof( short );

    return( TRUE );
}

static BYTE *skipString( BYTE *template )
/***************************************/
{
    #if defined(__NT__)
        // scan for zero word
        for( ; ((WORD)*template); template+=2 );
        template+=2;
    #else
        // scan for zero byte
        for( ; *template; template++ );
        template++;
    #endif

    return( template );
}

static BOOL hasFontInfo( BYTE *template )
/***************************************/
{
    _DLGTEMPLATE        *dt;

    dt = (_DLGTEMPLATE *)template;

    return( ( dt->dtStyle & DS_SETFONT ) != 0 );
}

static BYTE *findFontInfo( BYTE *template )
/*****************************************/
{
    // skip to the menu name
    template = template + sizeof( _DLGTEMPLATE );

    // skip the menu name
    template = skipString( template );

    // skip the class name
    template = skipString( template );

    // skip the caption text
    template = skipString( template );

    return( template );
}

static int getFontInfoSize( BYTE *fontinfo )
/******************************************/
{
    BYTE        *afterFontinfo;

    afterFontinfo = fontinfo + sizeof( short );
    afterFontinfo = skipString( afterFontinfo );
    return( afterFontinfo - fontinfo );
}

static BOOL getSystemFontTypeface( char **typeface, short *pointsize )
/********************************************************************/
{
#ifndef USE_SYSTEM_FONT
    *typeface = "�l�r ����";
    *pointsize = 10;

    return( TRUE );
#else
    HDC         hDc;
    HFONT       systemFont;
    LOGFONT     lf;
    int         logpixelsy;
    int         point;
    BOOL        roundup;

    systemFont = (HFONT)GetStockObject( SYSTEM_FONT );
    if( systemFont == (HFONT)NULL ) {
        return( FALSE );
    }

    if( !GetObject( systemFont, sizeof( LOGFONT ), &lf ) ) {
        return( FALSE );
    }

    *typeface = (char *)MemAlloc( strlen( lf.lfFaceName ) + 1 );
    if( *typeface == NULL ) {
        return( FALSE );
    }
    strcpy( *typeface, lf.lfFaceName );

    hDc = GetDC( (HWND)NULL );
    logpixelsy = GetDeviceCaps( hDc, LOGPIXELSY );
    ReleaseDC( (HWND)NULL, hDc );
    point = (( (unsigned long)lf.lfHeight * 720 ) / (unsigned long)logpixelsy);
    roundup = ( ( point % 10 ) > 4 );
    point /= 10;
    if( roundup ) {
        point++;
    }
    *pointsize = point;

    return( TRUE );
#endif
}

static HGLOBAL loadDialogTemplate( HINSTANCE hinst, LPCSTR lpszDlgTemp,
                                   DWORD *size )
/******************/
{
    HGLOBAL     htemplate;
    HRSRC       hrsrc;

    hrsrc = FindResource( hinst, lpszDlgTemp, RT_DIALOG );
    if( hrsrc == (HRSRC)NULL ) {
        return( NULL );
    }

    *size = SizeofResource( hinst, hrsrc );
    if( *size == 0 ) {
        return( NULL );
    }

    htemplate = LoadResource( hinst, hrsrc );
    if( htemplate == (HGLOBAL)NULL ) {
        return( NULL );
    }

    return( htemplate );
}

static HGLOBAL createJTemplate( HGLOBAL htemplate, DWORD size )
/*************************************************************/
{
    HGLOBAL     newHTemplate;
    DWORD       newSize;
    BYTE        *newTemplate;
    BYTE        *template;
    BYTE        *fontinfo;
    int         dlgHeaderSize;
    int         newdlgHeaderSize;
    int         ctlInfoSize;
    int         fontinfoSize;

    if( size == -1 ) {
        newSize = GlobalSize( htemplate );
    } else {
        newSize = size;
    }

    template = (BYTE *)LockResource( htemplate );
    if( template == NULL ) {
        return( NULL );
    }

    if( !hasFontInfo( template ) ) {
#ifndef __NT__
        UnlockResource( htemplate );
#endif
        return( NULL );
    }

    fontinfo = findFontInfo( template );
    fontinfoSize = getFontInfoSize( fontinfo );

    // calcualte the size of the original dialog header
    dlgHeaderSize = fontinfo - template + fontinfoSize;
    ADJUST_BLOCKLEN( dlgHeaderSize );
    ctlInfoSize = size - dlgHeaderSize;

    // calcualte the size of the new dialog header
    newdlgHeaderSize = fontinfo - template + JFontInfoLen;
    ADJUST_BLOCKLEN( newdlgHeaderSize );

    newSize = newdlgHeaderSize + ctlInfoSize;

    newHTemplate = GlobalAlloc( GHND, newSize );
    if( newHTemplate == (HGLOBAL)NULL ) {
#ifndef __NT__
        UnlockResource( htemplate );
#endif
        return( NULL );
    }

    newTemplate = (BYTE *)GlobalLock( newHTemplate );
    if( newTemplate == NULL ) {
        GlobalFree( newHTemplate );
#ifndef __NT__
        UnlockResource( htemplate );
#endif
        return( NULL );
    }

    // copy template data up to fontinfo
    memcpy( newTemplate, template, fontinfo - template );

    // copy the new fontinfo
    memcpy( newTemplate + ( fontinfo - template ), JFontInfo, JFontInfoLen );

    // copy the rest of the template data
    memcpy( newTemplate + newdlgHeaderSize, template + dlgHeaderSize,
            ctlInfoSize );

    GlobalUnlock( newHTemplate );
#ifndef __NT__
    UnlockResource( htemplate );
#endif

    return( newHTemplate );
}

BOOL JDialogInit( void )
/**********************/
{
    char        *typeface;
    short       pointsize;

    if( !GetSystemMetrics( SM_DBCSENABLED ) ) {
        return( TRUE );
    }

    if( !getSystemFontTypeface( &typeface, &pointsize ) ) {
        return( FALSE );
    }

    return( createFontInfoData( typeface, pointsize, &JFontInfo,
                                &JFontInfoLen ) );
}

void JDialogFini( void )
/**********************/
{
    if( JFontInfo ) {
        MemFree( JFontInfo );
        JFontInfo = NULL;
        JFontInfoLen = 0;
    }
}

static HWND cdIndirect( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                        HWND hwndOwner, DLGPROC dlgproc, DWORD size )
/*****************************************************/
{
    HGLOBAL     newtemplate;
    HWND        ret;

    if( JFontInfo == NULL ) {
        goto CDI_DEFAULT_ACTION;
    }

    newtemplate = createJTemplate( hglblDlgTemp, size );
    if( newtemplate == (HGLOBAL) NULL ) {
        goto CDI_DEFAULT_ACTION;
    }

    ret = CreateDialogIndirect( hinst, GlobalLock( newtemplate ),
                                hwndOwner, dlgproc );
    GlobalUnlock( newtemplate );

    GlobalFree( newtemplate );

    return( ret );

CDI_DEFAULT_ACTION:
    ret = CreateDialogIndirect( hinst, GlobalLock( hglblDlgTemp ),
                                hwndOwner, dlgproc );
    GlobalUnlock( hglblDlgTemp );

    return( ret );
}

static HWND cdIndirectParam( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                             HWND hwndOwner, DLGPROC dlgproc,
                             LPARAM lParamInit, DWORD size )
/*************************************/
{
    HGLOBAL     newtemplate;
    HWND        ret;

    if( JFontInfo == NULL ) {
        goto CDIP_DEFAULT_ACTION;
    }

    newtemplate = createJTemplate( hglblDlgTemp, size );
    if( newtemplate == (HGLOBAL) NULL ) {
        goto CDIP_DEFAULT_ACTION;
    }

    ret = CreateDialogIndirectParam( hinst, GlobalLock( newtemplate ),
                                     hwndOwner, dlgproc, lParamInit );
    GlobalUnlock( newtemplate );

    GlobalFree( newtemplate );

    return( ret );

CDIP_DEFAULT_ACTION:
    ret = CreateDialogIndirectParam( hinst, GlobalLock( hglblDlgTemp ),
                                     hwndOwner, dlgproc, lParamInit );
    GlobalUnlock( hglblDlgTemp );

    return( ret );
}

static int dbIndirect( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                       HWND hwndOwner, DLGPROC dlgproc, DWORD size )
/****************************************************/
{
    HGLOBAL     newtemplate;
    int         ret;

    if( JFontInfo == NULL ) {
        goto DBI_DEFAULT_ACTION;
    }

    newtemplate = createJTemplate( hglblDlgTemp, size );
    if( newtemplate == (HGLOBAL) NULL ) {
        goto DBI_DEFAULT_ACTION;
    }

    #if defined(__NT__)
        ret = DialogBoxIndirect( hinst, GlobalLock( newtemplate ),
                                 hwndOwner, dlgproc );
        GlobalUnlock( newtemplate );
    #else
        ret = DialogBoxIndirect( hinst, newtemplate, hwndOwner, dlgproc );
    #endif

    GlobalFree( newtemplate );

    return( ret );

DBI_DEFAULT_ACTION:
    return( DialogBoxIndirect( hinst, hglblDlgTemp, hwndOwner, dlgproc ) );
}

static int dbIndirectParam( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                            HWND hwndOwner, DLGPROC dlgproc,
                            LPARAM lParamInit, DWORD size )
/************************************/
{
    HGLOBAL     newtemplate;
    int         ret;

    if( JFontInfo == NULL ) {
        goto DBIP_DEFAULT_ACTION;
    }

    newtemplate = createJTemplate( hglblDlgTemp, size );
    if( newtemplate == (HGLOBAL) NULL ) {
        goto DBIP_DEFAULT_ACTION;
    }

    #if defined(__NT__)
        ret = DialogBoxIndirectParam( hinst, GlobalLock( newtemplate ),
                                      hwndOwner, dlgproc, lParamInit );
        GlobalUnlock( newtemplate );
    #else
        ret = DialogBoxIndirectParam( hinst, newtemplate,
                                      hwndOwner, dlgproc, lParamInit );
    #endif

    GlobalFree( newtemplate );

    return( ret );

DBIP_DEFAULT_ACTION:
    return( DialogBoxIndirectParam( hinst, hglblDlgTemp, hwndOwner, dlgproc, lParamInit ) );
}

int JDialogBoxIndirect( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                        HWND hwndOwner, DLGPROC dlgproc )
/**********************************/
{
    return( dbIndirect( hinst, hglblDlgTemp, hwndOwner, dlgproc, -1 ) );
}

int JDialogBoxIndirectParam( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                             HWND hwndOwner, DLGPROC dlgproc,
                             LPARAM lParamInit )
/*************************/
{
    return( dbIndirectParam( hinst, hglblDlgTemp, hwndOwner, dlgproc,
                             lParamInit, -1 ) );
}

HWND JCreateDialogIndirect( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                            HWND hwndOwner, DLGPROC dlgproc )
/**************************************/
{
    return( cdIndirect( hinst, hglblDlgTemp, hwndOwner, dlgproc, -1 ) );
}

HWND JCreateDialogIndirectParam( HINSTANCE hinst, HGLOBAL hglblDlgTemp,
                                 HWND hwndOwner, DLGPROC dlgproc,
                                 LPARAM lParamInit )
/**********************/
{
    return( cdIndirectParam( hinst, hglblDlgTemp, hwndOwner, dlgproc,
                             lParamInit, -1 ) );
}

int JDialogBox( HINSTANCE hinst, LPCSTR lpszDlgTemp,
                HWND hwndOwner, DLGPROC dlgproc )
/*********************************/
{
    HGLOBAL     template;
    DWORD       size;
    int         ret;

    if( JFontInfo == NULL ) {
        goto JDB_DEFAULT_ACTION;
    }

    template = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( template == (HGLOBAL) NULL ) {
        goto JDB_DEFAULT_ACTION;
    }

    ret = dbIndirect( hinst, template, hwndOwner, dlgproc, size );

    FreeResource( template );

    return( ret );

JDB_DEFAULT_ACTION:
    return( DialogBox( hinst, lpszDlgTemp, hwndOwner, dlgproc ) );
}

int JDialogBoxParam( HINSTANCE hinst, LPCSTR lpszDlgTemp,
                     HWND hwndOwner, DLGPROC dlgproc,
                     LPARAM lParamInit )
/************************/
{
    HGLOBAL     template;
    DWORD       size;
    int         ret;

    if( JFontInfo == NULL ) {
        goto JDBP_DEFAULT_ACTION;
    }

    template = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( template == (HGLOBAL) NULL ) {
        goto JDBP_DEFAULT_ACTION;
    }

    ret = dbIndirectParam( hinst, template, hwndOwner, dlgproc,
                           lParamInit, size );

    FreeResource( template );

    return( ret );

JDBP_DEFAULT_ACTION:
    return( DialogBoxParam( hinst, lpszDlgTemp, hwndOwner, dlgproc, lParamInit ) );
}

HWND JCreateDialog( HINSTANCE hinst, LPCSTR lpszDlgTemp,
                    HWND hwndOwner, DLGPROC dlgproc )
/*************************************/
{
    HGLOBAL     template;
    DWORD       size;
    HWND        ret;

    if( JFontInfo == NULL ) {
        goto JCD_DEFAULT_ACTION;
    }

    template = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( template == (HGLOBAL) NULL ) {
        goto JCD_DEFAULT_ACTION;
    }

    ret = cdIndirect( hinst, template, hwndOwner, dlgproc, size );

    FreeResource( template );

    return( ret );

JCD_DEFAULT_ACTION:
    return( CreateDialog( hinst, lpszDlgTemp, hwndOwner, dlgproc ) );
}

HWND JCreateDialogParam( HINSTANCE hinst, LPCSTR lpszDlgTemp,
                         HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit )
/******************************************************/
{
    HGLOBAL     template;
    DWORD       size;
    HWND        ret;

    if( JFontInfo == NULL ) {
        goto JCDP_DEFAULT_ACTION;
    }

    template = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( template == (HGLOBAL) NULL ) {
        goto JCDP_DEFAULT_ACTION;
    }

    ret = cdIndirectParam( hinst, template, hwndOwner, dlgproc,
                           lParamInit, size );

    FreeResource( template );

    return( ret );

JCDP_DEFAULT_ACTION:
    return( CreateDialogParam( hinst, lpszDlgTemp, hwndOwner,
                               dlgproc, lParamInit ) );
}

BOOL JDialogGetJFont( char **typeface, short *pointsize )
{
    return( getSystemFontTypeface( typeface, pointsize ) );
}
