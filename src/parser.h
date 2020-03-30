#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define REGISTER 0
#define IMMEDIATE 1
#define SHIFT 2
#define MAXINST 200
#define MAXREG 16

typedef enum {
	I_LABEL, I_NAME, I_NONE, I_RD_EXPR, I_RD_OP2,
	I_RD_RN_OP2, I_RD_RN_RM_RA, I_REGL, I_RT_ADDR,
} Type;

typedef enum {
	S_ASR, S_LSL, S_LSR, S_NONE, S_ROR, S_RRX,
} Shift_type;

typedef enum {
	ADC, ADD, AND,
	B, BIC, BL,
	CMN, CMP,
	EOR,
	LDR,
	MLA, MLS, MOV, MUL, MVN,
	NONE,
	ORR, ORN,
	POP, PUSH,
	RSB, RSC,
	SBC, STR, SUB,
	TEQ, TST,
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
	int		setflag;
	Cond		cond;
	int		dest;
	int		op1;
	int		op2;
	int		op2_imm;
	int		op3;
	Shift_type	shift_type;
	int		shift;
	int		shift_imm;
	int		*reglist;
	char		*label;
};

struct inst	addre[MAXINST];
static int	curr = 0;
static int	line_num = 0;

int		check_cond(Cond cond);
int		execute();
struct tok	expect(Token t);
Inst		get_inst(char *str, int *flag, int *cond);
struct tok	get_token();
int		parse();
struct inst	parse_instruction();
struct inst	parse_label();
struct tok	peek();
void		syntax_error(Token t);
void		unget_token();

#endif
