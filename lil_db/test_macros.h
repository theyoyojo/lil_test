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

#include <stdlib.h> // Literally just for exit(), NULL

/* Specification of documentation information */

 /*
  * Identifier:
  * 		DEFINED_IDENFIFIER(comma, seperated, params)
  * Purpose:
  *	
  * Inputs:
  * 		  comma : An explanation
  *
  * 	      saperated : Of each
  *
  * 	         params	: Paraaeter
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
  * 		TO_STRING(x)
  * Purpose:
  * 		Wrapper for # operator to improve readability
  *
  * Inputs:
  *	  	      x	: An arbitrary token
  *
  * Resolution:
  * 		A usable character string object contianing the text
  * 		of the identifier
  * 
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

/* SECTION: ASSERTIONS */

#define TEST_RETURN_FAIL 0 		/* Returned by failing assertion */
#define TEST_RETURN_PASS 1		/* Returned at end of test case control flow */

// TODO: Next level of abstraction
#define TEST_ASSERTION_TEMPLATE(action, boolean, predicate) NULL

#define TEST_CASE_PASS()

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
  * 		TEST_CHECK_SPACE(comma, seperated, params)
  * Purpose:
  *	
  * Inputs:
  * 		  comma : An explanation
  *
  * 	      separated : Of each
  *
  * 	         params	: Parameter
  *
  * Resolution:
  *		A description of the code generated after the macro
  *	       +is handled by the preprocessor
  */ 
#define TEST_CHECK_SPACE() \
	if (test_count_total >= test_buffspace) {\
	fprintf(stderr, \
		"Number of test cases cannot excceed value allocated" \
		" by TEST_INIT() (i.e. %d). Killing self...\n", \
		test_buffspace) ;\
		exit(1);}

#define TEST_CASE(name,...) TEST_CHECK_SPACE() ;\
	tests[test_count_total++] = \
		LAMBDA(int,(void) {\
			char * test_name = TO_STRING(test_##name) ; \
			(void)test_name ; \
			__VA_ARGS__ \
			return TEST_RETURN_PASS ; \
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
