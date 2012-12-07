/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file contains the functional tests for std::list.
*
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <list>
#include <cstdlib>
#include "sanity.cpp"
#include "allocxtr.hpp"


/* ------------------------------------------------------------------
 * construct_test()
 * test different constructors
 */
bool construct_test( )
{
    using namespace std;
    // test ctor( size_t, value )
    list< char > lst1( 99, 'd' );
    int i;
    
    if( INSANE(lst1) || lst1.size() != 99 || lst1.empty() ) FAIL
    for( i = 0; i < 99; i++ ){
        if( lst1.back() != 'd' ) FAIL
        lst1.pop_back();
    }
    if( INSANE(lst1) || lst1.size() || !lst1.empty() ) FAIL
    
    // test type dependent ctor
    // first when it template param is iterator
    int x[] = { 1,2,3,4,5,6 };
    list< int > lst2( x, x+6, allocator< int >() );
    if( INSANE(lst2) || lst2.size() != 6 || lst2.empty() ) FAIL
    for( i = 0; i < 6; i++ ){
        if( lst2.front() != i+1 ) FAIL
        lst2.pop_front();
    }
    if( INSANE(lst2) || lst2.size() || !lst2.empty() ) FAIL
    
    // now when template param is integer
    // hmmm, need to think of example when this version of template constructor
    // will get called, currently just converts params and calls explicit ctor
    // is this right? do we not need int version of templated ctor?
    list< int > lst3( 77L, 88L, allocator< int >() );
    if( INSANE(lst3) || lst3.size() != 77 || lst3.empty() ) FAIL
    for( i = 0; i < 77; i++ ){
        if( lst3.front() != 88 ) FAIL
        lst3.pop_front();
    }
    if( INSANE(lst3) || lst1.size() || !lst3.empty() ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * access_test
 * test insert, push_front, push_back, erase, pop_front, pop_back 
 */
bool access_test( )
{
    typedef std::list< int > list_t;
    list_t lst;
    list_t::iterator it;
    int i;
    //test insert
    for( i = 0; i < 10 ; i++ ){
        lst.insert( lst.begin(), i );
    }//now contains 9...0
    for( it = lst.begin(), i = 9; i >= 0; --i, ++it ){
        //std::cout<<*it<<",";
        if( INSANE(lst) || *it != i ) FAIL
    }
    //test push_front
    for( i = 10; i < 20 ; i++ ){
        lst.push_front( i );
    }//now contains 19...0
    for( it = lst.begin(), i = 19; i >= 0; --i, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    //test push_back
    for( i = -1; i > -11; i-- ){
        lst.push_back( i );
    }//now contains 19...-10
    for( it = lst.begin(), i = 19; i >= -10; --i, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    //test erase
    it = lst.begin();
    for( i = 0; i < 30; i+=2 ){
        lst.erase( it++ );
        ++it;
    }//now contains evens 18...-10
    for( it = lst.begin(), i = 18; i >= -10; i-=2, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    //test pop_front
    for( i = 18; i > 8; i-=2 ){
        if( lst.front() != i ) FAIL
        lst.pop_front( );
    }//now contains evens 8...-10
    for( it = lst.begin(), i = 8; i >= -10; i-=2, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    //test pop_back
    for( i = -10; i < 4; i+=2 ){
        if( lst.back() != i ) FAIL
        lst.pop_back( );
    }//now contains 8,6,4
    for( it = lst.begin(), i = 8; i >= 4; i-=2, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    lst.front() = 5;
    lst.back() = 7;
    for( it = lst.begin(), i = 5; i <= 7; ++i, ++it ){
        if( INSANE(lst) || *it != i ) FAIL
    }
    // insert( it, size_type, value )
    lst.clear();
    lst.insert( lst.begin(), (list_t::size_type)4, (list_t::value_type)9 );
    for( it = lst.begin(), i = 0; it != lst.end(); ++it ){
        if( INSANE(lst) || *it != 9 ) FAIL
        i++;
    }
    if( INSANE(lst) || i != 4 ) FAIL
    // template insert, int params
    lst.insert( lst.begin(), 5, 8 ); // now contains 4 9s and 5 8s
    for( it = lst.begin(), i = 0; it != lst.end(); ++it ){
        i += *it;
    }
    if( INSANE(lst) || i != 4*9+5*8 ) FAIL
    
    // template insert, iterator params
    int v[] = { 0,1,4,9,16 };
    lst.clear();
    lst.insert( lst.begin(), v, v+sizeof(v)/sizeof(v[0]) );
    if( INSANE(lst) || lst.size() != 5 ) FAIL
    for( it = lst.begin(), i = 0; it != lst.end(); ++it, ++i ){
        if( INSANE(lst) || *it != i*i ) FAIL
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * assign_test
 * test list assignment methods
 */
bool assign_test( )
{
    std::list< int > lst1, lst2;
    
    // check operator=
    for( int i =  1; i <= 10; ++i ) lst1.push_back( i );
    for( int i = 11; i <= 15; ++i ) lst2.push_back( i );
    lst1 = lst2;
    if( INSANE( lst1 ) || INSANE( lst2 ) ) FAIL
    if( lst1.size( ) != 5 || lst2.size( ) != 5 ) FAIL
    int i = 11;
    std::list< int >::iterator it = lst1.begin( );
    while( it != lst1.end( ) ) {
        if( *it != i ) FAIL
        ++i; ++it;
    }

    // check assign method (cast types so matches fn, not template fn)
    lst1.assign( (std::list<int>::size_type)10, -1 );
    if( INSANE( lst1 ) || lst1.size( ) != 10 ) FAIL
    for( it = lst1.begin( ); it != lst1.end( ); ++it ) {
        if( *it != -1 ) FAIL
    }
    
    // template assign, non integer types
    int x[] = { 50,51,52,53,54 };
    lst2.assign( x, x+5 );
    if( INSANE(lst2) || lst2.size() != 5 ) FAIL
    for( i = 0; i < 5; i++){
        if( lst2.front() != i+50 ) FAIL
        lst2.pop_front();
    }
    if( INSANE(lst2) || !lst2.empty() ) FAIL
    
    // template assign, integer types
    lst2.assign( 20, 50 );
    if( INSANE(lst2) || lst2.size() != 20 ) FAIL
    for( i = 0; i < 20; i++){
        if( lst2.front() != 50 ) FAIL
        lst2.pop_front();
    }
    if( INSANE(lst2) || !lst2.empty() ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * clear_test
 * test removal of elements, clear and destructor
 */
bool clear_test()
{
    std::list< int > lst, lst2, lst3;
    typedef std::list< int > list_t;
    list_t* lstp = new list_t;
    
    for( int i = 0; i < 10; i++ ){
        lst.push_front( i );
        lst2.push_front( i );
        lst3.push_front( i );
        lstp->push_front( i );
        if( INSANE( lst ) || lst.front() != i || lst.size() != i+1 ) FAIL
        if( INSANE( lst2 ) || lst.front() != i || lst.size() != i+1 ) FAIL
        if( INSANE( lst3 ) || lst.front() != i || lst.size() != i+1 ) FAIL
        if( INSANE( *lstp ) || lstp->front() != i || lst.size() != i+1 ) FAIL
    }
    //test clear
    lst.clear();
    if( INSANE( lst ) || lst.size() || !lst.empty() ) FAIL
    lst.clear();
    if( INSANE( lst ) || lst.size() || !lst.empty() ) FAIL
    //test removal
    for( int i = 0; i < 10; i++ ) lst2.pop_front();
    if( INSANE( lst2 ) || lst2.size() || !lst2.empty() ) FAIL
    //test destructor call
    delete lstp;
    //lst3 is cleared automatically 
    //(leak detect will fire if destructor is wrong)
    return( true );
}

/* ------------------------------------------------------------------
 * erase_test
 * test two argument form of erase
 */
bool erase_test( )
{
    using namespace std;  //try this for something different.
    //build a list.
    list< int > lst;
    for( int i = 1; i <= 20; ++i ) lst.push_back( i );
    list< int >::iterator it = find( lst.begin( ), lst.end( ), 11 );
    //do the deed
    lst.erase( lst.begin( ), it );
    if( INSANE( lst ) || lst.size( ) != 10 ) FAIL
    it = lst.begin( );
    for( int i = 1; i <= 10; ++i ) {
        if( *it != i + 10 ) FAIL
        ++it;
    }
    //erase everything
    lst.erase( lst.begin( ), lst.end( ) );
    if( INSANE( lst ) || lst.size( ) != 0 ) FAIL
    return( true );
}

/* ------------------------------------------------------------------
 * swap_test
 * test list swapping method
 */
bool swap_test( )
{
    std::list< int > lst_1, lst_2;
    lst_1.push_back( 1 );
    //try a swap and then check the result
    lst_1.swap( lst_2 );
    if( INSANE( lst_1 )    || INSANE( lst_2 ) ||
        lst_1.size( ) != 0 || lst_2.size( ) != 1) FAIL
    if( lst_2.front( ) != 1 ) FAIL
    //add some things to lst_1 and swap again
    lst_1.push_back( 10 );
    lst_1.push_back( 11 );
    lst_1.swap( lst_2 );
    if( INSANE( lst_1 )    || INSANE( lst_2 ) ||
        lst_1.size( ) != 1 || lst_2.size( ) != 2) FAIL
    if( lst_1.front( ) != 1 ) FAIL
    if( lst_2.front( ) != 10 ) FAIL
    lst_2.pop_front( );
    if( lst_2.front( ) != 11 ) FAIL
    return( true );
}

/* ------------------------------------------------------------------
 * remove_test( )
 * test the remove methods
 */
bool remove_test( )
{
    typedef std::list< int > l_t;
    l_t l;
    //prepare the list
    l.push_back( 0 );
    for( int i = 1; i <= 10; i++ ) {
        l.push_back( i );
    }
    l.push_back( 0 );
    l.push_back( 0 );
    for( int i = 11; i <= 20; i++ ) {
        l.push_back( i );
    }
    l.push_back( 0 );
    //do the deed
    l.remove( 0 );
    //did it work?
    if( INSANE( l ) || l.size( ) != 20 ) FAIL
    l_t::iterator it( l.begin( ) );
    for( int i = 1; i <= 20; i++ ) {
        if( *it != i ) FAIL
        ++it;
    }
    return( true );
}

/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    typedef std::list< int > l_t;
    l_t l;
    l_t::iterator it;
    l_t::const_iterator cit;
    for( int i = 0; i < 20; i++ ){
        l.push_back( i );
    }
    //test increment and dereferencing
    it = l.begin();
    int ans = *it;
    for( int i = 0; i < 20 ; i++ ){
        if( INSANE( l ) || ans != i || *it != i ) FAIL
        if( i%2 ) ans = (*(it++)) + 1;
        else ans = *(++it);
    }
    //and again with const iterator
    cit = l.begin();
    ans = *cit;
    for( int i = 0; i < 20 ; i++ ){
        if( INSANE( l ) || ans != i || *cit != i ) FAIL
        if( i%2 ) ans = *(cit++) + 1;
        else ans = *(++cit);
    }
    //test decrement
    it = l.end();
    for( int i = 19; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = *(--it);
        else ans = *(it--) - 1;
        if( INSANE( l ) || ans != i || *it != i ) FAIL
    }
    //and again with const iterator
    cit = l.end();
    for( int i = 19; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = *(--cit);
        else ans = *(cit--) - 1;
        if( INSANE( l ) || ans != i || *cit != i ) FAIL
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * reverse_iterator_test
 * make sure reverse iterators do something reasonable
 */
bool reverse_iterator_test( )
{
    std::list< int > lst;
    for( int i = 1; i <= 10; ++i ) lst.push_back( i );

    std::list< int >::reverse_iterator rit = lst.rbegin( );
    for( int i = 10; i >= 1; --i ) {
      if( *rit != i ) FAIL
      ++rit;
    }
    if( rit != lst.rend( ) ) FAIL
    return( true );
}

/* ------------------------------------------------------------------
 * copy_test
 * test copy constructor
 */
bool copy_test()
{
    std::list<int> lst1;
    for( int i = 0; i < 20; i++ ){
        lst1.push_front( -i );
    }
    std::list<int> lst2(lst1);
    if( INSANE( lst1 ) || lst1.size() != 20 ) FAIL
    if( INSANE( lst2 ) || lst2.size() != 20 ) FAIL
    for( int i = 0; i < 20; i++ ){
        if( lst2.back() != -i ) FAIL
        lst2.pop_back();
    }
    return( true );
}

/* ------------------------------------------------------------------
 * splice_test
 * test list splicing operations
 */
bool splice_test( )
{
    std::list< int > lst1, lst2;
    //two trivial (empty) lists
    lst1.splice( lst1.begin( ), lst2 );
    if( INSANE( lst1 )    || INSANE( lst2 )    ||
        lst1.size( ) != 0 || lst2.size( ) != 0 ) FAIL
    //non-trival list spliced into empty list
    for( int i = 1; i <= 10; ++i ) lst2.push_back( i );
    lst1.splice( lst1.begin( ), lst2 );
    if( INSANE( lst1 )     || INSANE( lst2 )    ||
        lst1.size( ) != 10 || lst2.size( ) != 0 ) FAIL
    //two non-trivial lists
    for( int i = 11; i <= 20; ++i ) lst2.push_back( i );
    std::list< int >::iterator it = lst1.begin( );
    ++it; ++it; ++it; ++it; ++it;
    lst1.splice( it, lst2 );
    if( INSANE( lst1 )     || INSANE( lst2 )    ||
        lst1.size( ) != 20 || lst2.size( ) != 0 ) FAIL
    it = lst1.begin( );
    //check final list contents
    for( int i = 1; i <= 5; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }
    for( int i = 11; i <= 20; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }
    for( int i = 6; i <= 10; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }

    std::list< int > lst3, lst4;
    for( int i = 1; i <= 5; ++i ) lst4.push_back( i );
    it = lst4.begin( ); ++it; ++it;
    lst3.splice( lst3.begin( ), lst4, it );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 1 || lst4.size( ) != 4 ) FAIL
    lst3.splice( lst3.end( ), lst4, lst4.begin( ) );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 2 || lst4.size( ) != 3 ) FAIL
    it = lst4.end( ); --it;
    lst3.splice( ++lst3.begin( ), lst4, it );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 3 || lst4.size( ) != 2 ) FAIL
    lst3.splice( lst3.begin( ), lst3, --lst3.end( ) );
    if( INSANE( lst3 ) || lst3.size( ) != 3 ) FAIL
    //check final list contents
    it = lst3.begin( );
    if( *it != 1 ) FAIL; ++it;
    if( *it != 3 ) FAIL; ++it;
    if( *it != 5 ) FAIL;

    std::list< int > lst5, lst6;
    for( int i = 1; i <= 10; ++i ) lst6.push_back( i );
    lst5.splice( lst5.begin( ), lst6, lst6.begin( ), lst6.end( ) );
    if( INSANE( lst5 )     || INSANE( lst6 )    ||
        lst5.size( ) != 10 || lst6.size( ) != 0 ) FAIL
    //check final list contents
    it = lst5.begin( );
    for( int i = 1; i <= 10; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }

    return( true );
}

/* ------------------------------------------------------------------
 * reverse_test
 * test list reverse method
 */
bool reverse_test( )
{
    std::list< int > lst;
    //try a zero sized list
    lst.reverse( );
    if( INSANE( lst ) || lst.size( ) != 0 ) FAIL
    //try a non-trivial list
    for( int i = 1; i <= 10; ++i ) lst.push_back( i );
    lst.reverse( );
    if( INSANE( lst ) || lst.size( ) != 10 ) FAIL
    //check final list contents
    std::list< int >::iterator it( lst.begin( ) );
    for( int i = 10; i >= 1; --i ) {
        if( *it != i ) FAIL
        ++it;
    }
    //let's try a reverse iteration too to check out the reverse links
    std::list< int >::reverse_iterator rit( lst.rbegin( ) );
    for( int i = 1; i <= 10; ++i ) {
        if( *rit != i ) FAIL
        ++rit;
    }
    return( true );
}

/* ------------------------------------------------------------------
 * merge_test
 * test list merging methods
 */

struct merge_data {
  int A[11];
  int B[11];
  int R[11];
};
struct merge_data merge_tests[] = {
  { { 0, 2, 4, -1 },
    { 1, 3, 5, -1 },
    { 0, 1, 2, 3, 4, 5, -1 } },

  { { 0, 2, 4, -1 },
    { -1 },
    { 0, 2, 4, -1 } },

  { { -1 },
    { 0, 2, 4, -1 },
    { 0, 2, 4, -1 } },

  { { 1, 3, 5, -1 },
    { 1, 2, -1 },
    { 1, 1, 2, 3, 5, -1 } },

  { { 1, 2, -1 },
    { 1, 3, 5, -1 },
    { 1, 1, 2, 3, 5, -1 } },

  { { 1, 2, -1 },
    { 3, 4, 5, -1 },
    { 1, 2, 3, 4, 5, -1 } },

  { { 3, 4, 5, -1 },
    { 1, 2, -1 },
    { 1, 2, 3, 4, 5, -1 } }
};
const int merge_test_count = sizeof(merge_tests)/sizeof(merge_data);

bool merge_test( )
{
  int *p;

  for( int test_no = 0; test_no < merge_test_count; ++test_no ) {
    std::list< int > lst_1, lst_2;

    //prepare the two lists and merge them.
    p = merge_tests[test_no].A;
    while( *p != -1 ) { lst_1.push_back( *p ); ++p; }
    p = merge_tests[test_no].B;
    while( *p != -1 ) { lst_2.push_back( *p ); ++p; }
    lst_1.merge( lst_2 );

    //did it work?
    if( INSANE( lst_1 ) || INSANE( lst_2 ) ) FAIL;
    p = merge_tests[test_no].R;
    while( *p != -1 ) {
      if( lst_1.front( ) != *p ) FAIL;
      lst_1.pop_front( );
      ++p;
    }
  }

  return( true );
}

/* ------------------------------------------------------------------
 * allocator_test
 * test stateful allocators and exception handling
 */
bool allocator_test( )
{
    typedef std::list< int, LowMemAllocator<int> > list_t;
    LowMemAllocator<int> mem(100);
    mem.SetTripOnAlloc();
    list_t lst( mem );
    bool thrown = false;
    
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            lst.push_front( i );
        }
    }catch( std::bad_alloc const & ){
        mem = lst.get_allocator();
        if( mem.GetNumAllocs() != 101 ) FAIL       //should fail on 101st
        if( INSANE(lst) || lst.size() != 99 ) FAIL //one alloc for sentinel
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    
    lst.clear();
    mem.Reset(100);
    mem.SetTripOnConstruct();
    thrown = false;
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            lst.push_back( i );
        }
    }catch( std::bad_alloc const & ){
        mem = lst.get_allocator();
        if( mem.GetNumConstructs() != 101 ) FAIL
        //should have cleaned up last one and left only 100 allocated items
        if( mem.GetNumAllocs() != 101 || mem.GetNumDeallocs() != 1 ) FAIL    
        if( INSANE(lst) || lst.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    //if container didn't deal with the exception and clean up the allocated 
    //memory then the leak detector will also trip later
    
    lst.clear();
    mem.Reset(100);
    thrown = false;
    for( int i = 0; i < 70; i++ ){
        lst.push_back( i );
    }
    //now reset the allocator so it trips at a lower threshold
    //and test the copy mechanism works right
    mem.Reset( 50 );
    mem.SetTripOnAlloc();
    try{
        list_t lst2( lst );
    }catch( std::bad_alloc ){
        if( mem.GetNumConstructs() != 49 ) FAIL  //sentinel not constructed
        if( mem.GetNumAllocs()     != 51 ) FAIL
        if( mem.GetNumDestroys()   != 49 ) FAIL  //sentinel not destroyed
        if( mem.GetNumDeallocs()   != 50 ) FAIL
        if( INSANE( lst ) || lst.size() != 70 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * sort_test
 * test sort function
 */
// helper for sort tests
// check ordered with operator<
template< class T >
bool chk_sorted( T const & lst )
{
    T::const_iterator it,nit;
    it = lst.begin();
    nit = it;
    ++nit;
    for( ; nit != lst.end(); ++it, ++nit ){
        if( *nit < *it ) return( false );
    }
    return( true );
}

// quick and nasty helper class for sort test
// used for checking sort is stable
struct MyPair{
    int x;
    int y;
    MyPair( int xx, int yy ) : x(xx), y(yy) {}
    MyPair( MyPair const & o ) : x(o.x), y(o.y) {}
    bool operator==( MyPair const & o ) { return( x == o.x ); }
};
bool operator<( MyPair const & t, MyPair const & o ) { return( t.x < o.x ); }


bool sort_test()
{
    typedef class std::list<int> li_t;
    li_t lst;
    
    // check some special cases
    
    // empty
    lst.sort();
    if( INSANE( lst ) || !lst.empty() ) FAIL
    
    // single element
    lst.push_back( 99 );
    lst.sort();
    if( INSANE( lst ) || lst.size() != 1 || lst.front() != 99 ) FAIL
    lst.clear();
    
    // try all combinations of 3 numbers
    // 1
    lst.push_back( 1 );
    lst.push_back( 2 );
    lst.push_back( 3 );
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();
    // 2
    lst.push_back( 1 ) ;
    lst.push_back( 3 ) ;
    lst.push_back( 2 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();
    // 3
    lst.push_back( 2 ) ;
    lst.push_back( 1 ) ;
    lst.push_back( 3 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();
    // 4
    lst.push_back( 2 ) ;
    lst.push_back( 3 ) ;
    lst.push_back( 1 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();
    // 5
    lst.push_back( 3 ) ;
    lst.push_back( 1 ) ;
    lst.push_back( 2 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();
    // 6
    lst.push_back( 3 ) ;
    lst.push_back( 2 ) ;
    lst.push_back( 1 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size() != 3 || !chk_sorted(lst) ) FAIL
    lst.clear();

    // hit it with a load of random numbers
    int const rand_size = 16383;
    for( int i = 0; i < rand_size; i++ ){
        lst.push_back( std::rand() );
    }
    lst.sort();
    if( INSANE( lst ) || lst.size() != rand_size || !chk_sorted(lst) ) FAIL
    lst.clear();

    // test the version that takes a functor
    for( int i = 0; i < rand_size; i++ ){
        lst.push_back( std::rand() );
    }
    lst.sort( std::greater<int>() );
    lst.reverse();
    if( INSANE( lst ) || lst.size() != rand_size || !chk_sorted(lst) ) FAIL
    lst.clear();
    
    // test sort is stable
    std::list< MyPair > l2;
    int c;
    // build array of random numbers
    for( c = 0; c < 10001; ){
        int r = std::rand() % 15;
        if( r == 7 ){
            // 7s are special, second value ascends and 
            // should stay in that order after sort
            l2.push_back( MyPair( 7, c ) ); 
            c++;
        }else{
            l2.push_back( MyPair( r, 0 ) );
        }
    }
    l2.sort();
    if( INSANE( l2 ) ) FAIL
    c = 0;
    int last = l2.front().x;
    while( !l2.empty()){
        // check in sorted order
        if( l2.front().x < last ) FAIL 
        last = l2.front().x;
        // check 7s still in order
        if( l2.front().x == 7 ){
            if( l2.front().y != c ) FAIL
            c++;
        }
        l2.pop_front();
    }
    // end stable test
    
    return( true );
}


int main( )
{
    int rc = 0;
    int original_count = heap_count( );
    //heap_dump();

    try {
        if( !construct_test( )        || !heap_ok( "t01" ) ) rc = 1;
        if( !access_test( )           || !heap_ok( "t02" ) ) rc = 1;
        //if( !string_test( )           || !heap_ok( "t03" ) ) rc = 1;
        //if( !torture_test( )          || !heap_ok( "t04" ) ) rc = 1;
        if( !assign_test( )           || !heap_ok( "t05" ) ) rc = 1;
        if( !clear_test( )            || !heap_ok( "t06" ) ) rc = 1;
        if( !erase_test( )            || !heap_ok( "t07" ) ) rc = 1;
        if( !swap_test( )             || !heap_ok( "t08" ) ) rc = 1;
        if( !remove_test( )           || !heap_ok( "t09" ) ) rc = 1;
        if( !iterator_test( )         || !heap_ok( "t10" ) ) rc = 1;
        if( !reverse_iterator_test( ) || !heap_ok( "t11" ) ) rc = 1;
        if( !copy_test( )             || !heap_ok( "t12" ) ) rc = 1;
        if( !splice_test( )           || !heap_ok( "t13" ) ) rc = 1;
        if( !reverse_test( )          || !heap_ok( "t14" ) ) rc = 1;
        if( !merge_test( )            || !heap_ok( "t15" ) ) rc = 1;
        if( !allocator_test( )        || !heap_ok( "t16" ) ) rc = 1;
        if( !sort_test( )             || !heap_ok( "t17" ) ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }

    if( heap_count( ) != original_count ) {
        std::cout << "Possible memory leak!\n";
        heap_dump();
        rc = 1;
    }
    return( rc );
}
