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
* Description:  Declares the items needed to parse and interpret the
*               information from .COP files:
*               an enum:
*                   text_type
*               the structs:
*                   cop_device
*                       box_block
*                       underscore_block
*                       intrans_block
*                       outtrans_block
*                           translation
*                       defaultfont_block
*                           default_font
*                       pause_block
*                           code_text
*                       devicefont_block
*                           device_font
*                               code_text
*                   cop_driver
*                       init_funcs
*                           init_block
*                               init_text
*                       finish_funcs
*                           code_text
*                       newline_funcs
*                           newline_block
*                       fontswitch_funcs
*                           fontswitch_block
*                               code_text
*                       fontstyle_group
*                           fontstyle_block
*                               code_text
*                               line_proc
*                                   code_text
*                       line_block
*                   cop_font
*                       intrans_block
*                       outtrans_block
*                           translation
*                       width_block
*                   record_buffer
*                   text_chars
*                   text_line
*                   wgml_font
*               the variables:
*                   bin_device
*                   bin_driver
*                   has_aa_block
*                   ps_device
*                   wgml_font_cnt
*                   wgml_fonts
*               the functions:
*                   cop_in_trans()
*                   cop_setup()
*                   cop_teardown()
*                   cop_text_width()
*                   cop_ti_table()
*                   cop_tr_table()
*                   fb_absoluteaddress()
*                   fb_dbox()
*                   fb_document()
*                   fb_document_page()
*                   fb_finish()
*                   fb_hline()
*                   fb_new_section()
*                   fb_output_textline()
*                   fb_position()
*                   fb_start()
*                   fb_vline()
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
****************************************************************************/

#ifndef COPFILE_H_INCLUDED
#define COPFILE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* enum definition. */

typedef enum {
    norm = 0,
    sup,
    sub
} text_type;

/* struct declarations. */

/* These structs are used by more than one of the top-level structs:
 *      intrans_block is used by both cop_device and cop_font
 *      outtrans_block is used by both cop_device and cop_font
 *      code_text is used by both cop_device and cop_driver
 */

/* To hold the data extracted from an IntransBlock struct.
 * intrans_block is a struct for consistency with outtrans_block.
 */

typedef struct {
    uint8_t         table[0x100];
} intrans_block;

#pragma disable_message( 128 ); // suppress: Warning! W128: 3 padding byte(s) added

/* To hold the data extracted from an OuttransData struct. */

typedef struct {
    uint8_t         count;
    uint8_t *       data;
} translation;

/* To hold the data extracted from an OuttransBlock struct.
 * The entry for a given character will be NULL if no out-translation
 * was specified.
 */

typedef struct {
    translation *   table[0x100];
} outtrans_block;

/* To hold the data extracted from a CodeBlock struct. */

typedef struct {
    uint16_t            count;
    uint8_t *           text;
} code_text;

/* These structs are unique to the top-level struct cop_device. */

/* Fonts in box_block and underscore_block.
 *
 * These structs have two font fields: font_name and font_number.
 *
 * For the UnderscoreBlock only, if specified_font is false, then both font_name
 * and font_number are to be ignored and whatever font is in use when the
 * underscore character is needed is to be used.
 *
 * If font_name is NULL, then font_name is to be ignored and font_number
 * is to be used.
 *
 * If font_name is not NULL, then font_name is to be used and font_number is
 * to be ignored.
 *
 * If the font_name is not NULL but the font indicated cannot be found, then
 * font number "0" should be used. There is always a font numbered "0".
 */

/* To hold the data from the BoxBlock struct. */

typedef struct {
    char *              font_name;
    uint8_t             font_number;
    char                horizontal_line;
    char                vertical_line;
    char                top_left;
    char                top_right;
    char                bottom_left;
    char                bottom_right;
    char                top_join;
    char                bottom_join;
    char                left_join;
    char                right_join;
    char                inside_join;
} box_block;

/* To hold the data from the UnderscoreBlock struct. */

typedef struct {
    bool                specified_font;
    char *              font_name;
    uint8_t             font_number;
    char                underscore_char;
} underscore_block;

/* To hold the data from the DefaultFont struct. */

typedef struct {
    char *              font_name;
    char *              font_style;
    uint16_t            font_height;
    uint16_t            font_space;
} default_font;

/* To hold the data from the DefaultfontBlock struct. */

typedef struct {
    uint16_t            font_count;
    default_font *      fonts;
} defaultfont_block;

/* To hold the data from the PauseBlock struct. */

typedef struct {
    code_text *         start_pause;
    code_text *         document_pause;
    code_text *         docpage_pause;
    code_text *         devpage_pause;
} pause_block;

/* To hold the data from the DeviceFont struct. */

typedef struct {
    char *              font_name;
    char *              font_switch;
    uint8_t             resident;
    code_text *         font_pause;
} device_font;

/* To hold the data from the DevicefontBlock struct. */

typedef struct {
    uint16_t            font_count;
    device_font *       fonts;
} devicefont_block;

/* These structs are unique to the top-level struct cop_driver. */

/* In all cases: a count of "0" or a NULL pointer indicates that the
 * corresponding item did not exist in the .COP file
 */

/* To hold the data extracted from a CodeBlock struct.
 * The :INIT block is unique in allowing multiple intermixed :VALUE and
 * :FONTVALUE blocks. Testing shows wgml invoking each :FONTVALUE block
 * multiple times but each :VALUE block only once, so is_fontvalue is provided
 * to allow wgml to determine if an init_text instance came from a :FONTVALUE
 * block or a :VALUE block.
 */

typedef struct {
    bool                is_fontvalue;
    uint16_t            count;
    uint8_t *           text;
} init_text;

/* To hold the data from the InitBlock struct. */
/* The field names do not all correspond to those in the Wiki. */

typedef struct {
    uint16_t            count;
    init_text *         codeblock;
} init_block;

/* To hold the data from the InitFuncs struct.
 * This struct bears no resemblence to that in the Wiki: it takes advantage
 * of the fact that there can be at most two :INIT blocks, and that they must
 * be distinguished because wgml processes them at different times.
 */

typedef struct {
    init_block *        start;
    init_block *        document;
} init_funcs;

/* The FinishBlock struct is implemented by the code_text struct. */

/* To hold the data from the FinishFuncs struct.
 * This struct bears no resemblence to that in the Wiki: it takes advantage
 * of the fact that there can be at most two :FINISH blocks, that wgml will
 * only use the first :VALUE block in a :FINISH block, so that only that one
 * needs to be presented here, and that they must be distinguished because
 * wgml processes at most one of them.
 */

typedef struct {
    code_text *         end;
    code_text *         document;
} finish_funcs;

/* To hold the data from the NewlineBlock struct.
 * There will be as many newline_blocks as there are distinct values of
 * "advance".
 */

typedef struct {
    uint16_t            advance;
    uint16_t            count;
    uint8_t *           text;
} newline_block;

/* To hold the data extracted from a NewlineFuncs struct. */

typedef struct {
    uint16_t            count;
    newline_block *     newlineblocks;
} newline_funcs;

/* To hold the data from the FontswitchBlock struct.
 * There will be as many fontswitch_blocks as there are distinct values of
 * "type". The bool fields are used to record information from some of the
 * 21 flags, as discussed in the Wiki. The other field names do not
 * correspond to the Wiki: they take advantage of the fact that there are
 * at most two CodeBlocks, one from a :STARTVALUE block and the other from
 * an :ENDVALUE block.
 */

typedef struct {
    char *              type;
    bool                do_always;
    bool                default_width_flag;
    bool                font_height_flag;
    bool                font_outname1_flag;
    bool                font_outname2_flag;
    bool                font_resident_flag;
    bool                font_space_flag;
    bool                line_height_flag;
    bool                line_space_flag;
    code_text *         startvalue;
    code_text *         endvalue;
} fontswitch_block;

/* To hold the data extracted from a FontswitchFuncs struct. */

typedef struct {
    uint16_t            count;
    fontswitch_block *  fontswitchblocks;
} fontswitch_funcs;

/* To hold some of the data extracted from a FontstyleFuncs struct.
 * This struct does not correspond to the struct in the Wiki. Instead, it takes
 * advantage of the fact that each :LINEPROC block can define at most one of each
 * of its sub-blocks.
 */

typedef struct {
    code_text *         startvalue;
    code_text *         firstword;
    code_text *         startword;
    code_text *         endword;
    code_text *         endvalue;
} line_proc;

/* To hold the data extracted from a ShortFontstyleBlock struct.
 * Only the first two fields are found in the Wiki struct. The next three take
 * advantage of the fact that a :FONTSTYLE block directly defines at most one of
 * each of two sub-blocks, plus any number of :LINEPROC blocks. The number of
 * line_proc instances is given by the value of the field "line_passes".
 */

typedef struct {
    uint16_t            line_passes;
    char *              type;
    code_text *         startvalue;
    code_text *         endvalue;
    line_proc *         lineprocs;
} fontstyle_block;

/* To hold the data extracted from a FontstyleGroup struct.
 * This struct bears only a functional relationship to the struct in the Wiki,
 * which must be seen to be believed.
 */

typedef struct {
    uint16_t            count;
    fontstyle_block *   fontstyleblocks;
} fontstyle_group;

/* To hold the data extracted from an HlineBlock, a VlineBlock, or a DboxBlock
 * struct.
 * This differs from the structs in the Wiki because each of them can contain at
 * most one CodeBlock, as well as the thickness.
 */

typedef struct {
    uint32_t            thickness;
    uint16_t            count;
    uint8_t *           text;
} line_block;

/* This struct is unique to the top-level struct cop_font. */

/* To hold the data extracted from a WidthBlock struct.
 * width_block is a struct for consistency with outtrans_block.
 */

typedef struct {
    uint32_t            table[0x100];
} width_block;

/* These are the top-level structs. These are the only structs intended to
 * be created and passed around independently.
 *
 * The comments within the structs refer to the blocks discussed in the Wiki.
 *
 * The first two fields are used internally and were used for sizing during
 * development
 *
 * The instance returned will be allocated as a single block and so can be
 * freed in one statement.
 */

/* This struct embodies the binary form of the :DEVICE block.
 *
 * Note that the "FunctionsBlock" is not mentioned. The various "CodeBlock"s
 * are instead provided as part of PauseBlock and DevicefontBlock.
 */

typedef struct {
    size_t              allocated_size;
    size_t              next_offset;
    /* The Attributes */
    char            *   driver_name;
    char            *   output_name;
    char            *   output_extension;
    uint32_t            page_width;
    uint32_t            page_depth;
    uint32_t            horizontal_base_units;
    uint32_t            vertical_base_units;
    /* PagegeometryBlock */
    uint32_t            x_start;
    uint32_t            y_start;
    uint32_t            x_offset;
    uint32_t            y_offset;
    /* BoxBlock */
    box_block           box;
    /* UnderscoreBlock */
    underscore_block    underscore;
    /* TranslationBlock */
    intrans_block   *   intrans;
    outtrans_block  *   outtrans;
    /* DefaultfontBlock */
    defaultfont_block   defaultfonts;
    /* PauseBlock */
    pause_block         pauses;
    /* DevicefontBlock */
    devicefont_block    devicefonts;
} cop_device;

/* This struct embodies the binary form of the :DRIVER block.
 *
 * The "unknown" block is not mentioned because it never has any data in it.
 */

typedef struct {
    size_t              allocated_size;
    size_t              next_offset;
    /* The Attributes */
    char            *   rec_spec;
    char                fill_char;
    /* PageAddressBlock */
    uint8_t             x_positive;
    uint8_t             y_positive;
    /* InitFuncs */
    init_funcs          inits;
    /* FinishFuncs */
    finish_funcs        finishes;
    /* NewlineFuncs */
    newline_funcs       newlines;
    /* Variant A FunctionsBlocks */
    code_text           newpage;
    code_text           htab;
    /* FontswitchFuncs */
    fontswitch_funcs    fontswitches;
    /* FontstyleGroup */
    fontstyle_group     fontstyles;
    /* Variant A FunctionsBlock */
    code_text           absoluteaddress;
    /* HlineBlock */
    line_block          hline;
    /* VlineBlock */
    line_block          vline;
    /* DboxBlock */
    line_block          dbox;
} cop_driver;

#pragma enable_message( 128 ); // reenable: Warning! W128: 3 padding byte(s) added

/* This struct embodies the binary form of the :FONT block.
 * Only the fonts need to be treated as a linked list.
 */

typedef struct cop_font {
    struct cop_font *   next_font;
    size_t              allocated_size;
    size_t              next_offset;
    /* For matching by defined name. */
    char            *   defined_name;
    /* The Attributes */
    char            *   font_out_name1;
    char            *   font_out_name2;
    uint32_t            line_height;
    uint32_t            line_space;
    uint32_t            scale_basis;
    uint32_t            scale_min;
    uint32_t            scale_max;
    uint32_t            char_width;
    /* CharacterDescriptionBlock */
    intrans_block   *   intrans;
    outtrans_block  *   outtrans;
    width_block     *   width;
} cop_font;

/* This struct was originally developed for use with the output buffer. It's
 * use has since expanded. current records the current write position, length
 * records the number of bytes pointed to by text, and text points to the
 * bytes to be inserted into the output buffer or otherwise processed.
 */

typedef struct {
    size_t              current;
    size_t              length;
    uint8_t         *   text;
} record_buffer;

/* This struct implements the text_chars struct in the Wiki. */

typedef struct text_chars {
    struct  text_chars  *   next;
    struct  text_chars  *   prev;
            uint32_t        x_address;
            uint32_t        width;
            uint16_t        count;
            uint16_t        length;
            text_type       type;
            uint8_t         font_number;
            uint8_t         text[1];
} text_chars;

/* This struct implements the text_line struct in the Wiki. */

typedef struct text_line {
    struct  text_line   *   next;
            uint32_t        line_height;
            uint32_t        y_address;
            text_chars  *   first;
            text_chars  *   last;
} text_line;

/* This struct implements the wgml_font struct in the Wiki. */

typedef struct {
    cop_font            *   bin_font;
    fontswitch_block    *   font_switch;
    code_text           *   font_pause;
    fontstyle_block     *   font_style;
    outtrans_block      *   outtrans;
    uint32_t                default_width;
    uint32_t                dv_base;
    uint32_t                em_base;
    uint32_t                font_height;
    uint32_t                font_space;
    uint32_t                line_height;
    uint32_t                line_space;
    uint32_t                spc_width;
    uint32_t                width_table[0x100];
    char                    font_resident;
    uint8_t                 shift_count;
    char                    shift_height[4];
} wgml_font;


/* Variable declarations. */

#ifndef global
    #define global  extern
#endif

global bool             has_aa_block;   // true if device defined :ABSOLUTEADDRESS
global bool             ps_device;      // true if device is PostScript
global cop_device   *   bin_device;     // binary device being used
global cop_driver   *   bin_driver;     // binary driver being used
global uint16_t         wgml_font_cnt;  // number of available fonts
global wgml_font    *   wgml_fonts;     // the available fonts

/* Reset so can be reused with other headers. */

#undef global

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

/* copfiles.c                          */

extern uint8_t              cop_in_trans( uint8_t in_char, uint8_t font );
extern void                 cop_setup( void );
extern void                 cop_teardown( void );
extern uint32_t             cop_text_width( uint8_t * text, uint32_t count, uint8_t font );
extern void                 cop_ti_table( uint8_t * data, uint32_t count );
extern void                 fb_dbox( uint32_t h_start, uint32_t v_start, uint32_t h_len, uint32_t v_len );
extern void                 fb_document( void );
extern void                 fb_document_page( void );
extern void                 fb_finish( void );
extern void                 fb_hline( uint32_t h_start, uint32_t v_start, uint32_t h_len );
extern void                 fb_output_textline( text_line * out_line );
extern void                 fb_position( uint32_t h_start, uint32_t v_start );
extern void                 fb_start( void );
extern void                 fb_vline( uint32_t h_start, uint32_t v_start, uint32_t v_len );

/* devfuncs.c                          */
extern void                 fb_absoluteaddress( void );
extern void                 fb_new_section( uint32_t v_start );

/* outbuff.c                           */
extern void                 cop_tr_table( uint8_t * data, uint32_t count );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPFILE_H_INCLUDED */
