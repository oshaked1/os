#include "string.h"

void memcpy(void *dst, const void *src, size_t len)
{
    byte *dst2 = (byte*)dst;
    byte *src2 = (byte*)src;
    size_t i;
    for (i = 0; i < len; i++)
        *(dst2 + i) = *(src2 + i);
}

void memset(void *buf, int val, size_t len)
{
    byte *buf2 = (byte*)buf;
    size_t i;
    for (i = 0; i < len; i++)
        *(buf2 + i) = (byte)val;
}

size_t strlen(const char *str)
{
    size_t len;
    for (len = 0; str[len] != '\0'; len++);
    return len;
}

void strrev(char *str)
{
    size_t len = strlen(str);
    if (len <= 1)
        return;
    size_t i;
    char temp;
    for (i = len - 1; i >= len/2; i--)
    {
        temp = str[len - i - 1];
        str[len - i - 1] = str[i];
        str[i] = temp;
    }
}

char *strcpy(char *dst, const char *src)
{
    size_t len = strlen(src);
    memcpy(dst, src, len);
    dst[len] = 0;
    return dst;
}