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
* Description:  Langdat tool mainline.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#ifdef __UNIX__
#include <stdlib.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <env.h>
#endif
#include "watcom.h"
#include "builder.h"

#define DEFCTLNAME      "files.dat"
#define DEFCTLENV       "FILES_DAT"

bool            Quiet;
FILE            *LogFile;
static ctl_file *CtlList;
       include  *IncludeStk;
static char     Line[MAX_LINE];
static char     ProcLine[MAX_LINE];
static unsigned VerbLevel;
static bool     UndefWarn;
static unsigned ParmCount;
static char     *Product = NULL;

/* Defaults for all output values */
static char     *DefType   = NULL;
static char     *DefRedist = NULL;
static char     *DefDir    = NULL;
static char     *DefUsr    = NULL;
static char     *DefRel    = NULL;
static char     *DefCond   = NULL;
static char     *DefPack   = NULL;
static char     *DefWhere  = NULL;
static char     *DefDesc   = NULL;
static char     *DefOld    = NULL;
static char     *DefPatch  = NULL;
static char     *DefDstvar = NULL;


static void AddCtlFile( const char *name )
{
    ctl_file    **owner;
    ctl_file    *curr;

    owner = &CtlList;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
        break;
        owner = &curr->next;
    }
    curr = Alloc( sizeof( *curr ) );
    curr->next = NULL;
    strcpy( curr->name, name );
    *owner = curr;
}

static char **getvalue( char **argv, char *buff )
{
    if( argv[0][2] != '\0' ) {
        strcpy( buff, &argv[0][2] );
        return( argv );
    }
    ++argv;
    strcpy( buff, argv[0] );
    return( argv );
}

static void Usage( void )
{
    printf( "Usage: langdat [-c <ctl_file>]* [-l <log_file>] [-v] [-u] [-q] <product>\n" );
    exit( 0 );
}

static void ProcessOptions( char *argv[] )
{
    char        parm_buff[_MAX_PATH];
    bool        opt_end;

    opt_end = FALSE;
    while( argv[0] != NULL ) {
        if( !opt_end && argv[0][0] == '-' ) {
            switch( tolower( argv[0][1] ) ) {
            case 'c':
                argv = getvalue( argv, parm_buff );
                AddCtlFile( parm_buff );
                break;
            case 'l':
                argv = getvalue( argv, parm_buff );
                if( LogFile != NULL ) {
                    Fatal( "-l option specified twice\n" );
                }
                OpenLog( parm_buff );
                break;
            case 'v':
                ++VerbLevel;
                break;
            case 'u':
                UndefWarn = TRUE;
                break;
            case 'q':
                Quiet = TRUE;
                break;
            case '-':
                opt_end = TRUE;
                break;
            default:
                fprintf( stderr, "Unknown option '%c'\n\n", argv[0][1] );
                /* fall through */
            case '?':
                Usage();
                break;
            }
        } else if( strchr( argv[0], '=' ) != NULL ) {
            putenv( argv[0] );
        } else {
            if( Product != NULL ) {
                Fatal( "Product already set (was %s, now %s)\n", Product, argv[0] );
            }
            Product = argv[0];
        }
        ++argv;
    }
}

static void PushInclude( const char *name )
{
    include     *new;
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fn;
    char        *ext;
    char        dir_name[_MAX_PATH];

    new = Alloc( sizeof( *new ) );
    new->prev = IncludeStk;
    new->skipping = 0;
    new->ifdefskipping = 0;
    new->reset_abit = NULL;
    IncludeStk = new;
    new->fp = fopen( name, "r" );
    if( new->fp == NULL ) {
        Fatal( "Could not open '%s': %s\n", name, strerror( errno ) );
    }
    strcpy( new->name, name );
    _splitpath2( name, buff, &drive, &dir, &fn, &ext );
    _makepath( dir_name, drive, dir, NULL, NULL );
    if( SysChdir( dir_name ) != 0 ) {
        Fatal( "Could not chdir to '%s': %s\n", dir_name, strerror( errno ) );
    }
    getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
}

static bool PopInclude( void )
{
    include     *curr;

    curr = IncludeStk;
    fclose( curr->fp );
    IncludeStk = curr->prev;
    free( curr );
    if( IncludeStk == NULL ) {
        return( FALSE );
    }
    SysChdir( IncludeStk->cwd );
    return( TRUE );
}

static bool GetALine( char *line )
{
    for( ;; ) {
        fgets( line, MAX_LINE, IncludeStk->fp );
        if( ferror( IncludeStk->fp ) ) {
            Fatal( "Error reading '%s': %s\n", IncludeStk->name, strerror( errno ) );
        }
        if( !feof( IncludeStk->fp ) ) {
            break;
        }
        if( !PopInclude() ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

static char *SubstOne( const char **inp, char *out )
{
    const char  *in;
    char        *p;
    char        *starpos;
    char        *rep;
    unsigned    parm;

    in = *inp;
    p = out;
    for( ;; ) {
        switch( *in ) {
        case '>':
            *p = '\0';
            // If the parameter is a number (n) followed by an asterisk,
            // copy from parameter n to the end to out. E.g. <2*>
            parm = 1;
            for( starpos = out; isdigit( *starpos ); starpos++ )
                ;
            if( stricmp( starpos, "*" ) == 0 ) {
                rep = NULL;
                p = out;
                sscanf( out, "%u", &parm );
                for( ; parm <= ParmCount; ++parm ) {
                    sprintf( out, "%d", parm );
                    rep = getenv( out );
                    if( rep != NULL ) {
                        if( out != p )
                            *out++ = ' ';
                        strcpy( out, rep );
                        out += strlen( out );
                    }
                }
                *inp = in + 1;
                *out = '\0';
                return( out );
            } else if( stricmp( out, "CWD" ) == 0 ) {
                rep = IncludeStk->cwd;
            } else {
                rep = getenv( out );
            }
            if( rep == NULL ) {
                if( UndefWarn ) {
                    Log( FALSE, "<%s> is undefined\n", out );
                }
                rep = "";
            }
            strcpy( out, rep );
            *inp = in + 1;
            return( out + strlen( out ) );
        case '<':
            ++in;
            p = SubstOne( &in, p );
            break;
        default:
            *p++ = *in++;
            break;
        }
    }
}

static void SubstLine( const char *in, char *out )
{
    bool        first;

    first = TRUE;
    in = SkipBlanks( in );
    for( ;; ) {
        switch( *in ) {
        case '^':
            ++in;
            switch( *in ) {
            case '\n':
            case '\0':
                break;
            default:
                *out++ = *in++;
                break;
            }
            break;
        case '[':                           // Surround special chars with a space
        case ']':
        case '(':
        case ')':
            if( !first )
                *out++ = ' ';
            *out++ = *in++;
            *out++ = ' ';
            break;
        case '<':
            ++in;
            out = SubstOne( &in, out );
            break;
        case '\n':
        case '\0':
            *out = '\0';
            return;
        default:
            *out++ = *in++;
        }
        first = FALSE;
    }
}

static char *FirstWord( char *p )
{
    char        *start;

    p = SkipBlanks( p );
    if( *p == '\0' )
        return( NULL );
    start = p;
    for( ;; ) {
        switch( *p ) {
        case '\0':
            p[1] = '\0';
            /* fall through */
        case ' ':
        case '\t':
            *p = '\0';
            return( start );
        }
        ++p;
    }
}

static char *NextWord( char *p )
{
    return( FirstWord( p + strlen( p ) + 1 ) );
}

static bool ContainsWord( const char *str, const char *word )
{
    char    *s_copy;
    char    *p;
    bool    result = FALSE;

    s_copy = strdup( str );
    p = FirstWord( s_copy );
    do {
        if( !strcmp( p, word ) ) {
            result = TRUE;
        }
        p = NextWord( p );
    } while( p != NULL );
    free( s_copy );
    return( result );
}

static void ProcessLine( const char *line )
{
    char    *cmd, *p, *str;
    char    *line_copy;
    char    *type, *redist, *dir, *usr, *rel, *cond;
    char    *pack, *where, *desc, *old, *patch, *dstvar;
    int     special = FALSE;

    type = DefType; redist = DefRedist; dir = DefDir; usr = DefUsr;
    rel = DefRel; cond = DefCond; pack = DefPack; where = DefWhere;
    desc = DefDesc; old = DefOld; patch = DefPatch; dstvar = DefDstvar;

    line_copy = strdup( line );
    p = SkipBlanks( line_copy );
    cmd = strtok( p, "=" );
    do {
        str = strtok( NULL, "\"" );
        if( !stricmp( cmd, "echo" ) ) {
            Log( TRUE, "%s\n", str );
            special = TRUE;
            break;
        } else if( !stricmp( cmd, "type" ) ) {
            type = str;
        } else if( !stricmp( cmd, "redist" ) ) {
            redist = str;
        } else if( !stricmp( cmd, "dir" ) ) {
            dir = str;
        } else if( !stricmp( cmd, "usr" ) ) {
            usr = str;
        } else if( !stricmp( cmd, "rel" ) ) {
            rel = str;
        } else if( !stricmp( cmd, "cond" ) ) {
            cond = str;
        } else if( !stricmp( cmd, "pack" ) ) {
            pack = str;
        } else if( !stricmp( cmd, "where" ) ) {
            where = str;
        } else if( !stricmp( cmd, "desc" ) ) {
            desc = str;
        } else if( !stricmp( cmd, "descr" ) ) {     //  Multiple spellings
            desc = str;
        } else if( !stricmp( cmd, "old" ) ) {
            old = str;
        } else if( !stricmp( cmd, "patch" ) ) {
            patch = str;
        } else if( !stricmp( cmd, "dstvar" ) ) {
            dstvar = str;
        } else {
            printf( "langdat warning: unknown keyword %s\n", cmd );
            printf( "(in file %s)\n", IncludeStk->name );
        }
        cmd = strtok( NULL, " \t=" );
    } while( cmd != NULL );
    if( !special ) {
        /* Check if 'where' matches specified product */
        if( !Product || !where || ContainsWord( where, Product ) ) {
            Log( TRUE, "<%s><%s><%s><%s><%s><%s><%s><%s><%s><%s>\n",
                type, redist, dir, old, usr, rel, where, dstvar, cond, desc );
        }
    }
    free( line_copy );
}

static void ProcessDefault( const char *line )
{
    char    *cmd, *p, *q, *str;
    char    *line_copy;

    /* Reset any existing defaults */
    if( DefType != NULL ) free( DefType );
    if( DefRedist != NULL ) free( DefRedist );
    if( DefDir != NULL ) free( DefDir );
    if( DefUsr != NULL ) free( DefUsr );
    if( DefRel != NULL ) free( DefRel );
    if( DefCond != NULL ) free( DefCond );
    if( DefPack != NULL ) free( DefPack );
    if( DefWhere != NULL ) free( DefWhere );
    if( DefDesc != NULL ) free( DefDesc );
    if( DefOld != NULL ) free( DefOld );
    if( DefPatch != NULL ) free( DefPatch );
    if( DefDstvar != NULL ) free( DefDstvar );
    DefType = DefRedist = DefDir = DefUsr = DefRel = DefCond
    = DefPack = DefWhere = DefDesc = DefOld = DefPatch = DefDstvar = NULL;

    /* Process new defaults (if provided) */
    line_copy = strdup( line );
    p = SkipBlanks( line_copy );
    q = strtok( p, "]" );
    q += strlen( q ) - 1;
    while( (q >= p) && ((*q == ' ') || (*q == '\t')) ) --q;
    if( *q == '\"' ) ++q;
    *q = '\0';
    cmd = strtok( p, "=" );
    if( cmd != NULL ) {
        do {
            str = strtok( NULL, "\"" );
            if( !stricmp( cmd, "type" ) ) {
                DefType = strdup( str );
            } else if( !stricmp( cmd, "redist" ) ) {
                DefRedist = strdup( str );
            } else if( !stricmp( cmd, "dir" ) ) {
                DefDir = strdup( str );
            } else if( !stricmp( cmd, "usr" ) ) {
                DefUsr = strdup( str );
            } else if( !stricmp( cmd, "rel" ) ) {
                DefRel = strdup( str );
            } else if( !stricmp( cmd, "cond" ) ) {
                DefCond = strdup( str );
            } else if( !stricmp( cmd, "pack" ) ) {
                DefPack = strdup( str );
            } else if( !stricmp( cmd, "where" ) ) {
                DefWhere = strdup( str );
            } else if( !stricmp( cmd, "desc" ) ) {
                DefDesc = strdup( str );
            } else if( !stricmp( cmd, "old" ) ) {
                DefOld = strdup( str );
            } else if( !stricmp( cmd, "patch" ) ) {
                DefPatch = strdup( str );
            } else if( !stricmp( cmd, "dstvar" ) ) {
                DefDstvar = strdup( str );
            } else {
                printf( "langdat warning: unknown default %s\n", cmd );
                printf( "(in file %s)\n", IncludeStk->name );
            }
            cmd = strtok( NULL, " \t=" );
        } while( cmd != NULL );
    }
    free( line_copy );
}

/****************************************************************************
*
* MatchFound. Examines a string of space separated words. If the first word or
* words (between parentheses) matches any of the words following it, returns 1.
* If not, returns 0. String is terminated by 0 or ']'.
* If there isn't at least one word in the string, terminates program.
*
***************************************************************************/
static int MatchFound( char *p )
{
    char   *Match[20];                     // 20 is enough for builder
    int     MatchWords = 0;
    int     i;
    int     EmptyOk = FALSE;
    int     WordsExamined = 0;

    p = NextWord( p );
    if( p == NULL )
        Fatal( "Missing match word\n" );

    if( *p == '(' ) { // Multiple match words, store them
        p = NextWord( p );
        for( ; MatchWords < 20; ) {
            if( p == NULL )
                Fatal( "Missing match word\n" );
            if( stricmp( p, "\"\"" ) == 0 ) // 'No parameter' indicator
                EmptyOk = TRUE;
            else
                Match[MatchWords++] = p;
            p = NextWord( p );
            if( strcmp( p, ")" ) == 0 ) {
                p = NextWord( p );
                break;
            }
        }
    } else {
        Match[MatchWords++] = p;
        p = NextWord( p );
    }

    // At this point, p must point to the first word after the (last) match word

    for( ;; ) {
        if( p == NULL || strcmp( p, "]" ) == 0 ) { // End of string
            if( WordsExamined == 0 && EmptyOk )
                return 1;
            else
                return 0;
        }
        WordsExamined++;
        for( i = 0; i < MatchWords; i++ )
            if( stricmp( Match[i], p ) == 0 )
                return 1;
        p = NextWord( p );
    }
}

static void ProcessCtlFile( const char *name )
{
    char        *p;
    char        *log_name;
    bool        logit;

    PushInclude( name );
    while( GetALine( Line ) ) {
        SubstLine( Line, ProcLine );
        p = ProcLine;
        switch( *p ) {
        case '#':
        case '\0':
            /* just a comment */
            break;
        case '[':
            /* a directive */
            p = FirstWord( p + 1 );
            if( stricmp( p, "INCLUDE" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    PushInclude( NextWord( p ) );
                }
            }
            else if( stricmp( p, "LOG" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    log_name = NextWord( p );
                    p = NextWord( log_name );
                    if( LogFile == NULL ) {
                        OpenLog( log_name );
                    }
                }
            } else if( stricmp( p, "BLOCK" ) == 0 ) {
                IncludeStk->skipping = 0;   // New block: reset skip flags
                IncludeStk->ifdefskipping = 0;
                if( !MatchFound( p ) )
                    IncludeStk->skipping++;
                break;
            } else if( stricmp( p, "IFDEF" ) == 0 ) {
                if( IncludeStk->ifdefskipping != 0 )
                    IncludeStk->ifdefskipping--;
                if( !MatchFound( p ) )
                    IncludeStk->ifdefskipping++;
                break;
            } else if( stricmp( p, "ENDIF" ) == 0 ) {
                if( IncludeStk->ifdefskipping != 0 )
                    IncludeStk->ifdefskipping--;
                break;
            } else if( stricmp( p, "DEFAULT" ) == 0 ) {
                ProcessDefault( p + strlen( p ) + 1 );
            } else {
                Fatal( "Unknown directive '%s'\n", p );
            }
            break;
        default:
            /* a command */
            logit = ( VerbLevel > 0 );
            if( *p == '@' ) {
                logit = FALSE;
                p = SkipBlanks( p + 1 );
            }
            if( IncludeStk->skipping == 0 && IncludeStk->ifdefskipping == 0 ) {
                if( logit ) {
                    Log( FALSE, "+++<%s>+++\n", p );
                }
                strcpy( Line, p );
                ProcessLine( p );
                LogFlush();
            } else if( logit && ( VerbLevel > 1 ) ) {
                Log( FALSE, "---<%s>---\n", p );
            }
        }
    }
}

static bool SearchUpDirs( const char *name, char *result )
{
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fn;
    char        *ext;
    char        *end;
    FILE        *fp;

    _fullpath( result, name, _MAX_PATH );
    for( ;; ) {
        fp = fopen( result, "r" );
        if( fp != NULL ) {
            fclose( fp );
            return( TRUE );
        }
        _splitpath2( result, buff, &drive, &dir, &fn, &ext );
        end = &dir[strlen( dir ) - 1];
        if( end == dir )
            return( FALSE );
        switch( *end ) {
        case '\\':
        case '/':
            --end;
        }
        for( ;; ) {
            if( end == dir ) {
                *end++ = '/';
                break;
            }
            if( *end == '\\' || *end == '/' )
                break;
            --end;
        }
        *end = '\0';
        _makepath( result, drive, dir, fn, ext );
    }
}


int main( int argc, char *argv[] )
{
    ctl_file    *next;
    char        *p;

    SysInit( argc, argv );
    ProcessOptions( argv + 1 );
    if( Product == NULL ) {
        printf( "langdat warning: no product specified, processing all entries\n" );
    }
    if( CtlList == NULL ) {
        p = getenv( DEFCTLENV );
        if( p == NULL )
            p = DEFCTLNAME;
        if( !SearchUpDirs( p, Line ) ) {
#ifdef __WATCOMC__
            _searchenv( p, "PATH", Line );
#else
            Line[0] = '\0';
#endif
            if( Line[0] == '\0' ) {
                Fatal( "Can not find '%s'\n", p );
            }
        }
        AddCtlFile( Line );
    }
    while( CtlList != NULL ) {
        ProcessCtlFile( CtlList->name );
        next = CtlList->next;
        free( CtlList );
        CtlList = next;
    }
    CloseLog();
    return( 0 );
}
