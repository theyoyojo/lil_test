/*
 *  Extremely lightweight testing framework for GNU C
 *  Copyright (C) 2019 Joel Savitz
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * lil_db.h header file
 * A lil debugging library to save some keystrokes
 * By Joel Savitz <jsavitz@redhat.com>
 */

#ifndef LIL_DB_H
#define LIL_DB_H

#include <stdarg.h>
#include <stdio.h>

#define LIL_DB_DEFAULT_BUFFSZ 247

// Used when the user wants to try a new aesthetic
#define LIL_DB_EMPHASIS_STYLE "\n[!!!]\n"

// Note to self: Keep it simple and don't dynamically allocate any memory

// Library data format
typedef struct lil_db_data {
	// Name of output file, can be long if that's what you're in the mood for
	char output_filename[LIL_DB_DEFAULT_BUFFSZ+1] ; // +1 for \0
	
	// Buffer for data to be written to output file
	char buff[LIL_DB_DEFAULT_BUFFSZ+1] ; // +1 for \0

	// The filestream to write output to
	FILE * output_filestream ;

	// Entry number in output file
	unsigned int entry_number ; 

	// The number characters are currently in the buffer	
	int buff_length:9 ; // Don't think I'll need all 9 bits but I have plenty to spare
	
	// Empty field that will be padded anyway, can be used later
	int empty:21 ;	   	

	// This should be nzero only if we have a problem with our normal operations
	unsigned int is_valid:1 ;
} lil_db_data_t ;

// Flags that can be passed throgh the options parameter to modify 
typedef enum lil_db_option {
	LIL_DB_OPTION_DEFAULT  = 0x0, 		// raw vsnprintfing
	LIL_DB_OPTION_EMPHASIS = 0x1,		// enhanced vsnprintfing
	LIL_DB_OPTION_NUMBERED = 0x2      	// slap a nice looking number in the front
} lil_db_option ;

// All functions return 0 on success and nonzero on failure unless otherwise specified
// See implementation for details
 
// Initialize buffer and output filestream, may fix an invalid library state
int lil_db_init(char * filename, size_t string_length) ;

// Clean up before the program terminates
int lil_db_kill(void) ;

// Oposite of normal function due to convention of 0 returned on success
// This function does exactly what you think it does
int lil_db_is_not_valid(void) ;

// Append contents of buffer to file, clear buffer (fill with 0s)
int lil_db_flush_buffer(int number_chars_not_copied) ;

// Perform the actions of lil_db_enqueue and subsequently lil_db_flush, but as a new entry
int lil_db_printf(lil_db_option options, char * format, ...) ;

#endif // LIL_DB_H
