/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML controls multi letter functions  &'substr(), ...
*               and calls corresponding function see gsfuncs.h
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*    SCR multi letter functions                                           */
/***************************************************************************/

#define pick( name, length, parms, optparms, routine ) \
            { name, length, parms, optparms, routine },

static  const   scrfunc scr_functions[] = {

#include "gsfuncs.h"

    { " ", 0, 0, 0, NULL }              // end
};

#define SCR_FUNC_MAX (sizeof( scr_functions ) / sizeof( scr_functions[0] ) - 1)

static const char   ampchar = '&';
static bool         multiletter_function;


/*  find end of parm for multi letter functions
 *     end of parm is either , or )
 *     but only if outside of string and not in deeper paren level
 *      string delimiters are several chars
 *
 */

static  char    * find_end_of_parm( char * pchar, char * pend )
{
#define max_paren 50                    // should be enough
    char    quotechar[max_paren];
    bool    instring[max_paren];
    int     paren_level;
    char    c;
    char    cm1;
    char    cm2;
    bool    finished;

    paren_level = 0;
    quotechar[paren_level] ='\0';
    instring[paren_level] = false;
    finished = false;
    cm1 = '\0';
    cm2 = '\0';
    for(  ; *pchar != '\0' ; pchar++ ) {
        cm2 = cm1;
        cm1 = c;
        c = *pchar;
        if( (cm1 == ampchar) && (c == '\'') ) {
            multiletter_function = true;
        }
        if( instring[paren_level] ) {
            if( c == quotechar[paren_level] ) {
                instring[paren_level] = false;
            }
        } else {
            if( is_quote_char( c ) ) {
                if( (cm1 == ampchar) || // &' sequence
                    ((cm2 == ampchar) && isalpha( cm1 )) ) {// &X' sequence
                            /* no instring change */
                } else {
                    instring[paren_level] = true;
                    quotechar[paren_level] = c;
                }
            } else {
                switch( c ) {
                case    '(' :
                    paren_level++;
                    if( paren_level < max_paren ) {
                        instring[paren_level] = false;
                    } else {
                        paren_level--;
                        finished = true;// error msg ??? TBD
                    }
                    break;
                case    ')' :
                    if( paren_level == 0 ) {
                        finished = true;
                    }
                    paren_level--;
                    break;
                case    ',' :
                    if( paren_level == 0 ) {
                        finished = true;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if( finished || (pchar >= pend)) {
            break;
        }
    }
    return( pchar );
}


/***************************************************************************/
/*  err_info  give some info about error                                   */
/***************************************************************************/

static  void    err_info( char * * result )
{
    char                linestr[MAX_L_AS_STR];

    if( input_cbs->fmflags & II_macro ) {
        utoa( input_cbs->s.m->lineno, linestr, 10 );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        utoa( input_cbs->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
    err_count++;
    show_include_stack();

    **result = '&';                 // result is & to preserve the input
    *result += 1;
    **result = '\0';
}


/***************************************************************************/
/*  scr_multi_funcs  isolate function name, lookup name in table           */
/*                   scan function arguments                               */
/*                   call corresponding function                           */
/***************************************************************************/

char    *scr_multi_funcs( char * in, char * end, char * * result, int32_t valsize )
{
    char            *   pchar;
    int                 rc;
    size_t              fnlen;
    int                 funcind;
    int                 k;
    int                 m;
    char                fn[FUN_NAME_LENGTH + 1];
    bool                found;
    parm                parms[MAX_FUN_PARMS];
    int                 parmcount;
    condcode            cc;

    rc = 0;
    fnlen = 0;
    pchar = in + 2;                     // over &'

    // collect function name
    while( *pchar && pchar <= end && is_function_char( *pchar ) ) {
        fn[fnlen] = *pchar++;
        if( fnlen < FUN_NAME_LENGTH ) {
            fnlen++;
        } else {
            break;
        }
    }
    fn[fnlen] = '\0';

    if( *pchar != '(' ) {         // open paren does not follow function name
        g_err( err_func_parm_miss );
        err_info( result );
        return( in + 1 );               // but avoid endless loop
    }

    // test for valid functionname
    found = false;
    for( k = 0; k < SCR_FUNC_MAX; k++ ) {
        if( fnlen == scr_functions[k].length
            && !stricmp( fn, scr_functions[k].fname ) ) {

            found = true;
            if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                out_msg( " Function %s found\n", scr_functions[k].fname );
            }
            break;
        }
    }
    if( !found ) {
        g_err( err_func_name, fn );
        err_info( result );
        return( in + 1 );               // but avoid endless loop
    }
    funcind = k;

    // collect the mandatory parm(s)

    for( k = 0; k < scr_functions[funcind].parm_cnt; k++ ) {

        parms[k].a = ++pchar;           // first over ( then over ,

        pchar = find_end_of_parm( pchar, end );

        parms[k].e = pchar - 1;
        parms[k + 1].a = pchar + 1;

        if( (*pchar == ')') || (pchar >= end) ) {
            break;                      // end of parms
        }
    }
    m = k + (k < scr_functions[funcind].parm_cnt);// mandatory parm count

    if( m < scr_functions[funcind].parm_cnt ) {
        g_err( err_func_parm_miss );
        err_info( result );
        return( in + 1 );               // but avoid endless loop
    }



    // collect the optional parm(s)
    if( (*pchar == ')') || (pchar >= end) ) {   // no optional parms
        k = 0;
    } else {
        for( k = 0; k < scr_functions[funcind].opt_parm_cnt; k++ ) {
            parms[m + k].a = ++pchar;

            pchar = find_end_of_parm( pchar, end );

            parms[m + k].e     = pchar - 1;
            parms[m + k + 1].a = pchar + 1;
            if( (*pchar == ')') || (pchar >= end) ) {
                break;                  // end of parms
            }
        }
        k +=  (k < scr_functions[funcind].opt_parm_cnt);
    }
    parmcount = m + k;                  // total parmcount
    parms[parmcount].a = NULL;          // end of parms indicator

    if( *pchar != ')' ) {
        g_err( err_func_parm_end );
        err_info( result );
        return( in + 1 );               // but avoid endless loop
    }

    ProcFlags.suppress_msg = multiletter_function;

    cc = scr_functions[funcind].fun( parms, parmcount, result, valsize );

    ProcFlags.suppress_msg = false;
    if( cc != pos ) {                   // error in function
        **result = '&';                 // result is & to preserve the input

        ProcFlags.unresolved = true;
        *result += 1;
        **result = '\0';
        return( in + 1 );
    }

    ProcFlags.substituted = true;
    return( pchar + 1 );                // all OK new scan position
}


#if 0

+++++++++++++++++++++++++++++++  used functions in OW documentation +++++++++
used single letter functions: others are unused
&e'  exist 0 or 1
&l'  length, if undefined length of name
&s'  subscript   low
&S'  superscript high
&u'  upper

used other functions:

&'delstr(
&'d2c(
&'index(
&'insert(
&'left(
&'length(
&'lower(
&'min(
&'pos(
&'right(
&'strip(
&'substr(
&'subword(
&'translate(
&'upper(
&'veclastpos(
&'vecpos(
&'word(
&'wordpos(
&'words(

#endif
