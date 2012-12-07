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
* Description:  Editor initialization.
*
****************************************************************************/


#include "vi.h"
#include "rxsupp.h"
#include "fcbmem.h"
#include "source.h"
#include "win.h"
#include "menu.h"
#include "getopt.h"
#include "sstyle.h"
#include "fts.h"
#ifdef __WIN__
    #include "subclass.h"
    #include "utils.h"
#endif
#ifdef __NT__
    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
#endif
#include "rcs.h"
#include "autoenv.h"

static char     nullFN[] = "no_name";
static char     defaultEDPath[] = DIR_SEP_STR "eddat";
static char     *cFN;
static char     *cfgFN = NULL;
static char     *cTag;
static char     *keysToPush;
#ifdef __WIN__
    static int  lineToGoTo = 0;
#endif
static char     goCmd[10];
static bool     wantNoReadEntireFile;
char            *WordDefnDefault = "::..\\\\__09AZaz";

#ifdef __WIN__
/*
 * SetConfigFileName
 */
void SetConfigFileName( char *fn )
{
    AddString2( &cfgFN, fn );

} /* SetConfigFileName */

/*
 * GetConfigFileName
 */
char *GetConfigFileName( void )
{
    return( cfgFN );

} /* GetConfigFileName */
#endif

void FiniCFName( void )
{
    MemFree( cfgFN );

} /* SetConfigFileName */

/*
 * checkFlags - check for command line flags
 */
static void checkFlags( int *argc, char *argv[], char *start[],
                        char *parms[], int *startcnt )
{
    int         ch;
#ifndef __WIN__
    int         len;
#endif

    cFN = NULL;
    while( 1 ) {
#ifndef __WIN__
        ch = GetOpt( argc, argv, "#-ndvqzirc:k:p:s:t:", NULL );
#else
        ch = GetOpt( argc, argv, "#-ndvqzirIP:c:k:p:s:t:", NULL );
#endif
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
#ifndef __WIN__
            strncpy( goCmd, OptArg, sizeof( goCmd ) -2 );
            goCmd[sizeof( goCmd ) - 2] = 0;
            len = strlen( goCmd );
            goCmd[len] = 'G';
            goCmd[len + 1] = 0;
#else
            lineToGoTo = atoi( OptArg );
#endif
            break;
#ifdef __WIN__
        case 'P':
            SetInitialWindowSize( OptArg );
            break;
        case 'I':
            cFN = GetInitialFileName();
            break;
#endif
#ifdef __IDE__
        case 'X':
            IDEInit();
            EditFlags.UseIDE = TRUE;
            break;
#endif
        case '-':
            EditFlags.StdIOMode = TRUE;
            EditFlags.NoInitialFileLoad = TRUE;
            break;
        case 'r':
            EditFlags.RecoverLostFiles = TRUE;
            EditFlags.NoInitialFileLoad = TRUE;
            break;
        case 'i':
            EditFlags.IgnoreLostFiles = TRUE;
            break;
        case 'z':
            EditFlags.IgnoreCtrlZ = TRUE;
            break;
        case 'q':
            EditFlags.Quiet = TRUE;
            break;
        case 'v':
            EditFlags.ViewOnly = TRUE;
            break;
        case 'c':
            EditFlags.BoundData = FALSE;
            AddString2( &cfgFN, OptArg );
            break;
        case 'd':
            EditFlags.BoundData = FALSE;
            AddString2( &cfgFN, "" );
            break;
        case 'k':
            keysToPush = OptArg;
            break;
        case 'p':
            if( *startcnt <= 0 ) {
                Quit( NULL, "No script to give parm list\n" );
            }
            parms[(*startcnt - 1)] = OptArg;
            break;
        case 's':
            if( *startcnt < MAX_STARTUP ) {
                start[*startcnt] = OptArg;
                parms[*startcnt] = NULL;
                (*startcnt)++;
            } else {
                Quit( NULL, "Too many scripts\n" );
            }
            break;
        case 't':
            cTag = OptArg;
            break;
        case 'n':
            wantNoReadEntireFile = TRUE;
            break;
        }
    }

    /*
     * now, check for null file name
     */
    if( cFN == NULL ) {
        if( (*argc) == 1 ) {
            cFN = nullFN;
        } else {
//          cFN = argv[(*argc)-1];
            cFN = argv[1];
        }
    }

} /* checkFlags */

/*
 * doInitializeEditor - do just that
 */
static void doInitializeEditor( int argc, char *argv[] )
{
    int         i, arg, cnt, ocnt, ln, startcnt = 0;
    int         k, j;
    char        tmp[FILENAME_MAX], c[1];
    char        buff[MAX_STR], file[MAX_STR], **list;
    char        cmd[MAX_STR * 2];
    char        *parm;
    char        *startup[MAX_STARTUP];
    char        *startup_parms[MAX_STARTUP];
    vi_rc       rc;
    vi_rc       rc1;

    /*
     * Make sure WATCOM is setup and if it is not, make a best guess.
     */
    watcom_setup_env();

    /*
     * If EDPATH is not set, use system default %WATCOM%\EDDAT.
     */
    if( getenv( "EDPATH" ) == NULL ) {
        char *watcom;

        watcom = getenv( "WATCOM" );
        if( watcom != NULL ) {
            char edpath[FILENAME_MAX];

            strcpy( edpath, watcom );
            strcat( edpath, defaultEDPath );

            if( setenv( "EDPATH", edpath, 0 ) != 0 ) {
                /*
                 * Bail out silently on error, as we will get error message later on.
                 */
            }
        }
    }

    /*
     * misc. set up
     */
    SpawnPrompt[0] = 0;
    MaxMemFree = MemSize();
    StaticStart();
    FTSInit();
    CheckForBoundData();
    EditFlags.Starting = TRUE;
    InitCommandLine();
    ChkExtendedKbd();
    SSInitBeforeConfig();

    GetCWD1( &HomeDirectory );
    GetCWD1( &CurrentDirectory );
    SetCWD( HomeDirectory );
    if( cfgFN == NULL ){
        AddString( &cfgFN, CFG_NAME );
    }

    checkFlags( &argc, argv, startup, startup_parms, &startcnt );
    ScreenInit();
    SetWindowSizes();
    EditFlags.ClockActive = FALSE;
    SetInterrupts();
#ifndef __WIN__
    InitColors();
#else
    InitClrPick();
    InitFtPick();
    SubclassGenericInit();
    CursorOp( COP_INIT );
#endif
    InitSavebufs();
    InitKeyMaps();

    /*
     * initial configuration
     */
    Majick = MemStrDup( "()~@" );
    FileEndString = MemStrDup( "[END_OF_FILE]" );
    MatchInit();

    if( cfgFN[0] != 0 ) {
        c[0] = 0;
        ln = 0;
        rc = Source( cfgFN, c, &ln );
        if( rc == ERR_FILE_NOT_FOUND ) {
#ifdef __WIN__
            CloseStartupDialog();
            MessageBox( (HWND)NULLHANDLE, "Could not locate configuration information; please make sure your EDPATH environment variable is set correctly",
                        EditorName, MB_OK );
            ExitEditor( -1 );
#else
            rc = ERR_NO_ERR;
#endif
        }
    } else {
        rc = ERR_NO_ERR;
    }
    SetGadgetString( NULL );
    if( wantNoReadEntireFile ) {
        EditFlags.ReadEntireFile = FALSE;
    }
    VerifyTmpDir();
    while( LostFileCheck() );
    HookScriptCheck();

    if( EditFlags.Quiet ) {
        EditFlags.Spinning = EditFlags.Clock = FALSE;
    }
    ExtendedMemoryInit();

    /*
     * more misc. setup
     */
    if( WordDefn == NULL ) {
        AddString( &WordDefn, &WordDefnDefault[6] );
        InitWordSearch( WordDefn );
    }
    if( WordAltDefn == NULL ) {
        AddString( &WordAltDefn, WordDefnDefault );
    }
    if( TagFileName == NULL ) {
        AddString( &TagFileName, "tags" );
    }
    WorkLine = MemAlloc( sizeof( line ) + MaxLine + 2 );
    DotBuffer = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    AltDotBuffer = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    DotCmd = MemAlloc( (maxdotbuffer + 2) * sizeof( vi_key ) );
    WorkLine->len = -1;
    SwapBlockInit( MaxSwapBlocks );
    ReadBuffer = MemAlloc( MAX_IO_BUFFER + 6 );
    WriteBuffer = MemAlloc( MAX_IO_BUFFER + 6 );
    FindHistInit( FindHist.max );
    FilterHistInit( FilterHist.max );
    CLHistInit( CLHist.max );
    LastFilesHistInit( LastFilesHist.max );
    GetClockStart();
    GetSpinStart();
    SelRgnInit();
    SSInitAfterConfig();
#if defined( VI_RCS )
    ViRCSInit();
#endif

    /*
     * create windows
     */
    StartWindows();
    InitMouse();
    rc1 = NewMessageWindow();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    DoVersion();
    rc1 = InitMenu();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    EditFlags.SpinningOurWheels = TRUE;
    EditFlags.ClockActive = TRUE;
    EditFlags.DisplayHold = TRUE;
    rc1 = NewStatusWindow();
    if( rc1 != ERR_NO_ERR ) {
        FatalError( rc1 );
    }
    EditFlags.DisplayHold = FALSE;
    MaxMemFreeAfterInit = MemSize();

    /*
     * start specified file(s)
     */
    arg = argc - 1;
    k = 1;
    cmd[0] = 'e';
    cmd[1] = 0;
    EditFlags.WatchForBreak = TRUE;

    /*
     * look for a tag: if there is one, set it up as the file to start
     */
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
#if defined( __NT__ ) && !defined( __WIN__ )
        {
            if( !EditFlags.Quiet ) {
                extern HANDLE OutputHandle;
                SetConsoleActiveScreenBuffer( OutputHandle );
            }
        }
#endif
        rc1 = LocateTag( cTag, file, buff );
        cFN = file;
        if( rc1 ) {
            if( rc1 == ERR_TAG_NOT_FOUND ) {
                Error( GetErrorMsg( rc1 ), cTag );
                ExitEditor( 0 );
            }
            FatalError( rc1 );
        }
    }

    while( TRUE && !EditFlags.NoInitialFileLoad ) {

        if( cFN == nullFN && !EditFlags.UseNoName ) {
            break;
        }

#ifdef __NT__
        {
            int     k2 = k;
            int     arg2 = arg;
            char    path[_MAX_PATH];
            int     found = 0;
            int     fd;

            /*
             * check for the existence of a file name containing spaces, and open it if
             * there is one
             */
            memset( path, 0, _MAX_PATH );
            while( argv[k2] != NULL && strlen( path ) +
                                       strlen( argv[k2] ) < _MAX_PATH ) {
                strcat( path, argv[k2] );
                fd = open( path, O_RDONLY );
                if( fd != -1 ) {
                    close( fd );
                    found = 1;
                    break;
                }
                k2++;
                arg2--;
                strcat( path, " " );
            }
            if( found ) {
                rc1 = NewFile( path, FALSE );
                if( rc1 != ERR_NO_ERR ) {
                    FatalError( rc1 );
                }
                k = k2 + 1;
                arg = arg2 - 1;
                cFN = argv[k];
                if( arg < 1 ) {
                    break;
                }
                continue;
            }
        }
#endif

        strcat( cmd, SingleBlank );
        strcat( cmd, cFN );
        ocnt = cnt = ExpandFileNames( cFN, &list );
        if( !cnt ) {
            cnt = 1;
        } else {
            cFN = list[0];
        }

        for( j = 0; j < cnt; j++ ) {

            rc1 = NewFile( cFN, FALSE );
            if( rc1 != ERR_NO_ERR && rc1 != NEW_FILE ) {
                FatalError( rc1 );
            }
            if( EditFlags.BreakPressed ) {
                break;
            }
            if( cnt > 0 && j < cnt - 1 ) {
                cFN = list[j + 1];
            }
        }
        if( ocnt > 0 ) {
            MemFreeList( ocnt, list );
        }
        if( EditFlags.BreakPressed ) {
            ClearBreak();
            break;
        }
        k++;
        arg--;
        if( cTag != NULL || arg < 1 ) {
            break;
        }
        cFN = argv[k];
    }
    if( EditFlags.StdIOMode ) {
        rc1 = NewFile( "stdio", FALSE );
        if( rc1 != ERR_NO_ERR ) {
            FatalError( rc1 );
        }
    }
    EditFlags.WatchForBreak = EditFlags.Starting = FALSE;

    /*
     * if there was a tag, do the appropriate search
     */
    if( cTag != NULL && !EditFlags.NoInitialFileLoad ) {
        if( buff[0] != '/' ) {
            i = atoi( buff );
            rc1 = GoToLineNoRelCurs( i );
        } else {
            rc1 = FindTag( buff );
        }
        if( rc1 > 0 ) {
            Error( GetErrorMsg( rc1 ) );
        }
    }

    /*
     * try to run startup file
     */
    if( EditFlags.RecoverLostFiles ) {
        startcnt = 0;
    }
    for( i = 0; i < startcnt; i++ ) {
        GetFromEnv( startup[i], tmp );
        AddString2( &cfgFN, tmp );
        if( cfgFN[0] != 0 ) {
            if( startup_parms[i] != NULL ) {
                parm = startup_parms[i];
            } else {
                c[0] = 0;
                parm = c;
            }
#if defined( __NT__ ) && !defined( __WIN__ )
            {
                if( !EditFlags.Quiet ) {
                    extern HANDLE OutputHandle;
                    SetConsoleActiveScreenBuffer( OutputHandle );
                }
            }
#endif
            rc = Source( cfgFN, parm, &ln );
        }
    }
    if( rc > ERR_NO_ERR ) {
        Error( "%s on line %d of \"%s\"", GetErrorMsg( rc ), ln, cfgFN );
    }
    if( argc == 1 ) {
        LoadHistory( NULL );
    } else {
        LoadHistory( cmd );
    }
    if( GrepDefault == NULL ) {
        AddString( &GrepDefault, "*.(c|h)" );
    }
    if( goCmd[0] != 0 ) {
        KeyAddString( goCmd );
    }
    if( keysToPush != NULL ) {
        KeyAddString( keysToPush );
    }
#ifdef __WIN__
    if( lineToGoTo != 0 ) {
        SetCurrentLine( lineToGoTo );
        NewCursor( CurrentWindow, NormalCursorType );
    }
#endif
    AutoSaveInit();
    HalfPageLines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES ) / 2 - 1;
#ifdef __386__
    VarAddGlobalStr( "OS386", "1" );
#endif
    if( StatusString == NULL ) {
        AddString( &StatusString, "L:$6L$nC:$6C" );
    }
    UpdateStatusWindow();
#ifdef __WIN__
    if( CurrentInfo == NULL ) {
        // no file loaded - screen is disconcertenly empty - reassure
        DisplayFileStatus();
    }
#endif
    NewCursor( CurrentWindow, NormalCursorType );
#if defined( __NT__ ) && !defined( __WIN__ )
    {
        extern HANDLE OutputHandle;
        SetConsoleActiveScreenBuffer( OutputHandle );
    }
#endif

} /* doInitializeEditor */

/*
 * InitializeEditor - start up
 */
void InitializeEditor( void )
{
    extern int  _argc;
    extern char **_argv;

    doInitializeEditor( _argc, _argv );

} /* InitializeEditor */
