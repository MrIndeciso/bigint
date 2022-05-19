#include <stdio.h>

#define TYPEDEF_BIGINT
#include "bigint.h"

int main()
{
    bigint val1 = bigint_init();
    bigint val2 = bigint_init();

    bigint_from_bin(val1, "11100101001");
    bigint_from_bin(val2, "110011010");

    bigint_mul(val1, val2);
    bigint_dump_bin(val1);

    printf("Done!\n");
    return 0;
}
