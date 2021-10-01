#include "types.h"

#define MAX_BASE 36

char *itoa(int value, char *str, int base, bool is_signed);
char *itoa_uppercase(int value, char *str, int base);
char *htoa(short value, char *str, int base, bool is_signed);
char *htoa_uppercase(short value, char *str, int base);
char *ltoa(long value, char *str, int base, bool is_signed);
char *ltoa_uppercase(long value, char *str, int base);
char *lltoa(long long value, char *str, int base, bool is_signed);
char *lltoa_uppercase(long long value, char *str, int base);
uint abs(int x);
ushort habs(short x);
ulong labs(long x);
ulonglong llabs(long long x);