/* asm - ARM assembly language emulator. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"
#include "lexer.h"
#include "parser.h"

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

	printf("- REGISTERS -\n");
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

	printf("\n- MEMORY -\n");
	for (int i = 0; i <= ADDRSPACE_SIZE; i++) {
		if (i == r[SP])
			printf("- STACK -\n");
		if (a[i] != 0 || i > r[SP])
			printf("%#04x (%d): %#04x\n", i, i, a[i]);
	}

	return 0;
}
