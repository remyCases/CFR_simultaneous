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
uint64_t binomial_coefficient_ym(uint16_t n, uint16_t t) {
    uint64_t res = 1;

    if (t != 0 && t != n) {
        if (t > n - t) t = n-t;
        for (uint16_t i = n; i > n-t; i--) {
            res = (res * i) / (n-i+1);
        }
    }

    return res;
}

uint8_t lexicographic_combinations(uint16_t n, uint16_t t, uint16_t* c, uint16_t* coded) {
    if (n < t) return EXIT_FAILURE;

    // Initialize
    uint16_t index_coded = 0;
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
        coded[index_coded] = 0;
        
        for (uint16_t i = t; i > 0; i--) {
            coded[index_coded] *= (n - t + i + 1);
            coded[index_coded] += c[i-1];
        }
        index_coded++;

        if (j > 0) {
            x = j;
            c[j-1] = x;
            j--;
            continue;
        } 

        // Easy Case
        if (c[0] + 1 < c[1]) {
            c[0]++;
            continue;
        }
        j = 2;

        // Find j
        do {
            c[j-2] = j-2;
            x = c[j-1] + 1;
            t4_cond = x == c[j];

            if (t4_cond) {
                j++;
            }

        } while(t4_cond);

        // Done ?
        end_cond = j > t;

        // Increase cj
        if (!end_cond) {
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