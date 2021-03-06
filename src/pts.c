/*******************************************************************************
 *
 * @file      pts.c
 * @brief     libpts is a library for reading pointclouds stored in pts files.
 * @details   libpts is a library for reading pointclouds stored in pts files.
 *            This includes additional information line remission values, color
 *            values or quality information. The pts file format is used for
 *            example at the University of Osnabrück. 
 * 
 * @author    Lars Kiesow (lkiesow), lkiesow@uos.de, Universität Osnabrück
 * @version   111010
 * @date      09/04/2011 12:47:50 PM
 * @date      2011-10-10 14:09:33
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include "pts.h"

void (*pts_callback_xyz)(double, double, double)    = NULL;
void (*pts_callback_rgb)(uint8_t, uint8_t, uint8_t) = NULL;
void (*pts_callback_remission)(double)              = NULL;
void (*pts_callback_quality)(double)                = NULL;
void (*pts_callback_colored)(int)                   = NULL;
void (*pts_callback_add_data)(char *)               = NULL;
void (*pts_callback_eol)(void)                      = NULL;


void pts_set_callback_xyz( void (* callback)(double, double, double) ) {
	pts_callback_xyz = callback;
}


void pts_set_callback_rgb( void (* callback)(uint8_t, uint8_t, uint8_t) ) {
	pts_callback_rgb = callback;
}


void pts_set_callback_remission( void (* callback)(double) ) {
	pts_callback_remission = callback;
}


void pts_set_callback_quality( void (* callback)(double) ) {
	pts_callback_quality = callback;
}


void pts_set_callback_colored( void (* callback)(int) ) {
	pts_callback_colored = callback;
}


void pts_set_callback_add_data( void (* callback)(char *) ) {
	pts_callback_add_data = callback;
}


void pts_set_callback_eol( void (* callback)(void) ) {
	pts_callback_eol = callback;
}


FILE * pts_open( const char * filename ) {
	return fopen( filename, "r" );
}


FILE * pts_create( const char * filename, const char * comment ) {

	if ( comment ) {
		/* Check if string is not too long. */
		if ( strlen( comment ) > 2047 ) {
			return NULL;
		}

		/* Check if string does not contain any newline characters. */
		if ( strchr( comment, '\n' ) || strchr( comment, '\n' ) ) {
			return NULL;
		}
	}

	FILE * f = fopen( filename, "w" );
	/* Check if file cound be opened. Return NULL otherwise. */
	if ( !f ) {
		return NULL;
	}
	/* Add comment to file or add a blank line if we have none. */
	fprintf( f, "%s\n", ( comment ? comment : "" ) );

	return f;

}


void pts_close( FILE * ptsfile ) {
	if ( ptsfile ) {
		fclose( ptsfile );
	}
}


uint32_t pts_count_points( FILE * ptsfile ) {

	if ( !ptsfile ) {
		return PTS_DATA_ERROR;
	}
	/* Start from the beginning */
	fseek( ptsfile, 0, SEEK_SET );

	/*  Count lines */
	char line[1024];
	uint32_t count = 0;
	while ( !feof( ptsfile ) ) {
		fgets( line, 1023, ptsfile );
		count++;
	}

	fseek( ptsfile, 0, SEEK_SET );

	/* The first line is a comment and the last line is counted twice because we
	 * use feof. Thus the amount of values is count minus two. */
	return count - 2;

}


char * pts_get_comment( FILE * ptsfile, char * comment ) {

	if ( !ptsfile || !comment ) {
		comment = "";
		return NULL;
	}
	/* Start from the beginning */
	fseek( ptsfile, 0, SEEK_SET );

	/* Read first line. */
	fgets( comment, 1023, ptsfile );

	/* Reset position. */
	fseek( ptsfile, 0, SEEK_SET );

	/* Remove newline character */
	char * nl = strchr( comment, '\n' );
	if ( nl ) {
		*nl = 0;
	}
	nl = strchr( comment, '\r' );
	if ( nl ) {
		*nl = 0;
	}

	return comment;

}


uint32_t pts_test_format( FILE * ptsfile ) {

	if ( !ptsfile ) {
		return PTS_DATA_ERROR;
	}
	/* Start from the beginning */
	fseek( ptsfile, 0, SEEK_SET );

	/* Determine amount of values per line */
	char line[1024];
	/* Jump over first line. */
	fgets( line, 1023, ptsfile );
	fgets( line, 1023, ptsfile );
	int valcount = 0;
	char * pch = strtok( line, "\t " );
	 while ( pch ) {
		if ( *pch != 0 && *pch != '\n' ) {
			valcount++;
		}
		pch = strtok( NULL, "\t " );
	}

	switch ( valcount ) {
		case 3: return PTS_DATA_XYZ;
		case 4: return PTS_DATA_XYZ | PTS_DATA_REMISSION;
		case 6: return PTS_DATA_XYZ | PTS_DATA_RGB;
		case 7: return PTS_DATA_XYZ | PTS_DATA_REMISSION | PTS_DATA_RGB;
		case 8: return PTS_DATA_XYZ | PTS_DATA_QUALITY | PTS_DATA_COLORED | PTS_DATA_RGB;
	}
	return PTS_DATA_ERROR;

	/* Start from the beginning */
	fseek( ptsfile, 0, SEEK_SET );

}


pts_info pts_load( FILE * ptsfile ) {

	pts_info info;
	/* Open file */
	if ( !ptsfile ) {
		info.count  = 0;
		info.format = PTS_DATA_ERROR;
		return info;
	}

	/* Determine amount of values per line */
	info.format = pts_test_format( ptsfile );
	/* Reset file position. */
	fseek( ptsfile, 0, SEEK_SET );
	/* Start after first line. */
	char line[1024];
	fgets( line, 1023, ptsfile );

	/* Determine if we have to read all values. */
	uint8_t val_per_line = 0;
	switch ( info.format ) {
		case PTS_DATA_XYZ: 
			val_per_line = pts_callback_xyz ? 3 : 0;
			break;
		case PTS_DATA_XYZ | PTS_DATA_REMISSION: 
			val_per_line = pts_callback_remission 
				? 4 : ( pts_callback_xyz 
						? 3 : 0 );
			break;
		case PTS_DATA_XYZ | PTS_DATA_RGB:
			val_per_line = pts_callback_rgb 
				? 6 : ( pts_callback_xyz 
						? 3 : 0 );
			break;
		case PTS_DATA_XYZ | PTS_DATA_REMISSION | PTS_DATA_RGB:
			val_per_line = pts_callback_rgb 
				? 7 : ( pts_callback_remission 
						? 4 : ( pts_callback_xyz 
							? 3 : 0 ) );
			break;
		case PTS_DATA_XYZ | PTS_DATA_QUALITY | PTS_DATA_COLORED | PTS_DATA_RGB:
			val_per_line = pts_callback_rgb 
				? 8 : ( pts_callback_colored
						? 5 : ( pts_callback_quality
							? 4 : ( pts_callback_xyz 
								? 3 : 0 ) ) );
	}

	/* Stop if nothing to do. */
	if ( !val_per_line ) {
		info.count = 0;
		return info;
	}


	double b1, b2, b3;
	char * s;
	char * nl;
	while ( 1 ) {
		fgets( line, 1023, ptsfile );
		if ( feof( ptsfile ) ) {
			break;
		}
		s = line;
		info.count++;

		b1 = strtod( s, &s );
		b2 = strtod( s, &s );
		b3 = strtod( s, &s );
		if ( pts_callback_xyz ) {
			pts_callback_xyz( b1, b2, b3 );
		}
		/* Check if we do not need additional data. */
		if ( val_per_line < 4 ) {
			if ( ( *s != 0 ) && ( *s != '\n' ) && pts_callback_add_data ) {
				/* Remove newline character. */
				if ( ( nl = strrchr( s, '\n' ) ) ) {
					*nl = 0;
				}
				pts_callback_add_data( s );
			}
			if ( pts_callback_eol ) {
				pts_callback_eol();
			}
			continue;
		}
		/*  Read remission value. */
		if ( info.format & PTS_DATA_REMISSION ) {
			b1 = strtod( s, &s );
			if ( pts_callback_remission ) {
				pts_callback_remission( b1 );
			}
			/* Check if we do not need additional data. */
			if ( val_per_line < 5 ) {
				if ( ( s != 0 ) && ( *s != '\n' ) && pts_callback_add_data ) {
					/* Remove newline character. */
					if ( ( nl = strrchr( s, '\n' ) ) ) {
						*nl = 0;
					}
					pts_callback_add_data( s );
				}
				if ( pts_callback_eol ) {
					pts_callback_eol();
				}
				continue;
			}
		}
		/* Read quality and color-flag. */
		if ( ( info.format & PTS_DATA_QUALITY ) && ( info.format & PTS_DATA_COLORED ) ) {
			b1 = strtod( s, &s );
			b2 = strtod( s, &s );
			if ( pts_callback_quality ) {
				pts_callback_quality( b1 );
			}
			if ( pts_callback_colored ) {
				pts_callback_colored( b2 > 0.5 );
			}
			/* Check if we do not need additional data. */
			if ( val_per_line < 6 ) {
				if ( ( s != 0 ) && ( *s != '\n' ) && pts_callback_add_data ) {
					/* Remove newline character. */
					if ( ( nl = strrchr( s, '\n' ) ) ) {
						*nl = 0;
					}
					pts_callback_add_data( s );
				}
				if ( pts_callback_eol ) {
					pts_callback_eol();
				}
				continue;
			}
		}
		/* Read RGB color information. */
		if ( info.format & PTS_DATA_RGB ) {
			if ( pts_callback_rgb ) {
				b1 = strtod( s, &s );
				b2 = strtod( s, &s );
				b3 = strtod( s, &s );
				pts_callback_rgb( (int) b1, (int) b2, (int) b3 );
			} else if ( pts_callback_add_data ) {
				/* Remove newline character. */
				if ( ( nl = strrchr( s, '\n' ) ) ) {
					*nl = 0;
				}
				pts_callback_add_data( s );
			}
		}
		if ( pts_callback_eol ) {
			pts_callback_eol();
		}
	}

	fseek( ptsfile, 0, SEEK_SET );
	return info;

}
