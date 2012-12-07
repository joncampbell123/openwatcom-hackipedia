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
* Description:  wgml path and separator char definitions, Environment variables
*               for includes, ...
*
****************************************************************************/

#ifndef GTYPE_H_INCLUDED
#define GTYPE_H_INCLUDED


#if defined(__QNX__) || defined(__LINUX__) // try to be nice to linux
    #define PATH_SEP        '/'
    #define INCLUDE_SEP     ':'
#elif defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__OSI__)
    #define PATH_SEP        '\\'
    #define INCLUDE_SEP     ';'
#else
    #error gtype.h not configured for system
#endif

#define ulong           unsigned long

//================= Some global defines ========================
#define MAX_NESTING     32              // max nesting of option files
#define MAX_PASSES      10              // max no of document passes
#define MAX_INC_DEPTH   255             // max include level depth
#define BUF_SIZE        512             // default buffersize for filecb e.a.
#define MAX_FILE_ATTR   15              // max size for fileattr (T:xxxx)
#define SCR_KW_LENGTH   2               // script control word length
#define FUN_NAME_LENGTH 11              // &'function name max length

#define TAG_NAME_LENGTH 15              // :tag name length
#define ATT_NAME_LENGTH 9               // :tag attr name length
#define VAL_LENGTH      10              // max length for attribute value
                                        // longer strings will be allocated

#define SYM_NAME_LENGTH 10              // symbol name length
#define MAC_NAME_LENGTH 8               // macro name length
#define MAX_MAC_PARMS   32              // maximum macro parm count
                                        // arbitrary value, not found in docu!!!
#define MAC_STAR_NAME   "_"             // local variable name for &*

#define MAX_FUN_PARMS   7             // max parmcount found in documentation

#define MAX_IF_LEVEL    10              // maximum nested .if s
#define MAX_L_AS_STR    16              // long as string

#define MAX_CPI         100             // not documented, assumed
#define MAX_LPI         100             // not documented, assumed


/* default filename extensions */
#define DEF_EXT         ".def"
#define ERR_EXT         ".err"
#define GML_EXT         ".gml"
#define LAY_EXT         ".lay"
#define OPT_EXT         ".opt"

#define GML_CHAR_DEFAULT    ':'         // start of GML tag
#define SCR_CHAR_DEFAULT    '.'         // start of Script keyword
#define CW_SEP_CHAR_DEFAULT ';'         // script controlline seperator


/* string start / end characters Possible incomplete list*/
#define d_q     '\"'                    // change also is_quote_char()
#define s_q     '\''                    // in gargutil.c
#define cent    0x9b                    // if list is extended
#define excl    '!'
#define not_c   '^'
#define slash   '/'
#define vbar1   '|'
#define vbar2   0xdd


/***************************************************************************/
/*  Space units Horiz + Vert              to be redesigned      TBD        */
/***************************************************************************/

typedef enum space_units {
    SU_undefined   = 0,                 // don't care = value zero
    SU_chars_lines = 16,                // undimensioned value
    SU_chars       = 1,                 // chars horizontal
    SU_lines       = 1,                 // lines vertical
    SU_cicero,                          // C   Cicero
    SU_cm,                              // cm  centimeter
    SU_dv,                              // dv  device units
    SU_ems,                             // M   Ems
    SU_inch,                            // i   inch
    SU_mm,                              // mm  millimeter
    SU_pica,                            // p   pica

    SU_lay_left    = 100,    // special values used for layout definition
    SU_lay_centre,           // ... :BANREGION indent, hoffset and width attr
    SU_lay_right,
    SU_lay_extend            // additions before SU_lay_left please
} space_units;

#define MAX_SU_CHAR     12            // length of space units in char format

typedef struct {
    char        su_txt[MAX_SU_CHAR];    // space unit as entered in chars
    int32_t     su_whole;               // integer part
    int32_t     su_dec;                 // decimal part (if any)
    int32_t     su_inch;                // value in inch with 4 decimals
    int32_t     su_mm;                  // value in mm with 4 decimals
    bool        su_relative;            // + - sign found
    space_units su_u;                   // unit
} su;

/***************************************************************************/
/*  Symbolic variables related                                             */
/***************************************************************************/

typedef enum {
    min_subscript = -1000000L,          // smallest valid subscript
    max_subscript =  1000000L,          // largest  valid subscript
    no_subscript  = 0x11223344          // value if not subscripted
                // must be outside of range min_subscript,max_subscript
} sub_index;

typedef enum {
    local_var   = 0x0001,
    subscripted = 0x0002,
    auto_inc    = 0x0004,
    predefined  = 0x0008,               // predefined at startup
    ro          = 0x0010,               // value not changable
    no_free     = 0x0020,               // symbol is defined at compile time
//    type_long   = 0x0040,
//    type_str    = 0x0080,
//    type_char   = 0x0100,
    access_fun  = 0x0200,               // get value via function call
    late_subst  = 0x0400,               // substituted not too early
    deleted     = 0x0800
} symbol_flags;


/***************************************************************************/
/*  entry for a (subscripted) symbolic variable                            */
/***************************************************************************/
typedef struct symsub {
    struct symsub   *   next;           // next subscript entry
    struct symvar   *   base;           // the base symvar
    sub_index           subscript;      // the subscript
    char            *   value;          // the value ptr
} symsub;


/***************************************************************************/
/*  Symbolic variable base entry                                           */
/***************************************************************************/
typedef struct symvar {
    struct symvar   *   next;           // next base entry
    char                name[SYM_NAME_LENGTH + 2];
    long                last_auto_inc;// last autoincremented subscript value
    long                subscript_used; // count of used subscripts
    symsub          *   subscripts;     // subscript entries
    symsub          *   sub_0;          // special subscript 0 entry
    void                (*varfunc)( struct symvar * e );// access function
    symbol_flags        flags;
} symvar;


/***************************************************************************/
/*  Flags for filecb and macrocb                                           */
/***************************************************************************/

typedef enum {
    FF_clear        = 0x0000,           // clear all flags
    FF_eof          = 0x0002,           // eof
    FF_err          = 0x0004,           // file error
    FF_crlf         = 0x0008,           // delete trailing CR and / or LF
    FF_macro        = 0x0010,           // entry is macro not file
    FF_tag          = 0x0030,           // entry is macro via tag
    FF_open         = 0x8000            // file is open
} fflags;

/***************************************************************************/
/*  List of (defined macro / input) lines                                  */
/***************************************************************************/
typedef struct inp_line {
    struct inp_line *   next;           // next line
    char                value[1];       // line content variable length
} inp_line;


/***************************************************************************/
/*  label control block                                                    */
/***************************************************************************/
typedef struct labelcb {
    struct labelcb  *   prev;
    fpos_t              pos;            // file position for label if file
    ulong               lineno;         // lineno of label
    char                label_name[MAC_NAME_LENGTH + 1];
} labelcb;


/***************************************************************************/
/*  macro definition entry  for macro dictionary                           */
/***************************************************************************/
typedef struct mac_entry {
    struct mac_entry    *   next;
    inp_line            *   macline;    // macro definition lines
    ulong                   lineno;     // lineno start of macro definition
    labelcb             *   label_cb;   // controlling label definitions
    char                    name[MAC_NAME_LENGTH + 1];  // macro name
    char                    mac_file_name[1];   // file name macro definition
                                            // var length
} mac_entry;


/***************************************************************************/
/*  entry for an (included) input file                                     */
/***************************************************************************/

typedef struct filecb {
    FILE        *   fp;                 // FILE ptr
    ulong           lineno;             // current line number
    ulong           linemin;            // first line number to process
    ulong           linemax;            // last line number to process
    size_t          usedlen;            // used data of filebuf
    fpos_t          pos;                // position for reopen
    labelcb     *   label_cb;           // controlling label definitions
    fflags          flags;
    char            fileattr[MAX_FILE_ATTR + 1];// T:xxxx
    char            filename[1];        // full filename var length
} filecb;

/***************************************************************************/
/*  parameter structure for macro call                                     */
/***************************************************************************/
typedef struct mac_parms {
    char        *   star;               // &*  complete parmline
    int             star0;              // &*0 parmcount
    inp_line    *   starx;              // &*1 - &*x parms
} mac_parms;


/***************************************************************************/
/*  Entry for an included macro                                            */
/***************************************************************************/

typedef struct  macrocb {
    ulong               lineno;         // current macro line number
    inp_line        *   macline;        // list of macro lines
    mac_entry       *   mac;            // macro definition entry
    struct gtentry  *   tag;            // tag entry if macro called via tag
    fflags              flags;
} macrocb;


/***************************************************************************/
/*  Stack for .if .th .el .do processing                                   */
/***************************************************************************/

typedef struct ifflags {

    unsigned    iflast  : 1;            // .if was last line
    unsigned    iftrue  : 1;            // last .if was true
    unsigned    iffalse : 1;            // last .if was false

    unsigned    ifthen  : 1;            // processing object of then
    unsigned    ifelse  : 1;            // processing object of else
    unsigned    ifdo    : 1;            // processing object of do group

    unsigned    ifcwte  : 1;            // .th or .el control word
    unsigned    ifcwdo  : 1;            // .do control word
    unsigned    ifcwif  : 1;            // .if control word

} ifflags;


typedef struct ifcb {
    int             if_level;           // nesting level
    ifflags         if_flags[MAX_IF_LEVEL + 1]; // index 0 not used
} ifcb;

/***************************************************************************/
/*  for .pe processing                                                     */
/***************************************************************************/
typedef struct pecb {                   // for .pe control
    char *  line;                       // line to perform n times
    int     ll;                         // length of line
    int     count;                      // value of .pe n  active if > 0
} pecb;


/***************************************************************************/
/*  Flags for input                                                        */
/***************************************************************************/

typedef enum {
    II_file     = 0x01,                 // inputcb is file
    II_macro    = 0x02,                 // inputcb is macro
    II_tag      = 0x06,                 // inputcb is macro via tag
    II_input    = II_file | II_macro | II_tag, // all input types
    II_research = 0x08,                 // research mode (for file only)
    II_eof      = 0x10,                 // end of file (input)
    II_sol      = 0x20,                 // start of line
    II_eol      = 0x40                  // end of line (last part)
} i_flags;

/***************************************************************************/
/*  input stack for files and macros                                       */
/***************************************************************************/
typedef struct  inputcb {
    struct inputcb  *   prev;
    inp_line        *   hidden_head;    // manage lines split at ; or :
    inp_line        *   hidden_tail;    // manage lines split at ; or :
    symvar          *   local_dict;     // local symbol dictionary
    ifcb            *   if_cb;          // for controlling .if .th .el
    pecb                pe_cb;          // for controlling .pe perform
    union  {
        filecb      *   f;              // used if input is from file
        macrocb     *   m;              // used if input is from macro/tag
    } s;
    i_flags             fmflags;
} inputcb;


/***************************************************************************/
/*  scr keywords                                                           */
/***************************************************************************/
typedef enum {
    cw_break    = 1,           // control word causes break, ie. flush output
    cw_o_t                              // control word produces output text
} scrflags;

typedef struct scrtag {
    char            tagname[SCR_KW_LENGTH + 2];
    void            (*tagproc)( void );
    scrflags        cwflags;
} scrtag;


/***************************************************************************/
/*  GML tags    predefined                                                 */
/***************************************************************************/

typedef enum {
    tag_only     = 1,                   // tag without any attribute
    tag_basic    = 2,                   // basic elements possible on tag line.
    tag_text     = 4,                   // text line possible
    etag_req     = 8,                   // eTAG required
    etag_opt     = 16,                  // eTAG optional
    tag_is_basic = 32,                  // basic tag
    tag_layout   = 64,                  // tag valid in layout
    tag_out_txt  = 128                  // tag produces output text
} gmlflags;


typedef struct gmltag {
   char             tagname[TAG_NAME_LENGTH + 1];
   size_t           taglen;
   void             (*gmlproc)( const struct gmltag * entry );
   gmlflags         tagflags;
} gmltag;


/***************************************************************************/
/*  GML tags   user defined                                                */
/*  i.e.  via .gt and .ga script control words                             */
/*  enum values have to be single bits 2**x                                */
/***************************************************************************/

typedef enum gavalflags {
    val_def     = 1,                    // value is default
    val_any     = 2,                    // any value allowed
    val_length  = 4,                    // max length of value
    val_range   = 8,                    // allowed range (numeric)
    val_value   = 16,                   // allowed value stored in union
    val_valptr  = 32,                   // allowed value allocated
    val_auto    = 64,                   // automatic (not used / implemented)
    val_reset   = 128                   // reset (not used / implemented)
} gavalflags;


/***************************************************************************/
/*  options B   from .ga control word                                      */
/***************************************************************************/

typedef struct gavalentry {
    struct gavalentry   *   next;
    union a {
       size_t   length;           // possible max length of (character) value
       long     range[4]; // min, max, default omitted, default without value
       char     value[VAL_LENGTH + 1];  // string value if short enough
       char *   valptr;                 // ... else allocated
    } a;
    gavalflags              valflags;
} gavalentry;


/***************************************************************************/
/*  options A   from .ga control word                                      */
/*  enum values have to be single bits 2**x                                */
/*  exception are the att_proc_xxx values                                  */
/***************************************************************************/

typedef enum {
    att_def         = 0x0001,           // attribute has default value
    att_range       = 0x0002,           // attribute has range
    att_auto        = 0x0004,           // attribute is automatic
    att_any         = 0x0008,           // attribute any value allowed
    att_req         = 0x0010,           // attribute required
    att_upper       = 0x0020,           // translate to upper
    att_off         = 0x0040,           // attribute is inactive

    att_proc_all    = 0x0f00,           // mask for processing flags

    att_proc_req    = 0x0100,           // req attr not yet seen
    att_proc_auto   = 0x0200,           // auto attr cannot be specified
    att_proc_seen   = 0x0400,           // attr specified
    att_proc_val    = 0x0800            // ... with value specified
} gaflags;


/***************************************************************************/
/*  entry from .ga control word                                            */
/***************************************************************************/

typedef struct gaentry {
    struct gaentry  *   next;
    gavalentry      *   vals;
    char                name[ATT_NAME_LENGTH + 1];
    gaflags             attflags;
} gaentry;


/***************************************************************************/
/*  GML tag options from the .gt Control word                              */
/*  enum values have to be single bits, powers of 2                        */
/***************************************************************************/

typedef enum {
    tag_attr     = 1,                   // tag has attributes
    tag_cont     = 2,                   // CONTinue specified
    tag_nocont   = 4,                   // NOCONTinue specified
    tag_csoff    = 8,                   // CSOFF specified
    tag_next     = 16,                  // TAGnext specified
    tag_textdef  = 32,                  // TEXTDef specified
    tag_texterr  = 64,                  // TEXTError specified
    tag_textline = 128,                 // TEXTLine specified
    tag_textreq  = 256,                 // TEXTRequired specified
    tag_off      = 512                  // tag OFF specified
} gtflags;


/***************************************************************************/
/*  Tag entry  from .gt control word                                       */
/***************************************************************************/

typedef struct gtentry {
    struct gtentry  *   next;
    gaentry         *   attribs;        // list of attributes
    ulong               usecount;
    size_t              namelen;        // actual length of name
    char                name[TAG_NAME_LENGTH + 1];
    char                macname[MAC_NAME_LENGTH + 2];   // macro to call
    gtflags             tagflags;
} gtentry;


/***************************************************************************/
/*  condcode  returncode for several conditions during parameterchecking   */
/*            loosely adapted from wgml 88.1 IBM S/360 ASM code            */
/***************************************************************************/

typedef enum condcode {            // return code for some scanning functions
    zero            = 0,
    omit            = 1,                // argument omitted
    pos             = 2,                // argument affirmative
    neg             = 4,                // argument negative
    quotes          = 8,                // argument within quotes
    quotes0         = 16,               // only quotes
    no              = 32,               // argument undefined
    notnum          = 32                 // value not numeric / overflow
}  condcode;


/***************************************************************************/
/*  scr string functions                                                   */
/***************************************************************************/

typedef struct parm {
    char    *       a;                  // start of parm ptr
    char    *       e;                  // end of parm ptr
} parm;


typedef struct scrfunc {
    const   char    fname[FUN_NAME_LENGTH + 1];   // function name
    const   size_t  length;             // actual length of fname
    const   size_t  parm_cnt;           // mandatory parms
    const   size_t  opt_parm_cnt;       // optional parms
    condcode        (*fun)( parm parms[MAX_FUN_PARMS], size_t parm_count,
                            char * * ppval, int32_t valsize );
} scrfunc;


/***************************************************************************/
/*  definitions for getnum routine  to be reworked  TBD                    */
/***************************************************************************/


typedef enum {
    selfdef     = 4,
    aritherr    = 8,
    ilorder     = 12,
    illchar     = 16,
    mnyerr      = 20,
    operr       = 24,
    parerr      = 28,
    enderr      = 32
} getnumrc;

typedef struct getnum_block {
    int         ignore_blanks;          // 1 if blanks are ignored
    char    *   argstart;
    char    *   argstop;
    char    *   errstart;
    char    *   first;
    long        length;
    long        result;                 // result as long
    char        resultstr[12];          // result in char format
    getnumrc    error;
    char        num_sign;              // remember absolute or relative value
    condcode    cc;
} getnum_block;

/***************************************************************************/
/*  struct used to hold parameters of option FONT                          */
/***************************************************************************/

typedef struct opt_font {
    struct opt_font *   nxt;
    char *              name;
    char *              style;
    uint32_t            space;
    uint32_t            height;
    uint8_t             font;
} opt_font;

/***************************************************************************/
/*  message numbers  + severities                                          */
/***************************************************************************/
typedef enum msg_ids  {
    #include "wgmlmsge.gh"              // as lowercase enums
} msg_ids;

#include "wgmlmsgs.gh"                  // as uppercase defines

typedef enum {
    SEV_BANNER,
    SEV_DEBUG,
    SEV_INFO,
    SEV_WARNING,
    SEV_ERROR,
    SEV_FATAL_ERR
} severity;



/***************************************************************************/
/*  Structures for storing index information from .ix control word         */
/***************************************************************************/

#define reflen  56                      // max length for pagenos in index

typedef struct ix_e_blk {               // entry for pagenos
    struct ix_e_blk * next;             // next entries (if any)
    uint32_t        freelen;            // remainig len of etext
    char            refs[reflen];       // the pagenos   3, 5, 8, 9, ...
} ix_e_blk;

typedef struct ix_h_blk {               // header with index text
    struct ix_h_blk * next;             // next ix header blk same level
    struct ix_h_blk * lower;            // next ix header blk next level
           ix_e_blk * entry;            // first ix entry blk
    uint32_t        len;                // header text length
    char            text[1];            // variable length textfield
} ix_h_blk;


/***************************************************************************/
/* enum for document sections  sequence is important, don't change         */
/***************************************************************************/

typedef enum doc_section {
    doc_sect_none,                      // nothing so far
    doc_sect_gdoc,                      // gdoc
    doc_sect_frontm,                    // front matter
    doc_sect_titlep,                    // title page
    doc_sect_etitlep,                   // end title page
    doc_sect_abstract,                  // abstract
    doc_sect_preface,                   // preface
    doc_sect_toc,                       // table of contents
    doc_sect_figlist,                   // figure list
    doc_sect_body,                      // body
    doc_sect_appendix,                  // appendix
    doc_sect_backm,                     // back matter
    doc_sect_index,                     // index
    doc_sect_egdoc                      // egdoc  has to be last
} doc_section;

/***************************************************************************/
/* enum for justify values                                                 */
/***************************************************************************/
typedef enum ju_enum {                  // for .ju(stify)
    ju_off,                             // ju_off must have lowest value
    ju_on,                              // ju_on next
    ju_half,
    ju_left,
    ju_right,
    ju_centre,
    ju_center = ju_centre,
    ju_inside,
    ju_outside
} ju_enum;

/***************************************************************************/
/*  enums for layout tags with attributes  (and ebanregion)                */
/*  the order is as shown by :convert output                               */
/***************************************************************************/

typedef enum lay_sub {
    el_zero     = 0,                    // dummy to make 0 invalid
    el_page     = 1,
    el_default,
    el_widow,
    el_fn,
    el_fnref,
    el_p,
    el_pc,
    el_fig,
    el_xmp,
    el_note,
    el_h0,
    el_h1,
    el_h2,
    el_h3,
    el_h4,
    el_h5,
    el_h6,
    el_heading,
    el_lq,
    el_dt,
    el_gt,
    el_dthd,
    el_cit,
    el_figcap,
    el_figdesc,
    el_dd,
    el_gd,
    el_ddhd,
    el_abstract,
    el_preface,
    el_body,
    el_backm,
    el_lp,
    el_index,
    el_ixpgnum,
    el_ixmajor,
    el_ixhead,
    el_i1,
    el_i2,
    el_i3,
    el_toc,
    el_tocpgnum,
    el_toch0,
    el_toch1,
    el_toch2,
    el_toch3,
    el_toch4,
    el_toch5,
    el_toch6,
    el_figlist,
    el_flpgnum,
    el_titlep,
    el_title,
    el_docnum,
    el_date,
    el_author,
    el_address,
    el_aline,
    el_from,
    el_to,
    el_attn,
    el_subject,
    el_letdate,
    el_open,
    el_close,
    el_eclose,
    el_distrib,
    el_appendix,
    el_sl,
    el_ol,
    el_ul,
    el_dl,
    el_gl,
    el_banner,
    el_banregion,
    el_ebanregion
} lay_sub;

/***************************************************************************/
/*  definitions for functioncodes inserted into input buffer               */
/*  function start should be an even value                                 */
/*  function end the following odd value                                   */
/*                                                                         */
/*  0xfe 0x02 subscripted text 0xfe 0x03     example for subscripted text  */
/*                                                                         */
/*  incomplete will change                                      TBD        */
/***************************************************************************/

typedef enum functs {
    function_escape         = 0xfe,
    function_end            = 0x01,

    function_subscript      = 0x02,
    function_sub_end        = 0x03,

    function_superscript    = 0x04,
    function_super_end      = 0x05
} functs;


/***************************************************************************/
/*  tags and controlwords as enums for distinction during processing       */
/***************************************************************************/

#undef pickg
#define pickg( name, length, routine, flags )  t_##name,

#undef picklab
#define picklab( name, routine, flags )  t_##label,

#undef picks
#define picks( name, routine, flags )  t_##name,

typedef enum e_tags {
    t_NONE,
#include "gtags.h"
#include "gscrcws.h"
    t_MAX                               // the last one for range check
} e_tags;

/***************************************************************************/
/*  stack of margins and other values for the nested tags et al            */
/***************************************************************************/

typedef struct tag_cb {
    struct  tag_cb   *   prev;
    uint32_t            left;           // margin
    uint32_t            right;          // margin
    uint32_t            post_skip;
    e_tags              c_tag;
} tag_cb;

#endif                                  // GTYPE_H_INCLUDED
