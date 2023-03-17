#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>

// Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

// Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

#define RST "\e[0m"

/* https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html for explanation on
 * the __VA_OPT__ builtin macro */

void pretty_vprintf(FILE *file, const char *const func,
                    const char *const base_color, const char *const arg_color,
                    const char *const fmt, ...);

#define DO_PRINT(base_color, arg_color, fmt, ...)                              \
  do {                                                                         \
    pretty_vprintf(stdout, __func__, base_color, arg_color,                    \
                   fmt "\n" __VA_OPT__(, ) __VA_ARGS__);                       \
  } while (0)

#define PRINTLN(fmt, args...) DO_PRINT(BGRN, BYEL, fmt, args)

#ifdef __DEBUG__
#define DEBUGLN(fmt, args...) DO_PRINT(CYN, BCYN, fmt, args)
#else
#define DEBUGLN(x...)
#endif

#define ERRORLN(fmt, args...) DO_PRINT(RED, BRED, fmt, args)

#define PERROR(str) ERRORLN(str ": %s", strerror(errno))
