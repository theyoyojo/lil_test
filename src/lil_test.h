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

/* SECTION: GENERAL INFORMATION */
		
/*
 * File description:
 * 		test_macros.h GNUC metasource header file
 *
 * Author:
 * 		Joel Savitz <jsavitz@redhat.com>
 *
 * Purpose:
 * 		Extremely lightweight testing framework for GNU C
 *
 * Note: 	
 * 		Any //-style comments are intended as temporary notes and TODOs
 * 	       +to eventually resolve and remove. Each section is intended to be
 * 	       +a layer of abstraction above the previous. 
 *
 * Architecture (very brief synopsis):
 * 		Test sets are defined in global scope and automatically
 * 	       +executed before main. Test cases are defined within test set
 * 	       +scope. A function is generated for each test set that executes
 * 	       +all test cases. The generated function has the following
 * 	       +phases:
 * 	       	
 * 	   Construction : The function defines all data needed to execute the
 * 	   		 +test set and allocates and initalizes all required
 * 	   		 +memory and defauly values. A pointer to the test set
 * 	   		 +data identified by "this" is initialized.
 * 	   
 * 	     Definition : All statements in the body of the test set are
 * 	      		 +executed. This section may include zero or more valid
 * 	      		 +statements and test cases. Statments within test
 * 	      		 +cases are NOT executed during this phase, but are
 * 	      		 +saved to their appropriate test case for the next 
 * 	      		 +stage. Notably, test cases will capture test set
 * 	      		 +scope by reference and not by value. Keep in mind,
 * 	      		 +however, that all variables will reflect the state
 * 	      		 +of memory subsequent to the execution of all
 * 	      		 +statements in the test set, regardless of whether the
 * 	      		 +statements proceed or follow the definition of a
 * 	      		 +test case in control flow.
 *
 *  	      Execution : All test cases defined in the test set are executed.
 *  	      		 +Passing and failing tests are reported. Subsequently,
 *  	      		 +the the name of the test set and the ratio of passed
 *  	      		 +tests to total tests is reported.
 *
 *  	    Destruction : All resouces allocated for the test set are free'd.
 *
 *		The macros are very roughly sorted in an order of escalating
 *	       +abstraction. That is no guarentee, but it may be useful to
 *	       +know for the purposes of understanding this file.
 */

#ifndef TEST_MACROS_H
#define TEST_MACROS_H

/* Dependencies */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: configuration option header?
// TODO: move all non-documentation inline commentary to GitHub issues
// TODO: remove procps and rename repo
// TODO: a license maybe
// TODO: better examples
// TODO: README
// TODO: output stream config, output format config
// TODO: identify potential config options

/* GNU C is REQUIRED for usage of these macros */
#ifndef __GNUC__
#error "__GNUC__ not defined"
#else

/* Output display options */
#define TEST_OPTION_VERBOSE 		 /* Show all passes */

// FIXME: what should the default cpp state be for this option?
//#define TEST_OPTION_TAP

/* Specification of documentation information */

 /*
  * Identifier:
  * 		DEFINED_IDENFIFIER(comma, separated, params)
  *
  * Purpose:
  *		Explanation of why this macro is not bloat
  *
  * Inputs:
  * 		  comma : An explanation of each paramater
  *
  * 	      separated : Separated by colon at column 25
  *
  * 	         params	: May be excluded if otherwise empty
  *
  * Resolution:
  *		A description of the code generated after the macro
  *	       +is handled by the preprocessor
  *
  * Requirements:
  * 		Any prerequisites to the processing of this macro, if any.
  * 		This section may be excluded if it would otherwise be empty
  */

/* SECTION: UTILITIES */

 /* 
  * Identifier:
  * 		TO_STRING(identifier)
  *
  * Purpose:
  * 		Wrapper for # operator to improve readability
  *
  * Inputs:
  *	     identifier : An arbitrary token
  *
  * Resolution:
  * 		A usable character string object contianing the text
  * 		of the identifier
  */
#define TO_STRING(identifier) #identifier

 /* 
  * Identifier:
  * 		LAMBDA(return_t, ...)
  *
  * Purpose:
  * 		Simple implementation of anonymous function objects
  *
  * Inputs:
  * 	       return_t	: a valid function return type
  *
  *    __VA_ARGS_ / ...	: The function body, which follows the syntax of
  * 			 +any arbitrary function definition, begining with
  * 			 +the tokens that immediately follow the function
  * 			 +identifier.
  * 			 + e.g:
  * 			 	(parameter_t parameter_id)
  * 			 	{
  * 			 		arbitary ;
  * 			 		statements ;
  * 			 		return value ;
  * 			 	}
  *
  * Resolution:
  * 		A callable anonymous function object. The function captures
  * 	       +it's enclosing scope at definition, but NOT at execution.
  */
#define LAMBDA(return_t,...)						       \
									       \
	__extension__	 	       /* turn off GCC's pedantry 	    */ \
	({			       /* begin block expression  	    */ \
	 return_t function_identifier  /* declare function id & return type */ \
	 __VA_ARGS__		       /* insert param list, function body  */ \
	 function_identifier ;	       /* expression evals to function ptr  */ \
	})			       /* close block, eval to func object  */ \
									       \
/* end #define LAMBDA							    */

 /*
  * Identifier:
  * 		TEST_ERROR_ALLOC_FAIL(bytes)
  *
  * Purpose:
  * 		Report an allocation error and exit the program.
  *
  * Inputs:
  * 	          bytes	: The size of the memory requested by the failed alloc
  *
  * Resolution:
  *	       	An error message is printed to stderr and the program exits.
  *
  * Requirements:
  *  		No other code needs to be executed after this point
  */
#define TEST_ERROR_ALLOC_FAIL(bytes) 					       \
									       \
	fprintf(stderr,			   	/* Begin an error message */   \
		"Reallocation of"					       \
		"%lu bytes failed. "			       		       \
		"Killing self...\n",					       \
		bytes				/* Got bytes? Not anymore */   \
	) ;								       \
	exit(1)       		     /* Storm out of the program in disgust */ \
									       \
/* end #define TEST_ERROR_ALLOC_FAIL					    */

 /*
  * Identifier:
  * 		REALLOCATE_OR_DIE(non_void_ptr, count)
  *
  * Purpose:
  *		Reallocate any memory held by the non_void_ptr to hold count
  *	       +number of items of it's type, or die trying (quit w/ error).
  * Inputs:
  *
  * 	   non_void_ptr : A non-void pointer to memory to be reallocated
  *
  * 	          count : The number of items (of the type the non_void_ptr
  * 	          	 +references) for which space will be allocated
  *
  * Resolution:
  * 		A lambda function is embeded in the current scope and executed
  * 	       +following it's definition. The lambda creates a temporary
  * 	       +void ptr to hold the result of a call to realloc() that targets
  * 	       +the memory at the address of non_void_ptr. If the reallocation
  * 	       +is successful, the new address is saved as the value of
  * 	       +non_void_ptr. If the reallocation fails, an error message is
  * 	       +printed to stderr and the program terminates itself.
  *
  * Requirements:
  *  		non_void_ptr MUST not be void, obviously. Type deduction is
  *  	       +done by dereferencing this parameter and passing it to typeof
  *  	       +and a void pointer must not be dereferenced. I suppose that
  *  	       +one would benefit from having free memory on their machine
  *  	       +as well if they don't like memory allocation errors.
  */
#define REALLOCATE_OR_DIE(non_void_ptr,count)				       \
									       \
	LAMBDA(void,(void) {		      /* We begin our simple lambda */ \
		void * temp = NULL;	      /* Initialize a temp void ptr */ \
		size_t new_size_bytes =       /* Calc the size to alloc     */ \
			sizeof(		      /* We use the sizeof keyword  */ \
			typeof(*non_void_ptr) /* On the type pointed at     */ \
			) * count ;	      /* And multiply by our count  */ \
		if (!(temp =		      /* Branch on NULL in temp     */ \
			realloc(	      /* Returns NULL on failure    */ \
				non_void_ptr, /* References our memory      */ \
				new_size_bytes/* We want this much space    */ \
		))) { TEST_ERROR_ALLOC_FAIL(  /* Report the error and exit  */ \
			new_size_bytes) ; }   /* This value is reported too */ \
		non_void_ptr = temp ;	      /* On success, save address   */ \
	})() ;				      /* Execute all the above      */ \
									       \
/* end #define REALLOCATE_OR_DIE					    */

 /*
  * Identifier:
  * 		TEST_MAIN()
  *
  * Purpose:
  *		Generate a stub main().
  *
  * Resolution:
  *		An main function definition that takes no parameters and does
  *	       +nothing but return 0.
  *
  * Requirements:
  *  		main() must not be defined elsewhere in the program
  */
#define TEST_MAIN() int main(void) { return 0 ; }

 /*
  * Identifier:
  * 		TEST_PRINTF()
  *
  * Purpose:
  * 		printf appropriate output for test consumers
  *
  * Resolution:
  * 		Just printf but the output format is modified if necessary
  *
  * Requirements:
  * 		No newlines in the middle if you are using TAP unless you
  * 	       +manually add the '#'s
  */
#ifdef TEST_OPTION_TAP
#define TEST_PRINTF(spec, ...) printf( "# " spec ,##__VA_ARGS__) ;
#else
#define TEST_PRINTF(spec, ...) printf(spec ,##__VA_ARGS__) ;
#endif /* #ifdef TEST_OPTION_TAP */

/* SECTION: ASSERTIONS */

#define TEST_RETURN_FAIL 0 		/* Returned by failing test case    */
#define TEST_RETURN_PASS 1		/* Returned by passing test case    */


 /*
  * Identifier:
  * 		TEST_CASE_REPORT_PASS()
  *
  * Purpose:
  *		Report appropriate information to the user when a test case
  *	       +passes
  *
  * Resolution:
  * 		ifndef OPTION_TAP, print informational message if
  * 	       +OPTION_VERBOSE is defined, else produce
  *
  * Requirements:
  * 		Must be run in the context of a passing test case
  */
#ifdef TEST_OPTION_TAP
#define TEST_CASE_REPORT_PASS()						       \
	fprintf(stdout,							       \
		"ok %zd - %s\n",					       \
		case_id + 1,						       \
		this->case_names[case_id]) ; /* case name used as directive */ \
									       \
// TODO print diagnostic messages as well
/* end #define TEST_CASE_REPORT_PASS */
#else
#define TEST_CASE_REPORT_PASS()						       \
	fprintf(stdout,		        /* Begin an informational message   */ \
		"PASS %s\n\n",	        /* Good news			    */ \
		this->case_names[case_id]) ;   	     /* Print case name     */ \
									       \
/* end #define TEST_CASE_REPORT_PASS */
#endif /* #ifdef TEST_OPTION_TAP */

 /*
  * Identifier:
  * 		TEST_CASE_PASS()
  *
  * Purpose:
  *		Pass a test case. Report to user unless asked not to
  *
  * Resolution:
  *	        Print a message to the standard output stream if the
  *	       +VERBOSE option is set. Return the PASS code and exit
  *	       +test case context.
  *
  * Requirements:
  * 		Must be run within the scope of a test case.
  */
#if defined(TEST_OPTION_VERBOSE) || defined(TEST_OPTION_TAP)
/* TEST_CASE_PASS cannot be silent if either macro is defined */
#define TEST_CASE_PASS()						       \
	TEST_CASE_REPORT_PASS()	;	/* Report success appropriately     */ \
	return TEST_RETURN_PASS  ; 	/* The test case has now passed	    */ \
									       \
/* end #define TEST_CASE_PASS						    */
#else
#define TEST_CASE_PASS() return TEST_RETURN_PASS ; /* Test passed           */
#endif /* #if defined(TEST_OPTION_VERBOSE) || defined(TEST_OPTION_TAP) */

 /*
  * Identifier:
  * 		TEST_CASE_REPORT_FAIL(why_string)
  *
  * Purpose:
  *		Report appropriate test failure information to user.
  *		
  * Inputs:
  *          why_string : an optional message to display to the standard output
  *         		 +stream
  *
  * Resolution:
  * 		Print a message to the user about the failing test
  *
  * Requirements:
  * 		Must be run within the scope of a failing test case
  */
#ifdef TEST_OPTION_TAP
#define TEST_CASE_REPORT_FAIL(why_string)				       \
	fprintf(stdout,							       \
		"not ok %zd - %s\n" 					       \
		"# %s\n",						       \
		case_id,						       \
		this->case_names[case_id],   /* case name used as directive */ \
		why_string) ;						       \
									       \
/* end #define TEST_CASE_REPORT_FAIL */
#else
#define TEST_CASE_REPORT_FAIL(why_string)				       \
	fprintf(stdout,		        /* Begin an informational message   */ \
		"FAIL %s:\n"            /* Bad news                         */ \
		"\t%s\n",               /* Indented why_string              */ \
		this->case_names[case_id],    /* Print case name            */ \
		why_string) ;  	        /* Print failure defailt            */ \
									       \
/* end #define TEST_CASE_REPORT_FAIL */
#endif /* #ifdef TEST_OPTION_TAP */

 /*
  * Identifier:
  * 		TEST_CASE_FAIL(why_string)
  *
  * Purpose:
  *		Fail a test case. Report to user unless the option is set.
  *		
  * Inputs:
  *          why_string : an optional message to display to the standard output
  *         		 +stream
  *
  * Resolution:
  * 		Print a message to the user about the failing test and exit
  * 	       +from current test case context
  *
  * Requirements:
  * 		Must be run within the scope of a test case.
  */
#define TEST_CASE_FAIL(why_string)					       \
	TEST_CASE_REPORT_FAIL(why_string) ;				       \
	return TEST_RETURN_FAIL ; 	/* The test case has now failed     */ \
									       \
/* end #define TEST_CASE_FAIL						    */

 /*
  * Identifier:
  *		TEST_CASE_FAIL_IF_FALSE(predicate)
  *
  * Purpose:
  * 		Fail a test case if a predicate is false
  *
  * Inputs:
  *    	      predicate	: An arbitrary boolean logical predicate
  *
  * Resolution:
  * 		A contitional statement that tests the predicate. If it is 
  * 	       +false the test fails and the false predicate is reported
  *
  * Requirements:
  * 		Must be run within the scope of a test case
  */
#define TEST_CASE_FAIL_IF_FALSE(predicate) 			               \
									       \
	if(!(predicate))						       \
	{ TEST_CASE_FAIL("FALSE: \"" TO_STRING(predicate) "\"") ; }	       \
									       \
/* end #define TEST_CASE_FAIL_IF_FALSE					    */

 /*
  * Identifier:
  *		TEST_CASE_FAIL_IF_TRUE(predicate)
  *
  * Purpose:
  * 		Fail a test case if a predicate is true
  *
  * Inputs:
  *    	      predicate	: An arbitrary boolean logical predicate
  *
  * Resolution:
  * 		A contitional statement that tests the predicate. If it is true
  * 	       +the test fails and the true predicate is reported
  *
  * Requirements:
  * 		Must be run within the scope of a test case
  */
#define TEST_CASE_FAIL_IF_TRUE(predicate)			               \
									       \
	if((predicate))						               \
	{ TEST_CASE_FAIL("TRUE: \"" TO_STRING(predicate) "\"") }	       \
									       \
/* end #define TEST_CASE_FAIL_IF_TRUE					    */

 /*
  * Identifier:
  * 		TEST_CASE_PASS_IF_FALSE(predicate)
  *
  * Purpose:
  * 		Pass a test case if a predicate is false
  *
  * Inputs:
  *    	      predicate	: An arbitrary boolean logical predicate
  *
  * Resolution:
  * 		A contitional statement that tests the predicate. If it is  true
  * 	       +the test passes.
  *
  * Requirements:
  * 		Must be run within the scope of a test case
  */
#define TEST_CASE_PASS_IF_FALSE(predicate) if(!(predicate)) TEST_CASE_PASS() ;

 /*
  * Identifier:
  *		TEST_CASE_PASS_IF_TRUE(predicate)
  *
  * Purpose:
  * 		Pass a test case if a predicate is false
  *
  * Inputs:
  *    	      predicate	: An arbitrary boolean logical predicate
  *
  * Resolution:
  * 		A contitional statement that tests the predicate. If it is  true
  * 	       +the test passes.
  *
  * Requirements:
  * 		Must be run within the scope of a test case
  */
#define TEST_CASE_PASS_IF_TRUE(predicate) if(predicate) TEST_CASE_PASS() ;

// TODO: cleanup on isle 395 FIXME this number changes
// ASSERT is an alias for TEST_CASE_FAIL_IF_FALSE (for now)

#define ASSERT(predicate) TEST_CASE_FAIL_IF_FALSE(predicate)

#define ASSERT_FALSE(predicate) TEST_CASE_FAIL_IF_FALSE(predicate)

#define ASSERT_TRUE(predicate) TEST_CASE_FAIL_IF_TRUE(predicate)

/* SECTION: TEST CASE GENERATION */

// TODO: move to configuration
#define TEST_DEFAULT_CASE_BUFFSIZE 100 /* Initial case_capacity (arbitrary)  */
#define TEST_DEFAULT_RESIZE_FACTOR 1.3 /* Ratio for capacity growth          */
			
 /*
  * Identifier:
  * 		TEST_CHECK_SPACE()
  *
  * Purpose:
  * 		Check if enough memory is allocated for an aditional test case
  * 	       +in the current test set. If more space is needed, allocate it.
  *
  * Resolution:
  *  	        A conditional statement that tests if the dynamically allocated
  *  	       +memory in the current test set's test_set_data struct is filled. In
  *  	       +the case of a true result, additional memory is allocated. If
  *  	       +the allocation fails, the program quits with an error message.
  */
#define TEST_CHECK_SPACE() 						       \
									       \
	if (this->case_count_total >= 	/* If count of test cases defined   */ \
		this->case_capacity) {  /* Is at/above allocated capacity   */ \
		this->case_capacity *=  /* Increase the capacity value 	    */ \
			TEST_DEFAULT_RESIZE_FACTOR ; /* By default factor   */ \
									       \
		REALLOCATE_OR_DIE(	/* First reallocate test case space */ \
			this->cases,        /* Located at this address 	    */ \
			this->case_capacity /* Using out new capacity value */ \
		) ;							       \
									       \
		REALLOCATE_OR_DIE(	/* Next reallocated case name space */ \
			this->case_names,   /* Located at this address 	    */ \
			this->case_capacity /* Using out new capacity value */ \
		) ;							       \
	} /* end if() */						       \
									       \
/* end #define TEST_CHECK_SPACE */
		
 /*
  * Identifier:
  * 		TEST_CASE(name,...)
  *
  * Purpose:
  *		Define a test case, the core unit of this system
  *
  * Inputs:
  * 		   name : A descriptive name of the test case
  *
  *    __VA_ARGS_ / ...	: A sequence of zero or more valid executable
  *    			 +statements
  *
  * Resolution:
  * 		A function named test_name is declared and defined and a pointer
  * 	       +to the function is saved to the current test set.
  *
  * Requirements:
  *  	        A test case must be defined directly within the scope of a test
  *  	       +set.
  */
#define TEST_CASE(name,...)						       \
									       \
	TEST_CHECK_SPACE() ;	/* Guarentee sufficent space for new tests  */ \
	this->cases[this->case_count_total++] = /* Add a new test case	    */ \
		LAMBDA(int,(size_t case_id) 	/* Defined using our lambda */ \
		{							       \
			char case_name[] =	/* Generate our case_name   */ \
				TO_STRING(test_##name) ; /* Using the cpp   */ \
									       \
			this->case_names[case_id] = NULL ;		       \
			REALLOCATE_OR_DIE(      /* Guarentee reallocation   */ \
				this->case_names[case_id], /* For case_name */ \
				sizeof(case_name)  	   /* Of this size  */ \
			) ;						       \
									       \
			 /* Safe strcpy(): buffer space and 		    */ \
			 /* \0-termination guarenteed above  		    */ \
			strcpy( 					       \
				this->case_names[case_id], /* alloc'd above */ \
				case_name 		   /* cpp generated */ \
			) ;						       \
									       \
			__VA_ARGS__ 	 /* Test case body: assertions, etc */ \
									       \
			TEST_CASE_PASS() ; /* If this runs, the test passes */ \
		});							       \
									       \
/* end #define TEST_CASE 				                    */

/* SECTION: TEST SET GENERATION */

 /*
  * Identifier:
  * 		TEST_SET_CONSTRUCTOR(name)
  *
  * Purpose:
  * 		Allocate memory to support a set of test cases
  *
  * Inputs:
  * 	           name : A descriptive name for the test set
  *
  * Resolution:
  * 		A test_set_data_t is allocated and it's fields are assigned
  * 	       +sensible defualt values. Some of these fields are themselves
  * 	       +dynamically allocated. Any allocation failure is fatal to
  * 	       +the entire program and is also kind of sad.
  *
  * Requirements:
  * 		Usage of this macro only really makes sense in the context
  * 	       +the definition of the TEST_SET macro, but I suppose there
  * 	       +isn't any reason why one could not use this anywhere they
  * 	       +would like to, provided that no identifiers are redeclared.
  */
#define TEST_SET_CONSTRUCTOR(name)					       \
									       \
	/* ALLOCATION OF TEST SET DATA STRUCTURE */			       \
	test_set_data_t * this = NULL ; /* Initialize pointer to set data   */ \
	REALLOCATE_OR_DIE(this,1) ;	/* Allocate space for data or die   */ \
									       \
	/* ASIGNMENT OF SENSIBLE DEFAULT VALUES				    */ \
	*this = (const test_set_data_t) /* Cast & assign to allocated space */ \
	{				/* begin compound literal           */ \
		NULL,			/* void (*cases)(size_t case_id)    */ \
		NULL,			/* char ** case_names		    */ \
		NULL,			/* char  * set_name		    */ \
		0,			/* size_t case_count_capacity       */ \
		0,			/* size_t case_count_total          */ \
		0,			/* size_t case_count passed         */ \
		sizeof(TO_STRING(name)) /* size_t set_name_length           */ \
	} ;				/* end compound literal             */ \
	/* From this point onward, this data is accessed via the this ptr   */ \
									       \
	/* DYNAMIC ALLOCATION OF TEST CASE SPACE */	       		       \
	this->case_capacity = 		/* Initally the capacity should be  */ \
		TEST_DEFAULT_CASE_BUFFSIZE ; /*+the default value           */ \
									       \
	REALLOCATE_OR_DIE(		/* Allocation failure is fatal here */ \
		this->cases,		/* Allocate memory for this ptr     */ \
		this->case_capacity ) ; /* To hold this many of it's type   */ \
									       \
	REALLOCATE_OR_DIE(		/* Same as previous 		    */ \
		this->case_names,       /* But for test name strings        */ \
		this->case_capacity ) ; /* Which should have same capacity  */ \
									       \
	memset(  /* Powerwash our array of (char*)s to clear garbage values */ \
		this->case_names,       /* Located at this address          */ \
		0,			/* (int)0 == NULL essentially       */ \
		this->case_capacity     /* Hold this many nothings          */ \
	) ; /* Note: without this call to memset(), realloc() makes a       */ \
	    /* a conditional jump that depends on an uninitialized value    */ \
									       \
	/* DYNAMIC ALLOCATION OF TEST SET NAME			            */ \
	REALLOCATE_OR_DIE(		/* Last but not least...	    */ \
		this->set_name,         /* Space for the set name string    */ \
		this->set_name_size ) ; /* Size in bytes determined above   */ \
									       \
	strcpy(  /* Safe: buffer space and \0-termination guarenteed above  */ \
		this->set_name, 	/* dest: the newly allocated buffer */ \
		TO_STRING(name) ) ;     /* src : const char[] from cpp      */ \
									       \
/* end #define TEST_SET_CONSTRUCTOR 				            */

 /*
  * Identifier:
  * 		TEST_SET_REPORT_RESULTS()
  *
  * Purpose:
  *		Report information about the test set to be executed
  *
  * Resolution:
  * 		A short header is printed. For TAP, this is the "plan" line
  *
  * Requirements:
  * 		Must be called in the context of a TEST_SET before any
  * 	       +other mechanisms that print to the output stream.
  */
#ifdef TEST_OPTION_TAP
#define TEST_SET_REPORT_PLAN()						       \
	fprintf(stdout,							       \
		"1..%zd\n",			  			       \
			this->case_count_total) ;			       \
									       \
/* end #define TEST_SET_REPORT_PLAN */
#else
#define TEST_SET_REPORT_PLAN()						       \
	fprintf(stdout,							       \
		"\nBEGIN TEST_SET: %s (%zd cases)\n",  			       \
			this->set_name,					       \
			this->case_count_total) ;			       \
									       \
/* end #define TEST_SET_REPORT_PLAN */
#endif /* #ifdef TEST_OPTION_TAP */

 /*
  * Identifier:
  * 		TEST_SET_REPORT_RESULTS()
  *
  * Purpose:
  *		Report the results of a test set to stdout
  *
  * Resolution:
  * 		Information about the results of the test set is printed
  * 	       +unless OPTION_TAP is defined.
  *
  * Requirements:
  * 		Must be called in the context of a TEST_SET and after all
  * 	       +have finished executing.
  */
#ifdef TEST_OPTION_TAP
#define TEST_SET_REPORT_RESULTS()					       \
	(void)0 ; /* TAP output will be summarized by the consumer */
#else									       
#define TEST_SET_REPORT_RESULTS()					       \
	fprintf(stdout,	/* When all test cases in the set have been run,    */ \
		"\nFINISHED TEST_SET: %s\n"   /* Report which set was run,  */ \
		"\tPassed %lu/%lu test cases.\n\n", /* and the pass ratio   */ \
			this->set_name,				               \
			this->case_count_passed,			       \
			this->case_count_total ) ;  			       \
/* end #define TEST_SET_REPORT_RESULTS					    */
#endif /* #ifdef TEST_OPTION_TAP */

 /*
  * Identifier:
  * 		TEST_SET_EXECUTOR()
  *
  * Purpose:
  *		Execute all test cases defined in a test set.
  *
  * Resolution:
  * 		All defined test cases in the cases function pointer array
  * 	       +are executed and the pass/total ratio is reported.
  *
  * Requirements:
  *  		TEST_SET_CONSTRUCTOR must be called earlier in scope.
  */
#define TEST_SET_EXECUTOR()						       \
	/* REPORT PLAN */						       \
	TEST_SET_REPORT_PLAN() ;					       \
									       \
	/* EXECUTION */							       \
	for (size_t i = 0;				/* Iterate through  */ \
		i < this->case_count_total; ++i) {      /* All test cases   */ \
		this->case_count_passed += 		/* Count passes     */ \
			this->cases[i](i) ;	/* By executing test cases  */ \
	}		/* A test case return 1 for pass and 0 for failure  */ \
									       \
	/* REPORT RESULTS */						       \
	TEST_SET_REPORT_RESULTS() ; 					       \
									       \
/* end #define TEST_SET_EXECUTOR 				            */

 /*
  * Identifier:
  * 		TEST_SET_DESTRUCTOR()
  *
  * Purpose:
  *		Free any dynamically allocated memory associated with a test
  *	       +set.
  *
  * Resolution:
  * 		All dynamically allocated fields in the test_set_data_t at
  * 	       +address "this" are freed, as well as the test_set_data_t
  * 	       +itself.
  *
  * Requirements:
  *  		TEST_SET_CONSTRUCTOR was called earlier in scope, and
  *  		TEST SET_DESTRUCTOR has not already been called since that
  *  	       +point. Like the constructor, it doesn't really make sense
  *  	       +to use this outside of the TEST_SET macro definition. Because
  *  	       +of that, I don't bother to check if any of these fields are
  *  	       +NULL, because the program will terminate on failed allocation
  *  	       +when used as intended. But hey, it's open source, do what you
  *  	       +want.
  */
#define TEST_SET_DESTRUCTOR() 						       \
									       \
	free(this->cases) ;			 /* Free function ptr array */ \
	for(size_t i = 0;			 /* Iterate through the     */ \
		i < this->case_count_total; ++i) /* Array of (char *)s      */ \
		{ free(this->case_names[i]) ; }  /* And free all of them    */ \
	free(this->case_names) ;		 /* Then free the (char**)  */ \
	free(this->set_name) ;			 /* Free the last (char*)   */ \
	free(this) ; 				 /* Lastly, free yourself   */ \
						 /* I'M FREE AT LAST	    */ \
/* end #define TEST_SET_DESTRUCTOR					    */

 /*
  * Identifier:
  * 		TEST_SET(name,...)
  *
  * Purpose:
  * 		Define a test set to be executed before main()
  *
  * Inputs:
  * 		   name : A descriptive name of the test set
  *
  *    __VA_ARGS_ / ...	: A sequence of zero or more valid executable
  *    			 +statements, including zero or more test case
  *    			 +definitions
  *
  * Resolution:
  * 		A function is declared with the constructor attribute so that
  *	       +it will run before main(). The function is then imediately
  *	       +defined with a function body that constructs the required data
  *	       +for a test set, executes an arbitary number of statements,
  *	       +optionally including test case definitions, executes any
  *	       +defined test cases, and frees all allocated memory.
  *
  * Requirements:
  *  		The inclusion of this header file.
  */
#define TEST_SET(name,...)\
									       \
	void test_set_##name (void) 	       /* Declare a function to be  */ \
		__attribute__((constructor)) ; /* autoexec'd before main()  */ \
									       \
	void test_set_##name (void) {          /* And immediately define it */ \
		TEST_SET_CONSTRUCTOR(name) ;         /* Phase: Construction */ \
		__VA_ARGS__ ;                        /* Phase: Definition   */ \
		TEST_SET_EXECUTOR() ;                /* Phase: Execution    */ \
		TEST_SET_DESTRUCTOR() ; }	     /* Phase: Destruction  */ \
									       \
/* end #define TEST_SET							    */

/* This is the decleration of the data format used to support a test set    */
typedef struct test_set_data {
	/* Test cases in the set. Takes case_id as a parameter in order to  */
	/* to index the case_names array via a ptr to this struct           */
	int (** cases)(size_t case_id) ;

	/* Name strings for each test case 				    */
	char ** case_names,

	/* The name of this test set 				            */
	      * set_name ;

	/* The capacity of the space allocated for case func ptrs           */
	size_t  case_capacity,

	/* The number of defined test cases reachable via the array 	    */
		case_count_total,

	/* The number of passed test cases determined via execution 	    */
		case_count_passed,

	/* The number of bytes in the set_name string, including \0  	    */
		set_name_size ;
} test_set_data_t ;

#endif /* ifndef __GNUC__ */

#endif /* ifndef TEST_MACROS_H */
