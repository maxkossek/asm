/* get - Parse input tokens and retrieve meaningful values. */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"
#include "get.h"
#include "parser.h"

/*
 * get_addr - Get the address value from an input string of form:
 * "[Rn, Rm {, LSL #n}]".
 */ 
int
get_addr(struct tok t)
{
	int	addr, lsl = 0, rm, rn;
	char	*str = t.value;

	if (*str++ != '[') {
		fprintf(stderr, "Invalid address, must start with a '['.\n");
		exit(EXIT_FAILURE);
	}
	rn = get_reg(str);
	addr = r[rn];
	while (*str != ',' && *str != ']' && *str != '\0')
		str++;

	if (*str == ',') {
		do {
			str++;
		} while (*str == ' ');
		if (*str == 'r')
			rm = get_reg(str);
		else {
			fprintf(stderr, "Invalid RM register in: %s\n",
			t.value);
			exit(EXIT_FAILURE);
		}
	
		while (*str != ',' && *str != ']' && *str != '\0')
			str++;

		if (*str == ',') {
			do {
				str++;
			} while (*str == ' ');
			if (*str == 'L')
				lsl = get_shift(str);
			else {
				fprintf(stderr, "Invalid LSL shift in: %s\n",
						t.value);
				exit(EXIT_FAILURE);
			}
		}

		addr += r[rm] << lsl;
	}

	if (addr > ADDRSPACE_SIZE || addr < 0) {
		fprintf(stderr, "Invalid address %d generated from: %s\n",
			addr, t.value);
		exit(EXIT_FAILURE);
	}

	return addr;
}

/* get_expr - Get the value from an expression of the form: "=num". */
int
get_expr(struct tok t)
{
	int	n = 0, i = 0;
	char	*str = t.value;

	if (str[i++] != '=') {
		fprintf(stderr, "Invalid expression, must start with a '='.\n");
		exit(EXIT_FAILURE);
	}
	if (str[1] == '0' && str[2] == 'x')
		n = get_hex(str + 3);
	else
		n = get_int(str + 1);

	return n;
}

/* get_hex - Return the hexadecimal value from an input string. */
int
get_hex(char *str)
{
	int	num = 0;
	char	c;

	c = *str;

	do {
		num *= 16;
		if (c >= '0' && c <= '9')
			num += c - 48;
		else if (c >= 'a' && c <= 'f')
			num += c - 87;
		else if (c >= 'A' && c <= 'F')
			num += c - 55;
		else {
			fprintf(stderr, "Invalid hex character: '%c'. "
				"Must be '0'-'9' or 'a'-'f'\n", c);
			exit(EXIT_FAILURE);
		}
		c = *++str;
	} while (c != '\0');

	return num;
}

/* get_imm - Get the numeric value of an immediate value. */
int
get_imm(char *str)
{
	int	n = 0;

	if (str[0] != '#') {
		fprintf(stderr, "Invalid immediate, must start with a '#'.\n");
		exit(EXIT_FAILURE);
	}

	if (str[1] == '0' && str[2] == 'x')
		n = get_hex(str + 3);
	else
		n = get_int(str + 1);

	return n;
}

/* get_inst - Find the instruction given an input string. */
Inst
get_inst(struct tok t)
{
	Inst inst = ERR;
	char *str = t.value;

	if (t.token != ID) {
		fprintf(stderr, "Can't get instruction from a non-ID token\n");
		exit(EXIT_FAILURE);
	}

	if (str[0] == 'A') {
		if (strcmp(str, "ADC") == 0)
			return ADC;
		else if (strcmp(str, "ADD") == 0)
			return ADD;
		else if (strcmp(str, "AND") == 0)
			return AND;
	}
	else if (str[0] == 'B') {
		if (strcmp(str, "BIC") == 0)
			return BIC;
	}
	else if (str[0] == 'C') {
		if (strcmp(str, "CMP") == 0)
			return CMP;
		if (strcmp(str, "CMN") == 0)
			return CMN;
	}
	else if (str[0] == 'E') {
		if (strcmp(str, "EOR") == 0)
			return EOR;
	}
	else if (str[0] == 'L') {
		if (strcmp(str, "LDR") == 0)
			return LDR;
	}
	else if (str[0] == 'M') {
		if (strcmp(str, "MLA") == 0)
			return MLA;
		if (strcmp(str, "MLS") == 0)
			return MLS;
		else if (strcmp(str, "MOV") == 0)
			return MOV;
		else if (strcmp(str, "MUL") == 0)
			return MUL;
		else if (strcmp(str, "MVN") == 0)
			return MVN;
	}
	else if (str[0] == 'O') {
		if (strcmp(str, "ORN") == 0)
			return ORN;
		if (strcmp(str, "ORR") == 0)
			return ORR;
	}
	else if (str[0] == 'P') {
		if (strcmp(str, "PUSH") == 0)
			return PUSH;
		else if (strcmp(str, "POP") == 0)
			return POP;
	}
	else if (str[0] == 'S') {
		if (strcmp(str, "SUB") == 0)
			return SUB;
		if (strcmp(str, "STR") == 0)
			return STR;
	}

	if (inst == ERR) {
		fprintf(stderr, "Invalid instruction opcode: %s\n", str);
		exit(EXIT_FAILURE);
	}

	return inst;
}

/* get_int - Get the integer value from an input string. */
int
get_int(char *str)
{
	int	num = 0;
	char	c;

	c = *str;

	do {
		num *= 10;
		if (c >= '0' && c <= '9')
			num += c - 48;
		else {
			fprintf(stderr, "Invalid number: %s.", str);
			exit(EXIT_FAILURE);
		}
		c = *++str;
	} while (c != '\0' && num < UINT_MAX);

	if (num < 0 || num > UINT_MAX) {
		fprintf(stderr, "Invalid number: %s\n", str);
		exit(EXIT_FAILURE);
	}

	return num;
}

/* get_reg - Get the numeric value of register. */
int
get_reg(char *str)
{
	int	n;
	int	separator;

	if (str[0] != 'r') {
		fprintf(stderr, "String '%s' not a register. Must being with a "
			"'r' character, not '%c'.\n", str, str[0]);
		exit(EXIT_FAILURE);
	}
	if (isdigit(str[1])) {
		n = str[1] - '0';
		separator = 2;
	}
	if (isdigit(str[1]) && isdigit(str[2])) {
		n *= 10;
		n += (str[2] - '0');
		separator = 3;
	}
	if (str[separator] != '\0' && str[separator] != ','
		&& str[separator] != '-' && str[separator] != '}'
		&& str[separator] != ']') {
		fprintf(stderr, "Invalid register: %s\n", str);
		exit(EXIT_FAILURE);
	}
		
	if (n < 0 || n > MAXREG) {
		fprintf(stderr, "Register '%s' is out of range (0-%d).\n",
			str, MAXREG);
		exit(EXIT_FAILURE);
	}

	return n;
}


int *
get_reglist (struct tok t)
{
	int	*reglist = malloc(REGLIST);
	int	reg_end;
	int	i = 0;
	char	*str = t.value;

	if (*str++ != '{') {
		fprintf(stderr, "Invalid reglist, must start with a '{'.\n");
		exit(EXIT_FAILURE);
	}
	while (*str != '}' && *str != '\0' && i < REGLIST) {
		reglist[i++] = get_reg(str);
		while (isalnum(*str))
			str++;
		/* Get range of register values (e.g. r1-r4). */
		if (*str == '-') {
			str++;
			reg_end = get_reg(str);
			if (reg_end > reglist[i - 1]) {
				for (int n = reglist[i - 1] + 1; n < reg_end
					&& i < REGLIST; n++)
					reglist[i++] = n;
			}
			else {
				fprintf(stderr, "Invalid reglist range: %s\n",
					t.value);
				exit(EXIT_FAILURE);
			}
		}
		if (*str == ',')
			str++;
	}
	reglist[i] = -1;

	if (*str != '}') {
		fprintf(stderr, "Invalid reglist, must end with a '}'.\n");
		exit(EXIT_FAILURE);
	}

	return reglist;
}

/* get_shift - Get the numeric value of a shift. */
int
get_shift(char *str)
{
	int	shift = -1;

	if (str[0] != 'L' || str[1] != 'S' || str[2] != 'L') {
		fprintf(stderr, "Invalid LSL shift: %s.\n", str);
		exit(EXIT_FAILURE);
	}
	str += 3;
	while (*str == ' ')
		str++;

	if (*str == '#' && isdigit(*(str + 1)) && *(str + 2) == ']')
		shift = *(str + 1) - '0';
	else {
		fprintf(stderr, "Invalid LSL shift: %s.\n", str);
		exit(EXIT_FAILURE);
	}

	if (shift < 0 || shift > 3) {
		fprintf(stderr, "Shift amount '%d' is out of range (0-3).\n",
			shift);
		exit(EXIT_FAILURE);
	}

	return shift;
}
