#include "types.h"

void memcpy(void *dst, const void *src, size_t len);
void memset(void *buf, int val, size_t len);
size_t strlen(const char *str);
void strrev(char *str);
char *strcpy(char *dst, const char *src);
void *memcchr(const void *buf, byte c, size_t len);