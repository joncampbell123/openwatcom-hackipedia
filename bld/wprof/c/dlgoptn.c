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
* Description:  Profiler options dialog.
*
****************************************************************************/


#include <unistd.h>
#include <stdio.h>

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlgoptn.h"
#include "sampinfo.h"
#include "msg.h"

//#include "dlgoptn.def"
//#include "wpdriver.def"
//#include "setsamps.def"
extern void GatherSetAll(sio_data *curr_sio,bint gather_active);
extern void AbsSetAll(sio_data *curr_sio,bint abs_bar);
extern void RelSetAll(sio_data *curr_sio,bint rel_bar);
extern void StretchSetAll(sio_data *curr_sio,bint bar_max);
extern void SortSetAll(sio_data *curr_sio,int sort_type);


bint    OptStretchGlobal = P_FALSE;
bint    OptAbsBarGlobal = P_TRUE;
bint    OptRelBarGlobal = P_TRUE;
bint    OptSortCountGlobal = P_TRUE;
bint    OptSortNameGlobal = P_FALSE;
bint    OptGatherGlobal = P_FALSE;
int     OptGatherCut = 10;

STATIC bint     getDlgValues( gui_window * );
STATIC void     setDlgValues( gui_window * );
STATIC void     setDlgDefaults( gui_window * );
STATIC bool     progEvent( gui_window *, gui_event, void * );

extern sio_data *   CurrSIOData;



extern void DlgGetOptions( a_window * wnd )
/*****************************************/
{
    CurrSIOData = WndExtra( wnd );
    DlgOpen( LIT( Options ), DLG_OPTS_ROWS, DLG_OPTS_COLS,
             &optionControls[0], ArraySize( optionControls ), &progEvent, NULL );
    if( CurrSIOData != NULL ) {
        WndDirty( CurrSIOData->sample_window );
    }
}



STATIC bint getDlgValues( gui_window * gui )
/******************************************/
{
    char *      endptr;
    double      fcut;
    int         sort_type;
    char        cut_buff[SMALL_CUT_WIDTH+1];

    GUIDlgBuffGetText( gui, CTL_GATHER_CUT, cut_buff, SMALL_CUT_WIDTH );
    fcut = strtod( cut_buff, &endptr );
    if( *endptr == NULLCHAR ) {
        if( fcut > 10.0 ) {
            fcut = 10.0;
        } else if( fcut < 0 ) {
            fcut = 0.0;
        }
        OptGatherCut = fcut * 10;
    }
    OptStretchGlobal = GUIIsChecked( gui, CTL_STRETCH );
    OptAbsBarGlobal = GUIIsChecked( gui, CTL_ABS_BAR );
    OptRelBarGlobal = GUIIsChecked( gui, CTL_REL_BAR );
    OptSortCountGlobal = GUIIsChecked( gui, CTL_SORT_COUNT );
    OptSortNameGlobal = GUIIsChecked( gui, CTL_SORT_NAME );
    OptGatherGlobal = GUIIsChecked( gui, CTL_GATHER );
    if( CurrSIOData != NULL ) {
        StretchSetAll( CurrSIOData, OptStretchGlobal );
        AbsSetAll( CurrSIOData, OptAbsBarGlobal );
        RelSetAll( CurrSIOData, OptRelBarGlobal );
        if( OptSortCountGlobal ) {
            sort_type = SORT_COUNT;
        } else {
            sort_type = SORT_NAME;
        }
        SortSetAll( CurrSIOData, sort_type );
        GatherSetAll( CurrSIOData, OptGatherGlobal );
    }
    return( P_TRUE );
}



STATIC void setDlgValues( gui_window * gui )
/******************************************/
{
    char        cut_buff[SMALL_CUT_WIDTH+1];

    GUISetChecked( gui, CTL_STRETCH, OptStretchGlobal );
    GUISetChecked( gui, CTL_ABS_BAR, OptAbsBarGlobal );
    GUISetChecked( gui, CTL_REL_BAR, OptRelBarGlobal );
    GUISetChecked( gui, CTL_SORT_COUNT, OptSortCountGlobal );
    GUISetChecked( gui, CTL_SORT_NAME, OptSortNameGlobal );
    GUISetChecked( gui, CTL_GATHER, OptGatherGlobal );
    sprintf( cut_buff, "%d.%d", OptGatherCut/10,
             OptGatherCut-(OptGatherCut/10)*10 );
    GUISetText( gui, CTL_GATHER_CUT, cut_buff );
}



STATIC void setDlgDefaults( gui_window * gui )
/********************************************/
{
    OptStretchGlobal = P_FALSE;
    OptAbsBarGlobal = P_TRUE;
    OptRelBarGlobal = P_TRUE;
    OptSortCountGlobal = P_TRUE;
    OptSortNameGlobal = P_FALSE;
    OptGatherGlobal = P_FALSE;
    OptGatherCut = 1;
    setDlgValues( gui );
}



STATIC bool progEvent( gui_window * gui, gui_event gui_ev, void * param )
/***********************************************************************/
{
    unsigned            id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        setDlgValues( gui );
        GUISetFocus( gui, CTL_GATHER );
        return( P_TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
            getDlgValues( gui );
            GUICloseDialog( gui );
            return( P_TRUE );
        case CTL_DEFAULTS:
            setDlgDefaults( gui );
            return( P_TRUE );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( P_TRUE );
        }
        return( P_FALSE );
    case GUI_DESTROY:
        return( P_TRUE );
    }
    return( P_FALSE );
}
