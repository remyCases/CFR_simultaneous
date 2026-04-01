// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#ifndef PLAYER_H__
#define PLAYER_H__

#include <stdint.h>
#include "xorshiro256.h"
#include "combinatory.h"
#include "game.h"

// structure that holds all information needed for a CRF
typedef struct player_s {
    uint16_t nb_cases;
    int32_t* cumulative_regret;
    float* strategy;
    float* sum_strategy;
    float* avg_strategy;

    int32_t sum_regret;
    uint16_t action;
} player_t;

void init_player(player_t* p_player, uint16_t nb_pure_strategy);
void free_player(player_t* p_player);

// compute functions, to be called at each iteration
void compute_strategies(player_t* p_player, player_t* p_player_other);
void compute_actions(player_t* p_player, player_t* p_player_other);
void compute_average_strategies(player_t* p_player, player_t* p_player_other);

// prettifying functions
void print_avg_strategies(player_t* p_player, player_t* p_player_other, game_t* game);
void export_pure_strategies(FILE* f, uint16_t nstrat);
void export_avg_strategies(FILE* f, player_t* p_player);
#endif // PLAYER_H__