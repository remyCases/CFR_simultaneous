// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#ifndef GAME_H__
#define GAME_H__

#include <stdint.h>

typedef struct game_s game_t;

typedef enum RPS {
    ROCK,
    PAPER,
    SCISSOR,
    NUMBER_CASES,
} RPS;

typedef enum GAME_TYPE {
    GAME_RPS,
    GAME_BLT,
} GAME_TYPE;

typedef struct game_rps_s 
{
    // void
} game_rps_t;

typedef struct game_blotto_s 
{
    uint16_t nb_battlefield;
    uint16_t nb_soldier;
    // Z is the number of gaps in the stars-and-bars problem
    uint16_t z;
} game_blotto_t;

struct game_s {
    GAME_TYPE type;
    union {
        game_rps_t rps;
        game_blotto_t blt;
    };
    uint16_t nb_pure_strategies;
    int8_t (*result)(game_t*, uint16_t, uint16_t);
};

int init_game_rps(game_t* game);
int init_game_blt(game_t* game, uint16_t nb_battlefield, uint16_t nb_soldier);

#endif // GAME_H__