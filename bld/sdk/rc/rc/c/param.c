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
* Description:  WRC command line parameter parsing.
*
****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "errors.h"
#include "global.h"
#include "param.h"
#include "rctypes.h"
#include "rcmem.h"
#include "swchar.h"
#include "dbtable.h"
#ifdef __OSI__
 #include "ostype.h"
#endif
#include "leadbyte.h"

#if defined(__UNIX__)
# define PATH_SPLIT_S       ":"     /* path seperator in string form        */
#else
# define PATH_SPLIT_S       ";"     /* path seperator in string form        */
#endif

/* forward declaration */
static bool scanEnvVar( const char *varname, int *nofilenames );

extern void RcAddCPPArg( char * newarg )
/***************************************/
{
    int     numargs;        /* number of args in list at end of this function */
    char ** arg;
    char ** cppargs;

    cppargs = CmdLineParms.CPPArgs;

    if (CmdLineParms.CPPArgs == NULL) {
        /* 3 is 1 for the command, 1 for newarg, 1 for NULL */
        numargs = 3;
        cppargs = RcMemMalloc( numargs * sizeof(char *) );
        /* cppargs[0] is reserved for the name of the command so set it */
        /* to the null string for now */
        cppargs[0] = "";
    } else {
        arg = CmdLineParms.CPPArgs;
        while( *arg != NULL ) {
            arg++;
        }
        /* + 2 for the NULL arg and the new arg */
        numargs = arg - CmdLineParms.CPPArgs + 2;
        cppargs = RcMemRealloc( cppargs, numargs * sizeof(char *) );
    }

    cppargs[ numargs - 2 ] = newarg;
    cppargs[ numargs - 1 ] = NULL;

    CmdLineParms.CPPArgs = cppargs;
} /* RcAddCPPArg */


/*
 * SetDBRange - set the CharSet array up to recognize double byte character
 *              sequences
 */
void SetDBRange( unsigned from, unsigned to ) {
/***********************************************/
    unsigned    i;

    for( i=from; i <= to; i++ ) {
        CharSet[i] = DB_CHAR;
    }
}

static void SetDBChars( const uint_8 *bytes ) {
/**********************************************/
    unsigned    i;

    for( i=0; i < 256; i++ ) {
        if( bytes[i] == 0 ) {
            CharSet[i] = 0;
        } else {
            CharSet[i] = DB_CHAR;
        }
    }
}

#if(0)
static char *fixNewDirs( char *arg ) {
/************************************/
    char        *ret;
    char        *src;
    char        *dst;

    src = arg;
    ret = RcMemMalloc( strlen( src ) + 1 );
    dst = ret;
    while( *src != '\0' ) {
        if( !isspace( *src ) ) {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
    RcMemFree( arg );
    return( ret );
}
#endif

static bool scanString( char *buf, const char *str, unsigned len )
/*****************************************************************/
{
    bool        have_quote;

    have_quote = FALSE;
    while( isspace( *str ) ) str++;
    while( *str != '\0' && len > 0 ) {
        if( *str == '\"' ) {
            have_quote = !have_quote;
        } else {
            *buf = *str;
            buf++;
            len--;
        }
        str++;
    }
    *buf = '\0';
    return( have_quote );
}

extern void AddNewIncludeDirs( const char * arg )
/***********************************************/
{
    int     len;
    int     oldlen;

    len = strlen( arg );
    if (len == 0) {
        return;
    }

    if (NewIncludeDirs == NULL) {
        /* + 1 for the '\0' */
        NewIncludeDirs = RcMemMalloc( len + 1 );
        NewIncludeDirs[ 0 ] = '\0';
        oldlen = 0;
    } else {
        /* + 2 for the '\0' and the ';' */
        oldlen = strlen( NewIncludeDirs );
        NewIncludeDirs = RcMemRealloc( NewIncludeDirs, oldlen + len + 2 );
        strcat( NewIncludeDirs + oldlen , PATH_SPLIT_S );
        oldlen ++; //for the semicolon
    }
    if( scanString( NewIncludeDirs + oldlen, arg, len + 1 ) ) {
        RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
    }
//    NewIncludeDirs = fixNewDirs( NewIncludeDirs );
} /* AddNewIncludeDirs */

static bool ScanMultiOptArg( const char * arg )
/*********************************************/
{
    bool    contok;

    contok = true;

    for( ; *arg != '\0' && contok; arg++ ) {
        switch( tolower( *arg ) ) {
        case 'e':
            CmdLineParms.GlobalMemEMS = TRUE;
            break;
        case 'k':
            CmdLineParms.SegmentSorting = SEG_SORT_NONE;
            break;
        case 'l':
            CmdLineParms.EMSDirect = TRUE;
            break;
        case 'm':
            CmdLineParms.EMSInstance = TRUE;
            break;
#if 0
        case 'n':
            CmdLineParms.NoProtectCC = TRUE;
            break;
#endif
        case 'p':
            CmdLineParms.PrivateDLL = TRUE;
            break;
        case 'q':
            CmdLineParms.Quiet = TRUE;
            break;
        case 'r':
            CmdLineParms.Pass1Only = TRUE;
            break;
        case 't':
            CmdLineParms.ProtModeOnly = TRUE;
            break;
        case '?':
            CmdLineParms.PrintHelp = TRUE;
            contok = FALSE;
            break;
        default:
            RcError( ERR_UNKNOWN_OPTION, *arg );
            contok = FALSE;
            break;
        }
    }
    return( contok );
} /* ScanMultiOptArg */

static bool ScanOptionsArg( const char * arg )
/********************************************/
{
    bool        contok;
    ExtraRes    *resfile;
    FRStrings   *frStrings;
    char        *temp=NULL;
    char        *p;
    char        *delims = ",";
    int         findlen = 0;
    int         replen = 0;

    contok = true;

    switch( tolower( *arg ) ) {
    case '\0':
        RcError( ERR_NO_OPT_SPECIFIED );
        contok = false;
        break;
    case 'a':
        arg++;
        if( tolower( *arg ) == 'd' ) {
            CmdLineParms.GenAutoDep = TRUE;
        } else if( tolower( *arg ) == 'p' ) {
            arg++;
            if( *arg == '=' ) arg++;
            strncpy( CmdLineParms.PrependString, arg, _MAX_PATH );
            if( scanString( CmdLineParms.PrependString, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            CmdLineParms.Prepend = TRUE;
            break;
        } else {
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = FALSE;
        }
        break;
    case '3':
        arg++;
        switch( tolower( *arg ) ) {
        case '0':
            CmdLineParms.VersionStamp = VERSION_30_STAMP;
            break;
        case '1':
            CmdLineParms.VersionStamp = VERSION_31_STAMP;
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = FALSE;
            break;
        }
        break;
    case 'b':
        arg++;
        if( tolower( *arg ) == 't' ) {
            arg++;
            if( *arg == '=' ) arg++;
            if( stricmp( arg, "windows" ) == 0 || stricmp( arg, "win" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
            } else if( stricmp( arg, "nt" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
            } else if( stricmp( arg, "os2" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_OS2;
            } else {
                RcError( ERR_UNKNOWN_TARGET_OS, arg );
                contok = FALSE;
            }
        } else {
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = FALSE;
        }
        break;
    case 'c':
        arg++;
        if( *arg == '=' ) arg++;
        strcpy( CmdLineParms.CodePageFile, arg );
        if( scanString( CmdLineParms.CodePageFile, arg, _MAX_PATH  ) ) {
            RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
        }
        break;
    case 'd':
        /* temporary until preprocessing done inline */
        /* -1 to get the '-' or '/' as well */
        /* the cast is so the argument won't be const */
        RcAddCPPArg( (char *) arg - 1 );
        break;
    case 'f':
        arg++;
        switch( tolower( *arg ) ) {
        case 'o':
            arg++;
            if( *arg == '=' ) arg++;
            strncpy( CmdLineParms.OutResFileName, arg, _MAX_PATH );
            if( scanString( CmdLineParms.OutResFileName, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            break;
        case 'r':
            arg++;
            if( *arg == '=' ) arg++;
            resfile = RcMemMalloc( sizeof( ExtraRes ) );
            if( scanString( resfile->name, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
                RcMemFree( resfile );
            } else {
                resfile->next = CmdLineParms.ExtraResFiles;
                CmdLineParms.ExtraResFiles = resfile;
            }
            break;
        case 'e':
            arg++;
            if( *arg == '=' ) arg++;
            if( scanString( CmdLineParms.OutExeFileName, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = FALSE;
            break;
        }
        break;
    case 'g':
        arg++;
        if( *arg == '=' ) arg++;
        temp = RcMemMalloc( strlen( arg ) + 1 );
        strcpy( temp, arg );
        if( scanString( temp, arg, _MAX_PATH  ) ) {
            RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
        }
        frStrings = RcMemMalloc( sizeof( FRStrings ) + strlen( arg ) + 2 );
        p = strtok( temp, delims );
        if( p != NULL ) {
            findlen = strlen( p );
            strcpy( frStrings->buf, p );
            frStrings->findString = frStrings->buf;
        } else {
            RcError( ERR_SYNTAX_STR, "/g=" );
            contok = FALSE;
        }
        p = strtok( NULL, delims );
        if( p != NULL ) {
            replen = strlen( p );
            strcpy( &frStrings->buf[findlen+1], p );
            frStrings->replaceString = &frStrings->buf[findlen+1];
        } else {
            RcError( ERR_SYNTAX_STR, frStrings->findString  );
            contok = FALSE;
        }
        frStrings->next = CmdLineParms.FindReplaceStrings;
        CmdLineParms.FindReplaceStrings = frStrings;
        CmdLineParms.FindAndReplace = TRUE;
        RcMemFree( temp );
        break;
    case 'i':
        arg++;
        if( *arg == '=' ) arg++;
        AddNewIncludeDirs( arg );
        break;
    case 'o':
        CmdLineParms.PreprocessOnly = TRUE;
        break;
    case 's':
        arg++;
        switch( tolower( *arg ) ) {
        case '0':
            CmdLineParms.SegmentSorting = SEG_SORT_NONE;
            break;
        case '1':
            CmdLineParms.SegmentSorting = SEG_SORT_PRELOAD_ONLY;
            break;
        case '2':
            CmdLineParms.SegmentSorting = SEG_SORT_MANY;
            break;
        }
        break;
    case 'w':
        arg++;
        if( *arg == 'r' ) {
//          CmdLineParms.WritableRes = TRUE;
        }
        break;
#if defined(YYDEBUG) || defined(SCANDEBUG)
    case 'v':
        arg++;
        switch( tolower( *arg ) ) {
    #if defined(YYDEBUG)
        case '1':
            CmdLineParms.DebugParser = 1;
            break;
    #endif
    #if defined(YYDEBUG) && defined(SCANDEBUG)
        case '2':
            CmdLineParms.DebugParser = 1;
            CmdLineParms.DebugScanner = 1;
            break;
        case '3':
            CmdLineParms.DebugScanner = 1;
            break;
    #endif
    #if defined(SCANDEBUG)
        default:
            CmdLineParms.DebugScanner = 1;
            break;
    #endif
        }
        break;
#endif
    case 'x':
        arg++;
        if( tolower( *arg ) == 'c' ) {
            CmdLineParms.IgnoreCWD= TRUE;
        } else {
            CmdLineParms.IgnoreINCLUDE = TRUE;
        }
        break;
    case 'z':
        arg++;
        switch( tolower( *arg ) ) {
        case 'm':
            CmdLineParms.MSResFormat = TRUE;
            break;
        case 'n':
            CmdLineParms.NoPreprocess = TRUE;
            break;
        /*
            Lead-byte and trail-byte ranges for code pages used in Far East
            editions of Windows 95.

                        Character           Code    Lead-Byte   Trail-Byte
        Language        Set Name            Page    Ranges      Ranges

        Chinese
        (Simplified)    GB 2312-80          CP 936  0xA1-0xFE   0xA1-0xFE

        Chinese
        (Traditional)   Big-5               CP 950  0x81-0xFE   0x40-0x7E
                                                                0xA1-0xFE

        Japanese        Shift-JIS (Japan
                        Industry Standard)  CP 932  0x81-0x9F   0x40-0xFC
                                                    0xE0-0xFC   (except 0x7F)

        Korean
        (Wansung)       KS C-5601-1987      CP 949  0x81-0xFE   0x41-0x5A
                                                                0x61-0x7A
                                                                0x81-0xFE

        Korean
        (Johab)         KS C-5601-1992      CP 1361 0x84-0xD3   0x41-0x7E
                                                    0xD8        0x81-0xFE
                                                    0xD9-0xDE   (Government
                                                    0xE0-0xF9   standard:
                                                                0x31-0x7E
                                                                0x41-0xFE)
        */
        case 'k':
            arg++;
            switch( *arg ) {
            case '1':
                SetDBRange( 0x81, 0xfe );
                CmdLineParms.DBCharSupport = DB_TRADITIONAL_CHINESE;
                break;
            case '2':
                SetDBRange( 0x81, 0xfe );
                CmdLineParms.DBCharSupport = DB_WANSUNG_KOREAN;
                break;
            case '3':
                SetDBRange( 0xA1, 0xfe );
                CmdLineParms.DBCharSupport = DB_SIMPLIFIED_CHINESE;
                break;
            case '0':
            case ' ':
            case '\0':
                SetDBRange( 0x81, 0x9f );
                SetDBRange( 0xe0, 0xfc );
                CmdLineParms.DBCharSupport = DB_KANJI;
                break;
//          case 'u':
                //
                // NYI - set up a new code page
                //
//              break;
            default:
                RcError( ERR_UNKNOWN_MULT_OPTION, arg - 2 );
                contok = FALSE;
                break;
            }
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = FALSE;
            break;
        }
        break;
    default:            /* option that could have others with it */
        contok = ScanMultiOptArg( arg ) && contok;
        break;
    }
    return( contok );
} /* ScanOptionsArg */

static void MakeFileName( char * infilename, char * outfilename, char * ext )
/***************************************************************************/
{
    char    name[ _MAX_FNAME ];
    char    dir[ _MAX_DIR ];
    char    drive[ _MAX_DRIVE ];

    _splitpath( infilename, drive, dir, name, NULL );
    _makepath( outfilename, drive, dir, name, ext );
} /* MakeFileName */

static void CheckExtension( char * filename, char * defext )
/**********************************************************/
{
    char    name[ _MAX_FNAME ];
    char    drive[ _MAX_DRIVE ];
    char    dir[ _MAX_DIR ];
    char    ext[ _MAX_EXT ];

    _splitpath( filename, drive, dir, name, ext );
    if( *ext == '\0' ) {
        _makepath( filename, drive, dir, name, defext );
    }
} /* CheckExtension */

/* extensions for Windows executables */
/* The strings are in the format of the _splitpath function */
static const char *ExeExt[] =   {
    ".EXE",
    ".DLL",
    ".DRV",
    ".SCR",                     /* Windows 3.1 screen saver apps */
    NULL
};

static void CheckPass2Only( void )
/********************************/
{
    char    ext[ _MAX_EXT ];
    char    **check_ext;

    _splitpath( CmdLineParms.InFileName, NULL, NULL, NULL, ext );
    if( stricmp( ext, ".RES" ) == 0 ) {
        CmdLineParms.Pass2Only = TRUE;
    } else {
        /* if the extension is in the ExeExt list then we want pass2 only */
        /* and there is no resource file to merge */
        for (check_ext = (char**)ExeExt; *check_ext != NULL; check_ext++) {
            if (stricmp( ext, *check_ext ) == 0) {
                CmdLineParms.Pass2Only = TRUE;
                CmdLineParms.NoResFile = TRUE;
            }
        }
    }
} /* CheckPass2Only */

static void SetIncludeVar( void )
/*******************************/
{
    char *  old_inc_dirs;

    if( !CmdLineParms.IgnoreINCLUDE ) {
        old_inc_dirs = RcGetEnv( "INCLUDE" );
        if( old_inc_dirs != NULL ) {
            AddNewIncludeDirs( old_inc_dirs );
        }
    }
} /* SetIncludeVar */

static void CheckParms( void )
/****************************/
{
    char        *defext;

    CheckExtension( CmdLineParms.InFileName, "rc" );
    CheckPass2Only();

    /* was an EXE file name given */
    if( CmdLineParms.InExeFileName[0] == '\0' ) {
        if (CmdLineParms.NoResFile) {
            strncpy( CmdLineParms.InExeFileName, CmdLineParms.InFileName,
                        _MAX_PATH );
        } else {
            MakeFileName( CmdLineParms.InFileName,
                             CmdLineParms.InExeFileName, "exe" );
        }
    } else {
        CheckExtension( CmdLineParms.InExeFileName, "exe" );
    }

    /* was an output RES file name given */
    if( CmdLineParms.PreprocessOnly ) {
        defext = "lst";
    } else {
        defext = "res";
    }
    if( CmdLineParms.OutResFileName[0] == '\0' ) {
        MakeFileName( CmdLineParms.InFileName,
                      CmdLineParms.OutResFileName, defext );
    } else {
        CheckExtension( CmdLineParms.OutResFileName, defext );
    }

    /* was an output EXE file name given */
    if( CmdLineParms.OutExeFileName[0] == '\0' ) {
        strncpy( CmdLineParms.OutExeFileName,
                CmdLineParms.InExeFileName, _MAX_PATH );
    } else {
        CheckExtension( CmdLineParms.OutExeFileName, "exe" );
    }

    /* check for the existance of the input files */
    if (! (CmdLineParms.Pass2Only && CmdLineParms.NoResFile) ) {
        if( access( CmdLineParms.InFileName, F_OK ) != 0 ) {
            RcFatalError( ERR_CANT_FIND_FILE, CmdLineParms.InFileName );
        }
    }
    if( !CmdLineParms.Pass1Only && !CmdLineParms.PreprocessOnly ) {
        if( access( CmdLineParms.InExeFileName, F_OK ) != 0 ) {
            RcFatalError( ERR_CANT_FIND_FILE, CmdLineParms.InExeFileName );
        }
    }

    if( CmdLineParms.GenAutoDep && CmdLineParms.MSResFormat ) {
        RcFatalError( ERR_OPT_NOT_VALID_TOGETHER, "-ad", "-zm" );
    }
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        switch( CmdLineParms.DBCharSupport ) {
        case DB_SIMPLIFIED_CHINESE:
            strcpy( CmdLineParms.CodePageFile, "936.uni" );
            break;
        case DB_TRADITIONAL_CHINESE:
            strcpy( CmdLineParms.CodePageFile, "950.uni" );
            break;
        case DB_KANJI:
            strcpy( CmdLineParms.CodePageFile, "kanji.uni" );
            break;
        case DB_WANSUNG_KOREAN:
            strcpy( CmdLineParms.CodePageFile, "949.uni" );
            break;
        }
    }
    if( CmdLineParms.PreprocessOnly && CmdLineParms.NoPreprocess ) {
        RcFatalError( ERR_OPT_NOT_VALID_TOGETHER, "-o", "-zn" );
    }
    SetIncludeVar();

} /* CheckParms */


static void defaultParms( void )
/******************************/
{
    #ifdef SCANDEBUG
        CmdLineParms.DebugScanner = FALSE;
    #endif
    #ifdef YYDEBUG
        CmdLineParms.DebugParser = FALSE;
    #endif
    CmdLineParms.DBCharSupport = DB_NONE;
    CmdLineParms.PrintHelp = FALSE;
    CmdLineParms.Quiet = FALSE;
    CmdLineParms.Pass1Only = FALSE;
    CmdLineParms.Pass2Only = FALSE;
    CmdLineParms.NoResFile = FALSE;
    CmdLineParms.IgnoreINCLUDE = FALSE;
    CmdLineParms.IgnoreCWD = FALSE;
    CmdLineParms.PrivateDLL = FALSE;
    CmdLineParms.GlobalMemEMS = FALSE;
    CmdLineParms.EMSInstance = FALSE;
    CmdLineParms.EMSDirect = FALSE;
    CmdLineParms.ProtModeOnly = FALSE;
    CmdLineParms.SegmentSorting = SEG_SORT_MANY;
    CmdLineParms.FindAndReplace = FALSE;
    CmdLineParms.Prepend = FALSE;
//    CmdLineParms.WritableRes = FALSE;
    CmdLineParms.InFileName[0] = '\0';
    CmdLineParms.InExeFileName[0] = '\0';
    CmdLineParms.OutResFileName[0] = '\0';
    CmdLineParms.OutExeFileName[0] = '\0';
    CmdLineParms.CodePageFile[0] = '\0';
    CmdLineParms.PrependString[0] = '\0';
    CmdLineParms.CPPArgs = NULL;
    CmdLineParms.VersionStamp = VERSION_31_STAMP;
    CmdLineParms.NoProtectCC = FALSE;
    CmdLineParms.NoPreprocess = FALSE;
    CmdLineParms.GenAutoDep = FALSE;
    CmdLineParms.PreprocessOnly = FALSE;
    CmdLineParms.ExtraResFiles = NULL;
    CmdLineParms.FindReplaceStrings = NULL;
    #ifdef __OSI__
        if( __OS == OS_NT ) {
            CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
        } else {
            CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
        }
    #elif defined(__NT__)
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
    #elif defined(__OS2__)
        CmdLineParms.TargetOS = RC_TARGET_OS_OS2;
    #else
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
    #endif
    NewIncludeDirs = NULL;
} /* defaultParms */


#if( 0 )

// copied from windows.h
#define CP_ACP      0
__declspec(dllimport) int __stdcall MultiByteToWideChar(
    uint_32 CodePage, uint_32 dwFlags, const char *lpMultiByteStr, int cchMultiByte,
    char *lpWideCharStr, int cchWideChar);

int NativeDBStringToUnicode( int len, const char *str, char *buf ) {
/*******************************************************************/
    int         ret;
    unsigned    outlen;

    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len * 2;
        }
        ret = MultiByteToWideChar( CP_ACP, 0, str, len, buf, outlen );
    } else {
        ret = 0;
    }
    return( ret * 2 );
}
#endif

static void getCodePage( void ) {
/********************************/

    RcStatus            ret;
    char                path[ _MAX_PATH ];

    if( CmdLineParms.CodePageFile[0] != '\0' ) {
        ret = OpenTable( CmdLineParms.CodePageFile, path );
        switch( ret ) {
        case RS_FILE_NOT_FOUND:
            RcFatalError( ERR_CANT_FIND_CHAR_FILE, CmdLineParms.CodePageFile );
            break;
        case RS_READ_ERROR:
            RcFatalError( ERR_READING_CHAR_FILE, path, strerror( errno ) );
            break;
        case RS_READ_INCMPLT:
        case RS_BAD_FILE_FMT:
            RcFatalError( ERR_BAD_CHAR_FILE, path );
            break;
        case RS_WRONG_VER:
            RcFatalError( ERR_WRONG_CHAR_FILE_VER, path );
            break;
        case RS_OPEN_ERROR:
            RcFatalError( ERR_CANT_OPEN_CHAR_FILE, path, strerror( errno ) );
            break;
        default:
            SetDBChars( GetLeadBytes() );
            break;
        }
    } else {
#ifdef NT_HOSTED
        if(DB_NONE == CmdLineParms.DBCharSupport){
            SetNativeLeadBytes();
            ConvToUnicode = NativeDBStringToUnicode;
        }
#endif
    }
}

static bool doScanParams( int argc, char *argv[], int *nofilenames )
/*******************************************************************/
{
    const char *arg;
    int     switchchar;
    bool    contok;         /* continue with main execution */
    int     currarg;

    contok = true;
    switchchar = _dos_switch_char();
    for( currarg = 1; currarg < argc && contok; currarg++) {
        arg = argv[ currarg ];
        if( *arg == switchchar || *arg == '-' ) {
            contok = ScanOptionsArg( arg + 1 ) && contok;
        } else if( *arg == '@' ) {
            contok = scanEnvVar( arg + 1, nofilenames ) && contok;
        } else if( *arg == '?' ) {
            CmdLineParms.PrintHelp = true;
            contok = false;
        } else if( *nofilenames == 0 ) {
            if( scanString( CmdLineParms.InFileName, arg, _MAX_PATH ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            (*nofilenames)++;
        } else if( *nofilenames == 1 ) {
            if( scanString( CmdLineParms.InExeFileName, arg, _MAX_PATH ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            (*nofilenames)++;
        } else {
            RcError( ERR_TOO_MANY_ARGS, arg );
            contok = false;
        }
    }
    return( contok );
}

extern unsigned ParseEnvVar( const char *env, char **argv, char *buf )
/********************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *env.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         switchchar;
    unsigned    argc;
    char        *bufend;
    bool        got_quote;

    switchchar = _dos_switch_char();
    bufend = buf;
    argc = 1;
    if( argv != NULL ) argv[0] = ""; //fill in the program name
    for( ;; ) {
        got_quote = FALSE;
        while( isspace( *env ) && *env != '\0' ) env++;
        start = env;
        if( buf != NULL ) {
            argv[ argc ] = bufend;
        }
        if( *env == switchchar || *env == '-' ) {
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env ++;
        }
        while( ( got_quote || !isspace( *env ) ) && *env != '\0' ) {
            if( *env == '\"' ) {
                got_quote = !got_quote;
            }
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env++;
        }
        if( start != env ) {
            argc++;
            if( buf != NULL ) {
                *bufend = '\0';
                bufend++;
            }
        }
        if( *env == '\0' ) break;
    }
    return( argc );
}

static bool scanEnvVar( const char *varname, int *nofilenames )
/*************************************************************/
{
    /*
     * Pass nofilenames and analysis of getenv(varname) into argc and argv
     * to doScanParams. Return view on usability of data. (TRUE is usable.)
     *
     * Recursion is supported but circularity is rejected.
     *
     * The analysis is fairly similar to that done in clib(initargv@_getargv).
     * It is possible to use that function but it is not generally exported and
     * ParseEnvVar() above is called from other places.
     */
    typedef struct EnvVarInfo {
        struct EnvVarInfo       *next;
        char                    *varname;
        char                    **argv; /* points into buf */
        char                    buf[1]; /* dynamic array */
    } EnvVarInfo;

    unsigned            argc;
    EnvVarInfo          *info;
    static EnvVarInfo   *stack = 0; // Needed to detect recursion.
    unsigned            argvsize;
    unsigned            argbufsize;
    char                *env;
    size_t              varlen;     // size to hold varname copy.
    bool                result;     // doScanParams Result.

    env = RcGetEnv( varname );
    if( env == NULL ) {
        RcWarning( ERR_ENV_VAR_NOT_FOUND, varname );
        return( TRUE );
    }
    // This used to cause stack overflow: set foo=@foo && wrc @foo.
    for( info = stack; info != NULL; info = info->next ) {
#if !defined(__UNIX__)
        if( stricmp( varname, info->varname ) == 0 ) // Case-insensitive
#else
        if( strcmp( varname, info->varname ) == 0 )  // Case-sensitive
#endif
            RcFatalError( ERR_RCVARIABLE_RECURSIVE, varname );
    }
    argc = ParseEnvVar( env, NULL, NULL );  // count parameters.
    argbufsize = strlen( env ) + 1 + argc;  // inter-parameter spaces map to 0
    argvsize = ( argc + 1 ) * sizeof( char * ); // sizeof argv[argc+1]
    varlen = strlen( varname ) + 1;         // Copy taken to detect recursion.
    info = RcMemMalloc( sizeof *info + argbufsize + argvsize + varlen );
    info->next = stack;
    stack = info;                           // push info on stack
    info->argv = (char **)info->buf;
    ParseEnvVar( env, info->argv, info->buf + argvsize );
    info->varname = info->buf + argvsize + argbufsize;
    strcpy( info->varname, varname );
    info->argv[argc] = NULL;    //there must be a NULL element on the end
                                // of the list
    result = doScanParams( argc, info->argv, nofilenames );
    stack = info->next;                     // pop stack
    RcMemFree( info );
    return( result );
}

bool ScanParams( int argc, char * argv[] )
/*****************************************/
{
    int     nofilenames;    /* number of filename parms read so far */
    bool    contok;         /* continue with main execution */

    nofilenames = 0;
    defaultParms();
    contok = doScanParams( argc, argv, &nofilenames );
    if( argc < 2 ) {                                    /* 26-mar-94 */
        CmdLineParms.PrintHelp = true;
        contok = false;
    }
    if( contok ) {
        if( nofilenames == 0 ) {
            RcError( ERR_FILENAME_NEEDED );
            contok = false;
        } else {
            CheckParms();
            getCodePage();
        }
    }
    return( contok );
} /* ScanParams */

extern void ScanParamShutdown( void )
/***********************************/
{
    ExtraRes            *tmpres;
    FRStrings           *strings;

    if( CmdLineParms.CPPArgs != NULL ) {
        RcMemFree( CmdLineParms.CPPArgs );
    }
    RcMemFree( NewIncludeDirs );
    while( CmdLineParms.ExtraResFiles != NULL ) {
        tmpres = CmdLineParms.ExtraResFiles;
        CmdLineParms.ExtraResFiles = CmdLineParms.ExtraResFiles->next;
        RcMemFree( tmpres );
    }
    while( CmdLineParms.FindReplaceStrings != NULL) {
        strings = CmdLineParms.FindReplaceStrings;
        CmdLineParms.FindReplaceStrings = CmdLineParms.FindReplaceStrings->next;
        RcMemFree( strings );
    }
} /* ScanParamShutdown */
