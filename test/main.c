#include "fir.h"
#include "debug.h"

#include <stdbool.h>

int main(void) {

    FirModule *module = fir_mod_create();

    FirFunc *func = fir_func_create(module, fir_sym_lit("main"));

    FirType params[] = { fir_ty_int(32) };
    fir_func_set_signature(func, fir_ty_void(), params, 1);

    FirBuilder *firb = fir_mod_get_builder(module);

    firb_set_insert_point(firb, fir_func_get_entry_blk(func));

    // bin op
    firb_add(firb,
        fir_ty_int(32),
        fir_imm_int(module, 10, false),
        fir_imm_int(module, 10, true)
    );
/*

    // if stmt
    FirIf fir_if = firb_if(firb, sum);

    firb_set_insert_point(firb, fir_if.then_blk);
    firb_set(firb, sum, fir_imm_int(5, true));

    firb_set_insert_point(firb, fir_if.else_blk);
    firb_set(firb, sum, fir_imm_int(5, true));

    firb_set_insert_point(firb, fir_if.join_blk);

*/

    fird_print_func(func);

    return 0;
}
