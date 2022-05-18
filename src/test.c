#include <stdio.h>

#define TYPEDEF_BIGINT
#include "bigint.h"

int main()
{
    bigint val1 = bigint_init();
    bigint_from_bin(val1, "1");
    bigint_shl(val1, 3);
    bigint_dump_bin(val1);
    bigint_free(val1);

    printf("Done!\n");
    return 0;
}
