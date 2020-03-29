#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define OP2_REG 0
#define OP2_IMM 1
#define MAXINST 200
#define MAXREG 16
#define MAXOP 4

typedef enum {
	I_LABEL, I_NAME, I_NONE, I_RD_EXPR, I_RD_OP2,
	I_RD_RN_OP2, I_RD_RN_RM_RA, I_REGL, I_RT_ADDR,
} Type;

typedef enum {
	ADC, ADD, AND,
	B, BIC, BL,
	CMN, CMP,
	EOR, ERR,
	LDR,
	MLA, MLS, MOV, MUL, MVN,
	ORR, ORN,
	POP, PUSH,
	RSB, RSC,
	SBC, STR, SUB
} Inst;

typedef enum {
	AL,
	CC, CS,
	EQ,
	GE, GT,
	HI, HS,
	LO, LE, LS, LT,
	MI,
	NE,
	PL,
	VC, VS,
} Cond;

struct inst {
	Type		type;
	Inst		mnemonic;
	Cond		cond;
	int		dest;
	int		op1;
	int		op2;
	int		imm;
	int		op3;
	int		lsl;
	int		*reglist;
	char		*label;
};

struct inst	addre[MAXINST];
static int	curr = 0;
static int	line_num = 0;

int		execute();
struct tok	expect(Token t);
struct tok	get_token();
int		parse();
struct inst	parse_instruction();
struct inst	parse_label();
struct tok	peek();
void		syntax_error(Token t);

#endif
