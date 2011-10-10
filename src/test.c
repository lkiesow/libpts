/*******************************************************************************
 *
 * @file      test.c
 * @brief     Test program for libpts.
 * 
 * @author    Lars Kiesow (lkiesow), lkiesow@uos.de, Universität Osnabrück
 * @date      09/04/2011 10:32:30 PM
 *
 ******************************************************************************/

#include	<stdlib.h>
#include <pts.h>


void handle_xyz( double x, double y, double z ) {
	printf( "xyz: % 12f % 12f % 12f\n", x, y, z );
}

void handle_rgb( uint8_t r, uint8_t g, uint8_t b ) {
	printf( "rgb: % 4u % 4u % 4u\n", r, g, b );
}

void handle_add_data( char * data ) {
	printf( "add_data: %s\n", data );
}

int main( int argc, char ** argv ) {

	if ( argc < 2 ) {
		printf( "Usage: %s ptsfile\n", *argv );
		return EXIT_SUCCESS;
	}

	FILE * f = pts_open( argv[1] );

	char buf[1024];
	if ( pts_get_comment( f, buf ) ) {
		printf( "Comment: %s\n", buf );
	}

	printf( "%u points in file.\n", pts_count_points( f ) );

	uint32_t pts_format = pts_test_format( f );
	printf( "xyz : %s\n", ( pts_format & PTS_DATA_XYZ       ) ? "yes" : "no" );
	printf( "rgb : %s\n", ( pts_format & PTS_DATA_RGB       ) ? "yes" : "no" );
	printf( "rem : %s\n", ( pts_format & PTS_DATA_REMISSION ) ? "yes" : "no" );
	printf( "qua : %s\n", ( pts_format & PTS_DATA_QUALITY   ) ? "yes" : "no" );
	printf( "col : %s\n", ( pts_format & PTS_DATA_COLORED   ) ? "yes" : "no" );
	printf( "\n" );

	pts_set_callback_xyz( &handle_xyz );
	pts_set_callback_rgb( &handle_rgb );
	pts_info info = pts_load( f );

	printf( "Second run (with add_data_callback\n" );
	pts_set_callback_rgb( NULL );
	pts_set_callback_add_data( &handle_add_data );
	info = pts_load( f );

	pts_close( f );
	return EXIT_SUCCESS;

}
