
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
* Description: implement .sk (skip)  script control word
*              and helper functions
*
*              calc_skip               calculate .sk skip amount
*              calc_skip_value
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"
#include "copfiles.h"


/***************************************************************************/
/*  .sk adjust                                                             */
/*                                                                         */
/*   g_skip_wgml4 contains  correction to mimick wgml4                TBD  */
/*                                                                         */
/***************************************************************************/
void    calc_skip( void )
{
    int32_t     skip_value;

    if( g_skip == 0 ) {
        g_skip_wgml4 = 0;
        ProcFlags.sk_cond = false;
        return;
    }
    if( g_skip < 0 ) {                  // overprint
        if( bin_driver->y_positive == 0x00 ) {
            g_cur_v_start += wgml_fonts[g_curr_font_num].line_height;
        } else {
            g_cur_v_start -= wgml_fonts[g_curr_font_num].line_height;
        }
    } else {
        if( bin_driver->y_positive == 0x00 ) {
            skip_value = g_skip * wgml_fonts[g_curr_font_num].line_height
                       - g_skip_wgml4;
            if( skip_value >= g_cur_v_start - g_page_bottom ) {
                g_cur_v_start = g_page_bottom - 1;  // force new page
            } else {
                g_cur_v_start -= skip_value;
            }
        } else {
            g_cur_v_start += g_skip * wgml_fonts[g_curr_font_num].line_height;
        }
    }

    g_skip = 0;
    g_skip_wgml4 = 0;
    ProcFlags.sk_cond = false;
}


/***************************************************************************/
/*                                                                         */
/*   calc_skip_value                                                       */
/*   used for sequences such as    .sk 5                                   */
/*                                 :p.                                     */
/***************************************************************************/
int32_t calc_skip_value( void )
{
    int32_t     skip_value;

    if( g_skip < 1 ) {
        skip_value = g_skip;
    } else {
        if( bin_driver->y_positive == 0x00 ) {
            skip_value = g_skip * wgml_fonts[g_curr_font_num].line_height
                       - g_skip_wgml4;
        } else {
            skip_value = g_skip * wgml_fonts[g_curr_font_num].line_height;
        }
    }
    g_skip = 0;
    g_skip_wgml4 = 0;
    ProcFlags.sk_cond = false;
    return( skip_value );
}


/**************************************************************************/
/* SKIP generates the specified number of blank lines.                    */
/*                                                                        */
/*      ����������������������������������������������������������Ŀ      */
/*      |       |                                                  |      */
/*      |  .SK  |    <1|v>  <A>  <C>                               |      */
/*      |       |                                                  |      */
/*      ������������������������������������������������������������      */
/*                                                                        */
/* This control word causes a break.   SKIPs  that occur at the top of an */
/* output column or page will not be printed;  this includes SKIPs at the */
/* start of any in-storage text block that  prints at the top of a column */
/* or page.                                                               */
/*                                                                        */
/* <1|n>:  If used without an integer operand,  "1" will be assumed.   An */
/*    operand of zero (0) is equivalent to the .BR control word.   If the */
/*    operand is minus one (-1),  the next  input text line will be over- */
/*    printed  on the  last output  line and  will not  count toward  the */
/*    number of lines  in the text area.   For values  greater than zero, */
/*    there will be  "v" blank lines generated if Single  Spacing (.SS or */
/*    .LS 0) is in effect.   If Multiple Line Spacing (.DS or .LS >0)  is */
/*    in effect then "v" times the line  spacing plus one blank lines are */
/*    generated.   For example,  in Double Space mode a ".SK 2" generates */
/*    four blank lines.                                                   */
/* <1|v> <A> <C>:  The "A" or "ABS"  operand may be specified to generate */
/*    only the specified number of blank  lines regardless of the current */
/*    .LS value.                                                          */
/*       The "C" or  "COND" operand may be specified to  define a "condi- */
/*    tional skip".    The effect of a  conditional skip depends  on what */
/*    follows.  If followed by output lines generated by a SKIP or SPACE, */
/*    the result will be the larger of the two requests.   If either is a */
/*    conditional space, then the result will be processed as a SPACE and */
/*    will generate blank lines even at the top of a column or page.   If */
/*    followed by a text output line, the "conditional skip" line(s) will */
/*    be printed before the text output line.  Conditional skips are also */
/*    ignored at the top of a column or page,  including those that occur */
/*    at the beginning  of in-storage blocks that  print at the top  of a */
/*    column or page.   If an in-storage text block ends with conditional */
/*    skips,  those conditional  skips will be put into  effect after the */
/*    block has printed, rather than as part of the block.                */
/*                                                                        */
/* When .SK -1 is used to generate an overprint line,  the visual appear- */
/* ance of the result  is dependent on whether the output  device has the */
/* ability to overprint lines.   For output directed to the line printer, */
/* and for output to online terminals that have a negative linefeed capa- */
/* bility,  the  result will appear  properly.   For output  devices that */
/* cannot handle printing  one line on top of another  the overprint line */
/* will display on the  subsequent line,  even though it will  not add to */
/* SCRIPT's internal counting of the number of lines in the text area for */
/* pagination purposes.                                                   */
/*                                                                        */
/**************************************************************************/


void    scr_sk( void )
{
    char        *   p;
    int             skip;
    int             sign;
    int             spc;

#if 0
    char            cwcurr[4];          // if errmsg becomes neccessary

    cwcurr[0] = SCR_char;
    cwcurr[1] = 's';
    cwcurr[2] = 'k';
    cwcurr[3] = '\0';
#endif

    skip = -1;
    sign = 1;                           // default sign +
    spc =  spacing;

    p = scan_start;
    while( *p && *p != ' ' ) {          // over cw
        p++;
    }
    if( *p ) {
        p++;                            // over space
        if( *p == '-' ) {
            sign = -1;
            p++;
        }
        while( isdigit( *p ) ) {
            if( skip < 0 ) {            // first digit
                skip = 0;               // .. init value
            }
            skip = skip * 10 + *p - '0';
            p++;
        }
        while( *p == ' ' ) {
            p++;
        }
        if( toupper( *p ) == 'A' ) {
            spc = 1;                    // with abs always single spacing
            while( isalpha( *p ) ) {
                p++;
            }
        }
        while( *p == ' ' ) {
            p++;
        }
        if( toupper( *p ) == 'C' ) {
            if( skip > 0 ) {
                ProcFlags.sk_cond = true;   // conditional skip
            }
        }
    }
    if( skip < 0 ) {                    // no skip value specified
        skip = 1;                       // set default value
    } else {
        skip = sign * skip;             // apply sign
    }
    skip = skip * spc;                  // apply spacing
    if( skip < -1 ) {                   // overprint
        skip = -1;                      // .. only current line possible
    }

    /***********************************************************************/
    /*  if testing for widows is active,  cancel test and output           */
    /*  buffered lines on this page                                        */
    /***********************************************************************/
    if( ProcFlags.test_widow ) {
        if( buf_lines_cnt > 0 ) {
            out_buf_lines( &buf_lines, false );
            buf_lines_cnt = 0;
        }
        ProcFlags.test_widow = false;   // terminate widow test
    }

    /***********************************************************************/
    /*  hack to mimick wgml4: for .sk N with a positive value              */
    /*  wgml4 skips less than N * line_height for device PS                */
    /*  difference is -1 for .sk  1 - 3                                    */
    /*                -2          4 - 6                                    */
    /*                -3          7 - 9                                    */
    /*                 ...                                                 */
    /***********************************************************************/
    if( skip > 0 ) {
        g_skip_wgml4 = 1 + (skip - 1) / 3;

    } else {
        g_skip_wgml4 = 0;
    }

    g_skip = skip;

    scan_restart = scan_stop + 1;
    return;
}

