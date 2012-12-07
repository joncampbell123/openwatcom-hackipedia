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


#include "pcscrnio.h"
#include "dbgdefn.h"
#include "kbio.h"
#include "dbgmem.h"
#include <stdui.h>
#include <dos.h>
#include <string.h>
#include "dbgscrn.h"
#include "tinyio.h"
#include "dbgswtch.h"
#include "dbginstr.h"

#define VID_STATE_SWAP  (VID_STATE_ALL)

extern void WndDirty(void);
extern void StartupErr(char *);

extern screen_state     ScrnState;

flip_types              FlipMech;
mode_types              ScrnMode;



char                 ActFontTbls;  /* assembly file needs access */

static bool             OnAlt;
static char          StrtRows;
static char          StrtPoints;
static char          SavPoints;
static addr_seg      SwapSeg;
static char          StrtMode;
static unsigned char StrtAttr;
static char          StrtSwtchs;
static int           StrtCurTyp;
static int           SavCurTyp;
static int           SavCurPos;
static char          SaveMode;
static char          SavePage;
static unsigned char SaveSwtchs;
static unsigned int  VIDPort;
static unsigned int  PageSize;
static unsigned int  CurOffst;
static unsigned int  RegCur;
static unsigned int  InsCur;
static unsigned int  NoCur;
static unsigned char DbgBiosMode;
static unsigned char DbgChrSet;
static unsigned char DbgRows;
static unsigned int  PgmMouse, DbgMouse;

#define DOUBLE_DOT_CHR_SET      0x12
#define COMPRESSED_CHR_SET      0x11
#define USER_CHR_SET            0x00

#define EGA_CURSOR_OFF  0x1e00
#define NORM_CURSOR_OFF 0x2000
#define CGA_CURSOR_ON   0x0607
#define MON_CURSOR_ON   0x0b0c


typedef enum {
        DISP_NONE,
        DISP_MONOCHROME,
        DISP_CGA,
        DISP_RESERVED1,
        DISP_EGA_COLOUR,
        DISP_EGA_MONO,
        DISP_PGA,
        DISP_VGA_MONO,
        DISP_VGA_COLOUR,
        DISP_RESERVED2,
        DISP_RESERVED3,
        DISP_MODEL30_MONO,
        DISP_MODEL30_COLOUR } hw_display_type;


typedef struct {
     hw_display_type active;
     hw_display_type alt;
} display_configuration;


extern unsigned MouseSaveSize();
#pragma aux MouseSaveSize =                                     \
0X29 0XDB       /* sub    bx,bx                         */      \
0XB8 0X15 0X00  /* mov    ax,0015                       */      \
0XCD 0X33       /* int    33                            */      \
                value [ bx ]                                    \
                modify [ ax ];


extern void MouseSaveState();
#pragma aux MouseSaveState =                                    \
0XB8 0X16 0X00  /* mov    ax,0016                       */      \
0XCD 0X33       /* int    33                            */      \
                parm [ es ] [ dx ]                              \
                modify [ ax ];


extern void MouseRestoreState();
#pragma aux MouseRestoreState =                                 \
0XB8 0X17 0X00  /* mov    ax,0017                       */      \
0XCD 0X33       /* int    33                            */      \
                parm [ es ] [ dx ]                              \
                modify [ ax ];


extern display_configuration BIOSDevCombCode();
#pragma aux BIOSDevCombCode =                                   \
0X55            /* push   bp                            */      \
0XB8 0X00 0X1A  /* mov    ax,1a00                       */      \
0XCD 0X10       /* int    10                            */      \
0X3C 0X1A       /* cmp    al,1a                         */      \
0X74 0X02       /* jz     *+2                           */      \
0X29 0XDB       /* sub    bx,bx                         */      \
0X5D            /* pop    bp                            */      \
                value   [ bx ]                                  \
                modify  [ ax ];


static display_configuration HWDisplay;

/*
   -1 ==> monochrome adapter
    0 ==> no display
    1 ==> colour adapter
*/

static signed char ColourAdapters[] = {  0,     /* NONE                 */
                                        -1,     /* MONOCHROME           */
                                         1,     /* CGA                  */
                                         0,     /* RESERVED             */
                                         1,     /* EGA COLOUR           */
                                        -1,     /* EGA MONO             */
                                         1,     /* PGA                  */
                                         1,     /* VGA MONO             */
                                         1,     /* VGA COLOUR           */
                                         0,     /* RESERVED             */
                                         0,     /* RESERVED             */
                                         1,     /* MODEL 30 MONO        */
                                         1 };   /* MODEL 30 COLOUR      */

enum {
        BD_SEG          = 0x40,
        BD_EQUIP_LIST   = 0x10,
        BD_CURR_MODE    = 0x49,
        BD_REGEN_LEN    = 0x4c,
        BD_CURPOS       = 0x50,
        BD_MODE_CTRL    = 0x65,
        BD_VID_CTRL1    = 0x87,
};

#define GetBIOSData( offset, var ) \
    movedata( BD_SEG, offset, FP_SEG( &var ), FP_OFF( &var ), sizeof( var ) );
#define SetBIOSData( offset, var ) \
    movedata( FP_SEG( &var ), FP_OFF( &var ), BD_SEG, offset, sizeof( var ) );



#pragma aux DoRingBell =                                        \
0X55            /* push   bp                            */      \
0XB8 0X07 0X0E  /* mov    ax,0E07                       */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];

extern void DoRingBell();

void RingBell()
{
    DoRingBell();
}


static bool ChkCntrlr( int port )
{
    char curr;
    bool rtrn;

    curr = VIDGetRow( port );
    VIDSetRow( port, 0X5A );
    VIDWait();
    VIDWait();
    VIDWait();
    rtrn = ( VIDGetRow( port ) == 0X5A );
    VIDSetRow( port, curr );
    return( rtrn );
}


static unsigned char GetSwtchs()
{
    auto unsigned char equip;

    GetBIOSData( BD_EQUIP_LIST, equip );
    return( equip );
}


static void SetSwtchs( unsigned char new )
{
    SetBIOSData( BD_EQUIP_LIST, new );
}


static void DoSetMode( unsigned char mode )
{
    switch( mode & 0x7f ) {
    case 0x7:
    case 0xf:
        SetSwtchs( (StrtSwtchs&0XCF)|0X30 );
        break;
    default:
        SetSwtchs( (StrtSwtchs&0XCF)|0X20 );
        break;
    }
    BIOSSetMode( mode );
}


static bool TstMono()
{
    if( !ChkCntrlr( VIDMONOINDXREG ) ) return( FALSE );
    return( TRUE );
}


static bool TstColour()
{
    if( !ChkCntrlr( VIDCOLRINDXREG ) ) return( FALSE );
    return( TRUE );
}


static void GetDispConfig()
{
    signed long         info;
    unsigned char       colour;
    unsigned char       memory;
    unsigned char       swtchs;
    unsigned char       curr_mode;
    hw_display_type     temp;

    HWDisplay = BIOSDevCombCode();
    if( HWDisplay.active != DISP_NONE ) return;
    /* have to figure it out ourselves */
    curr_mode = BIOSGetMode() & 0x7f;
    info = BIOSEGAInfo();
    memory = info;
    colour = info >> 8;
    swtchs = info >> 16;
    if( swtchs < 12 && memory <= 3 && colour <= 1 ) {
        /* we have an EGA */
        if( colour == 0 ) {
            HWDisplay.active = DISP_EGA_COLOUR;
            if( TstMono() ) HWDisplay.alt = DISP_MONOCHROME;
        } else {
            HWDisplay.active = DISP_EGA_MONO;
            if( TstColour() ) HWDisplay.alt = DISP_CGA;
        }
        if( HWDisplay.active == DISP_EGA_COLOUR
                && (curr_mode==7 || curr_mode==15)
         || HWDisplay.active == DISP_EGA_MONO
                && (curr_mode!=7 && curr_mode!=15) ) {
            /* EGA is not the active display */

            temp = HWDisplay.active;
            HWDisplay.active = HWDisplay.alt;
            HWDisplay.alt = temp;
        }
        return;
    }
    if( TstMono() ) {
        /* have a monochrome display */
        HWDisplay.active = DISP_MONOCHROME;
        if( TstColour() ) {
            if( curr_mode != 7 ) {
                HWDisplay.active = DISP_CGA;
                HWDisplay.alt    = DISP_MONOCHROME;
            } else {
                HWDisplay.alt    = DISP_CGA;
            }
        }
        return;
    }
    /* only thing left is a single CGA display */
    HWDisplay.active = DISP_CGA;
}


static bool ChkForColour( hw_display_type display )
{
    if( ColourAdapters[ display ] <= 0 ) return( FALSE );
    ScrnMode = MD_COLOUR;
    return( TRUE );

}


static void SwapActAlt()
{
    hw_display_type     temp;

    temp = HWDisplay.active;
    HWDisplay.active = HWDisplay.alt;
    HWDisplay.alt = temp;
    OnAlt = TRUE;
}

static bool ChkColour()
{
    if( ChkForColour( HWDisplay.active ) ) return( TRUE );
    if( ChkForColour( HWDisplay.alt    ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}


static bool ChkForMono( hw_display_type display )
{
    if( ColourAdapters[ display ] >= 0 ) return( FALSE );
    ScrnMode = MD_MONO;
    return( TRUE );
}


static bool ChkMono()
{
    if( ChkForMono( HWDisplay.active ) ) return( TRUE );
    if( ChkForMono( HWDisplay.alt    ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}


static bool ChkForEGA( hw_display_type display )
{
    switch( display ) {
    case DISP_EGA_COLOUR:
    case DISP_VGA_COLOUR:
        ScrnMode = MD_EGA;
        return( TRUE );
    case DISP_EGA_MONO:
    case DISP_VGA_MONO:
        ScrnMode = MD_EGA;
        return( TRUE );
    }
    return( FALSE );
}


static bool ChkEGA()
{
    if( ChkForEGA( HWDisplay.active ) ) return( TRUE );
    if( ChkForEGA( HWDisplay.alt    ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}


static void GetDefault()
{
    if( StrtMode == 0x07 || StrtMode == 0x0f ) {
        if( FlipMech == FLIP_TWO ) {
            if( ChkColour() == FALSE ) {
                FlipMech = FLIP_SWAP;
                ChkMono();
            }
        } else {
            ChkMono();
        }
    } else {
        if( FlipMech == FLIP_TWO ) {
            if( ChkMono() == FALSE ) {
                FlipMech = FLIP_PAGE;
                ChkColour();
            }
        } else {
            ChkColour();
        }
    }
}


static void ChkPage()
{
    switch( ScrnMode ) {
    case MD_MONO:
        FlipMech = FLIP_SWAP;
        break;
    case MD_EGA:
    case MD_COLOUR:
        break;
    default:
        FlipMech = FLIP_SWAP; /* for now */
        break;
    }
}


static void ChkTwo()
{
    if( HWDisplay.alt == DISP_NONE ) {
        FlipMech = FLIP_PAGE;
        ChkPage();
    }
}


static void SetChrSet( unsigned set )
{
    if( set != USER_CHR_SET ) {
        BIOSEGAChrSet( set );
    }
}


static unsigned GetChrSet( unsigned char rows )
{
    if( rows >= 43 ) return( DOUBLE_DOT_CHR_SET );
    if( rows >= 28 ) return( COMPRESSED_CHR_SET );
    return( USER_CHR_SET );
}


static void SetEGA_VGA( int double_rows )
{
    if( ScrnMode == MD_EGA ) {
        DbgRows = double_rows;
        DbgChrSet = DOUBLE_DOT_CHR_SET;
    } else if( FlipMech == FLIP_SWAP ) {
        DbgChrSet = GetChrSet( BIOSGetRows() );
        switch( DbgChrSet ) {
        case USER_CHR_SET:
            DbgRows = 25;
            break;
        case COMPRESSED_CHR_SET:
            DbgRows = 28;
            break;
        case DOUBLE_DOT_CHR_SET:
            DbgRows = double_rows;
            break;
        }
    } else {
        DbgRows = BIOSGetRows();
        DbgChrSet = USER_CHR_SET;
    }
}


static void SetMonitor()
{
    DbgChrSet = USER_CHR_SET;
    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        DbgBiosMode = 7;
        break;
    case DISP_CGA:
    case DISP_PGA:              /* just guessing here */
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
        if( StrtMode == 0x2 && !OnAlt ) {
            DbgBiosMode = 2;
        } else {
            DbgBiosMode = 3;
        }
        break;
    case DISP_EGA_MONO:
        DbgBiosMode = 7;
        SetEGA_VGA( 43 );
        break;
    case DISP_EGA_COLOUR:
        DbgBiosMode = 3;
        SetEGA_VGA( 43 );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        if( StrtMode == 0x7 && !OnAlt ) {
            DbgBiosMode = 7;
        } else {
            DbgBiosMode = 3;
        }
        SetEGA_VGA( 50 );
        break;
    }
    if( DbgRows == 0 ) DbgRows = 25;
    if( DbgBiosMode == 7 ) {
        VIDPort = VIDMONOINDXREG;
    } else {
        VIDPort = VIDCOLRINDXREG;
    }
    if( ((StrtMode & 0x7f) == DbgBiosMode) && (StrtRows == DbgRows) ) {
        GetBIOSData( BD_REGEN_LEN, PageSize );  /* get size from BIOS */
    } else {
        PageSize = (DbgRows == 25) ? 4096 : ( DbgRows * (80 * 2) + 256 );
    }
}


static void SaveBIOSSettings()
{
    SaveSwtchs = GetSwtchs();
    SaveMode = BIOSGetMode();
    SavePage = BIOSGetPage();
    SavCurPos = BIOSGetCurPos( SavePage );
    SavCurTyp = BIOSGetCurTyp( SavePage );

    if( SavCurTyp == CGA_CURSOR_ON && SaveMode == 0x7 ) {
        /* screwy hercules card lying about cursor type */
        SavCurTyp = MON_CURSOR_ON;
    }

    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        SavPoints = BIOSGetPoints();
        break;
    default:
        SavPoints = 8;
        break;
    }
}


/*
 * ForceLines -- force a specified number of lines for MDA/CGA systems
 */

void ForceLines( unsigned lines )
{
    DbgRows = lines;
}


/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

unsigned ConfigScreen()
{
    OnAlt = FALSE;

    GetDispConfig();

    SaveBIOSSettings();

    StrtPoints = SavPoints;
    StrtCurTyp = SavCurTyp;
    StrtSwtchs = SaveSwtchs;
    StrtMode   = SaveMode;
    if( StrtMode < 4 || StrtMode == 7 ) {
        StrtAttr = BIOSGetAttr( SavePage ) & 0x7f;
    } else {
        StrtAttr = 0;
    }
    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        StrtRows = BIOSGetRows();
        break;
    }


    /* get adapter to use */
    switch( ScrnMode ) {
    case MD_HERC: /* temp */
    case MD_DEFAULT:
        GetDefault();
        break;
    case MD_MONO:
        if( ChkMono() == FALSE ) GetDefault();
        break;
    case MD_COLOUR:
        if( ChkColour() == FALSE ) GetDefault();
        break;
    case MD_EGA:
        if( ChkEGA() == FALSE ) GetDefault();
        break;
    }
    /* get flip mechanism to use */
    switch( FlipMech ) {
    case FLIP_PAGE:
        ChkPage();
        break;
    case FLIP_TWO:
        ChkTwo();
        break;
    }
    SetMonitor();
    BIOSSetCurTyp( StrtCurTyp );
    return( PageSize );
}


static bool SetMode( unsigned char mode )
{
    if( (BIOSGetMode() & 0x7f) == (mode & 0x7f) ) return( FALSE );
    DoSetMode( mode );
    return( TRUE );
}


static void SetRegenClear()
{
    unsigned char regen;

    GetBIOSData( BD_VID_CTRL1, regen );
    regen &= 0x7f;
    regen |= SaveMode & 0x80;
    SetBIOSData( BD_VID_CTRL1, regen );
}


static unsigned RegenSize()
{
    unsigned    regen_size;

    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        regen_size = (DbgRows * (80*2) + 0x3ff) & ~0x3ff;
        break;
    case DISP_CGA:
    case DISP_PGA:
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
        regen_size = (DbgRows * (80*2) + 0x3ff) & ~0x3ff;
        regen_size *= 4;
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
    case DISP_EGA_COLOUR:
    case DISP_EGA_MONO:
        regen_size = PageSize*2 + 8*1024;
        break;
    }
    return( regen_size );
}


static void SetupEGA()
{
    _disablev( VIDPort + 6 );
    _seq_write( SEQ_MEM_MODE, MEM_NOT_ODD_EVEN );
    _graph_write( GRA_MISC, MIS_A000_64 | MIS_GRAPH_MODE );
    _graph_write( GRA_ENABLE_SR, 0 );
    _graph_write( GRA_DATA_ROT, ROT_UNMOD | 0 );
    _graph_write( GRA_GRAPH_MODE, GRM_EN_ROT );
}

static void SwapSave()
{
    switch( HWDisplay.active ) {
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        _vidstatesave( VID_STATE_SWAP, SwapSeg, PageSize * 2 + 8*1024 );
        /* fall through */
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        SetupEGA();
        _graph_write( GRA_READ_MAP, RMS_MAP_0 );
        movedata( 0xA000, 0, SwapSeg, 0, PageSize );
        _graph_write( GRA_READ_MAP, RMS_MAP_1 );
        movedata( 0xA000, 0, SwapSeg, PageSize, PageSize );
        _graph_write( GRA_READ_MAP, RMS_MAP_2 );
        movedata( 0xA000, 0, SwapSeg, PageSize * 2, 8 * 1024 );
        _graph_write( GRA_READ_MAP, RMS_MAP_0 );
        /* blank regen area (attributes) */
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        Fillb( 0xA000, 0, 0, PageSize );
        DoSetMode( DbgBiosMode | 0x80 );
        SetChrSet( DbgChrSet );
        break;
    case DISP_MONOCHROME:
        movedata( 0xb000, 0, SwapSeg, 0, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    default:
        movedata( 0xb800, 0, SwapSeg, 0, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    }
}

static unsigned char RestoreEGA_VGA()
{
    unsigned char       mode;

    SetupEGA();
    _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
    movedata( SwapSeg, 0, 0xA000, 0, PageSize );
    _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
    movedata( SwapSeg, PageSize, 0xA000, 0, PageSize );
    mode = SaveMode & 0x7f;
    if( mode < 4 || mode == 7 ) {
        DoSetMode( SaveMode | 0x80 );
        BIOSCharSet( 0x00, 32, 256, 0, SwapSeg, PageSize*2 );
//        BIOSCharSet( 0x10, SavPoints, 0, 0, 0, 0 );
    } else {
        _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
        movedata( SwapSeg, PageSize*2, 0xA000, 0, 8*1024 );
        DoSetMode( SaveMode | 0x80 );
    }
    SetRegenClear();
    return( mode );
}

static void SwapRestore()
{
    unsigned char       mode;

    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        mode = RestoreEGA_VGA();
        if( mode < 4 || mode == 7 ) {
            BIOSCharSet( 0x10, SavPoints, 0, 0, 0, 0 );
        }
        _seq_write( SEQ_CHAR_MAP_SEL, ActFontTbls );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        RestoreEGA_VGA();
        _vidstaterestore( VID_STATE_SWAP, SwapSeg, PageSize * 2 + 8*1024 );
        break;
    case DISP_MONOCHROME:
        SetMode( SaveMode );
        movedata( SwapSeg, 0, 0xb000, 0, RegenSize() );
        break;
    default:
        SetMode( SaveMode );
        movedata( SwapSeg, 0, 0xb800, 0, RegenSize() );
        break;
    }
}

static void SaveMouse( unsigned to )
{
    if( to != 0 ) MouseSaveState( SwapSeg, to );
}

static void RestoreMouse( unsigned from )
{
    if( from != 0 ) MouseRestoreState( SwapSeg, from );
}


static void AllocSave()
{
    unsigned    state_size;
    unsigned    mouse_size;
    unsigned    regen_size;
    tiny_ret_t  ret;

    regen_size = 2; /* make mouse swapping detection work */
    if( FlipMech == FLIP_SWAP ) {
        regen_size = RegenSize();
    }
    state_size = _vidstatesize( VID_STATE_SWAP ) * 64;
    mouse_size = _IsOn( SW_USE_MOUSE ) ? MouseSaveSize() : 0;
    ret = TinyAllocBlock( (regen_size + state_size + 2*mouse_size + 0xf) >> 4 );
    if( ret < 0 ) StartupErr( "unable to allocate swap area" );
    SwapSeg = ret;
    if( mouse_size != 0 ) {
        PgmMouse = regen_size + state_size;
        DbgMouse = PgmMouse + mouse_size;
    }
}


/*
 * InitScreen
 */

void InitScreen()
{
    unsigned    scan_lines;
    char        far *vect;

    /* check for Microsoft mouse */
    vect = *((char far * far *)MK_FP( 0, 4*0x33 ));
    if( vect == NULL || *vect == IRET ) _SwitchOff( SW_USE_MOUSE );

    AllocSave();
    SaveMouse( PgmMouse );
    SaveMouse( DbgMouse );
    CurOffst = 0;
    switch( FlipMech ) {
    case FLIP_SWAP:
        SwapSave();
        SetRegenClear();
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        break;
    case FLIP_PAGE:
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        SaveBIOSSettings();
        BIOSSetPage( 1 );
        GetBIOSData( BD_REGEN_LEN, CurOffst );
        CurOffst /= 2;
        break;
    case FLIP_TWO:
        DoSetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        break;
    case FLIP_OVERWRITE:
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        SaveBIOSSettings();
        break;
    }
    /* set cursor types */
    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        RegCur = MON_CURSOR_ON;
        NoCur  = NORM_CURSOR_OFF;
        break;
    case DISP_CGA:
    case DISP_PGA:              /* just guessing here */
        RegCur = CGA_CURSOR_ON;
        NoCur  = NORM_CURSOR_OFF;
        break;
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        /* scan lines per character */
        scan_lines = BIOSGetPoints();
        RegCur = (scan_lines - 1) + ((scan_lines - 2)<<8);
        NoCur = EGA_CURSOR_OFF;
        break;
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        RegCur = VIDGetCurTyp( VIDPort );
        NoCur = NORM_CURSOR_OFF;
        break;
    }
    InsCur = ( ((RegCur + 0x100) >> 1 & 0xFF00) + 0x100 ) | ( RegCur & 0x00FF );
}


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode()
{
    char user_mode;
    bool    usr_vis;

    if( (StrtAttr != 0) && (DbgBiosMode == StrtMode) ) {
        UIData->attrs[ATTR_NORMAL]  = StrtAttr;
        UIData->attrs[ATTR_BRIGHT]  = StrtAttr ^ 0x8;
        UIData->attrs[ATTR_REVERSE] = (StrtAttr&0x07)<<4 | (StrtAttr&0x70)>>4;
    }
    usr_vis = FALSE;
    if( FlipMech == FLIP_TWO ) {
        usr_vis = TRUE;
        SaveMouse( DbgMouse );
        RestoreMouse( PgmMouse );
        user_mode = (DbgBiosMode == 7) ? 3 : 7;
        DoSetMode( user_mode );
        SaveBIOSSettings();
        SaveMouse( PgmMouse );
        RestoreMouse( DbgMouse );
    }
    SaveSwtchs = GetSwtchs();
    if( HWDisplay.active == DISP_VGA_COLOUR
     || HWDisplay.active == DISP_VGA_MONO ) {
        UIData->colour = M_VGA;
    }
    if( DbgRows != UIData->height ) {
        UIData->height = DbgRows;
        if( _IsOn( SW_USE_MOUSE ) ) {
            extern int  initmouse( int );
            /*
                This is a sideways dive into the UI to get the boundries of
                the mouse cursor properly defined.
            */
            initmouse( 1 );
        }
    }
    return( usr_vis );
}


void DbgScrnMode()
{
    if( FlipMech == FLIP_PAGE ) {
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
            WndDirty();
        }
        BIOSSetPage( 1 );
    }
}


/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen()
{
    bool    usr_vis;

    usr_vis = TRUE;
    SaveMouse( PgmMouse );
    SaveBIOSSettings();
    switch( FlipMech ) {
    case FLIP_SWAP:
        SwapSave();
        BIOSSetPage( 0 );
        WndDirty();
        usr_vis = FALSE;
        break;
    case FLIP_PAGE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
            WndDirty();
        }
        BIOSSetPage( 1 );
        usr_vis = FALSE;
        break;
    case FLIP_OVERWRITE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
        }
        WndDirty();
        usr_vis = FALSE;
        break;
    }
    RestoreMouse( DbgMouse );
    uiswap();
    return( usr_vis );
}


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen()
{
    bool    dbg_vis;

    dbg_vis = TRUE;
    uiswap();
    SaveMouse( DbgMouse );
    switch( FlipMech ) {
    case FLIP_SWAP:
        SwapRestore();
        dbg_vis = FALSE;
        break;
    case FLIP_PAGE:
        dbg_vis = FALSE;
        break;
    }
    BIOSSetPage( SavePage );
    BIOSSetCurTyp( SavCurTyp );
    BIOSSetCurPos( SavCurPos, SavePage );
    SetSwtchs( SaveSwtchs );
    RestoreMouse( PgmMouse );
    return( dbg_vis );
}


static void ReInitScreen()
{
    RestoreMouse( PgmMouse );
    SetSwtchs( StrtSwtchs );
    BIOSSetMode( StrtMode );
    switch( StrtMode & 0x7f ) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x7:
        switch( HWDisplay.active ) {
        case DISP_EGA_MONO:
        case DISP_EGA_COLOUR:
        case DISP_VGA_MONO:
        case DISP_VGA_COLOUR:
            SetChrSet( GetChrSet( StrtRows ) );
            break;
        }
        break;
    }
    BIOSSetCurTyp( StrtCurTyp );
    if( StrtAttr != 0 ) BIOSSetAttr( StrtAttr );
}

/*
 * FiniScreen -- finish screen swapping/paging
 */

void FiniScreen()
{
    uifini();
    if( SaveSwtchs != StrtSwtchs
     || SaveMode   != StrtMode
     || SavPoints  != StrtPoints
     || FlipMech   == FLIP_OVERWRITE ) {
        ReInitScreen();
    } else {
        UserScreen();
    }
}


/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/


static char OldRow, OldCol, OldTyp;

void uiinitcursor()
{
}

#pragma off( unreferenced );
void uisetcursor( ORD row, ORD col, int typ, int attr )
#pragma off( unreferenced );
{
    unsigned    bios_cur_pos;

    if( typ == C_OFF ) {
        uioffcursor();
    } else if( (ScrnState & DBG_SCRN_ACTIVE) && ( VIDPort != NULL ) ) {
        if( row == OldRow && col == OldCol && typ == OldTyp ) return;
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        bios_cur_pos = BD_CURPOS;
        if( FlipMech == FLIP_PAGE ) bios_cur_pos += 2;
        SetBIOSData( bios_cur_pos+0, OldCol );
        SetBIOSData( bios_cur_pos+1, OldRow );
        VIDSetPos( VIDPort, CurOffst + row * UIData->width + col );
        VIDSetCurTyp( VIDPort, typ == C_INSERT ? InsCur : RegCur );
    }
}


void uioffcursor()
{
    if( (ScrnState & DBG_SCRN_ACTIVE) && ( VIDPort != NULL ) ) {
        OldTyp = C_OFF;
        VIDSetCurTyp( VIDPort, NoCur );
    }
}

void uiswapcursor()
{
}

void uifinicursor()
{
}

void uirefresh()
{
    extern void uidorefresh(void);

    if( ScrnState & DBG_SCRN_ACTIVE ) {
        uidorefresh();
    }
}

#if 0
#define OFF_SCREEN      200
static ORD OldMouseRow, OldMouseCol = OFF_SCREEN;
static bool MouseOn;
static ATTR OldAttr;


void uimouse( int func )
{
    if( func == 1 ) {
        MouseOn = TRUE;
    } else {
        uisetmouse( 0, OFF_SCREEN );
        MouseOn = FALSE;
    }
}


#pragma aux vertsync =                                          \
                    0xba 0xda 0x03      /* mov dx,3da   */      \
                    0xec                /* in al,dx     */      \
                    0xa8 0x08           /* test al,8    */      \
                    0x74 0xfb           /* jz -5        */      \
                modify [ax dx];
extern void vertsync();

static char far *RegenPos( unsigned row, unsigned col )
{
    char        far *pos;

    pos = (char far *)UIData->screen.origin + (row*UIData->screen.increment+col)*2 + 1;
    if( UIData->colour == M_CGA && _IsOff( SW_NOSNOW ) ) {
        /* wait for vertical retrace */
        vertsync();
    }
    return( pos );
}

void uisetmouse( ORD row, ORD col )
{
    char        far *old;
    char        far *new;

    if( OldMouseRow == row && OldMouseCol == col ) return;
    if( OldMouseCol != OFF_SCREEN ) {
        old = RegenPos( OldMouseRow, OldMouseCol );
        *old = OldAttr;
    }
    if( MouseOn ) {
        if( col != OFF_SCREEN ) {
            new = RegenPos( row, col );
            OldAttr = *new;
            *new = (OldAttr & 0x77) ^ 0x77;
        }
        OldMouseRow = row;
        OldMouseCol = col;
    }
}
#endif
