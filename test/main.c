#include "fir.h"
#include "debug.h"

#include <stdbool.h>

int main(void) {

    FirModule *module = fir_mod_create();

    FirFunc *func = fir_func_create(module, fir_sym_lit("main"));

    FirType params[] = { fir_ty_int(32) };
    fir_func_set_signature(func, fir_ty_void(), 1, params);

    FirBuilder *firb = fir_mod_get_builder(module);

    firb_set_insert_point(firb, fir_func_get_entry_blk(func));

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
    firb_add(firb, fir_ty_int(32), fir_imm_int(module, 100, false), sum);

    fird_print_func(func);

    return 0;
}
