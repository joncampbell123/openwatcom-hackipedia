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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include <sys/stat.h>
#include <io.h>
#ifdef __NT__
    #include <shlobj.h>
    typedef HRESULT (WINAPI *GetFolderPath)( HWND, int, HANDLE, DWORD, LPTSTR );
#endif
#include <direct.h>

#define CONFIG_INI "weditor.ini"
#define CONFIG_DIR "Open Watcom"
#define INI_FILE   "watcom.ini"

static char     *iniFile = NULL;
static char     *iniPath = NULL;
static char     *cfgFile = NULL;
static char     *keyToolBar = "ToolBar";
static char     *keyInitialPosition = "InitialPosition";
static char     *keyInitialPositionState = "InitialPositionState";
static char     *keySaveConfig = "SaveConfig";
static char     *keyCfgTime = "CfgTime";
static char     *keyChildrenMaximized = "ChildrenMaximized";
static DWORD    cfgTime;

static bool     saveConfig;
#if defined(__WINDOWS_386__)
    #define STUPIDINT       short
#elif defined(__NT__)
    #define STUPIDINT       LONG
#else
    #define STUPIDINT       int
#endif

/*
 * getInt - parse an int from a string
 */
static bool getInt( char *str, STUPIDINT *i )
{
    DWORD       tmp;
    bool        rc;

    rc = GetDWORD( str, &tmp );
    *i = (STUPIDINT) tmp;
    return( rc );

} /* getInt */

/*
 * getProfileString - get a string from the profile
 */
static void getProfileString( char *key, char *initial, char *buffer )
{
    GetPrivateProfileString( EditorName, key, initial, buffer, MAX_STR, iniFile );

} /* getProfileString */

/*
 * getProfileRect - get a rectangle from the profile
 */
static void getProfileRect( char *key, char *initial, RECT *r )
{
    char        str[MAX_STR];

    getProfileString( key, initial, str );
    getInt( str, &r->left );
    getInt( str, &r->top );
    getInt( str, &r->right );
    getInt( str, &r->bottom );

} /* getProfileRect */

/*
 * getProfileLong - get a long integer from the profile
 */
static long getProfileLong( char *key )
{
    char        buffer[32];

    GetPrivateProfileString( EditorName, key, "0", buffer, sizeof( buffer ), iniFile );
    return( atol( buffer ) );

} /* getProfileLong */

/*
 * writeProfileString - write a string to the profile
 */
static void writeProfileString( char *key, char *buffer )
{
    WritePrivateProfileString( EditorName, key, buffer, iniFile );

} /* writeProfileString */

/*
 * writeProfileRect - write a rectangle to the profile
 */
static void writeProfileRect( char *key, RECT *r )
{
    char        str[MAX_STR];

    MySprintf( str, "%d %d %d %d", r->left, r->top, r->right, r->bottom );
    writeProfileString( key, str );

} /* writeProfileRect */

/*
 * writeProfileLong - write a long integer to the profile
 */
static void writeProfileLong( char *key, long value )
{
    char        str[MAX_STR];

    MySprintf( str, "%l", value );
    writeProfileString( key, str );

} /* writeProfileLong */

/*
 * writeToolBarSize - get the tool bar size
 */
static void writeToolBarSize( void )
{
    writeProfileRect( keyToolBar, &ToolBarFloatRect );

} /* writeToolBarSize */

/*
 * readToolBarSize - get the tool bar size
 */
static void readToolBarSize( void )
{
    getProfileRect( keyToolBar, "300 200 500 300", &ToolBarFloatRect );

} /* readToolBarSize */

/*
 * readInitialPosition - get the initial position of the editor
 */
static void readInitialPosition( void )
{
    RECT        r;

    getProfileRect( keyInitialPosition, "0 0 0 0", &r );
    SetInitialWindowRect( &r );
    RootState = getProfileLong( keyInitialPositionState );

} /* readInitialPosition */

/*
 * writeInitialPosition - write out the initial position information
 */
void writeInitialPosition( void )
{
    if( !EditFlags.SavePosition ) {
        memset( &RootRect, 0, sizeof( RECT ) );
    }
    if( !IsIconic( Root ) ){
        writeProfileRect( keyInitialPosition, &RootRect );
        writeProfileLong( keyInitialPositionState, RootState );
    }

} /* writeInitialPosition */

/*
 * getConfigFilePaths - get the path to the directory containing the config files
 * shfolder.dll is loaded explicitly for compatability with Win98 -- calling
 * SHGetFolderPathA directly doesn't work, probably due to order of linking
 */
static void getConfigFilePaths( void )
{
    char path[FILENAME_MAX];
#ifdef __NT__
    HINSTANCE library = LoadLibrary( "shfolder.dll" );
    if ( library ) {
        GetFolderPath getpath = (GetFolderPath)GetProcAddress( library,
                                                               "SHGetFolderPathA" );
        if( SUCCEEDED( getpath( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0,
                                path ) ) ) {
            if( strlen( path ) + strlen( "\\" CONFIG_DIR ) + 12 < FILENAME_MAX ) {
                strcat( path, "\\" CONFIG_DIR );
                if( access( path, F_OK ) ) {    /* make sure CONFIG_DIR diretory is present */
                    mkdir( path );              /* if not, create it */
                }
            }
        }
        FreeLibrary( library );
    }
    else {                                          /* should only get here on old machines */
        GetWindowsDirectory( path, FILENAME_MAX );  /* that don't have shfolder.dll */
    }
#else
    GetWindowsDirectory( path, FILENAME_MAX );
#endif
    AddString2( &iniPath, path );           /* these freed in WriteProfile */
    VarAddGlobalStr( "IniPath", iniPath );  /* make accessable to scripts */
    strcat( path, "\\" INI_FILE );
    AddString2( &iniFile, path);
    strcpy( path, iniPath );
    strcat( path, "\\" CONFIG_INI );
    AddString2( &cfgFile, path);

} /* getConfigFilePaths */

/*
 * readConfigFileName - get the name of the config file that we are to read
 */
static void readConfigFileName( void )
{
    char        cname[FILENAME_MAX];
    //char      str[MAX_STR]; // not used if not prompting for new cfg files
    char        *cfgname;
    struct stat cfg;
    int         rc;

    cfgTime = getProfileLong( keyCfgTime );
    cfgname = GetConfigFileName();
    GetFromEnv( cfgname, cname );
    if( cname[0] != 0 ) {
        stat( cname, &cfg );
        if( cfgTime == 0 ) {
            cfgTime = cfg.st_mtime;
        }
    }

    if( access( cfgFile, ACCESS_RD ) != -1 ) {
        rc = IDNO;
#if 0
        // don't prompt for newer config files
        if( cfg.st_mtime > cfgTime ) {
            MySprintf( str, "The configuration file \"%s\" is newer than your .INI file, do you wish to use the new configuration?",
                       cname );
            rc = MessageBox( (HWND) NULL, str, EditorName, MB_YESNO | MB_TASKMODAL );
            if( rc == IDYES ) {
                cfgTime = cfg.st_mtime;
            }

        }
#endif
        if( rc == IDNO ) {
            SetConfigFileName( cfgFile );
        }
    } else {
        cfgTime = cfg.st_mtime;
    }
    saveConfig = getProfileLong( keySaveConfig );

} /* readConfigFileName */

/*
 * writeConfigFile - write the current config file name
 */
static void writeConfigFile( void )
{
    struct stat cfg;

    writeProfileLong( keySaveConfig, EditFlags.SaveConfig );
    if( !EditFlags.SaveConfig ) {
        writeProfileLong( keyCfgTime, cfgTime );
        return;
    }
    writeProfileLong( keyChildrenMaximized, 0 );
    GenerateConfiguration( cfgFile, FALSE );    /* never write over %watcom%\eddat\weditor.ini */
    stat( cfgFile, &cfg );
    writeProfileLong( keyCfgTime, cfg.st_mtime );

} /* writeConfigFile */

/*
 * SetSaveConfig - set SaveConfig flag to value that was found in profile
 */
void SetSaveConfig( void )
{
    EditFlags.SaveConfig = saveConfig;

} /* SetSaveConfig */

/*
 * ReadProfile - read our current status from the .ini file
 */
void ReadProfile( void )
{
    getConfigFilePaths();   /* get paths to ini files */
    readToolBarSize();
    readInitialPosition();
    readConfigFileName();

} /* ReadProfile */

/*
 * WriteProfile - write our current status to the .ini file
 */
void WriteProfile( void )
{
    writeToolBarSize();
    writeInitialPosition();
    writeConfigFile();

} /* WriteProfile */

/*
 * FiniProfile - free the profile path strings
 */
void FiniProfile( void )
{
    DeleteString( &cfgFile );
    DeleteString( &iniFile );
    DeleteString( &iniPath );

} /* FiniProfile */
