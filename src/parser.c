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

/*
 * check_cond - Check the condition of the instruction against the current 
 * state of the flags. Return 0 if the instruction is to be executed, else -1.
 */
int
check_cond(cond cond)
{
	int n_curr, z_curr, c_curr, v_curr;
	n_curr = r[APSR] & NFLAG;
	z_curr = r[APSR] & ZFLAG;
	c_curr = r[APSR] & CFLAG;
	v_curr = r[APSR] & VFLAG;

	switch (cond) {
	case AL:
		if (cond == AL)
			return 0;
		break;
	case CC:
		if (c_curr != CFLAG)
			return 0;
		break;
	case CS:
		if (c_curr == CFLAG)
			return 0;
		break;
	case EQ:
		if (z_curr == ZFLAG)
			return 0;
		break;
	case GE:
		if ((z_curr == ZFLAG && v_curr == VFLAG)
			|| (z_curr != ZFLAG && v_curr != VFLAG))
			return 0;
		break;
	case GT:
		if (z_curr != ZFLAG && ((z_curr == ZFLAG && v_curr == VFLAG)
			|| (z_curr != ZFLAG && v_curr != VFLAG)))
			return 0;
		break;
	case HI:
		if (c_curr == CFLAG && z_curr != ZFLAG)
			return 0;
		break;
	case HS:
		if (c_curr == CFLAG)
			return 0;
		break;
	case LE:
		if (z_curr == ZFLAG && ((n_curr != NFLAG && v_curr == VFLAG)
			|| (n_curr == NFLAG && v_curr != VFLAG)))
			return 0;
		break;
	case LO:
		if (c_curr != CFLAG)
			return 0;
		break;
	case LS:
		if (c_curr == CFLAG || z_curr == ZFLAG)
			return 0;
		break;
	case LT:
		if ((n_curr != NFLAG && v_curr == VFLAG)
			|| (n_curr == NFLAG && v_curr != VFLAG))
			return 0;
		break;
	case MI:
		if (n_curr == NFLAG)
			return 0;
		break;
	case NE:
		if (z_curr != ZFLAG)
			return 0;
		break;
	case PL:
		if (n_curr != NFLAG)
			return 0;
		break;
	case VC:
		if (v_curr != VFLAG)
			return 0;
		break;
	case VS:
		if (v_curr == VFLAG)
			return 0;
		break;
	default:
		fprintf(stderr, "Unknown condition code.\n");
		exit(EXIT_FAILURE);
	}
	

	return -1;
}


int
execute()
{
	int		pc;
	inst_type	type;
	r[PC] = 0;

	for ( ; r[PC] < addre_size; r[PC]++) {
		pc = r[PC];
		type = addre[pc].type;
		if (check_cond(addre[pc].cond) < 0)
			continue;

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

/* get_inst - Find the instruction given an input string. */
inst
get_inst(char *str, int *flag, int *cond)
{
	inst inst = NONE;
	char *ptr = str;

	if (str[0] == 'A') {
		if (strncmp(str, "ADC", 3) == 0) {
			inst = ADC;
			ptr += 3;
		}
		else if (strncmp(str, "ADD", 3) == 0) {
			inst = ADD;
			ptr += 3;
		}
		else if (strncmp(str, "AND", 3) == 0) {
			inst = AND;
			ptr += 3;
		}
	}
	else if (str[0] == 'B') {
		if (strncmp(str, "BIC", 3) == 0) {
			inst = BIC;
			ptr += 3;
		}
		else if (strncmp(str, "BL", 3) == 0) {
			inst = BL;
			ptr += 2;
		}
		else if (strncmp(str, "B", 1) == 0) {
			inst = B;
			ptr += 1;
		}
	}
	else if (str[0] == 'C') {
		if (strncmp(str, "CMP", 3) == 0) {
			inst = CMP;
			ptr += 3;
		}
		else if (strncmp(str, "CMN", 3) == 0) {
			inst = CMN;
			ptr += 3;
		}
	}
	else if (str[0] == 'E') {
		if (strncmp(str, "EOR", 3) == 0) {
			inst = EOR;
			ptr += 3;
		}
	}
	else if (str[0] == 'L') {
		if (strncmp(str, "LDR", 3) == 0) {
			inst = LDR;
			ptr += 3;
		}
	}
	else if (str[0] == 'M') {
		if (strncmp(str, "MLA", 3) == 0) {
			inst = MLA;
			ptr += 3;
		}
		else if (strncmp(str, "MLS", 3) == 0) {
			inst = MLS;
			ptr += 3;
		}
		else if (strncmp(str, "MOV", 3) == 0) {
			inst = MOV;
			ptr += 3;
		}
		else if (strncmp(str, "MUL", 3) == 0) {
			inst = MUL;
			ptr += 3;
		}
		else if (strncmp(str, "MVN", 3) == 0) {
			inst = MVN;
			ptr += 3;
		}
	}
	else if (str[0] == 'O') {
		if (strncmp(str, "ORN", 3) == 0) {
			inst = ORN;
			ptr += 3;
		}
		else if (strncmp(str, "ORR", 3) == 0) {
			inst = ORR;
			ptr += 3;
		}
	}
	else if (str[0] == 'P') {
		if (strncmp(str, "PUSH", 4) == 0) {
			inst = PUSH;
			ptr += 4;
		}
		else if (strncmp(str, "POP", 3) == 0) {
			inst = POP;
			ptr += 3;
		}
	}
	else if (str[0] == 'R') {
		if (strncmp(str, "RSB", 3) == 0) {
			inst = RSB;
			ptr += 3;
		}
		else if (strncmp(str, "RSC", 3) == 0) {
			inst = RSC;
			ptr += 3;
		}
	}
	else if (str[0] == 'S') {
		if (strncmp(str, "SBC", 3) == 0) {
			inst = SUB;
			ptr += 3;
		}
		else if (strncmp(str, "SUB", 3) == 0) {
			inst = SUB;
			ptr += 3;
		}
		else if (strncmp(str, "STR", 3) == 0) {
			inst = STR;
			ptr += 3;
		}
	}
	else if (str[0] == 'T') {
		if (strncmp(str, "TEQ", 3) == 0) {
			inst = TEQ;
			ptr += 3;
		}
		else if (strncmp(str, "TST", 3) == 0) {
			inst = TST;
			ptr += 3;
		}
	}

	if (get_cond(ptr, flag, cond) == -1 || inst == NONE) {
		fprintf(stderr, "Invalid instruction mnemonic: %s\n", str);
		exit(EXIT_FAILURE);
	}

	return inst;
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
	char		buf[16];

	i.cond = AL;
	i.setflag = 0;
	i.type = I_NONE;
	i.op2_type = V_REG;

	t = expect(TAB);
	t = expect(ID);
	i.mnemonic = get_inst(t.value, &i.setflag, (int *) &i.cond);
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
		if (get_reglist(t.value, &i.reglist) < 0)
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
			i.op2_type = V_IMM;
		}
		else if (t.token == ID) {
			i.type = I_RD_OP2;
			i.op1 = get_reg(t.value);
			if (i.op1 < 0)
				fprintf(stderr, "Invalid register: %s.\n",
					t.value);
			i.op2 = i.op1;
			i.op2_type = V_REG;
		}
		else if (t.token == EXPR) {
			i.type = I_RD_EXPR;
			i.op1 = get_expr(t.value);
		}
		else if (t.token == ADDR) {
			i.type = I_RT_ADDR;
			i.shift_method = S_LSL;
			if (get_addr(t.value, &i.op1, &i.op2, &i.shift,
				&i.shift_method, &i.shift_type) < 0)
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
			i.op2_type = V_IMM;
		}
		else if (t.token == ID) {
			i.type = I_RD_RN_OP2;
			i.op2 = get_reg(t.value);
			i.op2_type = V_REG;
			if (i.op2 < 0) {
				i.type = I_RD_OP2;
				i.op2_type = V_SHIFT;
				strlcpy(buf, t.value, sizeof(buf));
				t = get_token();
				strlcat(buf, t.value, sizeof(buf));
				i.shift = get_shift(buf, &i.shift,
					&i.shift_method, &i.shift_type);
				if (i.shift < 0)
					syntax_error(t.token);
			}
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

			if (i.op3 < 0) {
				i.type = I_RD_RN_OP2;
				i.op2_type = V_SHIFT;
				strlcpy(buf, t.value, sizeof(buf));
				t = get_token();
				strlcat(buf, t.value, sizeof(buf));
				i.shift = get_shift(buf, &i.shift,
					&i.shift_method, &i.shift_type);
				if (i.shift < 0)
					syntax_error(t.token);
			}
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

void
unget_token()
{
	curr--;
}
