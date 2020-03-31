#ifndef GET_H
#define GET_H

#include "parser.h"

int	get_addr(char *str, int *rn, int *rm, int *shift,
		shift_method *shift_method, val_type *shift_type);
int	get_cond(char *str, int *flag, cond *cond);
int	get_expr(char *str);
int	get_hex(char *str);
int	get_imm(char *str);
int	get_int(char *str);
int	get_reg(char *str);
int	get_reglist(char *str, int **reglist);
int	get_shift(char *str, int *shift_val, shift_method *shift_method,
		val_type *shift_type);

#endif
