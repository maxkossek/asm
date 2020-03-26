#ifndef GET_H
#define GET_H

#include "instruction.h"

#define MAXREG 16

int	get_addr(struct tok t);
int	get_expr(struct tok t);
int	get_hex(char *str);
int	get_imm(char *str);
Inst	get_inst(struct tok t);
int	get_int(char *str);
int	get_labeladdr(char *str);
int	get_reg(char *str);
int	*get_reglist(struct tok t);
int	get_shift(char *str);

#endif
