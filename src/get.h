#ifndef GET_H
#define GET_H

#include "instruction.h"

#define MAXREG 16

int	get_imm(struct tok t);
int	get_reg(char *str);
Inst	get_inst(struct tok t);
int	*get_reglist(struct tok t);

#endif
