/*
 * lil_db_test.c driver and testing file
 * A lil debugging library to save some keystrokes
 * By Joel Savitz <jsavitz@redhat.com>
 */

// Yeah you're going to need GNU C if you want any of this to make sense 
#ifndef __GNUC__
#error "__GNUC__ not defined"
#else

#include <stdio.h>

#include "lil_db.h"
#include "test_macros.h"

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

#endif // __GNUC__ ifndef wrapper

/* Valid: LAMBDA(void,(void){printf("λ\n");})() ; */
