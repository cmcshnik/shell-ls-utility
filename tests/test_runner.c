#include <stdlib.h>
#include "test_main.h"



int main(void) {
    SRunner *runner = srunner_create(NULL);
    // <-- YOUR CODE HERE -->

    srunner_add_suite(runner, MakeQuickSuite());

    srunner_run_all(runner, CK_VERBOSE);
    int num_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
