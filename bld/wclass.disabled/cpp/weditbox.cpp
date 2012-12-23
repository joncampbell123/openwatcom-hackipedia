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


#include "weditbox.hpp"


//      Define Edit Box
//      ===============


WEXPORT WEditBox::WEditBox( WWindow *p, const WRect& r, const char *text,
                            WStyle wstyle )
        : WEdit( p, GUI_EDIT, r, text, wstyle ) {
/***********************************************/

}

WEXPORT WEditBox::WEditBox( WStatDialog *p, unsigned id,
                            WStyle wstyle )
        : WEdit( p, id, wstyle ) {
/********************************/

}


WEXPORT WEditBox::~WEditBox() {
/*****************************/

}


//      Define Multiline Edit Box
//      =========================


WEXPORT WMultiLineEditBox::WMultiLineEditBox( WWindow *p, const WRect& r,
                                              const char *text,
                                              WStyle wstyle )
        : WEdit( p, GUI_EDIT_MLE, r, text,
                 wstyle | _ControlStyleToWStyle( GUI_CONTROL_WANTRETURN ) ) {
/***************************************************************************/

}

WEXPORT WMultiLineEditBox::WMultiLineEditBox( WStatDialog *p, unsigned id,
                                              WStyle wstyle )
        : WEdit( p, id,
                 wstyle | _ControlStyleToWStyle( GUI_CONTROL_WANTRETURN ) ) {
/***************************************************************************/

}


WEXPORT WMultiLineEditBox::~WMultiLineEditBox() {
/***********************************************/

}


//      Define General Edit Box
//      =======================


WEXPORT WEdit::WEdit( WWindow *parent, gui_control_class control_class,
                      const WRect& r, const char *text, WStyle wstyle )
        : WControl( parent, control_class, r, text, wstyle ) {
/************************************************************/

}

WEXPORT WEdit::WEdit( WStatDialog *parent, unsigned id,
                      WStyle wstyle )
        : WControl( parent, id, wstyle ) {
/****************************************/

}


WEXPORT WEdit::~WEdit() {
/***********************/

}


void WEXPORT WEdit::select( int first, int last ) {
/*************************************************/

    GUISetEditSelect( parent()->handle(), controlId(), first, last );
}


int WEXPORT WEdit::getSelectBounds( int *first, int *last ) {
/***********************************************************/

    GUIGetEditSelect( parent()->handle(), controlId(), first, last );
    return( *last - *first );
}
