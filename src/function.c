#include "fir.h"

fir_Function *fir_func_create(fir_Module *module, const char *name) {
    fir_Function *func = fir_arena_alloc(&module->arena, sizeof(fir_Module));
    func->module = module;
    func->name = fir_string_add(module, name);

    func->blocks = fir_array_init(fir_Block *, 8);
    fir_block_create(func, "entry");

    func->inputs = fir_array_init(fir_Type, 8);
    func->outputs = fir_array_init(fir_Type, 1);


    fir_array_append(fir_Function *, &module->funcs, func);

    return func;
}

void fir_func_destroy(fir_Function *func) {
    for (size_t i = 0; i < func->blocks.len; i++) {
        fir_block_destroy(*fir_array_get(fir_Block *, &func->blocks, i));
    }

    fir_array_deinit(&func->blocks);

    fir_array_deinit(&func->inputs);
    fir_array_deinit(&func->outputs);
}

fir_Block *fir_func_get_entry(fir_Function *func) {
    return *fir_array_get(fir_Block*, &func->blocks, 0);
}

void fir_func_add_input(fir_Function *func, fir_Type type) {
    fir_array_append(fir_Type, &func->inputs, type);

    fir_block_add_input(fir_func_get_entry(func), type);
}

void fir_func_add_output(fir_Function *func, fir_Type type) {
    fir_array_append(fir_Type, &func->outputs, type);
}

void fir_func_dump(fir_Function *func, FILE *fp) {
    char *name_str = fir_string_get(func->module, func->name);
    fprintf(fp, "@%s (", name_str);

    // input
    for (size_t i = 0; i < func->inputs.len; i++) {
        fir_Type in = *fir_array_get(fir_Type, &func->inputs, i);
        fir_type_dump(in, fp);

        if (i < func->inputs.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ") -> (");

    // output
    for (size_t i = 0; i < func->outputs.len; i++) {
        fir_Type out = *fir_array_get(fir_Type, &func->outputs, i);
        fir_type_dump(out, fp);

        if (i < func->outputs.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ")\n");

    for (size_t i = 0; i < func->blocks.len; i++) {
        fir_Block *blk = *fir_array_get(fir_Block *, &func->blocks, i);
        fir_block_dump(func, blk, fp);
    }
}
