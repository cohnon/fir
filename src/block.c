#include "fir.h"
#include <stdio.h>
#include <string.h>

fir_Block *fir_block_create(fir_Function *func, const char *name) {
    fir_Block *blk = fir_arena_alloc(&func->module->arena, sizeof(fir_Block));

    blk->func = func;
    blk->name = fir_string_add(func->module, name);

    blk->inputs = fir_array_init(fir_Type, 4);
    blk->instrs = fir_array_init(fir_Instruction *, 8);

    fir_array_append(fir_Block *, &func->blocks, blk);

    return blk;
}

void fir_block_add_input(fir_Block *block, fir_Type type) {
    fir_array_append(fir_Type, &block->inputs, type);
}

void fir_block_destroy(fir_Block *block) {
    fir_array_deinit(&block->inputs);
    fir_array_deinit(&block->instrs);

    block->func = NULL;
}

void fir_block_dump(fir_Function *func, fir_Block *block, FILE *fp) {
    const char *name = fir_string_get(func->module, block->name);

    if (strcmp(name, "entry") != 0) {
        fprintf(fp, ":%s", name);

        if (block->inputs.len > 0) {
            fprintf(fp, " (");
            for (size_t i = 0; i < block->inputs.len; i++) {
                fir_Type in = *fir_array_get(fir_Type, &block->inputs, i);
                fir_type_dump(in, fp);

                if (i < block->inputs.len - 1) {
                    fprintf(fp, ", ");
                }
            }
            fprintf(fp, ")");
        }

        fprintf(fp, "\n");
    }

    if (block->instrs.len > 0) {
        for (size_t i = 0; i < block->instrs.len; i++) {
            fir_Instruction *instr = *fir_array_get(fir_Instruction *, &block->instrs, i);

            fprintf(fp, "  .r%d = ", instr->idx);

            fir_instr_dump(block, instr, fp);
            fprintf(fp, "\n");
        }
    }
}
