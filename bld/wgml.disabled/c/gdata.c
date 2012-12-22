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
* Description:  define and initialize global variables for wgml
*
****************************************************************************/
#include    "wgml.h"
#include    "findfile.h"

#define global                          // allocate storage for global vars
#include    "gvars.h"
#undef  global

#include    "swchar.h"

/***************************************************************************/
/*  Init some global variables                                             */
/***************************************************************************/

void init_global_vars( void )
{

    memset( &GlobalFlags, 0, sizeof( GlobalFlags ) );
    GlobalFlags.wscript = 1;            // (w)script support + warnings
    GlobalFlags.warning = 1;

    try_file_name       = NULL;

    master_fname        = NULL;         // Master input file name
    master_fname_attr   = NULL;         // Master input file name attributes
    line_from           = 1;            // default first line to process
    line_to             = ULONG_MAX -1; // default last line to process
    dev_name            = NULL;         // device defined_name
    opt_fonts           = NULL;         // option FONT parameters (linked list)

    input_cbs           = NULL;         // list of active input sources
    inc_level           = 0;            // include nesting level
    max_inc_level       = 0;            // maximum include level
    out_file            = NULL;         // output file name
    out_file_attr       = NULL;         // output file name attributes

    switch_char         = _dos_switch_char();
    alt_ext             = mem_alloc( 5 );   // alternate extension   .xxx
    *alt_ext            = '\0';
    def_ext             = mem_alloc( sizeof( GML_EXT ) );
    strcpy_s( def_ext, sizeof( GML_EXT ), GML_EXT );

    gotarget[0]         = '\0';         // no .go to target yet
    gotargetno          = 0;            // no .go to target lineno

    err_count           = 0;            // total error count
    wng_count           = 0;            // total warnig count

    GML_char            = GML_CHAR_DEFAULT; // GML start char
    SCR_char            = SCR_CHAR_DEFAULT; // script start char
    CW_sep_char         = CW_SEP_CHAR_DEFAULT;// script control word seperator

    CPI                 = 10;           // chars per inch
    CPI_units           = SU_chars;

    LPI                 = 6;            // lines per inch
    LPI_units           = SU_lines;

    g_resh              = 1;            // dummy value to prevent divide by zero
    g_resv              = 1;            // dummy value to prevent divide by zero

    memset( &bind_odd, 0, sizeof( bind_odd ) ); // bind value odd pages
    bind_odd.su_u       = SU_chars_lines;

    memset( &bind_even, 0, sizeof( bind_even ) );   // bind value Even pages
    bind_even.su_u      = SU_chars_lines;

    passes              = 1;            // default number of passes

    apage               = 0;            // absolute pageno 1 - n
    page                = 0;            // current pageno (in body 1 - n)
    line                = 0;            // current output lineno on page
    lc                  = 0;            // remaining lines on page
    g_curr_font_num     = 0;
    tm                  = 0;            // top margin              &$tm
    bm                  = 0;            // bottom margin           &$bm
    fm                  = 0;            // footing margin          &$fm
    fm                  = 0;            // heading margin          &$hm

    in_esc              = ' ';
    t_line.first        = NULL;
    t_line.y_address    = 0;
    text_pool           = NULL;
    p_char              = NULL;
    line_pool           = NULL;


    lay_file            = NULL;         // filename from ( LAYout option

    index_dict          = NULL;

    init_dict( &global_dict );
    init_macro_dict( &macro_dict );
    init_tag_dict( &tag_dict );
    init_sys_dict( &sys_dict );

    tagname[0]          = '*';          // last defined GML tag name none
    tag_entry           = NULL;         // ... entry in tag_dict
    attname[0]          = '*';          // last defined GML attribute none
    att_entry           = NULL;         // ... entry in tag_dict

    research_file_name[0] = '\0';
    research_from         = 0;
    research_to           = 0;

    buf_size            = BUF_SIZE;
    buff2               = mem_alloc( buf_size );

    post_skip           = NULL;         // no post skip for first :P aragraph
    pre_space           = 0;            // experimental
    post_space          = 0;            // experimental
    post_space_save     = 0;            // experimental

    buf_lines           = NULL;         // no buffered widow line(s)
    buf_lines_cnt       = 0;

    g_skip_wgml4        = 0;
    wk_cb               = NULL;

}

/***************************************************************************/
/*  ProcFlags are initialized at each document pass start                  */
/***************************************************************************/

void init_pass_data( void )
{
    bool    flag_save = ProcFlags.fb_document_done;

    memset( &ProcFlags, 0, sizeof( ProcFlags ) );
    ProcFlags.fb_document_done = flag_save; // keep value
    ProcFlags.blanks_allowed = 1;       // blanks during scanning
                                        // i.e. .se var  =    7
                                        // .se var=7  without
    ProcFlags.concat    = true;         // .co on default
    ProcFlags.justify   = ju_on;        // .ju on default
    ProcFlags.doc_sect  = doc_sect_none;// no document section yet

    hpcount             = -1;           // no :HPx nesting active
}
