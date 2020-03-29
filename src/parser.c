/* parse - Parsing functions. */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"
#include "get.h"
#include "instruction.h"
#include "parser.h"

int	addre_size = 0;

void
unget_token()
{
	curr--;
}

int
execute()
{
	int	pc;
	Type	type;
	r[PC] = 0;

	for ( ; r[PC] < addre_size; r[PC]++) {
		pc = r[PC];
		type = addre[pc].type;

		if (type == I_NAME)
			;
		else if (type == I_LABEL)
			label(addre[pc]);
		else if (type == I_RD_EXPR)
			rd_expr(addre[pc]);
		else if (type == I_RD_OP2)
			rd_op2(addre[pc]);
		else if (type == I_RD_RN_OP2)
			rd_rn_op2(addre[pc]);
		else if (type == I_RD_RN_RM_RA)
			rd_rn_rm_ra(addre[pc]);
		else if (type == I_REGL)
			reglist(addre[pc]);
		else if (type == I_RT_ADDR)
			rt_addr(addre[pc]);
		else if (type == I_RT_ADDR)
			rt_addr(addre[pc]);
		else {
			fprintf(stderr, "Invalid instruction.\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

struct tok
expect(Token t)
{
	if (tokens[curr].token != t)
		syntax_error(t);

	return tokens[curr++];
}

struct tok
get_token()
{
	return tokens[curr++];
}

int
parse()
{
	int		entry = 0;
	struct tok	t;
	curr = 0;
	line_num = 1;

	t = get_token();
	while (t.token != END && t.token != ENDOFFILE) {
		if (t.token == TAB && entry == 1) {
			unget_token();
			addre[addre_size++] = parse_instruction();
		}
		else if (t.token == ID && entry == 1) {
			unget_token();
			addre[addre_size++] = parse_label();
		}
		else if (t.token == NEWLINE)
			line_num++;
		if (t.token == ENTRY)
			entry = 1;
		t = get_token();
	}

	return 0;
}

struct inst
parse_instruction()
{
	struct tok 	t;
	struct inst	i;

	t = expect(TAB);
	t = expect(ID);
	i.mnemonic = get_inst(t);
	i.cond = AL;
	i.type = I_NONE;
	i.imm = OP2_REG;
	expect(TAB);

	t = get_token();
	if (t.token == ID) {
		/* No destination register, so parse label. */
		if ((i.dest = get_reg(t.value)) == -1) {
			i.dest = -1;
			i.label = malloc(strlen(t.value) + 1);
			strlcpy(i.label, t.value, strlen(t.value) + 1);
			i.type = I_LABEL;
		}
	}
	else if (t.token == REGLIST) {
		if (get_reglist(t, &i.reglist) < 0)
			fprintf(stderr, "Invalid reglist: %s.\n", t.value);
		i.type = I_REGL;
	}
	else
		syntax_error(t.token);

	t = peek();
	if (t.token == COMMA) {
		expect(COMMA);
		t = get_token();
		if (t.token == IMM) {
			i.type = I_RD_OP2;
			i.op1 = get_imm(t.value);
			if (i.op1 < 0)
				fprintf(stderr, "Invalid immediate: %s.\n",
					t.value);
			i.op2 = i.op1;
			i.imm = OP2_IMM;
		}
		else if (t.token == ID) {
			i.type = I_RD_OP2;
			i.op1 = get_reg(t.value);
			if (i.op1 < 0)
				fprintf(stderr, "Invalid register: %s.\n",
					t.value);
			i.op2 = i.op1;
			i.imm = OP2_REG;
		}
		else if (t.token == EXPR) {
			i.type = I_RD_EXPR;
			i.op1 = get_expr(t.value);
			if (i.op1 < 0)
				fprintf(stderr, "Invalid expression: %s.\n",
					t.value);
		}
		else if (t.token == ADDR) {
			i.type = I_RT_ADDR;
			if (get_addr(t.value, &i.op1, &i.op2, &i.lsl) < 0)
				fprintf(stderr, "Invalid expression: %s.\n",
					t.value);
		}
		else
			syntax_error(t.token);
	}

	t = peek();
	if (t.token == COMMA) {
		expect(COMMA);
		t = get_token();
		if (t.token == IMM) {
			i.type = I_RD_RN_OP2;
			i.op2 = get_imm(t.value);
			if (i.op2 < 0)
				fprintf(stderr, "Invalid immediate: %s.\n",
					t.value);
			i.imm = OP2_IMM;
		}
		else if (t.token == ID) {
			i.type = I_RD_RN_OP2;
			i.op2 = get_reg(t.value);
			if (i.op2 < 0)
				fprintf(stderr, "Invalid register: %s.\n",
					t.value);
			i.imm = OP2_REG;
		}
		else
			syntax_error(t.token);
	}

	t = peek();
	if (t.token == COMMA) {
		expect(COMMA);
		t = get_token();
		if (t.token == ID) {
			i.type = I_RD_RN_RM_RA;
			i.op3 = get_reg(t.value);
			if (i.op3 < 0)
				fprintf(stderr, "Invalid register: %s.\n",
					t.value);
		}
		else
			syntax_error(t.token);
	}

	return i;
}


struct inst
parse_label()
{
	struct tok 	t;
	struct inst	i;

	i.type = I_NAME;

	t = expect(ID);
	i.label = malloc(strlen(t.value) + 1);
	strlcpy(i.label, t.value, strlen(t.value) + 1);
	expect(COLON);

	return i;
}

struct tok
peek()
{
	return tokens[curr];
}


void
syntax_error(Token t)
{
	fprintf(stderr, "Syntax error on line %d, expected token: %d\n",
		line_num, t);
	exit(EXIT_FAILURE);
}
