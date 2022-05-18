#include "util.h"

uint8_t byte_from_bin(char ch)
{
    return ch - '0';
}

uint8_t byte_from_hex(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else
        return 10 + ch - 'a';
}


uint8_t byte_from_dec(char ch)
{
    return ch - '0';
}

char byte_to_bin(uint8_t val)
{
    return (char)('0' + val);
}

char byte_to_hex(uint8_t val)
{
    if (val >= 10)
        return (char)('a' + val - 10);
    else
        return (char)('0' + val);
}

char byte_to_dec(uint8_t val)
{
    return (char)('0' + val);
}
