#ifndef GET_H
#define GET_H

#include "parser.h"

int	get_addr(char *str, int *rn, int *rm, int *lsl);
int	get_expr(char *str);
int	get_hex(char *str);
int	get_imm(char *str);
Inst	get_inst(struct tok t);
int	get_int(char *str);
int	get_reg(char *str);
int	get_reglist(struct tok t, int **reglist);
int	get_shift(char *str);

#endif
