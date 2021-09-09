#include "types.h"
#include "stdarg.h"

int vsnprintf(char *str, size_t maxlen, const char *format, va_list arg);
int snprintf(char *str, size_t maxlen, const char *format, ...);