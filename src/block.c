#include "fir.h"

fir_Block *fir_block_create(fir_Function *func, const char *name) {
    fir_Block *blk = fir_arena_alloc(&func->module->arena, sizeof(fir_Block));

    blk->name = fir_string_add(func->module, name);

    blk->inputs = fir_array_init(fir_Type, 4);

    fir_array_append(fir_Block *, &func->blocks, blk);

    return blk;
}

void fir_block_add_input(fir_Block *block, fir_Type type) {
    fir_array_append(fir_Type, &block->inputs, type);
}

void fir_block_destroy(fir_Block *block) {
    fir_array_deinit(&block->inputs);
}
