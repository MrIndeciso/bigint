#include <assert.h>
#include <stdio.h>

#define TYPEDEF_BIGINT
#include "bigint.h"

void basic_math_test()
{
    bigint int1 = bigint_init();
    bigint int2 = bigint_init();
    bigint_from_bin(int1, "10101110000101011101001011101011100001010111010010110100101001");
    bigint_from_bin(int2, "11110101110000101011101001011010011111010111000010101110100101101001111111");
    bigint_mul(int1, int2);
    bigint_dump_bin(int1);
    bigint_free(int1);
    bigint_free(int2);
}

int main()
{
    basic_math_test();
    return 0;
}
