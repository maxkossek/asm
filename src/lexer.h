#ifndef LEXER_H
#define LEXER_H

#define ID_SIZE 32
#define MAXTOKEN 256

typedef enum {
	ADDR, BRACKETL, BRACKETR, COLON, COMMA,
	EXPR, END, ENDOFFILE, ENTRY, ID, IMM, NEWLINE, NUM, REGLIST, TAB
} Token;

struct tok {
	Token	token;
	char	*value;
};

static int	count = 0;
struct tok 	tokens[MAXTOKEN];

int		lex_input(void);
struct tok	gettok(void);
struct tok	parse_id(void);
char		*parse_addr(void);
char		*parse_num(void);
char		*parse_reglist(void);

#endif
