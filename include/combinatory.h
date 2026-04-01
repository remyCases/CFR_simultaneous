// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#ifndef COMBINATORY_H__
#define COMBINATORY_H__

uint64_t binomial_coefficient_ym(uint16_t, uint16_t);
uint8_t lexicographic_combinations(uint16_t, uint16_t, uint16_t*);
uint16_t encode_combinatorial_number_system(uint16_t*, uint16_t, uint16_t);
uint16_t* decode_combinatorial_number_system(uint16_t, uint16_t, uint16_t);
uint16_t* decode_blotto_configuration(uint16_t, uint16_t, uint16_t);
void print_blotto_configuration(uint16_t, uint16_t, uint16_t);

#endif // COMBINATORY_H__