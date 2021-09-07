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

char *itoa(int value, char *str, int base)
{   
    // make sure base is valid
    if (base == 0 || base > MAX_BASE)
    {
        str[0] = 0;
        return str;
    }

    uint tempval;
    if (base != 10)
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
    if (base == 10 && value < 0)
        temp[i++] = '-';
    temp[i] = 0;
    
    // reverse the string
    strrev(temp);

    // copy to output
    strcpy(str, temp);
    return str;
}

uint abs(int x)
{
    if (x < 0)
        return (uint)-x;
    return x;
}