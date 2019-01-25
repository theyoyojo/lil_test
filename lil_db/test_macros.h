#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#include <stdlib.h> // Literally just for exit()

#define TO_STRING(x) #x

#define LAMBDA(return_t,...) __extension__ \
	({ return_t __fn__ __VA_ARGS__ __fn__ ; })

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


#define TEST_FAILURE 0
#define TEST_SUCCESS 1

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(predicate) \
	if(!(predicate)) { \
	fprintf(stderr, "In test case: %s\n"\
		"Assertion: \"%s\" failed!\n",\
			test_name,\
			TO_STRING(predicate)) ;\
	return TEST_FAILURE ; }

#endif // TEST_MACROS_H
