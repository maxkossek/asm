#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "parser.h"

extern int r[];
extern int a[];

int	label(struct inst in);
int	labeladdr(char *str);
int	rd_expr(struct inst in);
int	rd_op2(struct inst in);
int	rd_rn_op2(struct inst in);
int	rd_rn_rm_ra(struct inst in);
int	reglist(struct inst in);
int	rt_addr(struct inst in);
int	set_flags(int result, int op1, int op2);

#endif
