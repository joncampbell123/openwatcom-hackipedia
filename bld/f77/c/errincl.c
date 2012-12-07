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
* Description:  for error messages included in executable file
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"

char __FAR *            __FAR *PGroupTable = GroupTable;
char                    __FAR *PErrWord = ErrWord;

#define CARET_SHIFT  6
#define LENGTH_MASK  (1 << CARET_SHIFT) - 1


static  char    __FAR *GetMsg( unsigned int err ) {
// Get pointer to message.
    unsigned int    num;
    char            __FAR *msg;

    msg = PGroupTable[ err / 256 ];
    num = err % 256;
    while( num != 0 ) {
        msg += ( *msg & LENGTH_MASK ) + 1;
        --num;
    }
    return( msg );
}


static  char    __FAR *GetWord( unsigned int index ) {
// Get pointer to word.
    char        __FAR *word;

    word = PErrWord;
    while( index != 0 ) {
        word += *word + 1;
        --index;
    }
    return( word );
}


static  void    BldErrMsg( unsigned int err, char *buffer, va_list args ) {
// Build error message.
    char                __FAR *char_ptr;
    char                __FAR  *phrase_ptr;
    char                *buff_start;
    unsigned int        word_count;
    unsigned int        index;
    char                chr;
    int                 len;

    phrase_ptr = GetMsg( err );
    word_count = *phrase_ptr & LENGTH_MASK;
    phrase_ptr++;
    buff_start = buffer;
    *buffer = ' ';
    buffer++;
    for(;;) {
        index = 0;
        do {
            len = *phrase_ptr;
            index += len;
            ++phrase_ptr;
            --word_count;
        } while( len == 255 );
        char_ptr = GetWord( index );
        len = *char_ptr;
        char_ptr++;
        for(;;) {
            chr = *char_ptr;
            char_ptr++;
            if( --len < 0 ) break;
            *buffer = chr;
            buffer++;
        }
        if( word_count == 0 ) break;
#if _CSET != _KANJI
        *buffer = ' ';
        buffer++;
#endif
    }
    *buffer = '\0';
    Substitute( buff_start, buff_start, args );
}


static  void    ErrorInit( char *unused ) {
    unused = unused;
}


static  void    ErrorFini( void ) {
}


void    __InitError( void ) {
    __ErrorInit = &ErrorInit;
    __ErrorFini = &ErrorFini;
    __BldErrMsg = &BldErrMsg;
}
