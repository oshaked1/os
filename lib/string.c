#include "string.h"

/**
 * @brief Copies a buffer to another location
 * 
 * @param dst The destination buffer
 * @param src The source buffer
 * @param len The number of bytes to copy
 */
void memcpy(void *dst, const void *src, size_t len)
{
    byte *dst2 = (byte*)dst;
    byte *src2 = (byte*)src;
    size_t i;
    for (i = 0; i < len; i++)
        *(dst2 + i) = *(src2 + i);
}

/**
 * @brief Sets the contents of a buffer to a given value
 * 
 * @param buf The buffer
 * @param val The value (treated as a single byte)
 * @param len The number of bytes to set
 */
void memset(void *buf, int val, size_t len)
{
    byte *buf2 = (byte*)buf;
    size_t i;
    for (i = 0; i < len; i++)
        *(buf2 + i) = (byte)val;
}

/**
 * @brief Finds the length of a null-terminated string
 * 
 * @param str The string
 * @return size_t The length of the string, excluding the null-terminator
 */
size_t strlen(const char *str)
{
    size_t len;
    for (len = 0; str[len] != '\0'; len++);
    return len;
}

/**
 * @brief Reverses a null-terminated string, in place
 * 
 * @param str The string to reverse
 */
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

/**
 * @brief Copies a null-terminated string to another buffer
 * 
 * @param dst The destination buffer
 * @param src The source string
 * @return char* The destination buffer
 */
char *strcpy(char *dst, const char *src)
{
    size_t len = strlen(src);
    memcpy(dst, src, len);
    dst[len] = 0;
    return dst;
}

/**
 * @brief Search a buffer for the first byte that differs from the given value
 * 
 * @param buf The buffer to search
 * @param c The value to match against
 * @param len The number of bytes to search
 * @return void* Pointer to the first byte that is not the same as the given value
 */
void *memcchr(const void *buf, byte c, size_t len)
{
    size_t i;
    const byte *buf2 = (byte*)buf;
    for (i = 0; i < len; i++)
    {
        if (buf2[i] != c)
            return (void*)buf2;
    }
    return NULL;
}