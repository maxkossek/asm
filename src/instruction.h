#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "parser.h"

typedef enum {
	ADC, ADD, AND,
	BIC,
	CMN, CMP,
	EOR, ERR,
	LDR,
	MLA, MLS, MOV, MUL, MVN,
	ORR, ORN,
	POP, PUSH,
	RSB, RSC,
	SBC, SUB
} Inst;

int	rd_op2(struct inst in);
int	rd_rn_op2(struct inst in);
int	rd_rn_rm_ra(struct inst in);
int	reglist(struct inst in);

#endif
