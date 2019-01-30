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
 * 		Any //-style comments are intended as temporary notes and TODOs to
 * 	       +eventually resolve and remove.
 */

#ifndef TEST_MACROS_H
#define TEST_MACROS_H

/* Dependencies */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  *		Explanation of why this macro is not bloat
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
  * 		A callable anonymous function object. The function captures
  * 	       +it's enclosing scope at definition, but NOT at execution.
  */
#define LAMBDA(return_t,...)						       \
	__extension__	 		/* turn off GCC's pedantry 	    */ \
	({				/* begin block expression  	    */ \
	 return_t function_identifier   /* declare function id & return type*/ \
	 __VA_ARGS__			/* insert param list, function body */ \
	 function_identifier ;		/* expression evals to function ptr */ \
	})				/* close block, eval to func object */	
// Note on lambdas: I could throw a struct in here and straight up save state,
// or perform more advanced scope capture by accessing the stack frame directly,
// but this may not be the best use of my time. This could essentially be an
// implementation of the functionality of template functors in C++ (i.e.
// callable objects that save state

// TODO:  error macro section?
// Consider a beter way to handle the exit: deallcoation, goto destructor, etc

 /*
  * Identifier:
  * 		TEST_ERROR_ALLOC_FAIL(bytes)
  * Purpose:
  * 		Report an allocation error and exit the program.
  * Inputs:
  *
  * 	          bytes	: The size of the memory requested by the failed alloc
  *
  * Resolution:
  *	       	An error message is printed to stderr and the program exits.
  *
  * Requirements:
  *  		Nothing needs to be done after this point
  */ 
#define TEST_ERROR_ALLOC_FAIL(bytes) 				\
			fprintf(stderr,				\
			"Reallocation of %lu bytes failed. "	\
			"Killing self...\n",			\
			bytes) ;\
	exit(1) // Deallocate or hard crash? FIXME

 /*
  * Identifier:
  * 		REALLOCATE_OR_DIE(non_void_ptr, count)
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
#define REALLOCATE_OR_DIE(non_void_ptr,count)\
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

 /*
  * Identifier:
  * 		TEST_MAIN()
  * Purpose:
  *		Generate a stub main() using a macro to keep syntax consistent
  *
  * Resolution:
  *		An main function definition that takes no parameters and does
  *	       +nothing but return 0.
  * Requirements:
  *  		main() must not be defined elsewhere in the program
  */ 
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
		this->case_names[case_id]) ;   	/* Print case name          */ \
	return TEST_RETURN_PASS  ; 	/* The test case has now passed	    */
#else
#define TEST_CASE_PASS()						       \
	return TEST_RETURN_PASS  ; 	/* The test case has now passed	    */
#endif /* ifdef TEST_OPTION_VERBOSE */

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
		this->case_names[case_id], /* Print test case function name */ \
		why_string) ;  	        /* Print failure defailt            */ \
	return TEST_RETURN_FAIL ; 	/* The test case has now failed     */
#else
#define TEST_CASE_FAIL(why_string)					       \
	return TEST_RETURN_FAIL ; 	/* The test case has now failed     */
#endif /* ifndef TEST_OPTION_SUPPRESS_FAILURE */


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
#define TEST_PASS_IF_TRUE(predicate) if(predicate) TEST_CASE_PASS()

// ASSERT is an alias for TEST_FAIL_IF FALSE (for now)
#define ASSERT(predicate) TEST_FAIL_IF_FALSE(predicate)


/* SECTION: TEST CASE MACROS */

#define TEST_DEFAULT_CASE_BUFFSIZE 100 /* Initial case_capacity (arbitrary)  */
#define TEST_DEFAULT_RESIZE_FACTOR 1.3 /* Ratio for capacity growth          */
			
 /*
  * Identifier:
  * 		TEST_CHECK_SPACE()
  * Purpose:
  * 		Check if enough memory is allocated for an aditional test case
  * 	       +in the current test set. If more is needed, allocate it
  *
  * Resolution:
  *  	        A conditional statement that tests if the dynamically allocated
  *  	       +memory in the current test set's set_data struct is filled. In
  *  	       +the case of a true result, additional memory is allocated. If
  *  	       +the allocation fails, the program quits with an error message.
  */ 
#define TEST_CHECK_SPACE() 						       \
	if (set_data.case_count_total >= set_data.case_capacity) {\
		REALLOCATE_OR_DIE((set_data.cases),\
			(set_data.case_capacity *= \
			 TEST_DEFAULT_RESIZE_FACTOR )) ;	\
		REALLOCATE_OR_DIE(set_data.case_names,	\
				set_data.case_capacity) }
		
 /*
  * Identifier:
  * 		TEST_CASE(name,...)
  * Purpose:
  *		Define a test case, the core building block of this system
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
  *  	        A test case must be defined directly within the scope of a test
  *  	       +set
  */ 
#define TEST_CASE(name,...) TEST_CHECK_SPACE() ;\
	printf("case_total: %lu\n",this->case_count_total) ;\
	set_data.cases[set_data.case_count_total++] = \
		LAMBDA(int,(size_t case_id) {	\
			char case_name[] = TO_STRING(test_##name) ;\
			REALLOCATE_OR_DIE(this->case_names[case_id],sizeof(case_name)) ; \
			strncpy(this->case_names[case_id],case_name,sizeof(case_name)) ; \
			__VA_ARGS__ \
			TEST_CASE_PASS() ; \
		});\
// Note to self (joel) lambda scope capture is at definition NOT point of call!
	

/* SECTION: TEST SET MACROS */

 /* TODO: docs */
#define TEST_SET_CONSTRUCTOR(name)					       \
	struct set_data {						       \
		int (**cases)(size_t case_id) ;/* 8 bytes */                   \
		char ** case_names ;					       \
		size_t case_capacity ;					       \
		size_t case_count_total ;   /* size_t could be 4 or 8 bytes */ \
		size_t case_count_passed ;				       \
		char set_name[sizeof(TO_STRING(name))] ;/* size is constexpr */\
	} set_data = { NULL, NULL, 0,0,0, TO_STRING(name) } ;                  \
	REALLOCATE_OR_DIE((set_data.cases),				       \
		(set_data.case_capacity = TEST_DEFAULT_CASE_BUFFSIZE ) ) ;     \
	REALLOCATE_OR_DIE(set_data.case_names,				       \
		set_data.case_capacity ) ;				       \
	memset(set_data.case_names,0,set_data.case_capacity) ;		       \
	struct set_data * this = &set_data ;	/* Hell yeah */		       \
	this = this; /* shhh gcc */
// TODO: should this whole macro be broken down a bit more?

 /* TODO: fill this out
  * Identifier:
  * 		DEFINED_IDENFIFIER(comma, seperated, params)
  * Purpose:
  *		Explanation of why this macro is not bloat
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
#define TEST_SET_DESTRUCTOR() \
	for (size_t i = 0; i < set_data.case_count_total; ++i) {\
		set_data.case_count_passed += set_data.cases[i](i) ;\
	}\
	printf("case names (in %s):\n",this->set_name) ;\
	for(int i = 0; i < this->case_count_total;++i){\
		printf("%d: %s\n",i,this->case_names[i]) ;\
	}\
	fprintf(stdout,\
		"\nFINISHED TEST_SET: %s\n" \
		"\tPassed %lu/%lu test cases.\n\n",\
			set_data.set_name,\
			set_data.case_count_passed,\
			set_data.case_count_total ) ; \
	free(this->cases);\
	for(size_t i = 0; i < this->case_count_total; ++i)\
	{ free(this->case_names[i]) ; }\
	free(this->case_names) ;\
		


 /* TODO: fill this out
  * Identifier:
  * 		DEFINED_IDENFIFIER(comma, seperated, params)
  * Purpose:
  *		Explanation of why this macro is not bloat
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
#define TEST_SET(name,...)\
	void test_set_##name (void) __attribute__((constructor)) ;             \
	void test_set_##name (void) { \
		TEST_SET_CONSTRUCTOR(name) ; \
		__VA_ARGS__ ; \
		TEST_SET_DESTRUCTOR() ; }
// Priority as parameter?

#endif /* ifndef __GNUC__ */

#endif /* ifndef TEST_MACROS_H */
