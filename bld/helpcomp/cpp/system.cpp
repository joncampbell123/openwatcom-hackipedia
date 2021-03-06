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


/*
SYSTEM:  handle "global" WinHelp file information
*/

#include <time.h>
#include "system.h"
#include "hcerrors.h"


//  SystemText::SystemText

SystemText::SystemText( uint_16 flg, char *txt )
{
    _flag = flg;
    int length = strlen( txt ) + 1;

    // Impose a maximum size restriction: the size has to
    // be expressible as a uint_16.
    if( length > 0xFFFF ){
    _size = 0xFFFF;
    } else {
    _size = (uint_16) length;
    }

    _text = new char[ _size ];
    _text[_size-1] = '\0';
    strncpy( _text, txt, _size-1 );
}


//  SystemText::~SystemText

SystemText::~SystemText()
{
    delete[] _text;
}


//  SystemText::dump    --Overrides Dumpable::dump.

int SystemText::dump( OutFile * dest )
{
    dest->writebuf( &_flag, sizeof( uint_16 ), 1 );
    dest->writebuf( &_size, sizeof( uint_16 ), 1 );
    dest->writebuf( _text, 1, _size );
    return 1;
}


//  SystemNum::SystemNum

SystemNum::SystemNum( uint_16 flg, uint_32 val )
{
    _flag = flg;
    _num = val;
    _size = (uint_16) 4;
}


//  SystemNum::dump --Overrides Dumpable::dump.

int SystemNum::dump( OutFile * dest )
{
    dest->writebuf( &_flag, sizeof( uint_16 ), 1 );
    dest->writebuf( &_size, sizeof( uint_16 ), 1 );
    dest->writebuf( &_num, sizeof( uint_32 ), 1 );
    return 1;
}


//  SystemWin::SystemWin

SystemWin::SystemWin( uint_16 wflgs,
                      char const type[],
              char const name[],
              char const cap[],
              uint_16 x, uint_16 y,
              uint_16 w, uint_16 h,
              uint_16 use_max,
              uint_32 main_col,
              uint_32 back_col )
    : _winFlags(wflgs), _maximize(use_max),
      _rgbMain(main_col), _rgbNonScroll(back_col)
{
    _flag = HFSystem::SYS_WINDOW;
    _size = 90; // HLP_SYS_TYPE + HLP_SYS_NAME + HLP_SYS_CAP + 20
    _position[0] = x;
    _position[1] = y;
    _position[2] = w;
    _position[3] = h;
    strncpy( _type, type, HLP_SYS_TYPE );
    strncpy( _name, name, HLP_SYS_NAME );
    strncpy( _caption, cap, HLP_SYS_CAP  );
}


//  SystemWin::dump --Overrides Dumpable::dump.

int SystemWin::dump( OutFile * dest )
{
    dest->writebuf( &_flag, sizeof( uint_16 ), 1 );
    dest->writebuf( &_size, sizeof( uint_16 ), 1 );
    dest->writebuf( &_winFlags, sizeof( uint_16 ), 1 );
    dest->writebuf( _type, 1, HLP_SYS_TYPE );
    dest->writebuf( _name, 1, HLP_SYS_NAME );
    dest->writebuf( _caption, 1, HLP_SYS_CAP );
    dest->writebuf( _position, sizeof( uint_16 ), 4 );
    dest->writebuf( &_maximize, sizeof( uint_16 ), 1 );
    dest->writebuf( &_rgbMain, sizeof( uint_32 ), 1 );
    dest->writebuf( &_rgbNonScroll, sizeof( uint_32 ), 1 );
    return 1;
}


//  HFSystem::HFSystem

HFSystem::HFSystem( HFSDirectory *d_file, HFContext *h_file )
    : _hashFile( h_file )
{
    _compLevel = (uint_16) 0;
    _contentNum = 0;
    _size = 25; // 12 byte header + default SYS_COPYRIGHT,SYS_CONTENTS


    // Create default copyright and contents records.
    _first = new SystemText( SYS_COPYRIGHT, "" );
    _first->_next = new SystemNum( SYS_CONTENTS, 0 );
    _last = _first->_next;
    _last->_next = NULL;

    d_file->addFile( this, "|SYSTEM" );
}


//  HFSystem::~HFSystem

HFSystem::~HFSystem()
{
    SystemRec   *current = _first;
    SystemRec   *temp;

    // Delete the linked list.
    while( current != NULL ){
    temp = current;
    current = current->_next;
    delete temp;
    }
}


//  HFSystem::setCompress   --Set the compression level for the file.

#define NO_COMPRESSION  0x0000
#define LZW_COMPRESSED  0x0004

void HFSystem::setCompress( int val )
{
    if( val ){
    _compLevel = LZW_COMPRESSED;
    } else {
    _compLevel = NO_COMPRESSION;
    }
    return;
}


//  HFSystem::isCompressed  --Get the compression level.

int HFSystem::isCompressed()
{
    return _compLevel != NO_COMPRESSION;
}


//  HFSystem::setContents   --Set the "table of contents" topic.

void HFSystem::setContents( uint_32 hval )
{
    _contentNum = hval;
}


//  HFSystem::addRecord     --Add a system record.

void HFSystem::addRecord( SystemRec *nextrec )
{
    _size += nextrec->size();

    // For bookkeeping, SYS_COPYRIGHT and SYS_CONTENTS records
    // are kept at the head of the list.

    if( nextrec->flag() == SYS_COPYRIGHT ){
    nextrec->_next = _first->_next;
    _size -= _first->size();
    delete _first;
    _first = nextrec;
    } else if( nextrec->flag() == SYS_CONTENTS ){
    SystemRec   *temp = _first->_next;
    nextrec->_next = temp->_next;
    _size -= temp->size();
    delete temp;
    _first->_next = nextrec;
    } else {
    _last->_next = nextrec;
    nextrec->_next = NULL;
    _last = nextrec;
    }
}


//  HFSystem::winNumberOf   --Get the number of a window type.

const int HFSystem::NoSuchWin = 0x100;

int HFSystem::winNumberOf( char * win_name )
{
    SystemRec   *current;
    SystemWin   *cur_win;
    int     i;

    i = 0;
    current = _first;
    while( current != NULL && i<255 ){
    if( current->_flag == SYS_WINDOW ){
        cur_win = (SystemWin*) current;
        if( strcmp( cur_win->_name, win_name ) == 0 ){
        return i;
        } else {
        i++;
        }
    }
    current = current->_next;
    }
    return NoSuchWin;
}


//  HFSystem::dump  --Overrides Dumpable::dump.

int HFSystem::dump( OutFile *dest )
{
    // Get the address of the contents topic.
    if( _contentNum != 0 ){
    _contentNum = _hashFile->getOffset( _contentNum );
    if( _contentNum == HFContext::_badValue ){
        HCWarning( SYS_NOCONTENTS );
    } else {
        addRecord( new SystemNum( SYS_CONTENTS, _contentNum ) );
    }
    }

    // Write the |SYSTEM header to output.
    static const uint_16    magic[3] = { 0x036C, 0x0015, 0x0001 };
    dest->writebuf( magic, sizeof( uint_16 ), 3 );

    // Write the "time of creation" for the help file.
    uint_32 cur_time = time( NULL );
    dest->writebuf( &cur_time, sizeof( uint_32 ), 1 );

    // Write out the compression level.
    dest->writebuf( &_compLevel, sizeof( uint_16 ), 1 );

    // Dump the records in the linked list, in order.
    SystemRec   *current = _first;
    while( current != NULL ){
    current->dump( dest );
    current = current->_next;
    }
    return 1;
}
