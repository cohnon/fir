#include "fir.h"
#include "debug.h"

#include <stdbool.h>
#include <stdio.h>

int main(void) {

    // create module
    FirModule *module = fir_mod_create();

    // create main function
    FirType params[] = { fir_ty_void() };

    FirFunc *func = fir_func_create(module, fir_sym_lit("main"), fir_ty_int(32), params, 1);

    // create add function
    FirType add_params[] = { fir_ty_int(32), fir_ty_int(32) };
    FirFunc *add_func = fir_func_create(module, fir_sym_lit("add"), fir_ty_int(32), add_params, 2);

    // builder
    FirBuilder *firb = fir_mod_get_builder(module);
    firb_init_func(firb, func);

    // bin op
    FirVal sum = firb_add(firb,
        fir_ty_int(32),
        fir_imm_int(module, 10, false),
        fir_imm_int(module, 10, true)
    );

    // if stmt
    FirIfCtrl if_ctrl = firb_if(firb, sum);

    firb_set_insert_point(firb, if_ctrl.then_blk);
    firb_mov(firb, sum, fir_imm_int(module, 5, false));

    firb_set_insert_point(firb, if_ctrl.else_blk);
    firb_mov(firb, sum, fir_imm_int(module, 5, true));

    firb_set_insert_point(firb, if_ctrl.join_blk);
    FirVal sum2 = firb_add(firb, fir_ty_int(32), fir_imm_int(module, 100, false), sum);

    // func call
    FirVal args[] = {
        sum,
        sum2,
    };

    firb_call(firb, add_func, args, 2);

    FirVal ret_val = fir_imm_int(module, 0, false);
    firb_ret(firb, ret_val);

    // verify
    bool valid = fir_verify_module(module);
    if (!valid) {
        printf("uh oh: module is in an invalid state\n");

        return 1;
    }

    // build target
    FirTarget *target = fir_target_create(module, FirTarget_C);
    fir_target_build(target);
    
    fir_target_to_file(target, "ir.c");

    // debug
    printf("== ir ==\n");
    fird_print_func(func);
    printf("\n");

    printf("== target ==\n");
    fird_print_target(target);

    return 0;
}
