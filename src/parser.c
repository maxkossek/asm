/* parse - Parsing functions. */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"
#include "get.h"
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
	int	op_count, pc;
	r[PC] = 0;

	for ( ; r[PC] < addre_size; r[PC]++) {
		pc = r[PC];
		op_count = addre[pc].op_count;
		
		if (op_count == 0 && addre[pc].mnemonic.token == ID)
			;
		else if (op_count == 1 && addre[pc].op[0].token == REGLIST)
			reglist(addre[pc]);
		else if (op_count == 1 && addre[pc].op[0].token == ID)
			label(addre[pc]);
		else if (op_count == 2 && addre[pc].op[1].token == ADDR)
			rt_addr(addre[pc]);
		else if (op_count == 2 && addre[pc].op[1].token == EXPR)
			rd_expr(addre[pc]);
		else if (op_count == 2)
			rd_op2(addre[pc]);
		else if (op_count == 3)
			rd_rn_op2(addre[pc]);
		else if (op_count == 4)
			rd_rn_rm_ra(addre[pc]);
		else {
			fprintf(stderr, "Invalid instruction: %s.\n",
				addre[pc].mnemonic.value);
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
	int		count = 0;
	struct tok 	t;
	struct inst	i;

	t = expect(TAB);
	t = expect(ID);
	i.mnemonic = t;
	expect(TAB);

	t = peek();
	if (t.token == ID || t.token == REGLIST)
		i.op[count++] = get_token();
	else
		syntax_error(t.token);

	t = peek();
	while (t.token == COMMA && count <= MAXOP) {
		expect(COMMA);
		t = peek();
		if (t.token == ID || t.token == IMM || t.token == ADDR
			|| t.token == EXPR) {
			t = get_token();
			i.op[count++] = t;
			t = peek();
		}
	}

	if (count > MAXOP) {
		fprintf(stderr, "Operand count overflow\n.");
		exit(EXIT_FAILURE);
	}
	i.op_count = count;

	return i;
}


struct inst
parse_label()
{
	struct tok 	t;
	struct inst	i;

	i.op_count = 0;

	t = expect(ID);
	i.mnemonic = t;
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
