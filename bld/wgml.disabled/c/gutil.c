/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wgml utility functions
*
*               conv_hor_unit
*               conv_vert_unit
*               skip_to_quote
*               su_layout_special
*               to_internal_SU
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1       /* use safer C library             */

#include "wgml.h"
#include "gvars.h"


char    *skip_to_quote( char * p, char quote )
{
    while( *p && quote != *p ) {
        p++;
    }
    return( p + 1 );
}



/***************************************************************************/
/* extension for layout :BANREGION indent, hoffset and width attributes:   */
/*      symbolic units without a numeric value                             */
/***************************************************************************/
static  bool    su_layout_special( char * * scanp, su * converted )
{
    bool        converterror = false;
    char    *   p;
    char    *   ps;
    su      *   s;
    long        wh;
    long        wd;
    char        quote;

    s = converted;
    p = *scanp;
    ps = s->su_txt;
    *ps = '\0';
    wh = 0;
    wd = 0;
    quote = '\0';

    if( *p == '\'' || *p == '"' ) {     // ignore but remember quote
        quote = *p++;
    }
    if( !strnicmp( "left", p, 4 ) ) {
        s->su_u = SU_lay_left;
        strcpy( ps, "left" );
    } else if( !strnicmp( "right", p, 5 ) ) {
        s->su_u = SU_lay_right;
        strcpy( ps, "right" );
    } else if( !(strnicmp( "center", p, 6 )) && (strnicmp( "centre", p, 6 )) ) {
        s->su_u = SU_lay_centre;
        strcpy( ps, "centre" );
    } else if( !strnicmp( "extend", p, 6 ) ) {
        s->su_u = SU_lay_extend;
        strcpy( ps, "extend" );
    } else {
        converterror = true;
    }
    if( !converterror ) {
        s->su_whole = 0;
        s->su_dec   = 0;
        s->su_inch  = 0;
        s->su_mm    = 0;
        s->su_relative = false;
    }
    return( converterror );
}

/***************************************************************************/
/*  conversion routines for Horizontal / Vertical space units              */
/*  Accepted formats:                                                      */
/*       1234        assume chars / lines                                  */
/*       8m          Ems                                                   */
/*       22Dv        Device units                                          */
/*                                                                         */
/*       12.34i      inch             (1 inch = 2.54cm)                    */
/*                      The exact definition of Cicero and Pica (points)   */
/*                      differs between Europe and USA,                    */
/*                      so until a better solution turns up:               */
/*       5C11        Cicero  + points (12 points = 1C = 1/6 inch)          */
/*       6p10        Pica    + points (12 points = 1P = 1/6 inch)          */
/*       5.23cm      centimeter                                            */
/*       6.75mm      millimeter                                            */
/*                                                                         */
/*    the absolute units (the last 5) will be stored                       */
/*    in 0.0001 millimeter units and 0.0001 inch units,                    */
/*    the relative ones (the first 3) will not be converted.               */
/*                                                                         */
/* extension for layout :BANREGION indent, hoffset and width attributes:   */
/*      symbolic units without a numeric value                             */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*    returns  filled structure su, returncode false                       */
/*               or  returncode true in case of error                      */
/***************************************************************************/

bool    to_internal_SU( char * * scanp, su * converted )
{
    bool        converterror = true;
    char    *   p;
    char    *   ps;
    ldiv_t      div;
    su      *   s;
    long        wh;
    long        wd;
    char    *   pp;                  // ptr to decimal point or embedded unit
    char    *   pu;                     // ptr to trailing unit
    char    *   pd1;                    // ptr to 0.1 decimal
    char    *   pdn;                    // ptr to last digit +1
    char        unit[4];
    char        quote;
    long        k;
    char        sign;

    unit[3] = '\0';
    unit[2] = '\0';
    unit[1] = '\0';
    unit[0] = '\0';
    s = converted;
    p = *scanp;
    ps = s->su_txt;
    *ps = '\0';
    wh = 0;
    wd = 0;
    pp = NULL;
    quote = '\0';

    s->su_u = SU_undefined;
    if( *p == '\'' || *p == '"' ) {     // ignore but remember quote
        quote = *p++;
    }
    if( *p == '+' || *p == '-' ) {
        sign = *p;
        *ps++ = *p++;
        s->su_relative = true;  // value is added / subtracted from old value
    } else {
        sign = '+';
        s->su_relative = false;         // value replaces old value
    }

    /***********************************************************************/
    /*  Special for layout :BANREGION                                      */
    /***********************************************************************/

    if( quote == '\0' && isalpha( *p ) ) {
        converterror = su_layout_special( scanp, converted );
        if( !converterror ) {
            return( converterror );     // layout special ok
        }
    }
    while( *p >= '0' && *p <= '9' ) {   // whole part
        wh = 10 * wh + *p - '0';
        *ps++ = *p++;
        if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
    }
    pp = p;
    k = 0;
    while( *p && isalpha( *p ) ) {
        unit[k++] = tolower( *p );      // save Unit
        *ps++ = *p++;
        if( ps >= s->su_txt + sizeof( s->su_txt ) || k > 2 ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
    }
    if( p > pp + 2 ) {
         if( quote ) {
             p = skip_to_quote( p, quote );
         }
         *scanp = p;
         return( converterror );        // no unit has more than 2 chars
    }
    pd1 = NULL;
    pdn = NULL;

    if( p == pp && *p == '.' ) {        // no unit found, try dec point
        *ps++ = *p++;
        pd1 = p;                        // remember start of decimals
        if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
        while( *p >= '0' && *p <= '9' ) {   // try decimal part
            wd = 10 * wd + *p - '0';
            *ps++ = *p++;
            if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
            if( p > pd1 + 2 ) {         // more than two digits
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
        }
        pdn = p;
    } else {
        if( k ) {                       // unit found
            pd1 = p;
            if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
            while( *p >= '0' && *p <= '9' ) {   // try decimal part
                wd = 10 * wd + *p - '0';
                *ps++ = *p++;
                if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                    if( quote ) {
                        p = skip_to_quote( p, quote );
                    }
                    *scanp = p;
                    return( converterror );
                }
                if( p > pd1 + 2 ) {     // more than two digits
                    if( quote ) {
                        p = skip_to_quote( p, quote );
                    }
                    *scanp = p;
                    return( converterror );
                }
            }
            pdn = p;
        }
    }
    k = 0;
    pu = p;
    if( *p ) {                          // not yet at end
        while( *p && isalpha( *p ) ) {  // try trailing unit
            unit[k++] = tolower( *p );
            *ps++ = *p++;
            if( ps >= s->su_txt + sizeof( s->su_txt ) || k > 2 ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
        }
    }

    *ps = '\0';

    s->su_whole = wh;
    s->su_dec   = wd;

    if( (quote && *p != quote ) || (!quote && *p == '\'') ) {
        if( quote ) {
            p = skip_to_quote( p, quote );
        }
        *scanp = p;
        return( converterror );
    }
    if( quote ) {
        p++;                            // over quote
    }

    *scanp = p;                         // report back scan position

    if( k == 0 ) {                      // no trailing unit
        pu = NULL;
    } else {
        if( pu == pp ) {                // no decimals, no unit
            pu = NULL;
        }
    }
    if( *pp == '.' ) {                  // dec point found
        if( pu == NULL ) {              // need trailing unit
            return( converterror );
        }
    } else {                            // no decimals
        if( pu != NULL ) {              // but unit specified twice?
            return( converterror );
        }
    }

    /***********************************************************************/
    /*  check for valid unit                                               */
    /***********************************************************************/
    if( unit[1] == '\0' ) {           // single letter unit
        switch( unit[0] ) {
        case 'i' :
            s->su_u = SU_inch;
            break;
        case 'd' :
            s->su_u = SU_dv;
            break;
        case 'm' :
            s->su_u = SU_ems;
            break;
        case 'c' :
            s->su_u = SU_cicero;
            break;
        case 'p' :
            s->su_u = SU_pica;
            break;
        case '\0' :                     // no unit is characters or lines
            s->su_u = SU_chars_lines;
            break;
        default:
            return( converterror );
            break;
        }
    } else {                            // two letter unit
        if( unit[1] == 'm' ) {          // cm, mm ?
            if( unit[0] == 'c' ) {
                s->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                s->su_u = SU_mm;
            } else {                    // invalid unit
                return( converterror );
            }
        } else {                        // invalid unit
            return( converterror );
        }
    }

    s->su_inch = 0;
    s->su_mm   = 0;
    k = 1;
    if( pd1 != NULL ) {
        if( pdn - pd1 == 1 ) {
            k = 10;                 // only 0.1 digit
        }
    }
    switch( s->su_u ) {
    // the relative units are only stored, not converted
    case SU_chars_lines :
    case SU_dv :
    case SU_ems :
        if( wd != 0 ) {                 // no decimals allowed
            return( converterror );
        }
        break;
    case SU_inch :                      // inch, cm, mm valid with decimals
        s->su_mm   = (wh * 100L + wd * k) * 2540L;
        s->su_inch = (wh * 100L + wd * k) *  100L;
        break;
    case SU_cm :
        s->su_mm   = (wh * 100L + wd * k) * 1000L;
        s->su_inch = s->su_mm * 10L / 254L;
        break;
    case SU_mm :
        s->su_mm   = (wh * 100L + wd * k) *  100L;
        s->su_inch = s->su_mm * 10L / 254L;
        break;
    case SU_cicero :
    case SU_pica :                      // pica / Cicero
        if( wd > 11 ) {
            div = ldiv( wd, 12L);
            wh += div.quot;
            wd = div.rem;
        }
        s->su_inch = wh * 10000L / 6L + wd * 10000L / 72L;
        s->su_mm = s->su_inch * 254L / 10L;
        break;
    default:
        break;
    }
    if( sign == '-' ) {
        s->su_inch  = -s->su_inch;
        s->su_mm    = -s->su_mm;
        s->su_whole = -s->su_whole;
    }
    converterror = false;
    return( converterror );
}

/***************************************************************************/
/*  convert internal space units to device space units                     */
/*   use font 0 or current font???                              TBD        */
/*  return value is signed as space unit can be relative (+ -)             */
/***************************************************************************/

int32_t conv_hor_unit( su * s )
{
    int32_t    ds;

    switch( s->su_u ) {
    case SU_chars_lines :
        ds = s->su_whole * bin_device->horizontal_base_units / CPI;
        break;
    case SU_dv :
    case SU_ems :
        ds = s->su_whole * wgml_fonts[g_curr_font_num].default_width;
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        ds = (int64_t)s->su_inch * bin_device->horizontal_base_units / 10000L;
        break;
    default:
        ds = 0;
        break;
    }
    return( ds );
}

int32_t conv_vert_unit( su * s, uint8_t spc )
{
    int32_t    ds;
    uint8_t space;

    if( spc > 0 ) {                     // if spacing valid use it
        space = spc;
    } else {
        space = spacing;                // else default
    }
    switch( s->su_u ) {
    case SU_chars_lines :
        ds = space * s->su_whole * wgml_fonts[g_curr_font_num].line_height;
        break;
    case SU_dv :
    case SU_ems :
        ds = s->su_whole * wgml_fonts[g_curr_font_num].line_height;
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        ds = (int64_t)s->su_inch * bin_device->vertical_base_units / 10000L;
        break;
    default:
        ds = 0;
        break;
    }
    return( ds );
}



#if 0
int main( int argc, char *argv[] )      // standalone test routine
{
    bool    error;
    su      aus;
    char    ein1[] = "1.5I";
//  char    ein1[] = "3.81cm";
//  char    ein1[] = "38.1mm";
//  char    *ein1 = "'6p11'";
//  char    *ein1 = "'1C'";
//  char    *ein2 = "'1C12'";
    char    *ein2 = "'5C12'";
    char    *ein3 = "'0P73'";
    char    *ein4 = "'5P6'";

    char   *p = ein1;
    error = to_internal_SU( &p, &aus );

    p = ein2;
    error = to_internal_SU( &p, &aus );

    p = ein3;
    error = to_internal_SU( &p, &aus );

    p = ein4;
    error = to_internal_SU( &p, &aus );

    return(0);
}
#endif

