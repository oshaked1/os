#include "stdlib.h"
#include "string.h"

char digit_to_char(uint digit)
{
    // make sure digit is not larger than largest digit in largest supported base (base 36)
    if (digit >= MAX_BASE)
    {
        int temp = 5;
        return NULL;
    }
    
    char c;
    c = digit + '0';
    if (digit > 9)
        c += 'a' - '9' - 1;
    return c;
}

char *itoa(int value, char *str, int base, bool is_signed)
{   
    // make sure base is valid
    if (base == 0 || base > MAX_BASE)
    {
        str[0] = 0;
        return str;
    }

    uint tempval;
    if (base != 10 || !is_signed)
        tempval = (uint)value;
    else if (base == 10 && value < 0)
        tempval = abs(value);
    else
        tempval = (uint)value;
        
    // temporary char array for storing reversed representation of the number.
    // array size is the size in bits of the number +1 for null-terminator 
    // (longest possible representation is in binary, with a character for each bit)
    char temp[sizeof(int)*8+1];

    char c;
    uint digit;
    int i = 0;
    if (tempval == 0)
        temp[i++] = '0';
    while (tempval != 0)
    {
        digit = tempval % base;
        c = digit_to_char(digit);
        if (c == NULL)
        {
            str[0] = 0;
            return str;
        }
        temp[i++] = c;
        tempval /= (uint)base;
    }
    if (is_signed && base == 10 && value < 0)
        temp[i++] = '-';
    temp[i] = 0;
    
    // reverse the string
    strrev(temp);

    // copy to output
    strcpy(str, temp);
    return str;
}

char *itoa_uppercase(int value, char *str, int base)
{
    itoa(value, str, base, FALSE);
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
    return str;
}

char *htoa(short value, char *str, int base, bool is_signed)
{   
    // make sure base is valid
    if (base == 0 || base > MAX_BASE)
    {
        str[0] = 0;
        return str;
    }

    ushort tempval;
    if (base != 10 || !is_signed)
        tempval = (ushort)value;
    else if (base == 10 && value < 0)
        tempval = habs(value);
    else
        tempval = (ushort)value;
        
    // temporary char array for storing reversed representation of the number.
    // array size is the size in bits of the number +1 for null-terminator 
    // (longest possible representation is in binary, with a character for each bit)
    char temp[sizeof(short)*8+1];

    char c;
    uint digit;
    int i = 0;
    if (tempval == 0)
        temp[i++] = '0';
    while (tempval != 0)
    {
        digit = tempval % base;
        c = digit_to_char(digit);
        if (c == NULL)
        {
            str[0] = 0;
            return str;
        }
        temp[i++] = c;
        tempval /= (uint)base;
    }
    if (is_signed && base == 10 && value < 0)
        temp[i++] = '-';
    temp[i] = 0;
    
    // reverse the string
    strrev(temp);

    // copy to output
    strcpy(str, temp);
    return str;
}

char *htoa_uppercase(short value, char *str, int base)
{
    htoa(value, str, base, FALSE);
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
    return str;
}

char *ltoa(long value, char *str, int base, bool is_signed)
{   
    // make sure base is valid
    if (base == 0 || base > MAX_BASE)
    {
        str[0] = 0;
        return str;
    }

    ulong tempval;
    if (base != 10 || !is_signed)
        tempval = (ulong)value;
    else if (base == 10 && value < 0)
        tempval = labs(value);
    else
        tempval = (ulong)value;
        
    // temporary char array for storing reversed representation of the number.
    // array size is the size in bits of the number +1 for null-terminator 
    // (longest possible representation is in binary, with a character for each bit)
    char temp[sizeof(long)*8+1];

    char c;
    uint digit;
    int i = 0;
    if (tempval == 0)
        temp[i++] = '0';
    while (tempval != 0)
    {
        digit = tempval % base;
        c = digit_to_char(digit);
        if (c == NULL)
        {
            str[0] = 0;
            return str;
        }
        temp[i++] = c;
        tempval /= (uint)base;
    }
    if (is_signed && base == 10 && value < 0)
        temp[i++] = '-';
    temp[i] = 0;
    
    // reverse the string
    strrev(temp);

    // copy to output
    strcpy(str, temp);
    return str;
}

char *ltoa_uppercase(long value, char *str, int base)
{
    ltoa(value, str, base, FALSE);
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
    return str;
}

char *lltoa(long long value, char *str, int base, bool is_signed)
{   
    // make sure base is valid
    if (base == 0 || base > MAX_BASE)
    {
        str[0] = 0;
        return str;
    }

    ulonglong tempval;
    if (base != 10 || !is_signed)
        tempval = (ulonglong)value;
    else if (base == 10 && value < 0)
        tempval = llabs(value);
    else
        tempval = (ulonglong)value;
        
    // temporary char array for storing reversed representation of the number.
    // array size is the size in bits of the number +1 for null-terminator 
    // (longest possible representation is in binary, with a character for each bit)
    char temp[sizeof(long long)*8+1];

    char c;
    uint digit;
    int i = 0;
    if (tempval == 0)
        temp[i++] = '0';
    while (tempval != 0)
    {
        digit = tempval % base;
        c = digit_to_char(digit);
        if (c == NULL)
        {
            str[0] = 0;
            return str;
        }
        temp[i++] = c;
        tempval /= (uint)base;
    }
    if (is_signed && base == 10 && value < 0)
        temp[i++] = '-';
    temp[i] = 0;
    
    // reverse the string
    strrev(temp);

    // copy to output
    strcpy(str, temp);
    return str;
}

char *lltoa_uppercase(long long value, char *str, int base)
{
    lltoa(value, str, base, FALSE);
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
    return str;
}

uint abs(int x)
{
    if (x < 0)
        return (uint)-x;
    return x;
}

ushort habs(short x)
{
    if (x < 0)
        return (ushort)-x;
    return x;
}

ulong labs(long x)
{
    if (x < 0)
        return (ulong)-x;
    return x;
}

ulonglong llabs(long long x)
{
    if (x < 0)
        return (ulonglong)-x;
    return x;
}