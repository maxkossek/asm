/* lexer - Scans the input and divides it into tokens. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

int
lex_input()
{
	struct tok t;
	count = 0;

	while ((t = gettok()).token != ENDOFFILE) {
		tokens[count++] = t;
		/* PRINT TOKENS FOR DEBUGGING
		printf("[%d", t.token);
		if (t.value != NULL)
			printf(" = %s", t.value);
		printf("] ");
		if (t.token == NEWLINE)
			printf("\n");
		*/
	}
	t.token = ENDOFFILE;
	t.value = NULL;
	tokens[count] = t;

	return 0;
}

/* gettok - Returns the next token from the input. */
struct tok
gettok()
{
	struct tok	t;
	char		c = ' ';

	t.token = ENDOFFILE;
	t.value = NULL;

	while (c == ' ')
		c = getchar();

	if (c == EOF) {
		t.token = ENDOFFILE;
		return t;
	}
	else if (isalpha(c)) {
		ungetc(c, stdin);
		t = parse_id();
	}
	else if (isdigit(c)) {
		ungetc(c, stdin);
		t.token = NUM;
		t.value = parse_num();
	}
	else if (c == ',')
		t.token = COMMA;
	else if (c == ':')
		t.token = COLON;
	else if (c == '#') {
		if (isdigit(c = getchar())) {
			ungetc(c, stdin);
			ungetc('#', stdin);
			t.value = parse_num();
			t.token = IMM;
		}
		else {
			while (c != '\n')
				c = getchar();
			t = gettok();
		}
	}
	else if (c == '=') {
		ungetc(c, stdin);
		t.value = parse_num();
		t.token = EXPR;
	}
	else if (c == '[') {
		ungetc(c, stdin);
		t.value = parse_addr();
		t.token = ADDR;
	}
	else if (c == ']')
		t.token = BRACKETR;
	else if (c == '{') {
		ungetc(c, stdin);
		t.value = parse_reglist();
		t.token = REGLIST;
	}
	else if (c == '\n')
		t.token = NEWLINE;
	else if (c == '\t')
		t.token = TAB;
	else if (c == '@') {
		while (c != '\n')
			c = getchar();
		t = gettok();
	}
	else {
		fprintf(stderr, "Unknown token: %c", c);
		exit(EXIT_FAILURE);
	}

	return t;
}

struct tok
parse_id()
{
	struct tok	t;
	int		size = 0;
	char		id[ID_SIZE];
	char		*ptr;
	char		c;

	t.token = ID;
	t.value = NULL;

	c = getchar();
	if (isalnum(c))
		id[size++] = c;
	else {
		fprintf(stderr, "Error parsing id.\n");
		exit(EXIT_FAILURE);
	}
	c = getchar();
	while (isalnum(c) && size < ID_SIZE) {
		id[size++] = c;
		c = getchar();
	}
	ungetc(c, stdin);
	id[size] = '\0';

	if (size == ID_SIZE) {
		fprintf(stderr, "Identifier size overflow.\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp(id, "ENTRY") == 0)
		t.token = ENTRY;
	else if (strcmp(id, "END") == 0)
		t.token = END;
	else {
		ptr = malloc(size + 1);
		strlcpy(ptr, id, size + 1);
		t.value = ptr;
	}

	return t;
}

/* parse_addr - Parses an address of the form '[ADDR]'. */
char *
parse_addr()
{
	int		size = 0;
	char		c;
	char		str[ID_SIZE];
	char		*ptr;

	c = getchar();
	if (c == '[')
		str[size++] = c;
	else {
		fprintf(stderr, "Error parsing address.\n");
		exit(EXIT_FAILURE);
	}
	c = getchar();
	while (c != ']' && size < ID_SIZE - 1) {
		str[size++] = c;
		c = getchar();
	}
	str[size++] = c;
	str[size] = '\0';

	if (size == ID_SIZE) {
		fprintf(stderr, "Address size overflow.\n");
		exit(EXIT_FAILURE);
	}

	ptr = malloc(size + 1);
	strlcpy(ptr, str, size + 1);

	return ptr;
}

char *
parse_num()
{
	int		size = 0;
	char		c;
	char		str[ID_SIZE];
	char		*ptr;

	c = getchar();
	if (isdigit(c) || c == '#' || c == '=')
		str[size++] = c;
	else {
		fprintf(stderr, "Error parsing number.\n");
		exit(EXIT_FAILURE);
	}
	c = getchar();
	while (isalnum(c) && size < ID_SIZE) {
		str[size++] = c;
		c = getchar();
	}
	str[size] = '\0';
	ungetc(c, stdin);

	if (size == ID_SIZE) {
		fprintf(stderr, "Number size overflow.\n");
		exit(EXIT_FAILURE);
	}

	ptr = malloc(size + 1);
	strlcpy(ptr, str, size + 1);

	return ptr;
}


char *
parse_reglist()
{
	int		size = 0;
	char		c;
	char		str[ID_SIZE];
	char		*ptr;

	c = getchar();
	if (c == '{')
		str[size++] = c;
	else {
		fprintf(stderr, "Error parsing register list.\n");
		exit(EXIT_FAILURE);
	}
	c = getchar();
	while (c != '}' && size < ID_SIZE - 1) {
		str[size++] = c;
		c = getchar();
	}
	str[size++] = c;
	str[size] = '\0';

	if (size == ID_SIZE) {
		fprintf(stderr, "Number size overflow.\n");
		exit(EXIT_FAILURE);
	}

	ptr = malloc(size + 1);
	strlcpy(ptr, str, size + 1);

	return ptr;
}
