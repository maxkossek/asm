#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define MAXINST 200
#define MAXREG 16

typedef enum {
	I_LABEL, I_NAME, I_NONE, I_RD_EXPR, I_RD_OP2,
	I_RD_RN_OP2, I_RD_RN_RM_RA, I_REGL, I_RT_ADDR,
} inst_type;

typedef enum {
	S_ASR, S_LSL, S_LSR, S_NONE, S_ROR, S_RRX,
} shift_method;

typedef enum {
	V_NONE, V_IMM, V_REG, V_SHIFT,
} val_type;

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
} inst;

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
} cond;

struct inst {
	inst_type	type;
	inst		mnemonic;
	int		setflag;
	cond		cond;
	int		dest;
	int		op1;
	int		op2;
	val_type	op2_type;
	int		op3;
	int		shift;
	shift_method	shift_method;
	val_type	shift_type;
	int		*reglist;
	char		*label;
};

struct inst	addre[MAXINST];
static int	curr = 0;
static int	line_num = 0;

int		check_cond(cond cond);
int		execute();
struct tok	expect(Token t);
inst		get_inst(char *str, int *flag, cond *cond);
struct tok	get_token();
int		parse();
struct inst	parse_instruction();
struct inst	parse_label();
struct tok	peek();
void		syntax_error(Token t);
void		unget_token();

#endif
