#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buff[2048];

char *prefix[2] = {
#if defined( INLINETEST )
    "PASS decoding ",
#else
    "PASS executing ",
#endif
#if defined( __UNIX__ )
    "PASS source/"
#else
    "PASS source\\"
#endif
};
size_t plen[2];

int main( int argc, char **argv ) {
    FILE *fp;
    char *chk;
    size_t len;
    unsigned line;
    unsigned flip;

    if( argc != 2 ) {
	puts( "usage: chk_exec <file>" );
	puts( "FAIL" );
	return( EXIT_FAILURE );
    }
    fp = fopen( argv[1], "r" );
    if( !fp ) {
	puts( "cannot open input file" );
	puts( "FAIL" );
	return( EXIT_FAILURE );
    }
    plen[0] = strlen( prefix[0] );
    plen[1] = strlen( prefix[1] );
    line = 0;
    flip = 0;
    for( ; ; ) {
	chk = fgets( buff, sizeof( buff ), fp );
	if( chk == NULL ) break;
	++line;
	len = plen[ flip ];
	if( memcmp( buff, prefix[ flip ], len ) ) {
	    printf( "detected on line %u\n", line );
	    printf( "contents: %s", buff );
	    puts( "FAIL" );
	    return( EXIT_FAILURE );
	}
#if !defined( INLINETEST )
	flip ^= 1;
#endif
    }
#if defined( INLINETEST )
    if( line < 1 ) {
#else
    if( line <= 2 || line & 1 ) {
#endif
	puts( "incorrect # of lines!" );
	puts( "FAIL" );
	return( EXIT_FAILURE );
    }
    fclose( fp );
    puts( "PASS" );
    return( EXIT_SUCCESS );
}
