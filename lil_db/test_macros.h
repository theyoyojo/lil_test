/* SECTION: GENERAL INFORMATION */

/*
 * lil_db_test_suite
 * By Joel Savitz <jsavitz@redhat.com>
 *
 * Purpose: Extremely lightweight unit testing
 *
 */


#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#include <stdlib.h>

/* Specification of documentation information */

 /*
  * Identifier:
  * 		DEFINED_IDENFIFIER(comma, seperated, params)
  * Purpose:
  *	
  * Inputs:
  * 		  comma : An explanation of each paramater
  *
  * 	      separated : Sepateted by colon at column 25
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

/* SECTION: UTILITY MACROS */

 /* 
  * Identifier:
  * 		TO_STRING(identifier)
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
  * 		A callable anonymous function object
  */
#define LAMBDA(return_t,...)						       \
	__extension__	 		/* turn off GCC's pedantry 	    */ \
	({				/* begin block expression  	    */ \
	 return_t function_identifier   /* declare function id & return type*/ \
	 __VA_ARGS__			/* insert param list, function body */ \
	 function_identifier ;		/* expression evals to function ptr */ \
	 })				/* close block, eval to func object */	

/* SECTION: ASSERTION MACROS */

#define TEST_RETURN_FAIL 0 		/* Returned by passing test case    */
#define TEST_RETURN_PASS 1		/* Returned by passing test case    */

// TODO: Next level of abstraction
#define TEST_ASSERTION_TEMPLATE(action, boolean, predicate) NULL

// TODO
#define TEST_CASE_PASS()

// TODO
#define TEST_CASE_FAIL()

 /*
  * Identifier:
  *		TEST_FAIL_IF_FALSE(predicate)
  * Purpose:
  * 		Process an individual comparisson in a test case
  *
  * Inputs:
  *    	      predicate	: An arbitrary boolean logical predicate
  *
  * Resolution:
  * 		A contitional statement that tests the predicate, followed by
  * 	       +a block of code conditionally executed in the event of a
  * 	       +predicate wth a value of false. The block terminates with a
  * 	       +return of TEST_FAILIURE
  *
  * Requirements:
  * 		Must be run within the scope of a test case
  */			
#define TEST_FAIL_IF_FALSE(predicate) 						       \
	if(!(predicate)) { 		/* Branch on false predicate    	    */ \
	fprintf(stderr,		        /* Begin an error message	      	    */ \
		"Failure in test case %s:\n"                                           \
		"\tFALSE: \"%s\"\n",                                                   \
		test_name,      	/* Print the auto-declared test name string */ \
		TO_STRING(predicate)) ; /* Print predicate of failing assert        */ \
	return TEST_RETURN_FAIL ; } 	/* The test case has now failed             */ \

#define TEST_FAIL_IF_TRUE(predicate) 						       \
	if((predicate)) { 		/* Branch on true predicate       	    */ \
	fprintf(stderr,		        /* Begin an error message	      	    */ \
		"Failure in test case %s:\n"                                           \
		"\tTRUE: \"%s\"\n",                                                    \
		test_name,      	/* Print the auto-declared test name string */ \
		TO_STRING(predicate)) ; /* Print predicate of failing assertion     */ \
	return TEST_RETURN_FAIL ; } 	/* The test case has now failed             */ \

#define TEST_PASS_IF_FALSE(predicate) \
	if(!(predicate)) { 		/* Branch on false predicate    	    */ \
	fprintf(stdout,		        /* Begin an informational message      	    */ \
		"PASS: %s:\n" 							       \
		test_name,      	/* Print the auto-declared test name string */ \
	return TEST_RETURN_PASS  ; } 	/* The test case has now passed 	    */ \

#define TEST_PASS_IF_TRUE(predicate) \
	if((predicate)) { 		/* Branch on true predicate       	    */ \
	fprintf(stdout,		        /* Begin an informational message      	    */ \
		"PASS: %s:\n" 							       \
		test_name,      	/* Print the auto-declared test name string */ \
	return TEST_RETURN_PASS  ; } 	/* The test case has now passed 	    */ \

#define ASSERT(predicate) TEST_FAIL_IF_FALSE(predicate)

/* SECTION: CORE FUNCTIONALITY MACROS */

 /*
  * Identifier:
  * 		TEST_CHECK_SPACE()
  * Purpose:
  *		Exit with an error message if there is no more space allocated
  *	       +for aditional test cases.
  *
  * Resolution:
  *  		A block of code that prints a describtive error message and
  *  	       +exits the program, conditionally executed upon truth of
  *  	       +predicate comparing available space to used space using >=.
  */ 
#define TEST_CHECK_SPACE() \
	if (test_count_total >= test_buffspace) {\
	fprintf(stderr, \
		"Number of test cases cannot excceed value allocated" \
		" by TEST_INIT() (i.e. %d). Killing self...\n", \
		test_buffspace) ;\
		exit(1);}

 /*
  * Identifier:
  * 		TEST_CASE(name,...)
  * Purpose:
  *		Define a test case
  * Inputs:
  * 		   name : A descriptive name of the test case
  *
  *    __VA_ARGS_ / ...	: A sequence of zero or more valid executable
  *    			 +statements
  *
  * Resolution:
  * 		A function named test_name is declared and defined and a pointer
  * 	       +to the function is saved in the array of existing tests.
  *
  * Requirements:
  *  		TEST_INIT() must be called earilier in control flow
  */ 
#define TEST_CASE(name,...) TEST_CHECK_SPACE() ;\
	tests[test_count_total++] = \
		LAMBDA(int,(void) {\
			char * test_name = TO_STRING(test_##name) ; \
			(void)test_name ; \
			__VA_ARGS__ \
			return TEST_RETURN_PASS ; \
		})

 /*
  * Identifier:
  * 		TEST_INIT(buffspace)
  * Purpose:
  *		Initialize the testing framework and allocate required memory
  * Inputs:
  * 	      buffspace : The number of test cases to allocate space for
  *
  * Resolution:
  * 		A variable to count the total number of test cases is
  * 	       +initialized to 0. Space is allocated for buffspace number
  * 	       +of test cases.
  */
#define TEST_INIT(buffspace) int (*tests[buffspace])(void) ; \
	unsigned test_count_total = 0 ; \
	int test_buffspace = buffspace

 /*
  * Identifier:
  * 		TEST_EXEC()
  * Purpose:
  *		Execute all tests declared earlier in control flow	
  *
  * Resolution:
  *	        A for loop iterates through all declared test cases. A passing
  *	       +test returns an integer value of 1 and a failing test returns
  *	       +an integer value of 0, so the value returned by the test case
  *	       +is added to a count of all tests passed. Following the loop,
  *	       +the ratio of passed to total test cases is reported.
  *
  * Requirements:
  *  		TEST_INIT() must be called earilier in control flow
  */ 
#define TEST_EXEC() { unsigned test_count_passed = 0; \
	for (unsigned test_counter = 0; test_counter < test_count_total; ++test_counter) {\
		test_count_passed += tests[test_counter]() ;\
	}\
	fprintf(stdout, \
		"Passed %d/%d test cases.\n",test_count_passed,test_count_total) ;}

#endif // TEST_MACROS_H
