#include <stdio.h>

#define TYPEDEF_BIGINT
#include "bigint.h"

int main()
{
    bigint val1 = bigint_init();
    bigint val2 = bigint_init();

    bigint_from_bin(val1, "110011001100");
    bigint_from_bin(val2, "11011011");

    bigint_sub(val1, val2);

    bigint_dump_bin(val1);

    printf("Done!\n");
    return 0;
}
