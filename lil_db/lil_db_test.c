/*
 * lil_db_test.c driver and testing file
 * A lil debugging library to save some keystrokes
 * By Joel Savitz <jsavitz@redhat.com>
 */

#include <stdio.h>

#include "test_macros.h"
#include "lil_db.h"

void foo (void (*f)(void)) {
	(*f)() ;
}

void baz (void (*f)(void (*g)(void))) {
	(*f)(
		LAMBDA(void, (void)
		{
			printf("SUPER NESTING\n") ;
		})
	) ;
}

int main(int argc, char * argv[]) {
	(void)argc,(void)argv ; // shhhhh ycm

	TEST_INIT(200) ; // arbitrary value > # test cases

	TEST_EXEC() ;
	lil_db_init("xxxx",5) ;

	int x = 4 ;

	// Also demonstrated: capture of enclosing scope
	TEST_CASE(should_fail,
		printf("%d\n",x) ;
		ASSERT(1==2);
	) ;

	TEST_CASE(should_pass,
		ASSERT(1 == 1) ;
		TEST_CASE(nested_passing,
			ASSERT(1) ;
		) ;
		TEST_CASE(nested_failing,
			ASSERT(0) ;
		) ;
	) ;

	TEST_CASE(new_fail_if_true,
		TEST_FAIL_IF_TRUE(0) ;
	) ;

	TEST_CASE(will_fail_silently,TEST_CASE_FAIL("")) ;

	TEST_CASE(new_pass_if_false,TEST_PASS_IF_FALSE(0)) ;

	foo(LAMBDA(void, (void) {
		printf("in lambda\n") ;
		LAMBDA(void,(void) {
			printf("NESTY BOI\n") ;
		})() ;
	})) ;

	baz(LAMBDA(void, (void(*h)(void)) { (*h)(); })) ;

	TEST_EXEC() ;


	lil_db_kill() ;


	return 0 ;
}

/* Valid: LAMBDA(void,(void){printf("λ\n");})() ; */
