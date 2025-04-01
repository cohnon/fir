#include "util.h"
#include <fir.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static fir_Function *create_arithmetic_func(fir_Module *module) {
    char *expected =
        "@arithmetic (i32 %0, i32 %1, f32 %2, f32 %3) -> ()\n"
        "  i32 %4 = add %0, %1\n"
        "  i32 %5 = sub %0, %1\n"
        "  i32 %6 = mul %0, %1\n"
        
        "  f32 %7 = add %2, %3\n"
        "  f32 %8 = sub %2, %3\n"
        "  f32 %9 = mul %2, %3\n"

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
        "@tuple_ret (f32 %0, f32 %1) -> (f32, f32)\n"
        "  ret %0, %1\n"
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
        "@add (f32 %0, f32 %1) -> (f32)\n"
        "  f32 %2 = add %0, %1\n"
        "  ret %2\n"
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

static fir_Function *create_stack_func(fir_Module *module) {
    char *expected =
        "@stack () -> ()\n"
        "  i32 %0 = lit 128\n"
        "  ptr %1 = stack %0\n"
        "  i32 %2 = lit 4\n"
        "  ptr %3 = offset %1, %2\n"

        "  f32 %4 = lit 42\n"
        "  write %1, %4\n"

        "  i32 %6 = lit -42\n"
        "  write %3, %6\n"

        "  f32 %8 = read %1\n"
        "  i32 %9 = read %3\n"
        
        "  ret\n"
    ;

    fir_Function *func = fir_func_create(module, "stack");

    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *r0 = fir_instr_lit_int(entry, fir_type_int(32), 128);
    fir_Instr *stk0 = fir_instr_stack(entry, r0);
    fir_Instr *r2 = fir_instr_lit_int(entry, fir_type_int(32), 4);
    fir_Instr *stk1 = fir_instr_offset(entry, stk0, r2);

    fir_Instr *a = fir_instr_lit_float(entry, fir_type_float(32), 42);
    fir_instr_write(entry, stk0, a);

    fir_Instr *b = fir_instr_lit_int(entry, fir_type_int(32), -42);
    fir_instr_write(entry, stk1, b);

    fir_instr_read(entry, fir_type_float(32), stk0);
    fir_instr_read(entry, fir_type_int(32), stk1);

    fir_instr_ret(entry);

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("@stack", expected, got);
    free(got);

    return func;
}

void test_module(void) {
    fir_Module module = fir_module_init("test");

    fir_Function *func = fir_func_create(&module, "test");
    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *a_i32 = fir_instr_lit_int(entry, fir_type_int(32), 10);
    fir_Instr *b_i32 = fir_instr_lit_int(entry, fir_type_int(32), -10);
    fir_Instr *c_f32 = fir_instr_lit_float(entry, fir_type_float(32), 10.0f);
    fir_Instr *d_f32 = fir_instr_lit_float(entry, fir_type_float(32), -10.0f);


    // @arithmetic
    fir_Function *arithmetic_func = create_arithmetic_func(&module);
    fir_Instr *call0 = fir_instr_call(entry, arithmetic_func);
    fir_instr_call_arg(call0, a_i32);
    fir_instr_call_arg(call0, b_i32);
    fir_instr_call_arg(call0, c_f32);
    fir_instr_call_arg(call0, d_f32);

    
    // @tuple_ret
    fir_Function *tuple_ret_func = create_tuple_ret_func(&module);
    fir_Instr *call1 = fir_instr_call(entry, tuple_ret_func);
    fir_instr_call_arg(call1, c_f32);
    fir_instr_call_arg(call1, d_f32);

    fir_Instr *proj0 = fir_instr_proj(entry, call1, 0);
    fir_Instr *proj1 = fir_instr_proj(entry, call1, 1);


    // @add
    fir_Function *add_func = create_add_func(&module);
    fir_Instr *add_call = fir_instr_call(entry, add_func);
    fir_instr_call_arg(add_call, proj0);
    fir_instr_call_arg(add_call, proj1);

    
    // @stack
    fir_Function *stack_func = create_stack_func(&module);
    fir_instr_call(entry, stack_func);


    fir_instr_ret(entry);

    char *expected =
        "@test () -> ()\n"
        "  i32 %0 = lit 10\n"
        "  i32 %1 = lit -10\n"
        "  f32 %2 = lit 10\n"
        "  f32 %3 = lit -10\n"

        "  call @arithmetic %0, %1, %2, %3\n"

        "  (f32, f32) %5 = call @tuple_ret %2, %3\n"
        "  f32 %6 = proj %5 0\n"
        "  f32 %7 = proj %5 1\n"

        "  f32 %8 = call @add %6, %7\n"

        "  call @stack\n"

        "  ret\n"
    ;

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("@test", expected, got);
    free(got);

    if (get_error_count() == 0) {
        fir_module_dump(&module, stdout);
        printf("\n");
    }

    fir_module_deinit(&module);
}

