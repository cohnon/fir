#include "fir.h"
#include "debug.h"

#include <stdbool.h>

int main(void) {

    // create module
    FirModule *module = fir_mod_create();

    // create main function
    FirType params[] = { fir_ty_ptr() };

    FirFunc *func = fir_func_create(module, fir_sym_lit("main"), fir_ty_void(), params, 1);

    // create other function
    FirType other_params[] = { fir_ty_int(32), fir_ty_int(32) };
    FirFunc *other_func = fir_func_create(module, fir_sym_lit("other"), fir_ty_int(32), other_params, 2);

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

    firb_call(firb, other_func, args, 2);

    firb_ret_void(firb);

    // verify
    fir_verify_module(module);

    // debug
    fird_print_func(func);

    return 0;
}
