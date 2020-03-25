/* instruction.c - Processing ARM assembly instructions. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arm.h"
#include "get.h"
#include "lexer.h"
#include "instruction.h"

extern int r[];
extern int a[];


/*
 * Syntax: op{cond}{S} Rd, Rn, Op2
 * ADD, SUB, RSB.
 * AND, ORR, EOR, BIC, and ORN.
 */
int
rd_rn_op2(struct inst in) {
	int	rd, rn, op2;
	Inst	instruction;

	rd = get_reg(in.op[0].value);
	rn = get_reg(in.op[1].value);
	instruction = get_inst(in.mnemonic);

	if (in.op[2].token == IMM)
		op2 = get_imm(in.op[2]);
	else if (in.op[2].token == ID) {
		op2 = get_reg(in.op[2].value);
		op2 = r[op2];
	}
	else {
		fprintf(stderr, "Invalid third operand in ADD / SUB: %d.\n",
			in.op[2].token);
		exit(EXIT_FAILURE);
	}

	if (instruction == ADD)
		r[rd] = r[rn] + op2;
	else if (instruction == AND)
		r[rd] = r[rn] & op2;
	else if (instruction == BIC)
		r[rd] = r[rn] & (~op2);
	else if (instruction == EOR)
		r[rd] = r[rn] ^ op2;
	else if (instruction == MUL)
		r[rd] = r[rn] * op2;
	else if (instruction == ORN)
		r[rd] = r[rn] | (~op2);
	else if (instruction == ORR)
		r[rd] = r[rn] | op2;
	else if (instruction == RSB)
		r[rd] = op2 - r[rn];
	else if (instruction == SUB)
		r[rd] = r[rn] - op2;
	else {
		fprintf(stderr, "Invalid Rd, Rn, Op2 instruction: %s\n",
			in.mnemonic.value);
		exit(EXIT_FAILURE);
	}
	/*** TODO: HANDLE CARRY INSTRUCTION ADC, SBC, RSC. ***/

	return 0;
}

/*
 * Syntax: OP{S}{cond} Rd, Op2
 * CMP, CMN.
 * MOV, MOVN.
 */
int
rd_op2(struct inst in)
{
	int	rd, op2, val;
	Inst	instruction;

	rd = get_reg(in.op[0].value);
	instruction = get_inst(in.mnemonic);

	if (in.op[1].token == IMM)
		op2 = get_imm(in.op[1]);
	else if (in.op[1].token == ID) {
		op2 = get_reg(in.op[1].value);
		op2 = r[op2];
	}

	if (instruction == CMP)
		val = r[rd] - op2;
	else if (instruction == CMN)
		val = r[rd] + op2;
	else if (instruction == MOV)
		r[rd] = op2;
	else if (instruction == MVN)
		r[rd] = (~ op2);
	else {
		fprintf(stderr, "Invalid opcode\n");
		exit(EXIT_FAILURE);
	}

	if (instruction == CMP || instruction == CMN) {
		/* Set Flags: N, Z, C, V. */
		/*** TODO: Carry and overflow flags. */
		r[APSR] = 0;
		if (val < 0)
			r[APSR] = r[APSR] | NFLAG;
		else if (val == 0)
			r[APSR] = r[APSR] | ZFLAG;
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
	Inst	instruction;

	instruction = get_inst(in.mnemonic);
	rd = get_reg(in.op[0].value);
	rn = get_reg(in.op[1].value);
	rm = get_reg(in.op[2].value);
	ra = get_reg(in.op[3].value);

	if (instruction == MLA)
		r[rd] = r[rn] * r[rm] + r[ra];
	else if (instruction == MLS)
		r[rd] = r[rn] * r[rm] - r[ra];
	else {
		fprintf(stderr, "Invalid opcode\n");
		exit(EXIT_FAILURE);
	}

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
	int	*reglist;
	Inst	instruction;

	instruction = get_inst(in.mnemonic);
	reglist = get_reglist(in.op[0]);


	if (instruction != PUSH && instruction != POP) {
		fprintf(stderr, "Invalid PUSH/POP instruction: %s\n",
				in.mnemonic.value);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; reglist[i] != -1 && i < MAXREG; i++) {
		reg_num = reglist[i];
		if (instruction == PUSH) {
			r[SP]--;
			a[r[SP]] = r[reg_num];
			printf("MEM[%d] = r[%d]\n", r[SP], reg_num);
		}
		else if (instruction == POP) {
			r[reg_num] = a[r[SP]];
			r[SP]++;
			printf("r[%d] = MEM[%d]\n", reg_num, r[SP]);
		}

		if (r[SP] >= ADDRSPACE_SIZE) {
			fprintf(stderr, "Stack overflow\n");
			exit(EXIT_FAILURE);
		}
	}

	free(reglist);

	return 0;
}
