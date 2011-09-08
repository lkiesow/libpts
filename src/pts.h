/**
 *
 * @file      pts.h
 * @brief     libpts is a library for reading pointclouds stored in pts files.
 * @details   libpts is a library for reading pointclouds stored in pts files.
 *            This includes additional information line remission values, color
 *            values or quality information. The pts file format is used for
 *            example at the University of Osnabrück. 
 * 
 * @author    Lars Kiesow (lkiesow), lkiesow@uos.de, Universität Osnabrück
 * @version   110905
 * @date      Created: 09/04/2011 08:31:48 PM
 * @date      Modified: 9/05/11 00:02:37
 *
 **/
#include <stdio.h>
#include <stdint.h>

#define PTS_DATA_ERROR      0
#define PTS_DATA_XYZ        1
#define PTS_DATA_RGB        2
#define PTS_DATA_REMISSION  4
#define PTS_DATA_QUALITY    8
#define PTS_DATA_COLORED   16


typedef struct {
	uint32_t format;
	uint32_t count;
} pts_info;


/**
 * Set callback for position data. The callback is called every time new
 * xyz-values are read from the pts file.
 * @brief Set callback for position data.
 *
 * @param  callback  Function taking three arguments to handle position data.
 **/
void pts_set_callback_xyz( void (* callback)(double, double, double) );


/**
 * Set callback for color data. The callback is called every time new
 * rgb-values are read from the pts file.
 * @brief Set callback for color data.
 *
 * @param  callback  Function taking three arguments to handle color data.
 **/
void pts_set_callback_rgb( void (* callback)(uint8_t, uint8_t, uint8_t) );


/**
 * Set callback for remission data. The callback is called every time new
 * remission values are read from the pts file.
 * @brief Set callback for remission data.
 *
 * @param  callback  Function taking one argument to handle remission data.
 **/
void pts_set_callback_remission( void (* callback)(double) );


/**
 * Set callback for quality information. The callback is called every time new
 * quality values are read from the pts file.
 * @brief Set callback for quality information.
 *
 * @param  callback  Function taking one argument to handle quality infos.
 **/
void pts_set_callback_quality( void (* callback)(double) );


/**
 * Set callback for color information flag. The callback is called every time
 * new color validity infos are read from the pts file. These information
 * determine if the color information are valid.
 * @brief Set callback for the color information flag.
 *
 * @param  callback  Function taking one argument to handle color information
 *                   flags.
 **/
void pts_set_callback_colored( void (* callback)(int) );


/**
 * Set callback for additional data in the given pts file. Additional data are
 * all data behind the last value for which a callback is registered. The data
 * passed to this callback is a string containing all data and ending with a
 * newline character.
 * @brief Set callback for additional data.
 *
 * @param  callback  Function taking one argument to handle additional data.
 **/
void pts_set_callback_add_data( void (* callback)(char *) );


/**
 * Set callback for end of lines. As all data of one point are in one line in a
 * pts file, this callback also separates two points.
 * @brief Set callback for end of line.
 *
 * @param  callback  Function to handle line ends.
 **/
void pts_set_callback_eol( void (* callback)(void) );


/**
 * Open a pts file with given name for reading.
 * @brief Open a pts file for reading.
 *
 * @param  filename  Path of the pts file.
 * @return  Filepointer to the opened file.
 **/
FILE * pts_open( char * filename );


/**
 * Close opened pts file.
 * @brief Close opened pts file.
 *
 * @param  ptsfile  Filepointer to the opened file.
 **/
void pts_close( FILE * ptsfile );


/**
 * Count the amount of points in the given pts file. Only use this function if
 * you really have to, because the whole file has to be read to accomplish this
 * task.
 * @brief Count amount of points in pts file.
 *
 * @param  ptsfile  Filepointer to the opened file.
 * @return  Amount of points in pts file.
 **/
uint32_t pts_count_points( FILE * ptsfile );


/**
 * Test a given pts file and determine the type of data stored inside the file.
 * This function is automatically called by pts_load to check wich data can be
 * read and returned.
 * @brief Determine type of data stored in the pts file.
 *
 * @param  ptsfile  Filepointer to the opened file.
 * @return  Pts file format flag.
 **/
uint32_t pts_test_format( FILE * ptsfile );


/**
 * Read the content of the given pts file and transfer the data to the callback
 * functions.
 * @brief Read given pts file.
 *
 * @param  ptsfile  Filepointer to the opened file. 
 * @return  Information about amount and type of read data.
 **/
pts_info pts_load( FILE * ptsfile );
