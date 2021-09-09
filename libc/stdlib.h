#include "types.h"

#define MAX_BASE 36

char *itoa(int value, char *str, int base);
char *itoa_unsigned(int value, char *str, int base);
char *itoa_uppercase(int value, char *str, int base);
uint abs(int x);