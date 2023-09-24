// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#ifndef PLAYER_H__
#define PLAYER_H__

#include <stdint.h>
#include "xorshiro256.h"

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

void get_strategies(player_t* p_player, player_t* p_player_other);
void get_actions(player_t* p_player, player_t* p_player_other);
void get_average_strategies(player_t* p_player, player_t* p_player_other);

void print_avg_strategies(uint16_t* pure_strat, player_t* p_player, player_t* p_player_other);
#endif // PLAYER_H__