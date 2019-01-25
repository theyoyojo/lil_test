/*
 * lil_db.c source file
 * A lil debugging library to save some keystrokes
 * By Joel Savitz <jsavitz@redhat.com>
 */

#include "lil_db.h"
#include <string.h>

// The data object for this library. Sorta private-ish
static lil_db_data_t db_data = {{ 0 }} ;

/* RETURN MACROS woo what fun */

typedef enum lil_db_return_code {
	SUCCESS = 0,
	FILE_OPEN_ERROR,
	BUFFER_WRITE_ERROR,
	FILE_WRITE_ERROR,
	INVALID_STATE_ERROR
} lil_db_return_code ;

// Generic success case. Nothing extra to be done
#define LIL_DB_RETURN_SUCCESS SUCCESS
#define LIL_DB_RETURN_SUCCESS_DATA(data) data

// Error case: Cannot open output file for writing/appending
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_FILE_OPEN_ERROR(filename)                                \
		(db_data.is_valid = 0),                                        \
		fprintf(stderr, 					       \
		"fopen(%s,\"a+\") failed!"				       \
		"lil_db is now in an error state.\n",   		       \
		filename), FILE_OPEN_ERROR

// Error case: Cannot write any bytes to buffer, return nonzero value
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_BUFFER_WRITE_ERROR				       \
		(db_data.is_valid = 0),	                                       \
		fprintf(stderr,						       \
		"Unable to write any bytes to buffer! "			       \
		"lil_db is now in an error state\n"			       \
		), BUFFER_WRITE_ERROR

// Error case: Cannot write buffer to output file. Return nonzero value
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_FILE_WRITE_ERROR(filename)		               \
		(db_data.is_valid = 0),					       \
		fprintf(stderr,						       \
		"Unable to write buffer to file \"%s\"! "		       \
		"lil_db is now in an error state\n",			       \
		filename), FILE_WRITE_ERROR

// Error case: User calls library functions while it is in an error state
// lil_db is (still) not valid after a function returns this code
#define LIL_DB_RETURN_INVALID_STATE_ERROR(msg) 				       \
		(db_data.is_valid = 0),					       \
		fprintf(stderr,						       \
		"lil_db is not in a usable state!\n"	 		       \
		"Cannot continue with requested operation: %s\n",	       \
		msg), INVALID_STATE_ERROR


/* LIBRARY FUNCTIONS */

// Initialize buffer and output filestream
int lil_db_init(char * filename, size_t string_length)
{	
	// Powerwash my data struct to eliminate negative energies
	memset( 	       // Set the bits in memory
		&db_data,      // That begins where my struct lil_db_data begins
		0,	       // To 0
		sizeof(db_data)// For all bits in the struct
	) ;		       // The struct is now clean

	strncpy(                  // Copy the data
		db_data.output_filename, // To this field
		filename,	  // From this parameter
		string_length     // But no more than this many bytes
	) ;			  // The field is now correctly initialized

	// At this point, I would insert any actions neccessary to initalize the
	// buffer, but since it is already scrubbed above,
	// there is nothing to be done
	
	// Open the output file to be appended
	db_data.output_filestream = fopen(db_data.output_filename,"a+") ;
	if (!db_data.output_filestream) {
		// If this fails, we cannot continue
		// TODO: error message in buffer?
		return LIL_DB_RETURN_FILE_OPEN_ERROR(db_data.output_filename) ;
	}

	// Begin counting entries in the file at 0
	db_data.entry_number = 0 ;

	// There is nothing in the buffer, so it is 0 characters long
	db_data.buff_length = 0 ;
	
	// This field can stay empty, as God intended
	db_data.empty = 0 ;

	// We leave the lil_db object in a valid state if the above has executed
	db_data.is_valid = 1 ;

	// If this point is reached, everything has gone swimmingly
	return LIL_DB_RETURN_SUCCESS ;

}

// Append contents of buffer to file, clear buffer (fill with 0s)
int lil_db_flush_buffer(int number_chars_not_copied)
{
	// Check validity of library state
	if(!db_data.is_valid) return LIL_DB_RETURN_INVALID_STATE_ERROR
		(__FUNCTION__) ;
	
	// Write contents of buffer to output file
	if (fprintf(db_data.output_filestream, "%s", db_data.buff ) < 0 ) {
		// Case: There was an error writing to the file
		return LIL_DB_RETURN_FILE_WRITE_ERROR(db_data.output_filename) ;
	}	

	// Wipe buffer
	memset(db_data.buff,0,LIL_DB_DEFAULT_BUFFSZ) ;
	// It now contains 0 characters :)
	db_data.buff_length = 0 ;

	return LIL_DB_RETURN_SUCCESS_DATA(number_chars_not_copied) ;
}

// Perform the actions of lil_db_enqueue and subsequently lil_db_flush,
// but as a new entry
int lil_db_printf(lil_db_option options, char * format, ...)
{
	// Check validity of library state
	if(!db_data.is_valid) return LIL_DB_RETURN_INVALID_STATE_ERROR
		(__FUNCTION__) ;

	int number_chars_copied, number_chars_not_copied,
	    complete_string_length, available_buffer_space ;
	va_list va_args ;


	// Case: The user requests __EMPHASIS__
	if (options & LIL_DB_OPTION_EMPHASIS) {
		// If the user wants some emphasis, throw in some bangs or something idk
		db_data.buff_length += snprintf(
			db_data.buff,			// Write to primary buffer
			LIL_DB_DEFAULT_BUFFSZ+1,	// Don't overflow
			"%s",				// Just a string
			LIL_DB_EMPHASIS_STYLE		// Find this in the .h
		) ;
	}


	available_buffer_space = LIL_DB_DEFAULT_BUFFSZ + 1 - db_data.buff_length ;

	// Case: The user requests an enumerated prefix
	if  (options & LIL_DB_OPTION_NUMBERED) {
		// Give the people what they desire
		db_data.buff_length += snprintf(
			db_data.buff			// Write to primary buffer
				+ db_data.buff_length,  // Offset by length
			available_buffer_space,		// Don't overflow
			"[%d]. ",			// A formatted int
			db_data.entry_number++		// From internal data
		) ;
	}

	// Recalculate this value, as buff_length has changed
	available_buffer_space = LIL_DB_DEFAULT_BUFFSZ + 1 - db_data.buff_length ;

	va_start(va_args,format) ;

	// TODO: force newline?

	complete_string_length =		// We save the potential length
		vsnprintf(			// Using the save version
			db_data.buff			// Write to primary buffer
				+ db_data.buff_length,  // Offset by length
			available_buffer_space,		// Don't overflow
			format,				// with this fmt string
			va_args				// and the corresponding args
		) ;

	va_end(va_args) ;
	
	// Case: vsnprintf failed. Something is wrong with the object
	if (complete_string_length < 0) {
		return LIL_DB_RETURN_BUFFER_WRITE_ERROR ;
	}

	number_chars_copied = complete_string_length - (available_buffer_space - 1) ;
	// -1 to correct for '\0' taken into account by vsnprintf
	
	// Complement the above
	number_chars_not_copied = complete_string_length - number_chars_copied ;

	// Append buffer to file, clear it, and return result of that operation
	// Report on leftover uncopied chars
	return lil_db_flush_buffer(number_chars_not_copied) ;
}

int lil_db_kill(void)
{
	// Check validity of library state
	if(!db_data.is_valid) return LIL_DB_RETURN_INVALID_STATE_ERROR
		("You cannot kill what is already dead") ;

	// This is really all I need to clean up
	fclose(db_data.output_filestream) ; 
	
	// Without an active output filestream, the library is in an invalid state
	db_data.is_valid = 0 ;
	
	return LIL_DB_RETURN_SUCCESS ;
}

int lil_db_is_not_valid(void)
{
	// Extremely straightforward function
	return db_data.is_valid ;
}
