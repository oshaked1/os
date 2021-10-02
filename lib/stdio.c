#include "types.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

enum ARG_SIZE{INT, SHORT, LONG, LONGLONG};

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
    enum ARG_SIZE arg_size = INT;

    // string for holding temporary itoa conversions. max length is currently 12 (max length of long long signed integer)
    char temp[21];

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
                
                // short modifier
                case 'h':
                    arg_size = SHORT;
                    break;

                // long modifier
                case 'l':
                    // 2 l's in a row means long long
                    if (arg_size == LONG)
                        arg_size = LONGLONG;
                    
                    else
                        arg_size = LONG;
                    break;
                
                // signed decimal int
                case 'd':
                // signed decimal int, same as 'd'
                case 'i':
                    switch (arg_size)
                    {
                        case INT:
                            itoa(va_arg(args, int), temp, 10, TRUE);
                            break;
                        
                        case SHORT:
                            htoa(va_arg(args, int), temp, 10, TRUE);
                            break;
                        
                        case LONG:
                            ltoa(va_arg(args, long), temp, 10, TRUE);
                            break;
                        
                        case LONGLONG:
                            lltoa(va_arg(args, long long), temp, 10, TRUE);
                            break;
                    }

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
                    switch (arg_size)
                    {
                        case INT:
                            itoa(va_arg(args, int), temp, 10, FALSE);
                            break;
                        
                        case SHORT:
                            htoa(va_arg(args, int), temp, 10, FALSE);
                            break;
                        
                        case LONG:
                            ltoa(va_arg(args, long), temp, 10, FALSE);
                            break;
                        
                        case LONGLONG:
                            lltoa(va_arg(args, long long), temp, 10, FALSE);
                            break;
                    }

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
                    switch (arg_size)
                    {
                        case INT:
                            itoa(va_arg(args, int), temp, 16, FALSE);
                            break;
                        
                        case SHORT:
                            htoa(va_arg(args, int), temp, 16, FALSE);
                            break;
                        
                        case LONG:
                            ltoa(va_arg(args, long), temp, 16, FALSE);
                            break;
                        
                        case LONGLONG:
                            lltoa(va_arg(args, long long), temp, 16, FALSE);
                            break;
                    }

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
                    switch (arg_size)
                    {
                        case INT:
                            itoa_uppercase(va_arg(args, int), temp, 16);
                            break;
                        
                        case SHORT:
                            htoa_uppercase(va_arg(args, int), temp, 16);
                            break;
                        
                        case LONG:
                            ltoa_uppercase(va_arg(args, long), temp, 16);
                            break;
                        
                        case LONGLONG:
                            lltoa_uppercase(va_arg(args, long long), temp, 16);
                            break;
                    }

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

                // pointer, print as hex using the correct ineger size
                case 'p':
                    // long is equivalent to size_t
                    ltoa(va_arg(args, long), temp, 16, FALSE);

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

            // if not expecting a specifier, reset arg size to int
            if (!expecting_specifier)
                arg_size = INT;
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