#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define MAXINST 200
#define MAXOP 4

struct inst {
	int		addr;
	struct tok	mnemonic;
	int		op_count;
	struct tok	op[4];
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
