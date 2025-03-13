#include "fir.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    fir_Module module = fir_module_init(
        "test_module",
        malloc, realloc, free
    );

    fir_module_deinit(&module);

    printf("== tests passed ==\n");

    return 0;
}
