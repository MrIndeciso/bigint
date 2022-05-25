#include "bigint.h"
#include "bigint_const.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ENABLE_DEBUG_ASSERTIONS

#define min(A, B) ((A > B) ? B : A)
#define max(A, B) ((A > B) ? A : B)

static struct bigint_t* bigint_one = &(struct bigint_t) {
    .num = & (bigint_base_t) { 1 },
    .size = 1,
    .arr_count = 1
};
static struct bigint_t* bigint_zero = &(struct bigint_t) {
        .num = & (bigint_base_t) { 0 },
        .size = 1,
        .arr_count = 1
};

struct bigint_t* bigint_init(void)
{
    struct bigint_t* val = malloc(sizeof(struct bigint_t));
    val->size = 0;
    val->arr_count = 0;
    val->num = NULL;
    return val;
}

void bigint_clean(struct bigint_t *val)
{
    if (val->num != NULL)
        free(val->num);
    val->size = 0;
    val->arr_count = 0;
    val->num = NULL;
}

void bigint_free(struct bigint_t *val)
{
    if (val->num != NULL)
        free(val->num);
    val->size = 0;
    val->arr_count = 0;
    free(val);
}

void bigint_alloc(struct bigint_t *val, size_t size)
{
#ifdef ENABLE_DEBUG_ASSERTIONS
    assert(val->size == 0);
    assert(val->arr_count == 0);
    assert(val->num == NULL);
#endif

    val->size = size;
    val->arr_count = ((size / BASE_T_SIZE) + (size % BASE_T_SIZE != 0));
    val->num = calloc(val->arr_count, sizeof(bigint_base_t));
}

void bigint_zero_out(struct bigint_t *val)
{
    if (val->num == NULL) {
        val->size = 1;
        val->arr_count = 1;
        val->num = calloc(1, sizeof(bigint_base_t));
        val->num[0] = 0;
    } else
        for (size_t i = 0; i < val->arr_count; i++)
            val->num[i] = 0;
}

void bigint_grow(struct bigint_t *val, size_t size)
{
    val->size = size;
    val->arr_count = ((size / BASE_T_SIZE) + (size % BASE_T_SIZE != 0));
    val->num = realloc(val->num,  val->arr_count * sizeof(bigint_base_t));
}

void bigint_shrink(struct bigint_t *val, size_t size)
{
    if (val->size == size)
        return;
    if (size <= 0)
        size = 1;
    val->size = size;
    val->arr_count = ((size / BASE_T_SIZE) + (size % BASE_T_SIZE != 0));
    bigint_base_t *new_num = calloc(val->arr_count, sizeof(bigint_base_t));
    memcpy(new_num, val->num, val->arr_count * sizeof(bigint_base_t));
    free(val->num);
    val->num = new_num;
    if (size % BASE_T_SIZE) {
        val->num[val->arr_count - 1] <<= (64 - size % BASE_T_SIZE);
        val->num[val->arr_count - 1] >>= (64 - size % BASE_T_SIZE);
    }
}

static inline
bigint_base_t get_bit_at(const struct bigint_t* val, size_t offset)
{
    if (val->size < offset)
        return 0;

    size_t index = offset / BASE_T_SIZE;
    size_t pos = offset % BASE_T_SIZE;

    return (val->num[index] >> pos) & 1;
}

static inline
void set_bit_at(struct bigint_t* val, size_t offset, bigint_base_t bit)
{
#ifdef ENABLE_DEBUG_ASSERTIONS
    assert(val->size >= offset);
#endif
    size_t index = offset / BASE_T_SIZE;
    size_t pos = offset % BASE_T_SIZE;

    val->num[index] &= and_bitmask[pos];
    val->num[index] += (bit << pos);
}

void bigint_from_bin(struct bigint_t *val, char *str)
{
    size_t len = strlen(str);

    if (val->size != 0 || val->num != NULL)
        bigint_clean(val);

    bigint_alloc(val, len);

    len -= 1; //yeah arrays start at 0 apparently

    for (ssize_t i = (ssize_t)len; i >= 0; i--) //ssize_t cause we underflow with this for
        set_bit_at(val, len - i, byte_from_bin(str[i]));
}

void bigint_from_hex(struct bigint_t *val, char *str)
{
    size_t len = strlen(str);

    if (strncasecmp(str, "0x", 2) == 0) {
        len -= 2;
        str += 2;
    }

    if (val->size != 0 || val->num != NULL)
        bigint_clean(val);

    bigint_alloc(val, len * 4);

    len -= 1;

    for (ssize_t i = (ssize_t)len; i >= 0; i--) { //ssize_t cause we underflow with this for
        bigint_base_t new = (bigint_base_t)byte_from_hex(str[i]);
        for (int j = 0; j < 4; j++) {
            set_bit_at(val, (len - i) * 4 + j, new & 1);
            new >>= 1;
        }
    }
}

void bigint_dump_bin(const struct bigint_t *val)
{
    size_t len = val->size - 1;

    for (ssize_t i = (ssize_t)len; i >= 0; i--) //ssize_t cause we underflow with this for
        putchar(byte_to_bin(get_bit_at(val, i)));

    putchar('\n');
}

void bigint_dump_hex(const struct bigint_t *val)
{
    size_t len = val->size;
    size_t hex_len = (len / 4) + (len % 4 != 0);

    bigint_base_t new;

    for (ssize_t i = (ssize_t)hex_len; i >= 0; i--) { //ssize_t cause we underflow with this for
        new = 0;
        for (int j = 0; j < 4; j++)
            new += get_bit_at(val, i * 4 + j) << j;
#ifdef ENABLE_DEBUG_ASSERTIONS
        assert(new <= 15);
#endif
        putchar(byte_to_hex(new));
    }

    putchar('\n');
}

int bigint_eq(const struct bigint_t *n1, const struct bigint_t *n2)
{
    size_t i;

    for (i = 0; i < min(n1->size, n2->size); i++)
        if (get_bit_at(n1, i) != get_bit_at(n2, i))
            return 1;

    if (n1->size > n2->size) {
        for (i = n2->size; i < n1->size; i++)
            if (get_bit_at(n1, i) != 0)
                return 1;
    }
    else if (n2->size > n1->size) {
        for (i = n1->size; i < n2->size; i++)
            if (get_bit_at(n2, i) != 0)
                return 1;
    }
    // n1->size == n2->size case gets missed because we don't have to do anything there

    return 0;
}

static inline
size_t bigint_effective_len(const struct bigint_t *val)
{
    for (ssize_t i = (ssize_t)val->size - 1; i >=0; i--)
        if (get_bit_at(val, i) == 1)
            return i + 1;
    return 0;
}

void bigint_auto_shrink(struct bigint_t *val)
{
    size_t new_len = bigint_effective_len(val);
    bigint_shrink(val, new_len);
}

int bigint_is_zero(const struct bigint_t *val)
{
    return (bigint_effective_len(val) == 0);
}

int bigint_is_one(const struct bigint_t *val)
{
    return (bigint_effective_len(val) == 1);
}

int bigint_cmp(const struct bigint_t *v1, const struct bigint_t *v2)
{
    if (bigint_effective_len(v1) > bigint_effective_len(v2))
        return 1;
    else if (bigint_effective_len(v1) < bigint_effective_len(v2))
        return -1;

    if (bigint_eq(v1, v2) == 0)
        return 0;

    for (ssize_t i = (ssize_t)min(v1->size, v2->size) - 1; i >= 0; i--)
        if (get_bit_at(v1, i) > get_bit_at(v2, i)) {
            return 1;
        } else if (get_bit_at(v1, i) < get_bit_at(v2, i))
            return -1;

#ifdef ENABLE_DEBUG_ASSERTIONS
    assert(0); //This should never be reached
#endif
}

void bigint_clone(struct bigint_t *dest, const struct bigint_t *src)
{
    bigint_clean(dest);

    bigint_alloc(dest, src->size);

    for (size_t i = 0; i < src->arr_count; i++)
        dest->num[i] = src->num[i];
}

void bigint_mod(struct bigint_t *val, const struct bigint_t *mod) //TODO this is a naive inefficient implementation, switch to divide-multiply then subtract?
{
    while (bigint_cmp(val, mod) == 1)
        bigint_sub(val, mod);
}

static
void bigint_shl_small(struct bigint_t *val, uint8_t bits)
{
    size_t old_size = val->arr_count;

    bigint_grow(val, val->size + bits);

    bigint_base_t overflow = 0, res;

    for (size_t i = 0; i < old_size; i++) {
        res = (val->num[i] >> (BASE_T_SIZE - bits));
        val->num[i] <<= bits;
        val->num[i] += overflow;
        overflow = res;
    }

    val->num[val->arr_count - 1] += overflow;
}

static
void bigint_shr_small(struct bigint_t *val, uint8_t bits)
{
    bigint_base_t underflow = 0, temp;

    for (ssize_t i = (ssize_t)val->arr_count - 1; i >= 0; i--) {
        temp = val->num[i] & (((bigint_base_t)2 << bits) - (bigint_base_t)1);
        val->num[i] >>= bits;
        val->num[i] += underflow;
        underflow = temp << (BASE_T_SIZE - bits);
    }
}

void bigint_shl(struct bigint_t *val, int bits)
{
    while (bits > BASE_T_SIZE) {
        bigint_shl_small(val, BASE_T_SIZE/2);
        bits -= BASE_T_SIZE/2;
    }

    bigint_shl_small(val, bits);
}

void bigint_shr(struct bigint_t *val, int bits)
{
    while (bits > BASE_T_SIZE) { ////TODO we dont need to do this, just memmove the array into oblivion
        bigint_shr_small(val, BASE_T_SIZE/2);
        bits -= BASE_T_SIZE/2;
    }

    bigint_shr_small(val, bits);
}

static inline
void bigint_twos_complement(struct bigint_t *val) //TODO should we expose this? Handling negative things makes everything harder
{
    size_t i = 0;

    while (get_bit_at(val, i) == 0) //Go on 'till it's all zeros
        ++i;

    ++i; //Skip the first one

    for (; i < val->size; i++) //Invert everything after
        set_bit_at(val, i, !get_bit_at(val, i));
}

void bigint_add(struct bigint_t *v1, const struct bigint_t *v2)
{
    bigint_grow(v1, max(v1->size, v2->size) + 1);

    bigint_base_t overflow = 0, res;
    size_t i;

    for (i = 0; i < v2->arr_count; i++) {
        res = v1->num[i] + v2->num[i] + overflow;
        overflow = (max(v1->num[i], v2->num[i]) > res);
        v1->num[i] = res;
    }

    v1->num[min(i, v1->arr_count - 1)] += overflow;
}

void bigint_sub(struct bigint_t *v1, const struct bigint_t *v2)
{
    struct bigint_t *v2_inv = bigint_init();
    bigint_clone(v2_inv, v2);
    bigint_grow(v2_inv, max(v1->size, v2_inv->size) + 1);
    bigint_twos_complement(v2_inv);

    if (v1->size < v2_inv->size)
        bigint_grow(v1, v2_inv->size);

    bigint_base_t overflow = 0, res;
    size_t i;

    for (i = 0; i < v2_inv->arr_count; i++) {
        res = v1->num[i] + v2_inv->num[i] + overflow;
        overflow = max(v1->num[i], v2_inv->num[i]) > res;
        v1->num[i] = res;
    }

    v1->num[min(i, v1->arr_count - 1)] += overflow;

    bigint_free(v2_inv);

    bigint_auto_shrink(v1);
}

void bigint_mul(struct bigint_t *v1, const struct bigint_t *v2)
{
    if (bigint_is_one(v2))
        return;

    if (bigint_is_zero(v2)) {
        bigint_clean(v1);
        bigint_from_bin(v1, "0");
        return;
    }

    struct bigint_t *b = bigint_init();
    struct bigint_t *q = bigint_init();
    bigint_clone(b, v1);
    bigint_clone(q, v2);

    bigint_zero_out(v1);

    for (size_t i = 0; i < q->size; i++) {
        if (get_bit_at(q, i) == 1)
            bigint_add(v1, b);
        bigint_shl_small(b, 1);
    }

    bigint_auto_shrink(v1);

    bigint_free(b);
    bigint_free(q);
}

void bigint_div(struct bigint_t *num, const struct bigint_t *div, struct bigint_t *res)
{
    bigint_zero_out(res);

    int cmp = bigint_cmp(num, div);

    if (cmp == -1)
        return;
    else if (cmp == 0) {
        bigint_clean(res);
        bigint_clone(res, bigint_one);
        return;
    }

    struct bigint_t *temp_div = bigint_init();
    bigint_clone(temp_div, div);

    size_t bit_len_diff = bigint_effective_len(num) - bigint_effective_len(div);

    bigint_shl(temp_div, (int) bit_len_diff);

    for (size_t i = 0; i <= bit_len_diff; i++) {
        printf("num: %lu; temp_div: %lu\n", num->num[0], temp_div->num[0]);
        if (bigint_cmp(num, temp_div) >= 0) {
            bigint_sub(num, temp_div);
            bigint_add(res, bigint_one);
        }
        bigint_shl(res, 1);
        bigint_shr(temp_div, 1);
    }

    bigint_shr(res, 1);

    bigint_free(temp_div);
}

void bigint_pow(struct bigint_t *base, const struct bigint_t *exp)
{

}

static inline
void bigint_intpow(struct bigint_t *base, int exp)
{

}

void bigint_modpow(struct bigint_t *base, const struct bigint_t *exp, const struct bigint_t *mod)
{
    if (bigint_is_one(exp))
        return;

    if (bigint_is_zero(exp)) {
        bigint_clean(base);
        bigint_from_bin(base, "1");
        return;
    }

    struct bigint_t *b = bigint_init();
    struct bigint_t *e = bigint_init();
    bigint_clone(b, base);
    bigint_clone(e, exp);

    bigint_zero_out(base);
    set_bit_at(base, 0, 1);

    for (ssize_t i = (ssize_t)e->size - 1; i >=0; i--) {

    }

    bigint_free(b);
    bigint_free(e);
}