/* SECTION: GENERAL INFORMATION */

/*
 * test_macros.h metasource file
 * By Joel Savitz <jsavitz@redhat.com>
 *
 * Purpose: Extremely lightweight testing suite
 */

#ifndef TEST_MACROS_H
#define TEST_MACROS_H

/* Dependencies */

#include <stdio.h>
#include <stdlib.h>

/* GNU C is REQUIRED for usage of these macros */
#ifndef __GNUC__
#error "__GNUC__ not defined"
#else

/* Output display options */
#define TEST_OPTION_VERBOSE 		 /* Show all passes */
#undef TEST_OPTION_SUPPRESS_FAILURE	 /* Show all fails  */

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

// TODO: Documentation
#define TEST_SUITE_ERROR_ALLOC_FAIL() \
	exit(1) // STUB?

// TODO: Documentation
#define REALLOCATE_OR_DIE(non_void_ptr,count)\
	LAMBDA(void,(void) {\
		void * temp = NULL;\
		if (!(temp = realloc(non_void_ptr,\
			sizeof(typeof(*non_void_ptr)) * count))) {\
			fprintf(stderr,				\
			"Reallocation of %lu bytes failed. "	\
			"Killing self...\n",			\
			sizeof(typeof(non_void_ptr))) ;		\
		TEST_SUITE_ERROR_ALLOC_FAIL() ; }		\
		non_void_ptr = temp ;				\
	})() ; // Execute
	
	/* void * temp_##non_void_ptr =  \ */
	/* realloc(non_void_ptr,sizeof(typeof(*non_void_ptr)) * count) ;\ */
	/* if (!temp) { fprintf(stderr,				\ */
	/* 		"Reallocation of %lu bytes failed. "	\ */
	/* 		"Killing self...\n",			\ */
	/* 		count,					\ */
	/* 		sizeof(typeof(non_void_ptr))) ;		\ */
	/* 	TEST_SUITE_ERROR_ALLOC_FAIL() ;			\ */
	/* }							\ */
	/* non_void_ptr = (typeof(non_void_ptr))temp ; */

// TODO: documentation
#define TEST_MAIN() int main(void) {return 0;}

/* SECTION: ASSERTION MACROS */

#define TEST_RETURN_FAIL 0 		/* Returned by failing test case    */
#define TEST_RETURN_PASS 1		/* Returned by passing test case    */

 /*
  * Identifier:
  * 		TEST_CASE_PASS()
  *
  * Purpose:
  *		Pass a test case. Report to user if the option is set.
  *
  * Resolution:
  *	        Print a message to the standard output stream if the
  *	        VERBOSE option is set. Return the PASS code.
  *
  * Requirements:
  * 		Must be run within the scope of a test case.
  */ 
#ifdef TEST_OPTION_VERBOSE
#define TEST_CASE_PASS()						       \
	fprintf(stdout,		        /* Begin an informational message   */ \
		"PASS %s\n\n",	        /* Good news			    */ \
		test_name) ;      	/* Print test case function name    */ \
	return TEST_RETURN_PASS  ; 	/* The test case has now passed	    */
#else
#define TEST_CASE_PASS()						       \
	return TEST_RETURN_PASS  ; 	/* The test case has now passed	    */
#endif // ifdef TEST_OPTION_VERBOSE

 /*
  * Identifier:
  * 		TEST_CASE_FAIL(why_string)
  *
  * Purpose:
  *		Fail a test case. Report to user unless the option is set.
  *		
  * Inputs:
  *          why_string : an optional message to display to the standard error
  *         		 +stream
  *
  * Resolution:
  * 		Print a message to the user unless the SUPPRESS_FAILURE option
  * 	       +is set.
  *
  * Requirements:
  * 		Must be run within the scope of a test case.
  */
#ifndef TEST_OPTION_SUPPRESS_FAILURE
#define TEST_CASE_FAIL(why_string)					       \
	fprintf(stderr,		        /* Begin an error message	    */ \
		"FAIL %s:\n"            /* Bad news                         */ \
		"\t%s\n",               /* Indented why_string              */ \
		test_name,      	/* Print test case function name    */ \
		why_string) ;  	        /* Print failure defailt            */ \
	return TEST_RETURN_FAIL ; 	/* The test case has now failed     */
#else
#define TEST_CASE_FAIL(why_string)					       \
	return TEST_RETURN_FAIL ; 	/* The test case has now failed     */
#endif // ifndef TEST_OPTION_SUPPRESS_FAILURE


// TODO
//#define TEST_WARN_IF_TRUE()
//#define TEST_WARN_IF_FALSE()

// TODO: Hell yeah
//#define TEST_ALLOC_FREE(typename,copies)

 /*
  * Identifier:
  *		TEST_FAIL_IF_FALSE(predicate)
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
#define TEST_FAIL_IF_FALSE(predicate) 				              \
	if(!(predicate))						      \
	{ TEST_CASE_FAIL("FALSE: \"" TO_STRING(predicate) "\"") ; }

 /*
  * Identifier:
  *		TEST_FAIL_IF_TRUE(predicate)
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
#define TEST_FAIL_IF_TRUE(predicate)				               \
	if((predicate))						               \
	{ TEST_CASE_FAIL("TRUE: \"" TO_STRING(predicate) "\"") }

 /*
  * Identifier:
  * 		TEST_PASS_IF_FALSE(predicate)
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
#define TEST_PASS_IF_FALSE(predicate) if(!(predicate)) TEST_CASE_PASS()

 /*
  * Identifier:
  *		TEST_PASS_IF_TRUE(predicate)
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
#define TEST_PASS_IF_TRUE(predicate) if((predicate TEST_CASE_PASS()

// ASSERT is an alias for TEST_FAIL_IF FALSE (for now)
#define ASSERT(predicate) TEST_FAIL_IF_FALSE(predicate)


/* SECTION: TEST CASE MACROS */

#define TEST_DEFAULT_CASE_BUFFSIZE 100 /* Initial case_capacity */
			
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
	if (set_data.case_count_total >= set_data.case_capacity) {\
		REALLOCATE_OR_DIE((set_data.cases),	\
			(set_data.case_capacity *= 1.3) ) 	\
		REALLOCATE_OR_DIE(set_data.case_names,	\
				set_data.case_capacity) }
		
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
	set_data.cases[set_data.case_count_total++] = \
		LAMBDA(int,(void) {	\
			char * test_name = TO_STRING(test_##name) ; \
			(void)test_name ; \
			__VA_ARGS__ \
			TEST_CASE_PASS() ; \
		});\
	//set_data.case_names TODO
	//set_data.case_execution_id TODO
	

/* SECTION: TEST SET MACROS */

#define TEST_SET_CONSTRUCTOR() 						       \
	struct {							       \
		int (**cases)(void) ;/* 8 bytes */                             \
		char ** case_names ;					       \
		size_t case_capacity ;					       \
		size_t case_count_total ;   /* size_t could be 4 or 8 bytes */ \
		size_t case_count_passed ;				       \
		char test_set_name[sizeof(TO_STRING(name))] ; /* DEDUCED */    \
	} set_data = { NULL, NULL, 0,0,0, TO_STRING(name) } ;                  \
	REALLOCATE_OR_DIE((set_data.cases),				       \
		(set_data.case_capacity = TEST_DEFAULT_CASE_BUFFSIZE ) ) ;     \
	/* printf("asdfasdf\n"); */\
	REALLOCATE_OR_DIE(set_data.case_names,				       \
		set_data.case_capacity ) ;\

#define TEST_SET_DESTRUCTOR() \
	for (unsigned i = 0; i < set_data.case_count_total; ++i) {\
		set_data.case_count_passed += set_data.cases[i]() ;\
	}\
	fprintf(stdout,\
		"\nFINISHED: %s\n" \
		"\tPassed %lu/%lu test cases.\n\n",\
			__func__,\
			set_data.case_count_passed,\
			set_data.case_count_total ) ; \
	free(set_data.cases);\
	free(set_data.case_names) ;\
		

// Priority as paramater?
#define TEST_SET(name,...)\
	void test_set_##name (void) __attribute__((constructor)) ;             \
	void test_set_##name (void) { \
		TEST_SET_CONSTRUCTOR() ; \
		__VA_ARGS__ ; \
		TEST_SET_DESTRUCTOR() ; }

#endif // ifndef __GNUC__

#endif // ifndef TEST_MACROS_H
