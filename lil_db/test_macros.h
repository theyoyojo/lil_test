/*
 * lil_db_test_suite
 * By Joel Savitz <jsavitz@redhat.com>
 *
 * Purpose: Extremely lightweight unit testing
 *
 */
#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#include <stdlib.h> // Literally just for exit()

/* UTILITY MACROS */

 /* 
  * ID:
  * 		TO_STRING(x)
  * Purpose:
  * 		Wrapper for # operator to improve readability
  *
  * Inputs:
  * 		x: An arbitrary token
  *
  * Resolution:
  * 		A usable character string object contianing the text
  * 		of the identifier
  * 
  */
#define TO_STRING(identifier) #identifier

 /* 
  * ID:
  * 		LAMBDA(return_t, ...)
  * Purpose:
  * 		Simple implementation of anonymous function objects
  *
  * Inputs:
  * 		return_t: a valid function return type
  *
  * 		...	: The function body, which follows the syntax of
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
	 return_t function_identifier   /* declare function id return type  */ \
	 __VA_ARGS__			/* insert param list, function body */ \
	 function_identifier ;		/* expression evals to function ptr */ \
	 })				/* close block, eval to callable fn */	

/* PREDICATE PROCESSING MACROS */

#define TEST_FAILURE 0 		/* Returned by failing assertion */
#define TEST_SUCCESS 1		/* Returned at end of test case control flow */

#ifdef ASSERT			
#undef ASSERT			/* Ensure usage of our custom assertion */
#endif

 /*
  * ID:
  *		ASSERT(predicate)
  * Purpose:
  * 		Process an individual comparisson in a test case
  *
  * Inputs:
  * 		prediate: An arbitrary boolean logical predicate
  *
  * Resoltion:
  * 		
  */			
#define ASSERT(predicate) \
	if(!(predicate)) { \
	fprintf(stderr, "In test case: %s\n"\
		"Assertion: \"%s\" failed!\n",\
			test_name,\
			TO_STRING(predicate)) ;\
	return TEST_FAILURE ; }

/* CORE FUNCTIONALITY MACROS*/

#define TEST_CHECK_SPACE() \
	if (test_count_total >= test_buffspace) {\
	fprintf(stderr, \
		"Number of test cases cannot excceed value allocated" \
		" by TEST_INIT() (i.e. %d). Killing self...\n", test_buffspace) ;\
		exit(1);}

#define TEST_CASE(name,...) TEST_CHECK_SPACE() ;\
	tests[test_count_total++] = \
		LAMBDA(int,(void) {\
			char * test_name = TO_STRING(test_##name) ; \
			(void)test_name ; \
			__VA_ARGS__ \
			return TEST_SUCCESS ; \
		})

#define TEST_INIT(buffspace) int (*tests[buffspace])(void) ; \
	unsigned test_count_total = 0 ; \
	int test_buffspace = buffspace

#define TEST_EXEC() { unsigned test_count_passed = 0; \
	for (unsigned test_counter = 0; test_counter < test_count_total; ++test_counter) {\
		test_count_passed += tests[test_counter]() ;\
	}\
	printf("Passed %d/%d test cases.\n",test_count_passed,test_count_total) ;}

#endif // TEST_MACROS_H
