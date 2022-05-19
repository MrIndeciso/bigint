#include <stdio.h>

#define TYPEDEF_BIGINT
#include "bigint.h"

int main()
{
    bigint val1 = bigint_init();
    bigint val2 = bigint_init();

    bigint_from_bin(val1, "1011010011");
    bigint_from_bin(val2, "10");

    bigint_mod(val1, val2);
    bigint_dump_bin(val1);

    printf("Done!\n");
    return 0;
}
