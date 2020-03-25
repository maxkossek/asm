/* arm - ARM assembly language interpreter. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arm.h"
#include "lexer.h"
#include "parser.h"
/*
#include "instruction.h"
*/

/* Register and address space arrays. */
int	r[REGCOUNT] = { 0 };
int	a[ADDRSPACE_SIZE + 1] = { 0 };

int
main(int argc, char *argv[])
{
	int	n = 0;
	r[SP] = 999;
	a[999] = argc;

	n = lex_input();

	parse();
	execute();

	printf("\nREGISTERS:\n");
	for (int i = 0; i < (sizeof(r) / sizeof(int)); i++) {
		if (i == SP)
			printf("SP\t= %d\n", r[i]);
		else if (i == LR)
			printf("LR\t= %d\n", r[i]);
		else if (i == PC)
			printf("PC\t= %d\n", r[i]);
		else if (i == APSR)
			printf("APSR\t= %#0x\n", r[i]);
		else
			printf("r[%d]\t= %d\n", i, r[i]);
	}

	printf("\nSTACK:\n");
	for (int i = r[SP]; i <= ADDRSPACE_SIZE; i++)
		printf("%#04x (%d): %#04x\n", i, i, a[i]);

	return 0;
}
