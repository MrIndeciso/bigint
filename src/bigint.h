#ifndef BIGINT_H
#define BIGINT_H

#include <stddef.h>
#include <stdint.h>

struct bigint_t {
  size_t size;
  size_t arr_count;
  uint8_t* num;
};

#ifdef TYPEDEF_BIGINT
  typedef struct bigint_t* bigint;
#endif

  /* Interface things? */
struct bigint_t* bigint_init(void);
void bigint_clean(struct bigint_t*);
void bigint_free(struct bigint_t*);
void bigint_alloc(struct bigint_t*, size_t);
void bigint_grow(struct bigint_t*, size_t);
void bigint_shrink(struct bigint_t*, size_t);
void bigint_auto_shrink(struct bigint_t*);

void bigint_from_bin(struct bigint_t*, char*);
void bigint_from_hex(struct bigint_t*, char*);

void bigint_dump_bin(const struct bigint_t*);
void bigint_dump_hex(const struct bigint_t*);

  /* Math stuff? */
int bigint_eq(const struct bigint_t*, const struct bigint_t*);
int bigint_cmp(const struct bigint_t*, const struct bigint_t*);
void bigint_clone(struct bigint_t*, const struct bigint_t*);
void bigint_mod(struct bigint_t*, const struct bigint_t*);
void bigint_shl(struct bigint_t*, int);
void bigint_shr(struct bigint_t*, int);
void bigint_add(struct bigint_t*, const struct bigint_t*);
void bigint_sub(struct bigint_t*, const struct bigint_t*);
void bigint_mul(struct bigint_t*, const struct bigint_t*);
void bigint_long_mul(struct bigint_t*, struct bigint_t*);
void bigint_div(struct bigint_t*, const struct bigint_t*);
void bigint_pow(struct bigint_t*, const struct bigint_t*);

  /* Modular math stuff? */
void bigint_modpow(struct bigint_t*, const struct bigint_t*, const struct bigint_t*);
//Do i need modadd, modsub, modmul, moddiv?

#endif // BIGINT_H
