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
* Description:  Functions controlling the abstract type "a_variable".
*               The only reference to the GlobalVarList is through these
*               functions.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>

#include "gui.h"
#include "guiutil.h"
#include "setupinf.h"
#include "setup.h"
#include "utils.h"
#include "dlggen.h"
#include "dynarray.h"
#include "hash.h"
#include "genvbl.h"

#ifdef __NT__
    #include <windows.h>
#endif

// If too slow make hash size bigger
#define HASH_SIZE   1021

typedef struct  a_variable {
    char        *name;
    unsigned    id          : 14;
    unsigned    has_value   : 1;
    char        *strval;    /* value */
    char        *autoset;
    char        restriction;
    void        (*hook)( vhandle );
} a_variable;

            //only reference to this Structure is through functions.
static a_variable   *GlobalVarList = NULL;
static array_info   GlobalVarArray;
static hash_table   *GlobalVarHash = NULL;


vhandle NextGlobalVar( vhandle var_handle )
/*****************************************/
{
    if( var_handle == NO_VAR )
        return( 0 );
    if( ++var_handle >= GlobalVarArray.num )
        return( NO_VAR );
    return( var_handle );
}

extern void InitGlobalVarList( void )
/***********************************/
/* must be called before any other reference to GlobalVarList is made */
{
    GlobalVarArray.num = 0;
    GlobalVarArray.alloc = 20;
    GlobalVarArray.increment = 20;
    InitArray( (void **)&GlobalVarList, sizeof( a_variable ), &GlobalVarArray );
    GlobalVarHash = HashInit( HASH_SIZE, &stricmp );
}


void VarSetAutoSetCond( vhandle var_handle, char *cond )
/******************************************************/
{
    GUIStrDup( cond, &GlobalVarList[var_handle].autoset );
}


void VarSetAutoSetRestriction( vhandle var_handle, char *cond )
/*************************************************************/
{
    GlobalVarList[var_handle].restriction = cond[0]; // 't' or 'f'
}


int VarIsRestrictedFalse( vhandle var_handle )
/********************************************/
{
    return( GlobalVarList[var_handle].restriction == 'f' );
}


int VarIsRestrictedTrue( vhandle var_handle )
/*******************************************/
{
    return( GlobalVarList[var_handle].restriction == 't' );
}


const char *VarGetAutoSetCond( vhandle var_handle )
/*************************************************/
{
    if( GlobalVarList[var_handle].restriction == 't' )
        return( "true" );
    if( GlobalVarList[var_handle].restriction == 'f' )
        return( "false" );
    return( GlobalVarList[var_handle].autoset );
}

extern vhandle GetVariableByName( const char *vbl_name )
/******************************************************/
{
    int        i;

    if( GlobalVarHash ) {
        return( HashFind( GlobalVarHash, vbl_name ) );
    } else {
        for( i = 0; i < GlobalVarArray.num; i++ ) {
            if( stricmp( GlobalVarList[i].name, vbl_name ) == 0 ) {
                return( i );
            }
        }
    }
    return( NO_VAR );
}

extern vhandle GetVariableById( int id )
/**************************************/
{
    // id is always the same as var_handle!
    if( id >= GlobalVarArray.num )
        return( NO_VAR );
    return( id );
}


extern const char *VarGetName( vhandle var_handle )
/*************************************************/
{
    if( var_handle == NO_VAR )
        return( "" );
    return( GlobalVarList[var_handle].name );
}


extern int VarGetId( vhandle var_handle )
/***************************************/
{
    if( var_handle == NO_VAR )
        return( 0 );
    return( GlobalVarList[var_handle].id );
}


extern const char *VarGetStrVal( vhandle var_handle )
/***************************************************/
{
    if( var_handle == NO_VAR )
        return( "" );
    if( !GlobalVarList[var_handle].has_value )
        return( "" );
    return( GlobalVarList[var_handle].strval );
}

extern int VarGetIntVal( vhandle var_handle )
/*******************************************/
{
    return( atoi( VarGetStrVal( var_handle ) ) );
}

extern bool VarHasValue( vhandle var_handle )
/*******************************************/
{
    if( var_handle == NO_VAR )
        return( FALSE );
    return( GlobalVarList[var_handle].has_value );
}

extern void VarSetHook( vhandle var_handle, void (*hook)( vhandle ) )
/*******************************************************************/
{
    if( var_handle == NO_VAR )
        return;
    GlobalVarList[var_handle].hook = hook;
}

extern int GetVariableIntVal( const char *vbl_name )
/**************************************************/
{
    return( VarGetIntVal( GetVariableByName( vbl_name ) ) );
}

extern const char *GetVariableStrVal( const char *vbl_name )
/**********************************************************/
{
    return( VarGetStrVal( GetVariableByName( vbl_name ) ) );
}

static vhandle NewVariable( char *vbl_name )
/******************************************/
{
    a_variable  *tmp_variable;
    vhandle     var_handle;

    var_handle = GlobalVarArray.num;
    BumpArray( &GlobalVarArray );
    tmp_variable = &GlobalVarList[var_handle];
    GUIStrDup( vbl_name, &tmp_variable->name );
    tmp_variable->id = var_handle;
    tmp_variable->has_value = FALSE;
    tmp_variable->autoset = NULL;
    tmp_variable->restriction = 0;
    tmp_variable->hook = NULL;
    tmp_variable->strval = NULL;
    if( GlobalVarHash ) {
        HashInsert( GlobalVarHash, vbl_name, var_handle );
    }
    return( var_handle );
}

extern vhandle AddVariable( char *vbl_name )
/******************************************/
{
    vhandle var_handle;

    var_handle = GetVariableByName( vbl_name );
    if( var_handle == NO_VAR )
        var_handle = NewVariable( vbl_name );
    return( var_handle );
}

static vhandle DoSetVariable( vhandle var_handle, const char *strval, char *vbl_name )
/************************************************************************************/
{
    a_variable  *tmp_variable;

    if( strval == NULL ) {
        strval = "";
    }
    if( var_handle != NO_VAR ) {
        tmp_variable = &GlobalVarList[var_handle];
        if( tmp_variable->has_value ) {
            if( strcmp( tmp_variable->strval, strval ) == 0 ) {
                if( tmp_variable->hook ) {
                    tmp_variable->hook( var_handle );
                }
                return( var_handle );
            }
            GUIMemFree( tmp_variable->strval );   // free the old string
        }
    } else {
        var_handle = NewVariable( vbl_name );
    }
    tmp_variable = &GlobalVarList[var_handle];
    GUIStrDup( strval, &tmp_variable->strval );
    tmp_variable->has_value = TRUE;
    if( tmp_variable->hook ) {
        tmp_variable->hook( var_handle );
    }
    return( var_handle );
}

extern vhandle SetVariableByName( char *vbl_name, const char *strval )
/********************************************************************/
{
    return( DoSetVariable( GetVariableByName( vbl_name ), strval, vbl_name ) );
}

extern vhandle SetVariableByHandle( vhandle var_handle, const char *strval )
/**************************************************************************/
{
    if( var_handle == NO_VAR )
        return( NO_VAR );
    return( DoSetVariable( var_handle, strval, NULL ) );
}

extern void SetDefaultGlobalVarList( void )
/*****************************************/
{
    char    szBuf[_MAX_PATH];
#if defined( __NT__ )
    char *  last_slash;
#endif

    // create global variables for each default system
    SetVariableByName( "true", "1" );
    SetVariableByName( "false", "0" );
#if defined( __DOS__ )
    SetVariableByName( "IsDos", "1" );
    SetVariableByName( "IsOS2DosBox", _osmajor >= 10 ? "1" : "0" );
#else
    SetVariableByName( "IsDos", "0" );
#endif

#if defined( __WINDOWS__ )
    SetVariableByName( "IsWin", "1" );
    SetVariableByName( "IsOS2DosBox", _osmajor >= 10 ? "1" : "0" );
#else
    SetVariableByName( "IsWin", "0" );
#endif

#if defined( __OS2__ )
    SetVariableByName( "IsOS2", "1" );
#else
    SetVariableByName( "IsOS2", "0" );
#endif

#if defined( __NT__ )
    {
        DWORD   version = GetVersion();
        if( version < 0x80000000 ) {
            SetVariableByName( "IsWinNT", "1" );
            SetVariableByName( "IsWin32", "1" );
            if( LOBYTE( LOWORD( version ) ) >= 4 ) {
                SetVariableByName( "IsWinNT40", "1" );
            } else {
                SetVariableByName( "IsWinNT40", "0" );
            }
        } else if( LOBYTE( LOWORD( version ) ) < 4 ) {
            SetVariableByName( "IsWin32s", "1" );
        } else {
            SetVariableByName( "IsWin95", "1" );
            SetVariableByName( "IsWin32", "1" );
        }
        if( LOBYTE( LOWORD( version ) ) >= 5 ) {
            SetVariableByName( "IsWin98", "1" );
        } else {
            SetVariableByName( "IsWin98", "0" );
        }
    }
#else
    SetVariableByName( "IsWinNT", "0" );
    SetVariableByName( "IsWin95", "0" );
#endif
#if defined( __WINDOWS__ ) || defined( __NT__ )
    GetSystemDirectory( szBuf, sizeof( szBuf ) );
#else
    strcpy( szBuf, "C:\\WINDOWS\\SYSTEM" );
#endif
    SetVariableByName( "WinSystemDir", szBuf );
#if defined( __NT__ )
    last_slash = strrchr( szBuf, '\\' );
    strcpy( last_slash, "\\SYSTEM" );
#endif
    SetVariableByName( "WinSystem16Dir", szBuf );
#if defined( __WINDOWS__ ) || defined( __NT__ )
    GetWindowsDirectory( szBuf, sizeof( szBuf ) );
#else
    strcpy( szBuf, "C:\\WINDOWS" );
#endif
    SetVariableByName( "WinDir", szBuf );

#if defined( __NT__ )
    if( !SearchPath( NULL, "CMD.EXE", NULL, sizeof( szBuf ), szBuf, NULL ) ) {
        if( !SearchPath( NULL, "COMMAND.COM", NULL, sizeof( szBuf ), szBuf, NULL ) ) {
#endif
            strcpy( szBuf, "COMMAND.COM" );
#if defined( __NT__ )
        }
    }
#endif
    SetVariableByName( "Command", szBuf );

#if defined( __AXP__ )
    SetVariableByName( "IsAlpha", "1" );
#else
    SetVariableByName( "IsAlpha", "0" );
#endif

#if defined( __LINUX__ )
    SetVariableByName( "IsLinux", "1" );
#else
    SetVariableByName( "IsLinux", "0" );
#endif
}


extern void FreeGlobalVarList( bool including_real_globals )
/**********************************************************/
{
    int i, j;

    if( GlobalVarList == NULL )
        return;

    if( including_real_globals ) {
        for( i = 0; i < GlobalVarArray.num; i++ ) {
            GUIMemFree( GlobalVarList[i].name );
            GUIMemFree( GlobalVarList[i].strval );
            GUIMemFree( GlobalVarList[i].autoset );
        }
        GlobalVarArray.num = 0;
        GUIMemFree( GlobalVarList );
        if( GlobalVarHash ) {
            HashFini( GlobalVarHash );
            GlobalVarHash = NULL;
        }
    } else {
        for( i = 0; i < GlobalVarArray.num; ) {
            if( GlobalVarList[i].name[0] != '$' ) {
                GUIMemFree( GlobalVarList[i].name );
                GUIMemFree( GlobalVarList[i].strval );
                GUIMemFree( GlobalVarList[i].autoset );

                for( j = i; j < GlobalVarArray.num - 1; j++ ) {
                    memcpy( &GlobalVarList[j], &GlobalVarList[j + 1],
                        sizeof( a_variable ) );
                    GlobalVarList[j].id = j;
                    // This destroys the concept that a handle to a variable
                    // will always point to the same variable.  Between
                    // script launches, variable ids will change.
                }
                GlobalVarArray.num -= 1;
                BumpDownArray( &GlobalVarArray );
            } else {
                i++;
            }
        }
        // We have to rebuild the hash table
        if( GlobalVarHash ) {
            HashFini( GlobalVarHash );
            GlobalVarHash = HashInit( HASH_SIZE, &stricmp );
            for( i = 0; i < GlobalVarArray.num; i++ ) {
                HashInsert( GlobalVarHash, GlobalVarList[i].name, i );
            }
        }
    }
}

