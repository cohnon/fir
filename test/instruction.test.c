#include "util.h"
#include <fir.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static fir_Function *create_arithmetic_func(fir_Module *module) {
    char *expected =
        "@arithmetic (i32, i32, f32, f32) -> ()\n"
        "  i32 R0 = arg 0\n"
        "  i32 R1 = arg 1\n"
        "  f32 R2 = arg 2\n"
        "  f32 R3 = arg 3\n"

        "  i32 R4 = add R0, R1\n"
        "  i32 R5 = sub R0, R1\n"
        "  i32 R6 = mul R0, R1\n"
        
        "  f32 R7 = add R2, R3\n"
        "  f32 R8 = sub R2, R3\n"
        "  f32 R9 = mul R2, R3\n"

        "  ret\n"
    ;

    fir_Function *func = fir_func_create(module, "arithmetic");
    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_float(32));
    fir_func_add_input(func, fir_type_float(32));

    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *arg0 = fir_instr_arg(entry, 0);
    fir_Instr *arg1 = fir_instr_arg(entry, 1);
    fir_Instr *arg2 = fir_instr_arg(entry, 2);
    fir_Instr *arg3 = fir_instr_arg(entry, 3);

    fir_instr_add(entry, arg0, arg1);
    fir_instr_sub(entry, arg0, arg1);
    fir_instr_mul(entry, arg0, arg1);

    fir_instr_add(entry, arg2, arg3);
    fir_instr_sub(entry, arg2, arg3);
    fir_instr_mul(entry, arg2, arg3);

    fir_instr_ret(entry);

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("@arithmetic", expected, got);
    free(got);

    return func;
}

static fir_Function *create_tuple_ret_func(fir_Module *module) {
    char *expected =
        "@tuple_ret (f32, f32) -> (f32, f32)\n"
        "  f32 R0 = arg 0\n"
        "  f32 R1 = arg 1\n"
        "  ret R0, R1\n"
    ;

    fir_Function *func = fir_func_create(module, "tuple_ret");
    fir_func_add_input(func, fir_type_float(32));
    fir_func_add_input(func, fir_type_float(32));
    fir_func_add_output(func, fir_type_float(32));
    fir_func_add_output(func, fir_type_float(32));

    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *arg0 = fir_instr_arg(entry, 0);
    fir_Instr *arg1 = fir_instr_arg(entry, 1);
    
    fir_Instr *ret = fir_instr_ret(entry);
    fir_instr_ret_arg(ret, arg0);
    fir_instr_ret_arg(ret, arg1);

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("@tuple_ret", expected, got);
    free(got);

    return func;
}

static fir_Function *create_add_func(fir_Module *module) {
    char *expected =
        "@add (f32, f32) -> (f32)\n"
        "  f32 R0 = arg 0\n"
        "  f32 R1 = arg 1\n"
        "  f32 R2 = add R0, R1\n"
        "  ret R2\n"
    ;

    fir_Function *func = fir_func_create(module, "add");
    fir_func_add_input(func, fir_type_float(32));
    fir_func_add_input(func, fir_type_float(32));
    fir_func_add_output(func, fir_type_float(32));

    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *arg0 = fir_instr_arg(entry, 0);
    fir_Instr *arg1 = fir_instr_arg(entry, 1);

    fir_Instr *sum = fir_instr_add(entry, arg0, arg1);

    fir_Instr *ret = fir_instr_ret(entry);
    fir_instr_ret_arg(ret, sum);

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);
    
    char *got = file_to_string(fp);
    expect_string("@add", expected, got);
    free(got);

    return func;
}

void test_instr(bool dump) {
    fir_Module module = fir_module_init("instr_test");

    fir_Function *func = fir_func_create(&module, "test_instr");
    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *a_i32 = fir_instr_lit_int(entry, fir_type_int(32), 10);
    fir_Instr *b_i32 = fir_instr_lit_int(entry, fir_type_int(32), -10);
    fir_Instr *c_f32 = fir_instr_lit_float(entry, fir_type_float(32), 10.0f);
    fir_Instr *d_f32 = fir_instr_lit_float(entry, fir_type_float(32), -10.0f);


    // @arithmetic
    fir_Function *arithmetic_func = create_arithmetic_func(&module);
    fir_Instr *arithmetic_call = fir_instr_call(entry, arithmetic_func);
    fir_instr_call_arg(arithmetic_call, a_i32);
    fir_instr_call_arg(arithmetic_call, b_i32);
    fir_instr_call_arg(arithmetic_call, c_f32);
    fir_instr_call_arg(arithmetic_call, d_f32);

    
    // @add
    fir_Function *add_func = create_add_func(&module);
    fir_Instr *add_call = fir_instr_call(entry, add_func);
    fir_instr_call_arg(add_call, c_f32);
    fir_instr_call_arg(add_call, d_f32);

    
    // @tuple_ret
    fir_Function *tuple_ret_func = create_tuple_ret_func(&module);
    fir_Instr *tuple_ret_call = fir_instr_call(entry, tuple_ret_func);
    fir_instr_call_arg(tuple_ret_call, c_f32);
    fir_instr_call_arg(tuple_ret_call, d_f32);

    fir_instr_ret(entry);

    char *expected =
        "@test_instr () -> ()\n"
        "  i32 R0 = lit 10\n"
        "  i32 R1 = lit -10\n"
        "  f32 R2 = lit 10\n"
        "  f32 R3 = lit -10\n"

        "  call @arithmetic R0, R1, R2, R3\n"

        "  f32 R5 = call @add R2, R3\n"

        "  (f32, f32) R6 = call @tuple_ret R2, R3\n"
        // "  f32 R7 = field R6 0\n"
        // "  f32 R8 = field R6 1\n"

        "  ret\n"
    ;

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("@test_instr", expected, got);
    free(got);

    if (get_error_count() == 0 && dump) {
        fir_module_dump(&module, stdout);
        printf("\n");
    }

    fir_module_deinit(&module);
}

