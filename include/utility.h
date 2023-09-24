// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#ifndef UTILITY_H__
#define UTILITY_H__

#include <stdint.h>
#include "../include/player.h"

typedef struct utility_s {
    uint16_t nb_strat;
    int8_t* utility;
} utility_t;

void init_utility(utility_t* p_utility, uint16_t nb_utility);
void free_utility(utility_t* p_utility);

void compute_all_utilities(uint16_t* pure_strat, utility_t* p_utilitiy, int8_t (*result_game)(uint16_t, uint16_t));
void accumulate_regret(player_t* p_player, player_t* p_player_other, utility_t* p_utility);

void print_utilities(uint16_t* pure_strat, utility_t* p_utility);
#endif // UTILITY_H__