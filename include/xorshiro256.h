#ifndef XORSHIRO256_H__
#define XORSHIRO256_H__

void set_seed(uint64_t seed);
uint64_t next_xorshiro256(void);

double to_double(uint64_t x);
#endif // XORSHIRO256_H__