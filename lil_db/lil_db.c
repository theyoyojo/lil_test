#include "lil_db.h"

#include <string.h>

/* FYI (from lil_db.h)
typedef struct lil_db_data {
	// Name of output file, can be long if that's what you're in the mood for
	char output_filename[LIL_DB_DEFAULT_BUFFSZ+1] ; // +1 for \0
	
	// Buffer for data to be written to output file
	char buff[LIL_DB_DEFAULT_BUFFSZ+1] ; // +1 for \0

	// The filestream to write output to
	FILE * output_filestream ;

	// Entry number in output file
	unsigned int entry_number ; 
	
	// Should the next flush of the buffer be prefixed by the entry number?
	int new_entry:1 ;
	
	// The number characters are currently in the buffer	
	int buff_length:9 ; 	// Don't think I'll need all 9 bits
			    	//but I have plenty to spare
	
	// Empty field that will be padded anyway, can be used later
	int empty:21 ;	   	

	// This should be nonzero only if we
	// have a problem with our normal operations
	int is_valid:1 ;
} lil_db_data ;
} lil_db_data ;
*/

// The data object for this library. Sorta private-ish
static lil_db_data db_data ;

/* RETURN MACROS woo what fun */

// Generic success case. Nothing extra to be done
#define LIL_DB_RETURN_SUCCESS 0

// Error case: Cannot open output file for writing/appending
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_FILE_OPEN_ERROR(filename)                                \
		(db_data.is_valid = 0),                                        \
		fprintf(stderr, 					       \
		"fopen(%s,\"a+\") failed!"				       \
		"lil_db is now in an error state.\n",   		       \
		filename), 1	// Real value returned is 1

// Error case: Cannot write all bytes to buffer, return number of bytes written
// lil_db is STILL VALID after this because it is designed to recover from this
#define LIL_DB_RETURN_BUFFER_OVERFLOW_AVOIDED(number_chars_not_printed)        \
	number_chars_not_printed

// Error case: Cannot write any bytes to buffer, return nonzero value
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_BUFFER_WRITE_FAILED                                      \
		(db_data.is_valid = 0),	                                       \
		fprintf(stderr,						       \
		"Unable to write any bytes to buffer! "			       \
		"lil_db is now in an error state\n"			       \
		), 2	// Real value returned is 2

// Error case: Cannot write buffer to output file. Return nonzero value
// lil_db is not valid after a function returns this code
#define LIL_DB_RETURN_FILE_WRITE_ERROR(filename)		               \
		(db_data.is_valid = 0),					       \
		fprintf(stderr,						       \
		"Unable to write buffer to file \"%s\"! "		       \
		"lil_db is now in an error state\n",			       \
		filename), 3	// Real value returned is 3

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

	// The first entry is a new entry, so prefix it
	db_data.new_entry = 1 ;

	// There is nothing in the buffer, so it is 0 characters long
	db_data.buff_length = 0 ;
	
	// This field can stay empty, as God intended
	db_data.empty = 0 ;

	// We leave the lil_db object in a valid state if the above has executed
	db_data.is_valid = 1 ;

	// If this point is reached, everything has gone swimmingly
	return LIL_DB_RETURN_SUCCESS ;

}

// Write data to buffer using printf-like syntax
int lil_db_enqueue(char * format, ...) {
	va_list va_args ;
	size_t remaining_buff_space ;
	int complete_string_length ;

	va_start(va_args, format) ;
	
	// How much space is left in the buffer? (+1 for \0)
	remaining_buff_space = LIL_DB_DEFAULT_BUFFSZ + 1 - db_data.buff_length ;
	
	// We can't write any more than that number of bytes
	complete_string_length =
		vsnprintf(db_data.buff, remaining_buff_space, format, va_args) ;

	va_end(va_args) ;

	// Case: vsnprintf failed. Something is wrong with the object
	if (complete_string_length < 0) {
		return LIL_DB_RETURN_BUFFER_WRITE_FAILED ;
	}
	else
	// Case: Not enough space in the buffer, printing was truncated
	if (complete_string_length >= (int)remaining_buff_space) {
		// Return the number of characters not copied but don't
		// return a complete success
		return LIL_DB_RETURN_BUFFER_OVERFLOW_AVOIDED(
			complete_string_length - (remaining_buff_space - 1)) ;
			// -1 for \0 that should not be accounted for
	}
	else
	// Case: The string was successfully vsnprintf'd to the buffer
	{
		return LIL_DB_RETURN_SUCCESS ;
	}
}

// Append contents of buffer to file, clear buffer (fill with 0s)
int lil_db_flush_buffer(void) {
	
	// Case: This is a new entry
	if (db_data.new_entry) {
		fprintf(db_data.output_filestream,
				"\n[%d]. ",		// Write entry number
				db_data.entry_number++  // And incremement
		       ) ;
		// Don't do this again unless the user requests it
		db_data.new_entry = 0 ;
	}
	
	// Write contents of buffer to output file
	if (fprintf(db_data.output_filestream, "%s", db_data.buff ) < 0 ) {
		// Case: There was an error writing to the file
		return LIL_DB_RETURN_FILE_WRITE_ERROR(db_data.output_filename) ;
	}	

	// Wipe buffer
	memset(db_data.buff,0,LIL_DB_DEFAULT_BUFFSZ) ;
	// It now contains 0 characters
	db_data.buff_length = 0 ;

	return LIL_DB_RETURN_SUCCESS ;
}

// Perform the actions of lil_db_enqueue and subsequently lil_db_flush
int lil_db_printf(char * format, ...) {
	int number_chars_not_printed ;
	va_list va_args ;

	// Wrapper for lil_db_enqueue()
	va_start(va_args, format) ;

	vfprintf(db_data.output_filestream,format,va_args) ;

	if ( (number_chars_not_printed = lil_db_enqueue(format, va_args)) ) {
		// Case: Not all characters were copied to buffer
		// TODO: print the rest
		// but honestly it's not essential as things stand
		// How do I start from where I left off?
		// I'll just pass on the return value for now
		return LIL_DB_RETURN_BUFFER_OVERFLOW_AVOIDED(
				number_chars_not_printed) ;
	}

	va_end(va_args) ;

	// Don't forget to flush
	lil_db_flush_buffer() ;

	return LIL_DB_RETURN_SUCCESS ;
}

// Ensure that the next write to the output file is prefixed by
// the current entry number + 1
int lil_db_new_entry(void) {
	db_data.new_entry = 1 ;
	return LIL_DB_RETURN_SUCCESS ;
}

// Perform the actions of lil_db_enqueue and subsequently lil_db_flush,
// but as a new entry
int lil_db_printf_entry(char * format, ...) {
	int number_chars_not_printed ;
	va_list va_args ;

	lil_db_new_entry() ;
	
	// Wrapper for lil_db_printf()
	va_start(va_args,format) ;
	if ( (number_chars_not_printed = lil_db_printf(format, va_args)) ) {
		return LIL_DB_RETURN_BUFFER_OVERFLOW_AVOIDED(
			number_chars_not_printed) ;
		// FIXME: This is not a good way to handle this afaik
	}
	va_end(va_args) ;

	return LIL_DB_RETURN_SUCCESS ;
}
