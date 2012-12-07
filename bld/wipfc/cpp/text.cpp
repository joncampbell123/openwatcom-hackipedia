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
* Description:  An abstract text element (WHITESPACE, WORD, ENTITY,
*               or PUNCTUATION).
*
****************************************************************************/


#include <cwctype>
#include "text.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "gdword.hpp"
#include "page.hpp"

Text::Text( Document* d, Element* p, const std::wstring* f, unsigned int r,
            unsigned int c, const std::wstring& txt, bool ts ) :
            Element( d, p, f, r, c ), whiteSpace( Tag::NONE), toggleSpacing( ts )
{
    GlobalDictionaryWord* word( new GlobalDictionaryWord( txt ) );
    text = document->addWord( word );   //insert into global dictionary
}
/***************************************************************************/
std::pair< bool, bool > Text::buildLocalDict( Page* page )
{
    std::pair< bool, bool > retval( false, false );
    if( text ) {
        retval.first = page->addWord( text );
        retval.second = toggleSpacing;
    }
    return retval;
}
/***************************************************************************/
void Text::buildText( Cell* cell )
{
    if( text ) {
        if( toggleSpacing )
            cell->addByte( 0xFC );
        cell->addText( text->index() );
        if( cell->textFull() )
            printError( ERR1_LARGEPAGE );
    }
}
