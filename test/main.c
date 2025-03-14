#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool test_arena(void);
bool test_array(void);

int main(void) {
    assert(test_arena());
    printf("== arena tests passed ==\n");

    assert(test_array());
    printf("== array tests passed ==\n");

    fir_Module module = fir_module_init("test_module");

    fir_module_deinit(&module);

    printf("== tests passed ==\n");

    return 0;
}
