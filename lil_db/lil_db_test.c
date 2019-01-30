/*
 * lil_db_test.c driver and testing file
 * A lil debugging library to save some keystrokes
 * By Joel Savitz <jsavitz@redhat.com>
 */

#include "test_macros.h"
#include "lil_db.h"

TEST_SET(demo1,
	int x = 4 ;

	TEST_CASE(should_fail,
		ASSERT(x != 4);
		x = 4 ;
	) ;


	TEST_CASE(should_pass,
		/* ASSERT(1 == 1) ; */
		/* TEST_CASE(nested_passing, */
		/* 	ASSERT(1) ; */
		/* ) ; */
		/* TEST_CASE(nested_failing, */
		/* 	ASSERT(0) ; */
		/* ) ; */
	) ;
)

TEST_SET(demo2,
	TEST_CASE(new_fail_if_true,
		TEST_FAIL_IF_TRUE(0) ;
	) ;
	TEST_CASE(will_fail_silently,TEST_CASE_FAIL("")) ;

	TEST_CASE(passes,TEST_PASS_IF_TRUE(1)) ;

	TEST_CASE(new_pass_if_false,TEST_PASS_IF_FALSE(0)) ;
)

// Everything in test_set scope is done before anything in a test_case scope
TEST_SET(demo3,

	char filename[] = "DUMMY" ;	
	lil_db_init(filename,sizeof(filename)) ;
	lil_db_kill() ;

	TEST_CASE(file_created,
		FILE * test ;
		ASSERT((test = fopen("DUMMY","a+"))) ;
		fclose(test) ;
	) ;

	TEST_CASE(file_removed,

		TEST_PASS_IF_FALSE(remove("DUMMY")) ;
	) ;

	TEST_CASE(aaa) ;
	TEST_CASE(bbb,) ;
	TEST_CASE(ccc,
		printf("test CCC-1\n") ;	
	) ;
	TEST_CASE(ccc,
		printf("test CCC-2\n") ;	
	) ;
) ;


TEST_MAIN() ;

/* 
 * The following is valid GNU C and it's output is λ, given LAMBDA is defined
 * LAMBDA(void,(void){printf("λ\n");})() ;
 */
