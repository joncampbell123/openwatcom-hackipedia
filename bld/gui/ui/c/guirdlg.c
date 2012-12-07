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


#include "guiwind.h"
#include <string.h>
#include <stdlib.h>

#include "guiutil.h"
#include "guixscal.h"
#include "guildstr.h"
#include "watcom.h"
#include "resdiag.h"

#define DLG_X_MULT      4
#define DLG_Y_MULT      14

void GUIFreeDialogBoxControlPtrs( DialogBoxControl *dbc )
{
    if( dbc ) {
        if( dbc->ClassID ) {
            GUIMemFree( dbc->ClassID );
            dbc->ClassID = NULL;
        }
        if( dbc->Text ) {
            GUIMemFree( dbc->Text );
            dbc->Text = NULL;
        }
    }
}

void GUIFreeDialogBoxHeader( DialogBoxHeader *hdr )
{
    if( hdr ) {
        if( hdr->MenuName ) {
            GUIMemFree( hdr->MenuName );
            hdr->MenuName = NULL;
        }
        if( hdr->ClassName ) {
            GUIMemFree( hdr->ClassName );
            hdr->ClassName = NULL;
        }
        if( hdr->Caption ) {
            GUIMemFree( hdr->Caption );
            hdr->Caption = NULL;
        }
        if( hdr->FontName ) {
            GUIMemFree( hdr->FontName );
            hdr->FontName = NULL;
        }
        GUIMemFree( hdr );
    }
}

static char *ResNameOrOrdinalToStr( ResNameOrOrdinal *name, int base )
{
    char        temp[15];
    char        *cp;
    bool        ok;

    cp = NULL;

    if( name != NULL ) {
        if( name->ord.fFlag == 0xff) {
            utoa( name->ord.wOrdinalID, temp,  base );
            ok = GUIStrDup( temp, &cp );
        } else {
            ok = GUIStrDup( name->name, &cp );
        }
        if( !ok || ( cp == NULL ) ) {
            return( NULL );
        }
    }

    return( cp );
}

static ControlClass *Data2ControlClass( uint_8 **data )
{
    ControlClass        *new;
    uint_8              *data8;
    int                 stringlen;
    int                 len;

    if( !data || !*data ) {
        return( NULL );
    }

    stringlen = 0;
    len = sizeof(ControlClass);
    data8 = (uint_8 *)*data;
    if( ( *data8 & 0x80 ) == 0 ) {
        stringlen = strlen( (char *)(*data) ) + 1;
        len = stringlen;
    }

    new = (ControlClass *)GUIMemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    if( stringlen == 0 ) {
        new->Class = data8[0];
    } else {
        memcpy( new, *data, len );
    }

    (*data) += len;

    return( new );
}

static ResNameOrOrdinal *Data2NameOrOrdinal( uint_8 **data )
{
    ResNameOrOrdinal    *new;
    uint_8              *data8;
    int                 stringlen;
    int                 len;

    if( !data || !*data ) {
        return( NULL );
    }

    data8 = (uint_8 *)(*data);

    if( *data8 == 0xff ) {
        len = sizeof(ResNameOrOrdinal);
    } else {
        stringlen = strlen( (char *)data8 ) + 1;
        len = max( sizeof(ResNameOrOrdinal), stringlen );
    }

    new = (ResNameOrOrdinal *)GUIMemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    if( *data8 == 0xff ) {
        memcpy( new, data8, sizeof(ResNameOrOrdinal) );
        *data += len;
    } else {
        memcpy( &new->name[0], data8, stringlen );
        *data += stringlen;
    }

    return( new );
}

static bool Template2DlgCntl( uint_8 **data, DialogBoxControl *dbc )
{
    bool        ok;

    ok = ( data && *data && dbc );

    if( ok ) {
        memcpy( dbc, *data, offsetof( DialogBoxControl, ClassID ) );
        *data += offsetof( DialogBoxControl, ClassID );
        dbc->ClassID = Data2ControlClass( data );
        ok = ( dbc->ClassID != NULL );
    }

    if( ok ) {
        dbc->Text = Data2NameOrOrdinal( data );
        ok = ( dbc->Text != NULL );
    }

    if( ok ) {
        dbc->ExtraBytes = (uint_8)((*data)[0]);
        *data += sizeof(uint_8);
    }

    if( !ok ) {
        GUIFreeDialogBoxControlPtrs( dbc );
    }

    return( ok );
}

static DialogBoxHeader *Template2DlgHdr( uint_8 **data )
{
    DialogBoxHeader     *hdr;
    bool                ok;
    uint_8              *p;

    hdr = NULL;

    ok = ( data && *data );

    if( ok ) {
        hdr = (DialogBoxHeader *) GUIMemAlloc( sizeof(DialogBoxHeader) );
        ok = ( hdr != NULL );
    }

    if( ok ) {
        memcpy( hdr, *data, offsetof( DialogBoxHeader, MenuName ) );
        *data += offsetof( DialogBoxHeader, MenuName );
        hdr->MenuName = Data2NameOrOrdinal( data );
        ok = ( hdr->MenuName != NULL );
    }

    if( ok ) {
        hdr->ClassName = Data2NameOrOrdinal( data );
        ok = ( hdr->ClassName != NULL );
    }

    if( ok ) {
        ok = GUIStrDup( (char *)(*data), (char **)&p );
        /* Have to do this with a temp because &hdr->Caption is unaligned */
        hdr->Caption = (char *)p;
        ok = ok && ( p != NULL );
        if( ok ) {
            *data += ( strlen( (char *)p ) + 1 );
        }
    }

    if( ok ) {
        hdr->PointSize = 0;
        hdr->FontName = NULL;
        if( hdr->Style & DS_SETFONT ) {
            // data may not be aligned -- need memcpy for UNIX platforms
            memcpy( &hdr->PointSize, *data, sizeof( hdr->PointSize ) );
            *data += sizeof(uint_16);
            ok = GUIStrDup( (char *)*data, (char **)&p );
            /* Have to do this with a temp because &hdr->FontName is unaligned */
            hdr->FontName = (char *)p;
            ok = ok && ( hdr->FontName != NULL );
            if( ok ) {
                *data += ( strlen( hdr->FontName ) + 1 );
            }
        }
    }

    if( !ok ) {
        if( hdr ) {
            GUIFreeDialogBoxHeader( hdr );
            hdr = NULL;
        }
    }

    return( hdr );
}

static bool Template2Dlg( DialogBoxHeader **hdr, DialogBoxControl **cntls,
                          uint_8 *data, int size )
{
    bool        ok;
    uint_8      *d;
    int         index;

    ok = ( hdr && cntls && data && size );

    if( ok ) {
        d = data;
        *hdr = NULL;
        *cntls = NULL;
        *hdr = Template2DlgHdr( &data );
        ok = ( *hdr != NULL );
    }

    if( ok ) {
        *cntls = (DialogBoxControl *)
            GUIMemAlloc( (*hdr)->NumOfItems * sizeof( DialogBoxControl ) );
        ok = ( *cntls != NULL );
    }

    if( ok ) {
        for( index = 0; ok && index < (*hdr)->NumOfItems; index++ ) {
            ok = Template2DlgCntl( &data, &((*cntls)[index]) );
        }
    }

    if( ok ) {
        ok = ( size >= ( data - d ) );
    }

    if( !ok ) {
        if( *hdr ) {
            GUIFreeDialogBoxHeader( *hdr );
            *hdr = NULL;
        }
        if( *cntls ) {
            for( index = 0; ok && index < (*hdr)->NumOfItems; index++ ) {
                GUIFreeDialogBoxControlPtrs( cntls[index] );
            }
            GUIMemFree( *cntls );
            *cntls = NULL;
        }
    }

    return( ok );
}

static gui_control_styles GetControlStyles( DialogBoxControl *ctl,
                                            gui_control_class control_class )
{
    gui_control_styles  styles;

    styles = GUI_NOSTYLE;

    if( ctl->Style & WS_TABSTOP ) {
        styles |= GUI_TAB_GROUP;
    }

    switch( control_class ) {
        case GUI_CHECK_BOX:
            styles |= GUI_GROUP;
            if( ( ctl->Style & 0xf ) == BS_3STATE ) {
                styles |= GUI_CONTROL_3STATE;
            }
            if( ( ctl->Style & 0xf ) == BS_AUTO3STATE ) {
                styles |= GUI_CONTROL_3STATE | GUI_AUTOMATIC;
            }
            break;
        case GUI_RADIO_BUTTON:
            if( ( ctl->Style & 0xf ) == BS_AUTORADIOBUTTON ) {
                styles |= GUI_AUTOMATIC;
            }
            break;
        case GUI_LISTBOX:
            if( ctl->Style & LBS_NOINTEGRALHEIGHT ) {
                styles |= GUI_CONTROL_NOINTEGRALHEIGHT;
            }
            if( ctl->Style & LBS_SORT ) {
                styles |= GUI_CONTROL_SORTED;
            }
            break;
        case GUI_STATIC:
            if( ctl->Style & SS_NOPREFIX ) {
                styles |= GUI_CONTROL_NOPREFIX;
            }
            if( ( ctl->Style & 0xf ) == SS_CENTER ) {
                styles |= GUI_CONTROL_CENTRE;
            }
            if( ( ctl->Style & 0xf ) == SS_LEFTNOWORDWRAP ) {
                styles |= GUI_CONTROL_LEFTNOWORDWRAP;
            }
            break;
        case GUI_EDIT_COMBOBOX:
        case GUI_COMBOBOX:
            if( ctl->Style & CBS_NOINTEGRALHEIGHT ) {
                styles |= GUI_CONTROL_NOINTEGRALHEIGHT;
            }
            if( ctl->Style & CBS_SORT ) {
                styles |= GUI_CONTROL_SORTED;
            }
            break;
        case GUI_EDIT:
        case GUI_EDIT_MLE:
            if( ctl->Style & ES_MULTILINE ) {
                styles |= GUI_CONTROL_MULTILINE;
            }
            if( ctl->Style & ES_WANTRETURN ) {
                styles |= GUI_CONTROL_WANTRETURN;
            }
            break;
        default :
            break;
    }

    return( styles );
}

#define CHK_BSTYLE(s,m) ( ( s & m ) == m )

static gui_control_class GetControlClass( DialogBoxControl *ctl )
{
    gui_control_class   control_class;

    control_class = GUI_BAD_CLASS;

    if( ctl && ctl->ClassID && ( ctl->ClassID->Class & 0x80 ) ) {
        switch( ctl->ClassID->Class ) {
            case CLASS_BUTTON:
                control_class = GUI_PUSH_BUTTON;
                if( CHK_BSTYLE( ctl->Style, BS_GROUPBOX ) ) {
                    control_class = GUI_GROUPBOX;
                } else if( CHK_BSTYLE( ctl->Style, BS_AUTORADIOBUTTON ) ||
                           CHK_BSTYLE( ctl->Style, BS_RADIOBUTTON ) ) {
                    control_class = GUI_RADIO_BUTTON;
                } else if( CHK_BSTYLE( ctl->Style, BS_AUTOCHECKBOX ) ||
                           CHK_BSTYLE( ctl->Style, BS_CHECKBOX ) ||
                           CHK_BSTYLE( ctl->Style, BS_3STATE ) ||
                           CHK_BSTYLE( ctl->Style, BS_AUTO3STATE ) ) {
                    control_class = GUI_CHECK_BOX;
                } else if( CHK_BSTYLE( ctl->Style, BS_DEFPUSHBUTTON ) ) {
                    control_class = GUI_DEFPUSH_BUTTON;
                }
                break;
            case CLASS_EDIT:
                control_class = GUI_EDIT;
                if( ctl->Style & ES_MULTILINE ) {
                    control_class = GUI_EDIT_MLE;
                }
                break;
            case CLASS_STATIC:
                control_class = GUI_STATIC;
                break;
            case CLASS_LISTBOX:
                control_class = GUI_LISTBOX;
                break;
            case CLASS_SCROLLBAR:
                control_class = GUI_SCROLLBAR;
                break;
            case CLASS_COMBOBOX:
                control_class = GUI_EDIT_COMBOBOX;
                if( ctl->Style & CBS_DROPDOWNLIST ) {
                    control_class = GUI_COMBOBOX;
                }
                break;
        }
    }

    return( control_class );
}

static bool DialogBoxControl2GUI( DialogBoxControl *ctl,
                                  gui_control_info *gci )
{
    SAREA               area;
    bool                ok;

    ok = ( ctl && gci );

    if( ok ) {
        // initialize the create struct
        memset( gci, 0, sizeof( gui_control_info ) );

        // set the control class
        gci->control_class = GetControlClass( ctl );
        ok = ( gci->control_class != GUI_BAD_CLASS );
    }

    if( ok ) {
        // set the control style
        gci->style = GetControlStyles( ctl, gci->control_class );
    }

    if( ok ) {
        // set the initial text. NULL is ok
        gci->text = ResNameOrOrdinalToStr( ctl->Text, 10 );

        // set the control id
        gci->id = ctl->ID;

        // set the scroll styles
        gci->scroll = GUI_NOSCROLL;
        if( ctl->Style & WS_HSCROLL ) {
            gci->scroll = GUI_HSCROLL;
        }
        if( ctl->Style & WS_VSCROLL ) {
            gci->scroll = GUI_VSCROLL;
        }

        // set the control postion
        area.row = ctl->Size.y / DLG_Y_MULT;
        area.col = ctl->Size.x / DLG_X_MULT;
        area.width = max( 1, ( ( ctl->Size.width + DLG_X_MULT/2 ) / DLG_X_MULT ) );
        area.height = max( 1, ( ( ctl->Size.height + DLG_Y_MULT/2 ) / DLG_Y_MULT ) );
        ok = GUIScreenToScaleRectR( &area, &gci->rect );
    }

    if( !ok ) {
        if( gci ) {
            if( gci->text ) {
                GUIMemFree( gci->text );
            }
        }
    }

    return( ok );
}

static gui_create_info *DialogBoxHeader2GUI( DialogBoxHeader *hdr )
{
    gui_create_info     *gci;
    SAREA               area;
    SAREA               bounding;
    bool                ok;

    ok = ( hdr != NULL );

    if( ok ) {
        gci = (gui_create_info *)GUIMemAlloc( sizeof( gui_create_info ) );
        ok = ( gci != NULL );
    }

    if( ok ) {
        // initialize the create struct
        memset( gci, 0, sizeof( gui_create_info ) );

        // set the initial text
        GUIStrDup( hdr->Caption, &gci->text ); // NULL text is ok

        // set the dialog postion remembering to add the size of the frame
        GUIGetScreenArea( &bounding );
        area.width = hdr->Size.width / DLG_X_MULT + 2;
        area.height = hdr->Size.height / DLG_Y_MULT + 2;
        area.row = max( 0, ( bounding.height - area.height ) / 2 );
        area.col = max( 0, ( bounding.width - area.width ) / 2 );
        ok = GUIScreenToScaleRect( &area, &gci->rect );
    }

    if( ok ) {
        // set the scroll styles
        gci->scroll = GUI_NOSCROLL;
        if( hdr->Style & WS_HSCROLL ) {
            gci->scroll = GUI_HSCROLL;
        }
        if( hdr->Style & WS_VSCROLL ) {
            gci->scroll = GUI_VSCROLL;
        }
        // set the window styles
        gci->style = GUI_NONE;
        if( hdr->Style & WS_THICKFRAME ) {
            gci->style = GUI_RESIZEABLE;
        }
        if( hdr->Style & WS_THICKFRAME ) {
            gci->style = GUI_RESIZEABLE;
        }
        if( hdr->Style & WS_MINIMIZEBOX ) {
            gci->style = GUI_MINIMIZE;
        }
        if( hdr->Style & WS_MAXIMIZEBOX ) {
            gci->style = GUI_MAXIMIZE;
        }
        if( hdr->Style & WS_SYSMENU ) {
            gci->style = GUI_SYSTEM_MENU;
        }
        if( hdr->Style & WS_POPUP ) {
            gci->style = GUI_POPUP;
        }
    }

    if( !ok ) {
        if( gci ) {
            if( gci->text ) {
                GUIMemFree( gci->text );
            }
            GUIMemFree( gci );
            gci = NULL;
        }
    }

    return( gci );
}

bool GUICreateDialogFromRes( int id, gui_window *parent, GUICALLBACK cb,
                             void *extra )
{
    DialogBoxHeader     *hdr;
    DialogBoxControl    *cntls;
    gui_create_info     *gui_dlg;
    gui_control_info    *gui_cntls;
    uint_8              *data;
    int                 size;
    int                 index;
    int                 last_was_radio;
    bool                ok;

    hdr = NULL;
    cntls = NULL;
    data = NULL;
    gui_dlg = NULL;
    gui_cntls = NULL;
    size = 0;

    ok = ( cb != NULL );

    if( ok ) {
        ok = GUILoadDialogTemplate( id, (char **)&data, &size );
    }

    if( ok ) {
        ok = Template2Dlg( &hdr, &cntls, data, size );
    }

    if( ok ) {
        gui_dlg = DialogBoxHeader2GUI( hdr );
        ok = ( gui_dlg != NULL );
    }

    if( ok ) {
        gui_cntls = (gui_control_info *)
            GUIMemAlloc( sizeof( gui_control_info ) * hdr->NumOfItems );
        ok = ( gui_cntls != NULL );
    }

    last_was_radio = -1;
    if( ok ) {
        memset( gui_cntls, 0, sizeof( gui_control_info ) * hdr->NumOfItems );
        for( index = 0; ok && index < hdr->NumOfItems; index++ ) {
            ok = DialogBoxControl2GUI( &cntls[index], &gui_cntls[index] );
            if( ok ) {
                if( gui_cntls[index].control_class == GUI_RADIO_BUTTON ) {
                    if( last_was_radio != 1 ) {
                        gui_cntls[index].style |= GUI_GROUP;
                    }
                    last_was_radio = 1;
                } else {
                    if( last_was_radio == 1 ) {
                        gui_cntls[index-1].style |= GUI_GROUP;
                    }
                    last_was_radio = 0;
                }
            }
        }
    }

    if( ok ) {
        gui_dlg->parent = parent;
        gui_dlg->call_back = cb;
        gui_dlg->extra = extra;
        ok = GUICreateDialog( gui_dlg, hdr->NumOfItems, gui_cntls );
    }

    if( gui_cntls ) {
        for( index = 0; ok && index < hdr->NumOfItems; index++ ) {
            GUIMemFree( gui_cntls[index].text );
        }
        GUIMemFree( gui_cntls );
    }

    if( gui_dlg ) {
        if( gui_dlg->text ) {
            GUIMemFree( gui_dlg->text );
        }
        GUIMemFree( gui_dlg );
    }

    if( cntls ) {
        for( index = 0; ok && index < hdr->NumOfItems; index++ ) {
            GUIFreeDialogBoxControlPtrs( &cntls[index] );
        }
        GUIMemFree( cntls );
    }

    if( hdr ) {
        GUIFreeDialogBoxHeader( hdr );
    }

    if( data ) {
        GUIMemFree( data );
    }

    return( ok );
}

