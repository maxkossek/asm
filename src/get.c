/* get - Parse input tokens and retrieve meaningful values. */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get.h"

/*
 * get_addr - Get the address value from an input string of form:
 * "[Rn, Rm {, LSL #n}]".
 */ 
int
get_addr(char *str, int *rn, int *rm, int *shift,
	shift_method *shift_method, val_type *shift_type)
{
	*rn = -1;
	*rm = -1;
	*shift = 0;
	*shift_method = S_NONE;
	
	if (*str++ != '[') {
		fprintf(stderr, "Invalid address: %s. Must start with a '['.\n"
			, str);
		return -1;
	}
	*rn = get_reg(str);
	while (*str != ',' && *str != ']' && *str != '\0')
		str++;

	if (*str == ',') {
		do {
			str++;
		} while (*str == ' ');
		if (*str == 'r')
			*rm = get_reg(str);
		else {
			fprintf(stderr, "Invalid Rm register: %s\n", str);
			return -1;
		}
	}
	
	while (*str != ',' && *str != ']' && *str != '\0')
		str++;

	if (*str == ',') {
		do {
			str++;
		} while (*str == ' ');
		if (*str == 'L') {
			if (get_shift(str, shift, shift_method,
				shift_type) < 0 ||
				*shift_method != S_LSL) {
				fprintf(stderr, "Invalid non-LSL shift in"
					" addr.\n");
				return -1;
			}
			if (*shift < 0 || *shift > 3) {
				fprintf(stderr, "Shift amount '%d' is "
				"out of range (0-3).\n", *shift);
				return -1;
			}
		}
		else {
			fprintf(stderr, "Invalid LSL shift in: %s\n",
					str);
			return -1;
		}
	}

	return 0;
}

/* get_cond - Get the setflag and condition suffix from an instruction. */
int
get_cond(char *str, int *flag, int *cond)
{
	/* No suffix. */
	if (*str == '\0')
		return 0;
	/* Set flag. */
	else if (*str == 'S') {
		*flag = 1;
		str++;
	}

	if (*str == '\0')
		return 0;
	else if (*str == 'A') {
		if (strcmp(str, "AL") == 0)
			*cond = AL;
	}
	else if (*str == 'C') {
		if (strcmp(str, "CC") == 0)
			*cond = CC;
		else if (strcmp(str, "CS") == 0)
			*cond = CS;
	}
	else if (*str == 'E') {
		if (strcmp(str, "EQ") == 0)
			*cond = EQ;
	}
	else if (*str == 'G') {
		if (strcmp(str, "GE") == 0)
			*cond = GE;
		else if (strcmp(str, "GT") == 0)
			*cond = GT;
	}
	else if (*str == 'H') {
		if (strcmp(str, "HI") == 0)
			*cond = HI;
		else if (strcmp(str, "HS") == 0)
			*cond = HS;
	}
	else if (*str == 'L') {
		if (strcmp(str, "LO") == 0)
			*cond = LO;
		else if (strcmp(str, "LE") == 0)
			*cond = LE;
		else if (strcmp(str, "LS") == 0)
			*cond = LS;
		else if (strcmp(str, "LT") == 0)
			*cond = LT;
	}
	else if (*str == 'M') {
		if (strcmp(str, "MI") == 0)
			*cond = MI;
	}
	else if (*str == 'N') {
		if (strcmp(str, "NE") == 0)
			*cond = NE;
	}
	else if (*str == 'P') {
		if (strcmp(str, "PL") == 0)
			*cond = PL;
	}
	else if (*str == 'V') {
		if (strcmp(str, "VC") == 0)
			*cond = VC;
		else if (strcmp(str, "VS") == 0)
			*cond = VS;
	}
	else
		return -1;

	str += 2;
	/* Set flag. */
	if (*str == 'S') {
		*flag = 1;
		str++;
	}

	if (*str != '\0')
		return -1;

	return 0;
}

/* get_expr - Get the value from an expression of the form: "=num". */
int
get_expr(char *str)
{
	int	n = 0, i = 0;

	if (str[i++] != '=') {
		fprintf(stderr, "Invalid expression, must start with a '='.\n");
		return -1;
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
			return -1;
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
		return -1;
	}

	if (str[1] == '0' && str[2] == 'x')
		n = get_hex(str + 3);
	else
		n = get_int(str + 1);

	return n;
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
			return -1;
		}
		c = *++str;
	} while (c != '\0' && c != ']' && num < UINT_MAX);

	if (num < 0 || num > UINT_MAX) {
		fprintf(stderr, "Invalid number: %s\n", str);
		return -1;
	}

	return num;
}



/* get_reg - Get the numeric value of register. */
int
get_reg(char *str)
{
	int	n;
	int	separator;

	if (str[0] != 'r')
		return -1;
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
		fprintf(stderr, "Invalid register: %s.\n", str);
		exit(EXIT_FAILURE);
	}
		
	if (n < 0 || n > MAXREG) {
		fprintf(stderr, "Register '%s' is out of range (0-%d).\n",
			str, MAXREG);
		exit(EXIT_FAILURE);
	}

	return n;
}


int
get_reglist (char *str, int **reglist)
{
	*reglist = malloc(REGLIST);
	int	reg_end;
	int	i = 0;
	int	*ptr = *reglist;

	if (*str++ != '{') {
		fprintf(stderr, "Invalid reglist, must start with a '{'.\n");
		return -1;
	}
	while (*str != '}' && *str != '\0' && i < REGLIST) {
		ptr[i++] = get_reg(str);
		while (isalnum(*str))
			str++;
		/* Get range of register values (e.g. r1-r4). */
		if (*str == '-') {
			str++;
			reg_end = get_reg(str);
			if (reg_end > ptr[i - 1]) {
				for (int n = ptr[i - 1] + 1; n < reg_end
					&& i < REGLIST; n++)
					ptr[i++] = n;
			}
			else {
				fprintf(stderr, "Invalid reglist range: %s\n",
					str);
				return -1;
			}
		}
		if (*str == ',')
			str++;
	}
	ptr[i] = -1;

	if (*str != '}') {
		fprintf(stderr, "Invalid reglist, must end with a '}'.\n");
		return -1;
	}

	return 0;
}

/* get_shift_amount - Get the numeric value of a shift. */
int
get_shift(char *str, int *shift_amount, shift_method *shift_method,
		val_type *shift_type)
{
	char *ptr = str;

	*shift_method = S_NONE;
	if (strncmp(str, "ASR", 3) == 0)
		*shift_method = S_ASR;
	else if (strncmp(str, "LSL", 3) == 0)
		*shift_method = S_LSL;
	else if (strncmp(str, "LSR", 3) == 0)
		*shift_method = S_LSR;
	else if (strncmp(str, "ROR", 3) == 0)
		*shift_method = S_ROR;
	else if (strncmp(str, "RRX", 3) == 0) {
		*shift_method = S_RRX;
		*shift_amount = 1;
	}
	else {
		fprintf(stderr, "Invalid shift: %s.\n", str);
		return -1;
	}

	ptr += 3;
	while (*ptr == ' ')
		ptr++;

	if (*ptr == '#' && *shift_method != S_RRX) {
		*shift_type = V_IMM;
		*shift_amount = get_imm(ptr);
	}
	else if (*ptr == 'r' && *shift_method != S_RRX) {
		*shift_type = V_REG;
		*shift_amount = get_reg(ptr);
	}
	else {
		fprintf(stderr, "Invalid shift: %s.\n", str);
		return -1;
	}

	if ((*shift_method == S_ASR || *shift_method == S_LSR) &&
		(*shift_amount < 1 || *shift_amount > 32)) {
		fprintf(stderr, "Invalid shift_amount (out of range 1-32): %s.\n",
			str);
		return -1;
	}
	else if (*shift_method == S_LSL && (*shift_amount < 0 || *shift_amount > 31)) {
		fprintf(stderr, "Invalid LSL shift_amount (out of range 0-31): %s.\n",
			str);
		return -1;
	}
	else if (*shift_method == S_ROR && (*shift_amount < 1 || *shift_amount > 31)) {
		fprintf(stderr, "Invalid ROR shift_amount (out of range 0-31): %s.\n",
			str);
		return -1;
	}

	while (isalnum(*++ptr))
		;
	if (*ptr != '\0' && *ptr != ']') {
		fprintf(stderr, "Invalid shift: %s.\n", str);
		return -1;
	}

	return 0;
}
