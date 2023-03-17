#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

/* sorted most used first (%d, %u, %s, %p, %x, %e, %i) */
const char fmt_specs[] = "duspxeiSXEbBofgGaAcCnm";
const char *const header_suffix = "(): ";

static int isfmtspec(char c) {
	for (const char *o = fmt_specs; *o; o++)
		if (*o == c)
			return 1;
	return 0;
}

static int index_of_fmt_spec(const char *const fmt, int len, int j) {
	for (; j < len; j++) {
		/* seek for valid fmt spec*/
		if (isfmtspec(fmt[j]))
			return j;

		/* halt on white space and skip past it */
		if (isspace(fmt[j]))
			return -j;
	}

	return -1;
}

void pretty_vprintf(FILE *file, const char *const func, const char *const base_color,
					const char *const arg_color, const char *const fmt, ...) {
	/* static variables are baked into the binary and do not take up stack space */
#define BUFFER_SIZE 1 << 16
	static char buffer[BUFFER_SIZE];

	int len = strlen(fmt), rlen, roff = 0, woff = 0;
	int bcol_len = strlen(base_color);
	int arg_len = strlen(arg_color);
	int func_len = strlen(func);

	if (func_len > 20)
		/* truncate */
		func_len = 20;

	/* write arg color */
	memcpy(buffer + woff, arg_color, arg_len);
	woff += arg_len;

	/* write func name */
	memcpy(buffer + woff, func, func_len);
	woff += func_len;

	/* write base color */
	memcpy(buffer + woff, base_color, bcol_len);
	woff += bcol_len;

	/* write `(): `*/
	memcpy(buffer + woff, "(): ", 4);
	woff += 4;

	/* write padding */
	memset(buffer + woff, ' ', 24 - (func_len + 4));
	woff += 24 - (func_len + 4);

	/* prepend and append correct color codes to all fmt specifiers */
	for (int i = 0; i < len - 1; i++) {
		if (fmt[i] != '%')
			continue;

		int j = index_of_fmt_spec(fmt, len, i);

		if (j == -1)
			/* hit end of string, no fmt direcives left */
			break;
		else if (j < 0) {
			/* hit a whitespace, skip to after the whitespace */
			i = -j;
			continue;
		}

		/* hit valid fmt spec */
		int flen = j - i + 1;

		rlen = i - roff;

		if (woff + rlen + arg_len + flen + bcol_len >= BUFFER_SIZE)
			goto failure;

		/* write everything since last copy */
		memcpy(buffer + woff, fmt + roff, rlen);
		woff += rlen;
		roff += rlen;

		/* prefix fmt specifier with arg color code */
		memcpy(buffer + woff, arg_color, arg_len);
		woff += arg_len;

		/* write the fmt specifier */
		memcpy(buffer + woff, fmt + i, flen);
		woff += flen;
		roff += flen;

		/* reset to base color */
		memcpy(buffer + woff, base_color, bcol_len);
		woff += bcol_len;

		/* step past fmt specifier */
		i += flen - 1;
	}

	if (woff + sizeof RST - 1 + 2 >= BUFFER_SIZE)
		goto failure;

	/* write any remaining chars */
	rlen = len - roff;

	memcpy(buffer + woff, fmt + roff, rlen);
	woff += rlen;
	roff += rlen;

	/* write rst */
	memcpy(buffer + woff, RST, sizeof RST - 1);
	woff += sizeof RST - 1;

	/* write null terminator */
	buffer[woff] = 0;

	/* print it */
	va_list args;
	va_start(args, fmt);
	vfprintf(file, buffer, args);
	va_end(args);

	return;

failure:
	printf("pretty_printf(): exceeded buffer");
}
