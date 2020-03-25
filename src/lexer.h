#ifndef LEXER_H
#define LEXER_H

#define ID_SIZE 32
#define TOKENSIZE 256

typedef enum {
	ADDR, BRACKETL, BRACKETR, COLON, COMMA,
	END, ENDOFFILE, ENTRY, ID, IMM, NEWLINE, NUM, REGLIST, TAB
} Token;

struct tok {
	Token	token;
	char	*value;
};

static int	count = 0;
struct tok 	tokens[TOKENSIZE];

int		lex_input();
struct tok	gettok();
struct tok	parse_id();
char		*parse_num();
char		*parse_reglist();

#endif
