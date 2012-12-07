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
#include <setjmp.h>
#include "source.h"
#include "expr.h"
#include "rxsupp.h"


static int      bPos;
static char     *bPtr;
static int      recurseDepth;

/*
 * addChar - add a character to new expanded line
 */
static bool addChar( char ch )
{
    bPtr[bPos] = ch;
    bPos++;
    if( bPos >= MAX_SRC_LINE - 1 ) {
        bPtr[bPos] = 0;
        return( FALSE );
    }
    return( TRUE );

} /* addChar */

/*
 * Expand - expand local variables
 */
void Expand( char *data, vlist *vl )
{
    char        result[MAX_SRC_LINE];
    char        varname[MAX_SRC_LINE];
    char        *ptr, *odata;
    int         paren_level;
    char        ch;
    bool        done;
    bool        has_var;
    bool        need_closebracket;
    vars        *v;
    char        *obptr;
    int         obpos;

    obpos = bPos;
    obptr = bPtr;
    if( recurseDepth >= 3 ) {
        return;
    }
    recurseDepth++;
    odata = data;

    bPos = 0;
    bPtr = result;

    while( 1 ) {
        ch = *data++;
        if( ch == '%' ) {
            if( *data == '%' ) {
                if( !addChar( '%' ) ) {
                    break;
                }
                data++;
                continue;
            }
            has_var = FALSE;
            if( *data != '(' ) {
                varname[0] = *data++;
                varname[1] = 0;
            } else {
                data++;
                paren_level = 1;
                ptr = varname;
                while( 1 ) {
                    ch = *data++;
                    if( ch == 0 ) {
                        break;
                    }
                    if( ch == '%' ) {
                        has_var = TRUE;
                    } else if( ch == '(' ) {
                        paren_level++;
                    } else if( ch == ')' ) {
                        paren_level--;
                        if( paren_level == 0 ) {
                            break;
                        }
                    }
                    *ptr++ = ch;
                }
                *ptr = 0;
            }
            if( has_var ) {
                Expand( varname, vl );
            }

            v = VarFind( varname, vl );
            if( v != NULL ) {
                ptr = v->value;
                if( EditFlags.CompileScript && !EditFlags.CompileAssignmentsDammit ) {
                    if( varname[0] >= 'A' && varname[0] <= 'Z' ) {
                        ptr = varname;
                    }
                }
            } else {
                ptr = varname;
            }
            done = FALSE;
            need_closebracket = FALSE;
            if( ptr == varname ) {
                if( !addChar( '%' ) ) {
                    done = TRUE;
                } else if( varname[1] != 0 ) {
                    if( !addChar( '(' ) ) {
                        done = TRUE;
                    } else {
                        need_closebracket = TRUE;
                    }
                }
            }

            while( !done ) {
                if( *ptr == 0 ) {
                    break;
                }
                if( !addChar( *ptr ) ) {
                    done = TRUE;
                    break;
                }
                ptr++;
            }
            if( done ) {
                break;
            }
            if( need_closebracket ) {
                if( !addChar( ')' ) ) {
                    done = TRUE;
                    break;
                }
            }
        } else {
            if( !addChar( ch ) ) {
                break;
            }
            if( ch == 0 ) {
                break;
            }
        }
    }
    memcpy( odata, result, bPos );
    recurseDepth--;
    bPos = obpos;
    bPtr = obptr;

} /* Expand */
