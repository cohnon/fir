// == prelude ==
#include <stddef.h>
#include <stdint.h>

// == forward decls ====
int32_t puts(void* param_0);
int32_t main(void);
int32_t add(int32_t param_0, int32_t param_1);

// == definitions ====
int32_t main(void) {
	blk_entry0:;
	int32_t reg_0 = 10 + -10;
	if (reg_0) { goto blk_if_then0; } else { goto blk_if_else0; }

	blk_if_then0:;
	reg_0 = 5;
	goto blk_if_join0;

	blk_if_else0:;
	reg_0 = -5;
	goto blk_if_join0;

	blk_if_join0:;
	int32_t reg_1 = 100 + reg_0;
	int32_t reg_2 = add(reg_0, reg_1);
	int32_t reg_3 = puts();
	return 0;

}

int32_t add(int32_t param_0, int32_t param_1) {
	blk_entry0:;
	int32_t reg_0 = param_0;
	int32_t reg_1 = param_1;
	int32_t reg_2 = reg_0 + reg_1;
	return reg_2;

}

