#include <stdlib.h>
#include <stddef.h>
#include <check.h>

#include "../main.h"

START_TEST(test_simple1) {
    
} END_TEST


Suite* MakeQuickSuite(void) {
    Suite *s = suite_create("QuickSort");
    TCase *tc;

    tc = tcase_create("SimpleTests");
    tcase_add_test(tc, test_simple1);

    suite_add_tcase(s, tc);

    tcase_set_timeout(tc, 1);   

    return s;
}