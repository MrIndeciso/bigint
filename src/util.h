#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

uint8_t byte_from_bin(char);
uint8_t byte_from_hex(char);
uint8_t byte_from_dec(char);

char byte_to_bin(uint8_t);
char byte_to_hex(uint8_t);
char byte_to_dec(uint8_t);


#endif // UTIL_H
