/* instruction.c - Processing ARM assembly instructions. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"
#include "instruction.h"

extern int addre_size;

/*
 * Syntax: op{cond} label
 * B, BL.
 */
int
label(struct inst in)
{
	int	label;
	inst	instruction;

	instruction = in.mnemonic;
	label = labeladdr(in.label);

	if (instruction == B)
		r[PC] = label;
	else if (instruction == BL) {
		r[LR] = r[PC];
		r[PC] = label;
	}
	else {
		fprintf(stderr, "Invalid instruction opcode.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* get_labeladdr - Get the address of a label. */
int
labeladdr(char *str)
{
	int	addr = -1;

	for (int i = 0; i < addre_size || addr < 0; i++) {
		if (addre[i].type == I_NAME &&
			strcmp(addre[i].label, str) == 0)
			addr = i;
	}

	if (addr == -1) {
		fprintf(stderr, "Label '%s' not found\n.", str);
		return -1;
	}

	return addr;
}

/*
 * Syntax: op{cond} Rd,=[expr | label-expression]
 * LDR.
 */
int
rd_expr(struct inst in)
{
	int	rd, expr;
	inst	instruction;

	rd = in.dest;
	expr = in.op1;
	instruction = in.mnemonic;

	if (instruction == LDR)
		r[rd] = expr;
	else {
		fprintf(stderr, "Invalid instruction opcode.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}


/*
 * Syntax: op{S}{cond} Rd, Op2
 * CMP, CMN, MOV, MOVN.
 */
int
rd_op2(struct inst in)
{
	int	rd, op2, val;
	inst	instruction;

	instruction = in.mnemonic;
	rd = in.dest;

	if (in.op2_type == V_IMM)
		op2 = in.op2;
	else if (in.op2_type == V_REG || in.op2_type == V_SHIFT) {
		op2 = in.op2;
		/* Value of op2 is PC+8 if r15 is used. */
		if (op2 == PC)
			op2 = r[op2] + 2;
		else
			op2 = r[op2];

		if (in.op2_type == V_SHIFT) {
			shift_apply(&op2, op2, in.shift_method, in.shift_type,
					in.shift, in.setflag);
		}
	}
	else {
		fprintf(stderr, "Unknown operand 2 value type\n.");
		exit(EXIT_FAILURE);
	}

	if (instruction == CMP)
		val = r[rd] - op2;
	else if (instruction == CMN)
		val = r[rd] + op2;
	else if (instruction == MOV)
		r[rd] = op2;
	else if (instruction == MVN)
		r[rd] = (~ op2);
	else if (instruction == TEQ)
		val = r[rd] ^ op2;
	else if (instruction == TST)
		val = r[rd] & op2;
	else {
		fprintf(stderr, "Invalid instruction opcode.\n");
		exit(EXIT_FAILURE);
	}

	if (instruction == CMP)
		set_flags(val, r[rd], -op2);
	else if (instruction == CMN)
		set_flags(val, r[rd], op2);
	else if (instruction == TEQ || instruction == TST)
		set_flags(val, 0, 0);

	return 0;
}

/*
 * Syntax: op{cond}{S} Rd, Rn, Op2
 * ADD, SUB, RSB.
 * AND, ORR, EOR, BIC, and ORN.
 */
int
rd_rn_op2(struct inst in) {
	int	rd, rn, op2;
	inst	instruction;

	instruction = in.mnemonic;
	rd = in.dest;
	rn = in.op1;

	if (in.op2_type == V_IMM)
		op2 = in.op2;
	else if (in.op2_type == V_REG || in.op2_type == V_SHIFT) {
		op2 = in.op2;
		/* Value of op2 is PC+8 if r15 is used. */
		if (op2 == PC)
			op2 = r[op2] + 2;
		else
			op2 = r[op2];

		if (in.op2_type == V_SHIFT) {
			shift_apply(&op2, op2, in.shift_method, in.shift_type,
					in.shift, in.setflag);
		}
	}
	else {
		fprintf(stderr, "Unknown operand 2 value type\n.");
		exit(EXIT_FAILURE);
	}

	if (instruction == ADC) {
		if ((r[APSR] & CFLAG) == CFLAG)
			op2++;
		r[rd] = r[rn] + op2;
	}
	else if (instruction == ADD)
		r[rd] = r[rn] + op2;
	else if (instruction == AND)
		r[rd] = r[rn] & op2;
	else if (instruction == BIC) {
		op2 = ~op2;
		r[rd] = r[rn] & op2;
	}
	else if (instruction == EOR)
		r[rd] = r[rn] ^ op2;
	else if (instruction == MUL)
		r[rd] = r[rn] * op2;
	else if (instruction == ORN) {
		op2 = ~op2;
		r[rd] = r[rn] | op2;
	}
	else if (instruction == ORR)
		r[rd] = r[rn] | op2;
	else if (instruction == RSB)
		r[rd] = op2 - r[rn];
	else if (instruction == RSC) {
		if ((r[APSR] & CFLAG) != CFLAG)
			op2--;
		r[rd] = op2 - r[rn];
	}
	else if (instruction == SBC) {
		if ((r[APSR] & CFLAG) != CFLAG)
			op2--;
		r[rd] = op2 - r[rn];
	}
	else if (instruction == SUB)
		r[rd] = r[rn] - op2;
	else {
		fprintf(stderr, "Invalid Rd, Rn, Op2 instruction.\n");
		exit(EXIT_FAILURE);
	}

	if (in.setflag == 1) {
		if (instruction == SUB || instruction == SBC)
			set_flags(r[rd], r[rn], -op2);
		else if (instruction == RSB || instruction == RSC)
			set_flags(r[rd], op2, -r[rn]);
		else
			set_flags(r[rd], r[rn], op2);
	}

	return 0;
}

/*
 * op{S}{cond} {Rd}, Rn, Rm, Ra.
 * MLA, MLS.
 */
int
rd_rn_rm_ra(struct inst in) {
	int	rd, rn, rm, ra;
	inst	instruction;

	instruction = in.mnemonic;
	rd = in.dest;
	rn = in.op1;
	rm = in.op2;
	ra = in.op3;

	if (instruction == MLA)
		r[rd] = r[rn] * r[rm] + r[ra];
	else if (instruction == MLS)
		r[rd] = r[rn] * r[rm] - r[ra];
	else {
		fprintf(stderr, "Invalid instruction opcode.\n");
		exit(EXIT_FAILURE);
	}

	if (in.setflag == 1 && instruction == MLA)
		set_flags(r[rd], r[rn] * r[rm], r[ra]);

	return 0;
}

/*
 * Syntax: op{cond} reglist
 * PUSH, POP.
 */
int
reglist(struct inst in)
{
	int	reg_num;
	inst	instruction;

	instruction = in.mnemonic;

	if (instruction != PUSH && instruction != POP) {
		fprintf(stderr, "Invalid reglist instruction.\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; in.reglist[i] != -1 && i < MAXREG; i++) {
		reg_num = in.reglist[i];
		if (instruction == PUSH) {
			r[SP]--;
			a[r[SP]] = r[reg_num];
		}
		else if (instruction == POP) {
			r[reg_num] = a[r[SP]];
			r[SP]++;
		}

		if (r[SP] >= ADDRSPACE_SIZE) {
			fprintf(stderr, "Stack overflow\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

/*
 * Syntax: op{type}{cond} Rt, [Rn, Rm {, LSL #n}]
 * LDR, STR.
 */
int
rt_addr(struct inst in)
{
	int	rt, addr;
	inst	instruction;

	instruction = in.mnemonic;
	rt = in.dest;
	if (in.shift_method == S_LSL)
		addr = r[in.op1] + (r[in.op2] << in.shift);
	else if (in.shift_method == S_NONE)
		addr = r[in.op1] + r[in.op2];
	else {
		fprintf(stderr, "Invalid shift type\n");
		exit(EXIT_FAILURE);
	}

	if (addr > ADDRSPACE_SIZE || addr < 0) {
		fprintf(stderr, "Invalid address %d\n", addr);
		exit(EXIT_FAILURE);
	}

	if (instruction == LDR)
		r[rt] = a[addr];
	else if (instruction == STR)
		a[addr] = r[rt];
	else {
		fprintf(stderr, "Invalid instruction opcode.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}


/* set_flags - Set Flags: N, Z, C, V. */
int
set_flags(int result, int op1, int op2)
{
	unsigned uop1, uop2, uresult, carry;
	uop1 = (unsigned int) op1;
	uop2 = (unsigned int) op2;
	uresult = uop1 + uop2;
	carry = uresult < uop1;

	/*** TODO: Carry and overflow flags. */
	if (result < 0)
		r[APSR] = r[APSR] | NFLAG;
	else
		r[APSR] = r[APSR] & (r[APSR] | ~NFLAG);
	if (result == 0)
		r[APSR] = r[APSR] | ZFLAG;
	else
		r[APSR] = r[APSR] & (r[APSR] | ~ZFLAG);
	if (carry > 0)
		r[APSR] = r[APSR] | CFLAG;
	else
		r[APSR] = r[APSR] & (r[APSR] | ~CFLAG);
	if ((op1 > 0 && op2 > 0 && result < 0)
		|| (op1 < 0 && op2 < 0 && result > 0))
		r[APSR] = r[APSR] | VFLAG;
	else
		r[APSR] = r[APSR] & (r[APSR] | ~VFLAG);

	return 0;
}

int
shift_apply(int *result, int op2, shift_method s_method, val_type v_type,
	int shift_val, int set_flag)
{
	int shift_amount;

	if (v_type == V_IMM)
		shift_amount = shift_val;
	else if (v_type == V_REG) {
		/* Value of op2 is PC+8 if r15 is used. */
		if (shift_val == PC)
			shift_amount = r[shift_val] + 2;
		else
			shift_amount = r[shift_val];
	}
	else {
		fprintf(stderr, "Unknown shift value type\n.");
		exit(EXIT_FAILURE);
	}

	if (s_method == S_ASR) {
		if (shift_amount > 32 || shift_amount < 1) {
			fprintf(stderr, "ASR shift out of range (1-32).\n");
			return -1;
		}
		*result = op2 >> shift_amount;
	}
	else if (s_method == S_LSL) {
		if (shift_amount > 31 || shift_amount < 0) {
			fprintf(stderr, "LSR shift out of range (0-31).\n");
			return -1;
		}
		*result = op2 << shift_amount;
	}
	else if (s_method == S_LSR) {
		if (shift_amount > 32 || shift_amount < 1) {
			fprintf(stderr, "LSR shift out of range (1-32).\n");
			return -1;
		}
		*result = op2 >> shift_amount;
	}
	else if (s_method == S_ROR) {
		if (shift_amount > 31 || shift_amount < 1) {
			fprintf(stderr, "ROR shift out of range (1-31).\n");
			return -1;
		}
		*result = (op2 >> shift_amount % 32) |
			(op2 << (32 - shift_amount) % 32);
	}
	else if (s_method == S_RRX) {
		if (shift_amount != 1) {
			fprintf(stderr, "RRX shift value not 1.\n");
			return -1;
		}
		*result = op2 >> 1;
		if (set_flag == 1) {
			/* Set carry flag if bit[0] is set. */
			if ((op2 & 0x1) == 0x1)
				r[APSR] = r[APSR] | CFLAG;
			else
				r[APSR] = r[APSR] & (r[APSR] | ~CFLAG);
		}
	}
	else {
		fprintf(stderr, "Unknown shift method specified.\n");
		return -1;
	}

	return 0;
}
