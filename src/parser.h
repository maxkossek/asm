#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define MAXINST 200
#define MAXOP 4

struct inst {
	struct tok	mnemonic;
	int		op_count;
	struct tok	op[4];
};


struct inst	addre[MAXINST];
static int	addre_size = 0;
static int	curr = 0;
static int	line_num = 0;

void		syntax_error(Token t);
struct tok	peek();
struct tok	get_token();
struct tok	expect(Token t);
int		parse();
int		execute();
struct inst	parse_instruction();

#endif
