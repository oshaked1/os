#include "types.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

/**
 * @brief Minimal implementation of vsnprintf.
 * Supported specifiers: c, d, i, u, x, X, s.
 * Flags and other modifiers are not supported.
 * 
 * @param str String buffer to copy formatted string into
 * @param maxlen Maximum number of characters to put in the buffer
 * @param fmt Format
 * @param args va_list of arguments
 * @return int Length of formatted string if successful, otherwise a negative number is returned.
 * If maxlen was reached, maxlen is returned to indicate that more characters would have been written if maxlen was large enough.
 */
int vsnprintf(char *str, size_t maxlen, const char *fmt, va_list args)
{
    int i, k;
    int j = 0;
    bool expecting_specifier = FALSE;
    char temp[12]; // string for holding temporary itoa conversions. max length is currently 12 (max length of signed integer).
    for (i = 0; i < strlen(fmt); i++)
    {
        if (expecting_specifier == FALSE)
        {
            if (fmt[i] == '%')
                expecting_specifier = TRUE;
            else
            {
                if (j < maxlen-1)
                    str[j++] = fmt[i];
                else 
                    {
                        str[maxlen-1] = 0;
                        return maxlen;
                    }
            }
        }

        else
        {
            switch (fmt[i])
            {
                // '%' escaping using another '%'
                case '%':
                    expecting_specifier = FALSE;
                    if (j < maxlen-1)
                        str[j++] = '%';
                    else 
                    {
                        str[maxlen-1] = 0;
                        return maxlen;
                    }
                    break;
                
                // spaces are completely ignored when expecting a specifier
                case ' ':
                    break;

                // character
                case 'c':
                    if (j < maxlen-1)
                        str[j++] = va_arg(args, int);
                    else 
                    {
                        str[maxlen-1] = 0;
                        return maxlen;
                    }
                    expecting_specifier = FALSE;
                    break;
                
                // signed decimal int
                case 'd':
                    itoa(va_arg(args, int), temp, 10);
                    for (k = 0; k < strlen(temp); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = temp[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;
                
                // signed decimal int, same as 'd'
                case 'i':
                    itoa(va_arg(args, int), temp, 10);
                    for (k = 0; k < strlen(temp); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = temp[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;
                
                // unsigned decimal int
                case 'u':
                    itoa_unsigned(va_arg(args, int), temp, 10);
                    for (k = 0; k < strlen(temp); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = temp[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;
                
                // unsigned hexadecimal int
                case 'x':
                    itoa(va_arg(args, int), temp, 16);
                    for (k = 0; k < strlen(temp); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = temp[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;
                
                // unsigned hexadecimal int, uppercase
                case 'X':
                    itoa_uppercase(va_arg(args, int), temp, 16);
                    for (k = 0; k < strlen(temp); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = temp[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;
                
                case 's':
                    char *argstr = va_arg(args, char*);
                    for (k = 0; k < strlen(argstr); k++)
                    {
                        if (j < maxlen-1)
                            str[j++] = argstr[k];
                        else 
                        {
                            str[maxlen-1] = 0;
                            return maxlen;
                        }
                    }
                    expecting_specifier = FALSE;
                    break;

                // all unexpected specifiers are treated as part of the string
                default:
                    if (j < maxlen-1)
                            str[j++] = fmt[i];
                    else 
                    {
                        str[maxlen-1] = 0;
                        return maxlen;
                    }
                    expecting_specifier = FALSE;
                    break;
            }
        }
    }
    str[j] = 0;
    return strlen(str);
}

int snprintf(char *str, size_t maxlen, const char *fmt, ...)
{
    int res;
    va_list args;
    va_start(args, fmt);
    res = vsnprintf(str, maxlen, fmt, args);
    va_end(args);
    return res;
}