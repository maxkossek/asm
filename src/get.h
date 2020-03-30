#ifndef GET_H
#define GET_H

int	get_addr(char *str, int *rn, int *rm, int *shift, int *shift_type,
		int *shift_imm);
int	get_cond(char *str, int *flag, int *cond);
int	get_expr(char *str);
int	get_hex(char *str);
int	get_imm(char *str);
int	get_int(char *str);
int	get_reg(char *str);
int	get_reglist(char *str, int **reglist);
int	get_shift(char *str, int *shift, int *type, int *shift_imm);

#endif
