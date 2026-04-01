// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/combinatory.h"

// Araujo, L.C., Sansão, J.P.H. & Vale-Cardoso, A.S. Fast computation of binomial coefficients. Numer Algor 86, 799–812 (2021). https://doi.org/10.1007/s11075-020-00912-x
// Yannis Manolopoulos. 2002. Binomial coefficient computation: recursion or iteration? SIGCSE Bull. 34, 4 (December 2002), 65–67. https://doi.org/10.1145/820127.820168
uint64_t binomial_coefficient_ym(uint16_t n, uint16_t t) 
{
    if (t > n) return 0;
    uint64_t res = 1;

    if (t != 0 && t != n) {
        if (t > n - t) t = n-t;
        for (uint16_t i = n; i > n-t; i--) {
            res = (res * i) / (n-i+1);
        }
    }

    return res;
}

uint16_t* decode_blotto_configuration(uint16_t coded, uint16_t b, uint16_t s)
{
    uint16_t* decoded = decode_combinatorial_number_system(coded, b, s);
    uint16_t* battlefields = (uint16_t*)calloc(b, sizeof(uint16_t));

    battlefields[0] = decoded[0];
    for(uint16_t i = 1; i < b - 1; i++) 
    {
        battlefields[i] = decoded[i] - decoded[i-1] - 1;
    }
    battlefields[b - 1] = b + s - 1 - decoded[b - 2] - 1;
    
    free(decoded);
    return battlefields;
}

void print_blotto_configuration(uint16_t coded, uint16_t b, uint16_t s)
{
    uint16_t* d = decode_blotto_configuration(coded, b, s);
    for (int j = 0; j < b; j++)
    {
        printf("%u, ", d[j]);
    }
    free(d);
}

// TODO : review for improvement of the naive method
// Aviezri S. Fraenkel, Moshe Mor, Combinatorial Compression and Partitioning of Large Dictionaries,
// The Computer Journal, Volume 26, Issue 4, November 1983, Pages 336–343, https://doi.org/10.1093/comjnl/26.4.336

// from Knuth Donald, The Art Of Computer Programming, Volume 4A, Third Edition
// 7.2.1.3 Answers to Exercises, p727
// Answer 17
uint16_t* decode_combinatorial_number_system(uint16_t coded, uint16_t b, uint16_t s)
{   
    uint16_t n = b + s - 1;
    uint16_t t = b - 1;

    uint16_t* decoded = (uint16_t*)calloc(t, sizeof(uint16_t));

    uint16_t x = 1;
    uint16_t c = t;

    while (x <= coded)
    {
        c += 1;
        x = (x*c) / (c-t);
    }
    x = x * (c-t) / c;
    c -= 1;

    while(t > 1)
    {
        decoded[t-1] = c;
        coded -= x;

        x = x*t / c;
        t -= 1;
        c -= 1;
        while(x > coded)
        {
            x = x * (c-t) / c;
            c -= 1;
        }
    }

    decoded[0] = coded;
    return decoded;
}

uint16_t encode_combinatorial_number_system(uint16_t* strat, uint16_t b, uint16_t s)
{
    uint16_t n = b + s - 1;
    uint16_t t = b - 1;
    uint16_t encoded = 0;

    for (uint16_t i = 0; i < t; i++)
    {
        encoded += (uint16_t)binomial_coefficient_ym(strat[i], i+1);
    }

    return encoded;
}

// from Knuth Donald, The Art Of Computer Programming, Volume 4A, Third Edition
// 7.2.1.3 Generating All Combinations, p359
// Algorithm T
// Compute all lexicographic combination of choosing t element from a set of n elements
// c is used as a temporary array for storing one combination
uint8_t lexicographic_combinations(uint16_t n, uint16_t t, uint16_t* c)
{
    if (n < t) return EXIT_FAILURE;

    // Initialize
    uint16_t j = 0;
    uint16_t x = 0;
    uint8_t t4_cond = 0;
    uint8_t end_cond = 0;

    for (j = 0; j < t; j++) {
        c[j] = j;
    } 
    c[t] = n;
    c[t+1] = 0;
    j = t;

    do {
        // Visit
        for (uint16_t i = 0; i < t; i++) {
            printf("%d,", c[i]);
        } 
        printf("\n");

        if (j > 0) 
        {
            x = j;
            c[j-1] = x;
            j--;
            continue;
        } 

        // Easy Case
        if (c[0] + 1 < c[1]) 
        {
            c[0]++;
            continue;
        }
        j = 2;

        // Find j
        do {
            c[j-2] = j-2;
            x = c[j-1] + 1;
            t4_cond = x == c[j];

            if (t4_cond) 
            {
                j++;
            }

        } while(t4_cond);

        // Done ?
        end_cond = j > t;

        // Increase cj
        if (!end_cond) 
        {
            c[j-1] = x;
            j--;
        }
    } while(!end_cond);

    return EXIT_SUCCESS;
}

uint8_t lexicographic_combinations_goto(uint16_t n, uint16_t t, uint16_t* c) {
    if (n < t) return EXIT_FAILURE;

    // Initialize
    T1:
    uint16_t j = 0;
    uint16_t x = 0;
    uint8_t t4_cond = 0;

    for (j = 0; j < t; j++) {
        c[j] = j;
    } 
    c[t] = n;
    c[t+1] = 0;
    j = t;

    // Visit
    T2:
    for (uint16_t i = 0; i < t; i++) {
        printf("%d,", c[i]);
    } 
    printf("\n");

    if (j > 0) {
        x = j;
        goto T6;
    }

    // Easy Case
    T3:
    if (c[0] + 1 < c[1]) {
        c[0]++;
        goto T2;
    } else {
        j = 2;
    }

    // Find j
    T4:
    do {
        c[j-2] = j-2;
        x = c[j-1] + 1;
        t4_cond = x == c[j];

        if (t4_cond) {
            j++;
        }

    } while(t4_cond);
    
    // Done ?
    T5:
    if (j > t) return EXIT_SUCCESS;
    
    // Increase cj
    T6:
    c[j-1] = x;
    j--;
    goto T2;
}