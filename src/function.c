#include "fir.h"

fir_Function *fir_func_create(fir_Module *module, const char *name) {
    fir_Function *new = fir_arena_alloc(&module->arena, sizeof(fir_Module));

    new->module = module;
    new->name = fir_string_add(module, name);
    new->input = fir_array_init(fir_Arg, 4);
    new->output = fir_array_init(fir_Arg, 4);

    fir_array_append(fir_Function *, &module->funcs, new);

    return new;
}

void fir_func_add_input(fir_Function *func, fir_Type type) {
    fir_Arg arg;
    arg.type = type;

    fir_array_append(fir_Arg, &func->input, arg);
}

void fir_func_add_output(fir_Function *func, fir_Type type) {
    fir_Arg arg;
    arg.type = type;

    fir_array_append(fir_Arg, &func->output, arg);
}

void fir_func_dump(fir_Function *func, FILE *fp) {
    char *name_str = fir_string_get(func->module, func->name);
    fprintf(fp, "@%s : (", name_str);

    // input
    for (size_t i = 0; i < func->input.len; i++) {
        fir_Arg in = *fir_array_get(fir_Arg, &func->input, i);
        fir_type_dump(in.type, fp);

        if (i < func->input.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ") -> (");

    // output
    for (size_t i = 0; i < func->output.len; i++) {
        fir_Arg out = *fir_array_get(fir_Arg, &func->output, i);
        fir_type_dump(out.type, fp);

        if (i < func->output.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ")\n");
}
