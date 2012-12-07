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


#include <windows.h>
#include <string.h>
#include <mbstring.h>
#include <stdlib.h>

#include "wdeglbl.h"
#include "wdeactn.h"
#include "wderes.h"
#include "wdefordr.h"
#include "wdefmenu.h"
#include "wdefont.h"
#include "wdemem.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdeedit.h"
#include "wderesiz.h"
#include "wdeinfo.h"
#include "wdemain.h"
#include "wdesym.h"
#include "wdefutil.h"
#include "wdectool.h"
#include "wdelist.h"
#include "wdealign.h"
#include "wdeopts.h"
#include "wde_rc.h"
#include "wdedebug.h"
#include "wdefbase.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_WORLD_PAD     20
#define WDE_WORLD_WIDTH   1024
#define WDE_WORLD_HEIGHT  WDE_WORLD_WIDTH

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC             dispatcher;
    OBJPTR              object_handle;
    OBJ_ID              object_id;
    OBJPTR              parent;
    OBJPTR              o_item;
    HFONT               font;
    int                 num_children;
    LIST                *children;
    LIST                *ochildren;
    WdeResizeRatio      resizer;
    WdeResInfo          *res_info;
    Bool                has_hscroll;
    Bool                has_vscroll;
    Bool                in_destroy;
} WdeBaseObject;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WINEXPORT    WdeBaseDispatcher  ( ACTION, WdeBaseObject *,
                                              void *, void *);

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static BOOL WdeBaseDraw            ( WdeBaseObject *, RECT *, HDC *);
static BOOL WdeBaseDestroy         ( WdeBaseObject *, BOOL *, void *);
static BOOL WdeBaseIsMarkValid     ( WdeBaseObject *, BOOL *, void *);
static BOOL WdeBaseGetResizeInfo   ( WdeBaseObject *, RESIZE_ID *, void *);
static BOOL WdeBaseValidateAction  ( WdeBaseObject *, ACTION *, void *);
static BOOL WdeBaseGetWindowHandle ( WdeBaseObject *, HWND *, void *);
static BOOL WdeBaseAddSubObject    ( WdeBaseObject *, OBJPTR, void *);
static BOOL WdeBaseFindSubObjects  ( WdeBaseObject *, SUBOBJ_REQUEST *,
                                     LIST **objlist );
static BOOL WdeBaseFindObjectsPt   ( WdeBaseObject *, POINT *, LIST ** );
static BOOL WdeBaseRemoveSubObject ( WdeBaseObject *, OBJPTR, void *);
static BOOL WdeBaseFirstChild      ( WdeBaseObject *, void *, void *);
static BOOL WdeBasePutChildFirst   ( WdeBaseObject *, OBJPTR, void *);
static BOOL WdeBaseGetFirstChild   ( WdeBaseObject *, OBJPTR *, void *);
static BOOL WdeBaseNotify          ( WdeBaseObject *, NOTE_ID *, void *);
static BOOL WdeBaseLocation        ( WdeBaseObject *, RECT *, void *);
static BOOL WdeBaseIdentify        ( WdeBaseObject *, OBJ_ID *, void *);
static BOOL WdeBaseGetSubObjectList( WdeBaseObject *, LIST **, void *);
static BOOL WdeBaseGetFont         ( WdeBaseObject *, HFONT *, void *);
static BOOL WdeBaseGetResizer      ( WdeBaseObject *, WdeResizeRatio *,
                                     OBJPTR * );
static BOOL WdeBaseGetNCSize       ( WdeBaseObject *, RECT *, void *);
static BOOL WdeBaseGetScrollRect   ( WdeBaseObject *, RECT *, void *);
static Bool WdeBaseGetResizeInc    ( WdeBaseObject *, POINT *, void *);
static Bool WdeBaseGetNextChild    ( WdeBaseObject *, OBJPTR *, Bool * );
static BOOL WdeBaseResolveSymbol   ( WdeBaseObject *, Bool *, Bool * );
static BOOL WdeBaseResolveHelpSymbol( WdeBaseObject *, Bool *, Bool * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static FARPROC WdeBaseDispatch;

static DISPATCH_ITEM WdeBaseActions[] = {
    { DRAW               ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseDraw             }
,   { LOCATE             ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseLocation         }
,   { DESTROY            ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseDestroy          }
,   { VALIDATE_ACTION    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseValidateAction   }
,   { NOTIFY             ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseNotify           }
,   { RESIZE_INFO        ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetResizeInfo    }
,   { FIND_SUBOBJECTS    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseFindSubObjects   }
,   { FIND_OBJECTS_PT    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseFindObjectsPt    }
,   { ADD_SUBOBJECT      ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseAddSubObject     }
,   { REMOVE_SUBOBJECT   ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseRemoveSubObject  }
,   { GET_WINDOW_HANDLE  ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetWindowHandle  }
,   { GET_SUBOBJ_LIST    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetSubObjectList }
,   { IDENTIFY           ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseIdentify         }
,   { GET_FONT           ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetFont          }
,   { GET_RESIZER        ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetResizer       }
,   { GET_NC_SIZE        ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetNCSize        }
,   { BECOME_FIRST_CHILD ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseFirstChild       }
,   { GET_FIRST_CHILD    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetFirstChild    }
,   { PUT_ME_FIRST       ,  (BOOL (*)(OBJPTR, void *, void *))WdeBasePutChildFirst    }
,   { GET_SCROLL_RECT    ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetScrollRect    }
,   { GET_RESIZE_INC     ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetResizeInc     }
,   { IS_MARK_VALID      ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseIsMarkValid      }
,   { RESOLVE_SYMBOL     ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseResolveSymbol    }
,   { RESOLVE_HELPSYMBOL ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseResolveHelpSymbol}
,   { GET_NEXT_CHILD     ,  (BOOL (*)(OBJPTR, void *, void *))WdeBaseGetNextChild     }
};

#define MAX_ACTIONS      ( sizeof( WdeBaseActions ) / sizeof ( DISPATCH_ITEM ) )

OBJPTR WINEXPORT WdeBaseCreate ( OBJPTR parent, RECT *obj_rect,
                                 OBJPTR handle)
{
    RECT                rect;
    WdeBaseObject       *new;
    char                *text;
    char                *cp;
    int                 point_size;
    Bool                use_default;

    /* touch unused vars to get rid of warning */
    _wde_touch(obj_rect);

    WdeDebugCreate("Base", parent, obj_rect, handle);

    new = (WdeBaseObject *) WdeMemAlloc ( sizeof(WdeBaseObject) );
    if ( new == NULL ) {
        WdeWriteTrail("WdeBaseCreate: Malloc failed");
        return ( new );
    }
    memset ( new, 0, sizeof(WdeBaseObject) );

    new->object_id     = BASE_OBJ;
    new->parent        = parent;
    new->font          = WdeGetEditFont();
    new->dispatcher    = WdeBaseDispatch;
    new->res_info      = WdeGetCurrentRes();

    if ( !new->font || !new->res_info ) {
        WdeWriteTrail ( "WdeBaseCreate: NULL font or NULL res_info!" );
        return ( NULL );
    }

    if( new->res_info->edit_win != NULL ) {
        GetClientRect( new->res_info->edit_win, &rect );
    } else {
        SetRectEmpty( &rect );
    }
    InflateRect( &rect, WDE_WORLD_PAD, WDE_WORLD_PAD );

    if ( handle ==  NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->o_item = Create( OBJ_ITEM, parent, &rect, new->object_handle );

    use_default = TRUE;
    text = WdeAllocRCString( WDE_BASEOBJECTFONT );
    if( text ) {
        cp = (char *)_mbschr( (unsigned char const *)text, '.' );
        if( cp ) {
            *cp = '\0';
            cp++;
            point_size = atoi( cp );
            use_default = FALSE;
        }
    }

    if( use_default ) {
        WdeGetResizerFromFont( &new->resizer, "Helv", 8 );
    } else {
        WdeGetResizerFromFont( &new->resizer, text, point_size );
    }

    if( text ) {
        WdeFreeRCString( text );
    }

    return( new );
}

BOOL WINEXPORT WdeBaseDispatcher (ACTION act, WdeBaseObject *obj,
                                   void *p1, void *p2)
{
    int i;

    WdeDebugDispatch("Base", act, obj, p1, p2);

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeBaseActions[i].id == act ) {
            return( (WdeBaseActions[i].rtn)( obj, p1, p2 ) );
        }
    }

    return (Forward ((OBJPTR)obj->o_item, act, p1, p2));
}

Bool WdeBaseInit ( Bool first )
{
    _wde_touch(first);
    WdeBaseDispatch = MakeProcInstance((FARPROC)WdeBaseDispatcher,
                                           WdeGetAppInstance());
    return( TRUE );
}

void WdeBaseFini ( void )
{
    FreeProcInstance(WdeBaseDispatch);
}

BOOL WdeBaseIsMarkValid ( WdeBaseObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);
    _wde_touch(obj);

    *flag = FALSE;

    return ( TRUE );
}

Bool WdeBaseGetNextChild ( WdeBaseObject *obj, OBJPTR *o, Bool *up )
{
    return ( WdeGetNextChild ( &obj->ochildren, o, *up ) );
}

BOOL WdeBaseResolveSymbol ( WdeBaseObject *obj, Bool *b, Bool *from_id )
{
    LIST   *olist;
    OBJPTR  child;

    if ( obj->res_info->hash_table ) {
        for ( olist = obj->children; olist; olist = ListNext ( olist ) ) {
            child =  ListElement ( olist );
            Forward ( child, RESOLVE_SYMBOL, b, from_id );
        }
    }

    return ( TRUE );
}

BOOL WdeBaseResolveHelpSymbol ( WdeBaseObject *obj, Bool *b, Bool *from_id )
{
    LIST   *olist;
    OBJPTR  child;

    if ( obj->res_info->hash_table ) {
        for ( olist = obj->children; olist; olist = ListNext ( olist ) ) {
            child =  ListElement ( olist );
            Forward ( child, RESOLVE_HELPSYMBOL, b, from_id );
        }
    }

    return ( TRUE );
}

BOOL WdeBaseDestroy ( WdeBaseObject *obj, BOOL *flag, void *p2 )
{
    OBJPTR  sub_obj;
    LIST   *clist;

    /* touch unused vars to get rid of warning */
    _wde_touch(p2);


    /* if the user initiated this destroy then don't do it! */
    if ( *flag ) {
        return ( FALSE );
    }

    obj->in_destroy = TRUE;

    /* destroy all children */
    clist = WdeListCopy ( obj->children );
    for( ; clist; clist = ListConsume(clist) ) {
        sub_obj = ListElement( clist );
        Destroy ( sub_obj, *flag );
    }

    WdeFreeOrderedList ( obj->ochildren );

    Destroy ( obj->o_item, *flag );

    WdeMemFree( obj );

    return ( TRUE );
}

BOOL WdeBaseGetScrollRect ( WdeBaseObject *obj, RECT *r, void *p2 )
{
    return ( WdeBaseLocation ( obj, r, p2 ) );
}

Bool WdeBaseGetResizeInc ( WdeBaseObject *obj, POINT *p, void *p2 )
{
    DialogSizeInfo      d;
    RECT                r;

    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    p->x = 1;
    p->y = 1;

    d.x = 0;
    d.y = 0;
    d.width = WdeGetOption( WdeOptReqGridX );
    d.height = WdeGetOption( WdeOptReqGridY );

    if( WdeDialogToScreen( obj, &(obj->resizer), &d, &r ) ) {
        p->x = r.right;
        p->y = r.bottom;
    }

    return ( TRUE );
}

BOOL WdeBaseGetResizer ( WdeBaseObject *obj, WdeResizeRatio *resizer,
                         OBJPTR *o )
{
    *resizer = obj->resizer;
    if ( o ) {
        *o = obj;
    }

    return ( TRUE );
}

BOOL WdeBaseGetFont ( WdeBaseObject *obj, HFONT *font, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *font = obj->font;

    return ( TRUE );
}

BOOL WdeBaseGetNCSize( WdeBaseObject *obj, RECT *size, void *p2)
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    memset ( size, 0, sizeof(RECT) );

    return ( TRUE );
}

BOOL WdeBaseFirstChild ( WdeBaseObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p1);
    _wde_touch(p2);

    return ( TRUE );
}

BOOL WdeBaseGetFirstChild ( WdeBaseObject *obj, OBJPTR *first, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( ( obj->num_children == 0 ) || ( obj->children == NULL ) ) {
        *first = NULL;
    } else {
        *first = ListElement ( obj->children );
    }

    return ( TRUE );
}

BOOL WdeBaseDraw( WdeBaseObject *obj, RECT *area, HDC *dc )
{
    POINT   origin;
    RECT    client_rect;
    RECT    intersect;
    LIST   *olist;
    OBJPTR  child;
    OBJPTR  tops;
    RECT    child_rect;
    Bool    show_child;

    /* touch unused vars to get rid of warning */
    _wde_touch(dc);

    GetOffset( &origin );

    GetClientRect( obj->res_info->edit_win, &client_rect );

    OffsetRect( &client_rect, origin.x, origin.y );

    child = NULL;

    /* we must process the children in reverse order */
    if( obj->children ) {
        WdeListLastElt( obj->children, &olist );
    } else {
        olist = NULL;
    }

    tops = NULL;
    for( olist = obj->children; olist; olist = ListNext( olist ) ) {
        child = ListElement( olist );
        Location( child, &child_rect );
        show_child = IntersectRect( &intersect, &child_rect, &client_rect );
        Forward( child, SHOW_WIN, &show_child, NULL );
        if( !show_child ) {
            child = NULL;
        } else {
            if( !Forward( child, DRAW, area, NULL ) ) {
                WdeWriteTrail("WdeBaseDraw: child draw failed!");
            }
            if( !tops ) {
                tops = child;
            }
        }
    }

    if( IntersectRect( &intersect, area, &client_rect ) ) {
        OffsetRect( &intersect, -origin.x, -origin.y );
        RedrawWindow( obj->res_info->edit_win, &intersect, (HRGN) NULL,
                      RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
        if( child && !Forward ( tops, ON_TOP, NULL, NULL) ) {
            WdeWriteTrail("WdeBaseDraw: child ON_TOP failed!");
            return( FALSE );
        }
    }

    return( TRUE );
}

BOOL WdeBaseGetResizeInfo ( WdeBaseObject *obj, RESIZE_ID *info, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *info = R_NONE;

    return ( TRUE );
}

BOOL WdeBaseIdentify ( WdeBaseObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    *id = obj->object_id;

    return ( TRUE );
}

BOOL WdeBaseValidateAction ( WdeBaseObject *obj, ACTION *act, void *p2 )
{
    int     i;

    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( *act == MOVE ) {
        return ( FALSE );
    }

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeBaseActions[i].id == *act ) {
            return ( TRUE );
        }
    }

    return ( ValidateAction( (OBJPTR) obj->o_item, *act, p2 ) );
}

BOOL WdeBaseGetWindowHandle ( WdeBaseObject *obj, HWND *hwin, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    *hwin = obj->res_info->edit_win;

    return ( TRUE );
}

BOOL WdeBaseAddSubObject ( WdeBaseObject *base_obj, OBJPTR obj, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    /* make sure objects are inserted at the end of list */
    WdeInsertObject ( &(base_obj->children), obj );
    WdeAddOrderedEntry ( &(base_obj->ochildren), obj );
    base_obj->num_children++;
    Notify ( obj, NEW_PARENT , base_obj->object_handle );
    WdeCheckBaseScrollbars( FALSE );

    return ( TRUE );
}

Bool WdeBaseFindSubObjects ( WdeBaseObject *obj, SUBOBJ_REQUEST *req,
                             LIST **obj_list )
{
    return ( WdeFindSubObjects ( req, obj_list, obj->children ) );
}

BOOL WdeBaseFindObjectsPt ( WdeBaseObject *obj, POINT *pt, LIST **obj_list )
{
    LIST   *subobjs;
    OBJPTR  child;
    OBJ_ID  id;

    if ( WdeFindObjectsAtPt ( pt, &subobjs, obj->children ) ) {
        child = ListElement ( subobjs );
        if ( !( Forward ( child, IDENTIFY, &id, NULL ) &&
                ( id == DIALOG_OBJ ) ) ||
             !Forward ( child, FIND_OBJECTS_PT, pt, obj_list ) ) {
            ListAddElt ( obj_list, child );
        }
        ListFree ( subobjs );
    } else {
        ListAddElt ( obj_list, obj );
    }

    return ( TRUE );
}

BOOL WdeBaseRemoveSubObject ( WdeBaseObject *base_obj, OBJPTR obj, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( base_obj->num_children && ListFindElt (base_obj->children, obj) ) {
        ListRemoveElt ( &(base_obj->children), obj);
        WdeRemoveOrderedEntry ( base_obj->ochildren, obj);
        base_obj->num_children--;
        if( !base_obj->num_children ) {
            base_obj->children = NULL;
            base_obj->res_info->next_current = NULL;
        }
    } else {
        return ( FALSE );
    }

    return ( TRUE );
}

BOOL WdeBasePutChildFirst ( WdeBaseObject *obj, OBJPTR child, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( obj->num_children ) {
        if ( !WdePutObjFirst ( child, &(obj->children) ) ) {
            WdeWriteTrail("WdeBasePutChildFirst: WdePutObjFirst failed!");
            return ( FALSE );
        }
    } else {
        WdeWriteTrail("WdeBasePutChildFirst: No children!");
        return ( FALSE );
    }

    return ( TRUE );
}

BOOL WdeBaseNotify ( WdeBaseObject *base_obj, NOTE_ID *id, void *p2 )
{
    OBJPTR           obj;
    WdeInfoStruct    is;

    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    switch ( *id ) {
        case PRIMARY_OBJECT:
            obj = base_obj->res_info->next_current;
            if ( obj && ( obj != base_obj ) ) {
                MakeObjectCurrent( obj );
            } else if ( base_obj->num_children ) {
                obj = ListElement ( base_obj->children );
                MakeObjectCurrent( obj );
            } else {
                WdeSetBaseObject ( IDM_SELECT_MODE );
                WdeSetBaseObjectMenu(base_obj->res_info->hash_table != NULL);

                memset ( &is, 0, sizeof(WdeInfoStruct) );

                is.obj_id      = BASE_OBJ;
                is.obj         = base_obj->object_handle;
                is.res_info    = base_obj->res_info;

                WdeWriteInfo ( &is );
            }
            return ( TRUE );
    }

    return ( Notify ( (OBJPTR) base_obj->o_item, *id, p2 ) );
}

BOOL WdeBaseLocation( WdeBaseObject *base_obj, RECT *obj_rect, void *p2 )
{
    RECT onion;
    RECT rect1;
    RECT rect2;

    /* touch unused vars to get rid of warning */
    _wde_touch(base_obj);
    _wde_touch(p2);

    if( base_obj->res_info->edit_win != NULL ) {
        GetClientRect( base_obj->res_info->edit_win, &rect1 );
        if( IsRectEmpty( &rect1 ) ) {
            rect1.right++;
            rect1.bottom++;
        }
    } else {
        SetRectEmpty( &rect1 );
    }

    WdeGetBoundingRectFromList( base_obj->children, &rect2 );
    InflateRect( &rect2, WDE_WORLD_PAD, WDE_WORLD_PAD );

    UnionRect( &onion, &rect1, &rect2 );

    *obj_rect = onion;

    return( TRUE );
}

BOOL WdeBaseGetSubObjectList ( WdeBaseObject *base_obj, LIST **l, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    *l = base_obj->children;

    return( TRUE );
}

static void isWorldInBaseClient( WdeBaseObject *base_obj,
                                 BOOL *x_in, BOOL *y_in )
{
    RECT        client_rect;
    RECT        bounding_rect;

    *x_in = FALSE;
    *y_in = FALSE;

    if( base_obj->res_info->edit_win == NULL ) {
        return;
    }

    if( base_obj->children == NULL ) {
        return;
    }

    GetClientRect( base_obj->res_info->forms_win, &client_rect );

    WdeGetBoundingRectFromList( base_obj->children, &bounding_rect );
    InflateRect( &bounding_rect, WDE_WORLD_PAD, WDE_WORLD_PAD );

    if( ( bounding_rect.left < client_rect.left ) ||
        ( bounding_rect.right > client_rect.right ) ) {
        *x_in = TRUE;
    }

    if( ( bounding_rect.top < client_rect.top ) ||
        ( bounding_rect.bottom > client_rect.bottom ) ) {
        *y_in = TRUE;
    }

    return;
}

static BOOL enableFormsScrollbars( WdeBaseObject *base_obj,
                                   BOOL enablex, BOOL enabley )
{
    STATE_HDL   state_handle;
    SCR_CONFIG  scroll_config;

    state_handle = GetCurrFormID();
    if( state_handle == (STATE_HDL)NULL ) {
        return( FALSE );
    }

    scroll_config = SCROLL_NONE;

    if( enablex ) {
        scroll_config |= SCROLL_HORZ;
    }

    if( enabley ) {
        scroll_config |= SCROLL_VERT;
    }

    SetFormEditWnd( state_handle, base_obj->res_info->forms_win,
                    MENU_NONE, scroll_config );

    return( TRUE );
}

BOOL WdeCheckBaseScrollbars( BOOL in_resize )
{
    WdeBaseObject       *base_obj;
    BOOL                enablex;
    BOOL                enabley;
    RECT                rect;

    if( WdeInCleanup() ) {
        return( TRUE );
    }

    base_obj = (WdeBaseObject *)GetMainObject();

    if( !base_obj || !base_obj->res_info ) {
        return( FALSE );
    }

    if( base_obj->in_destroy ) {
        return( TRUE );
    }

    enablex = FALSE;
    enabley = FALSE;

    isWorldInBaseClient( base_obj, &enablex, &enabley );

    if( ( enablex == base_obj->has_hscroll ) &&
        ( enabley == base_obj->has_vscroll ) ) {
        return( TRUE );
    }

    if( enablex != base_obj->has_hscroll ) {
        if( !enablex ) {
            SetScrollPos( base_obj->res_info->forms_win, SB_HORZ, 0, FALSE );
        }
        SetScrollRange( base_obj->res_info->forms_win, SB_HORZ, 0, (enablex) ?100 :0, FALSE );
        ShowScrollBar( base_obj->res_info->forms_win, SB_HORZ, enablex );
    }

    if( enabley != base_obj->has_vscroll ) {
        if( !enabley ) {
            SetScrollPos( base_obj->res_info->forms_win, SB_VERT, 0, FALSE );
        }
        SetScrollRange( base_obj->res_info->forms_win, SB_VERT, 0, (enabley) ?100 :0, FALSE );
        ShowScrollBar( base_obj->res_info->forms_win, SB_VERT, enabley );
    }

    enableFormsScrollbars( base_obj, enablex, enabley );

    if( !in_resize ) {
        GetClientRect( base_obj->res_info->forms_win, &rect );
        MoveWindow( base_obj->res_info->edit_win, 0, 0,
                    (rect.right  - rect.left),
                    (rect.bottom - rect.top), TRUE );
    }

    UpdateScroll();

    base_obj->has_hscroll = enablex;
    base_obj->has_vscroll = enabley;

    return( TRUE );
}

