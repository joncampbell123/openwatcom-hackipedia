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


#include "variety.h"
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include "defwin.h"
#include "qread.h"

extern  void    _dos_kb_input( void _WCNEAR *s );
extern  void    _bd_dos_kb_input( unsigned short seg, unsigned off );

#if defined(__386__)
#pragma aux     _dos_kb_input =     \
                                    0xb4 0x0a   /* select keyboard input */\
                                    0xcd 0x21   /* int 21h */\
                        parm caller [edx];

#pragma aux     _bd_dos_kb_input =  0x1e        /* push ds */\
                                    0x8e 0xd8   /* mov ds,ax */\
                                    0xb4 0x0a   /* select keyboard input */\
                                    0xcd 0x21   /* int 21h */\
                                    0x1f        /* pop ds */\
                        parm caller [ax] [edx];
#elif defined( _M_I86 )
#pragma aux     _dos_kb_input =     \
                                    0xb4 0x0a   /* select keyboard input */\
                                    0xcd 0x21   /* int 21h */\
                        parm caller [dx];

#pragma aux     _bd_dos_kb_input =  0x1e        /* push ds */\
                                    0x8e 0xd8   /* mov ds,ax */\
                                    0xb4 0x0a   /* select keyboard input */\
                                    0xcd 0x21   /* int 21h */\
                                    0x1f        /* pop ds */\
                        parm caller [ax] [dx];
#else
#error platform not supported
#endif

_WCRTLINK char *cgets( char *s )
{
    int len;
    char *p;

#ifdef DEFAULT_WINDOWING
    if( _WindowsStdin != 0 ) {  // Default windowing
        __qread( STDIN_FILENO, s + 2, *s - 1 );
        p = s + 2;
        len = *s;
        for(;;) {
            if( len <= 1 ) break;
            if( *p == '\r' || *p == '\0' ) break;
            ++p;
            --len;
        }
        *p = '\0';
        s[1] = p - s - 2;
        return( s + 2 );
    }
#endif
#if defined(__SMALL_DATA__)
    _dos_kb_input( s );
#else
    _bd_dos_kb_input( FP_SEG(s), FP_OFF(s) );
#endif
    len = *s;
    p = s + 2;
    for(;;) {
        if( len <= 1 ) break;
        if( *p == '\r' ) break;
        ++p;
        --len;
    }
    *p = '\0';
    return( s + 2 );
}
