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
* Description:  Process nt/ent tags
*
*   :nt / :ent
*       text='' (in place of data from nls file)
*   New paragraph, contents follows 'Note:  ' on line
*   Text left aligns on column after 'text'
*
****************************************************************************/

#include "nt.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "ipfbuff.hpp"
#include "p.hpp"
#include "page.hpp"
#include "util.hpp"
#include "whtspc.hpp"

Lexer::Token Nt::parse( Lexer* lexer )
{
    std::wstring temp;
    std::wstring* fname( new std::wstring() );
    prepBufferName( fname, *( document->dataName() ) );
    fname = document->addFileName( fname );
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"text" ) {
                temp = L":hp2.";
                temp += value;
                temp += L":ehp2.";
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    if( temp.empty() ) {
        temp = L":hp2.";
        temp += document->note();
        temp += L":ehp2.";
    }
    document->pushInput( new IpfBuffer( fname, document->dataLine(),
        document->dataCol(), temp ) );
    bool oldBlockParsing( document->blockParsing() );
    document->setBlockParsing( true );
    whiteSpace = Tag::LITERAL;
    appendChild( new P( document, this, document->dataName(), document->dataLine(),
        document->dataCol() ) );
    tok = document->getNextToken(); //first token from buffer
    while( tok != Lexer::END ) {
        if( parseInline( lexer, tok ) )
            parseCleanup( lexer, tok );
    }
    whiteSpace = Tag::NONE;
    document->setBlockParsing( oldBlockParsing );
    document->popInput();
    appendChild( new WhiteSpace( document, this, document->dataName(), document->dataLine(),
        document->dataCol(), L"  ", Tag::LITERAL, false ) );
    appendChild( new NtLm( document, this, document->dataName(), document->dataLine(),
        document->dataCol() ) );
    tok = document->getNextToken(); //next token from main stream
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::ENT )
                    break;
            else if( lexer->tagId() == Lexer::H1 ||
                lexer->tagId() == Lexer::H2 ||
                lexer->tagId() == Lexer::H3 ||
                lexer->tagId() == Lexer::H4 ||
                lexer->tagId() == Lexer::H5 ||
                lexer->tagId() == Lexer::H6 ||
                lexer->tagId() == Lexer::ACVIEWPORT ||
                lexer->tagId() == Lexer::CAUTION ||
                lexer->tagId() == Lexer::ECAUTION ||
                lexer->tagId() == Lexer::FN ||
                lexer->tagId() == Lexer::NOTE ||
                lexer->tagId() == Lexer::NT ||
                lexer->tagId() == Lexer::WARNING ||
                lexer->tagId() == Lexer::EWARNING )
                    parseCleanup( lexer, tok );
            else if( parseBlock( lexer, tok ) ) {
                if( parseListBlock( lexer, tok ) )
                    parseCleanup( lexer, tok );
            }
        }
    }
    return tok;
}
/***************************************************************************/
void ENt::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( 0 );     //reset
    cell->addByte( 0xFA );  //line break
}
/***************************************************************************/
void NtLm::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x1C );  //set left margin to current position (reset at end of paragraph)
}
/*****************************************************************************/
void Nt::prepBufferName( std::wstring* buffer, const std::wstring& fname )
{
    buffer->assign( L"Nt text from " );
    buffer->append( fname );
}

