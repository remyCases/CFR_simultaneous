// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include "../include/utility.h"

void init_utility(utility_t* p_utility, uint16_t nb_strat) 
{
    p_utility->nb_strat = nb_strat;
    p_utility->utility = calloc(nb_strat * nb_strat, sizeof(int8_t));
}

void free_utility(utility_t* p_utility) 
{
    free(p_utility->utility);
}

void compute_all_utilities(utility_t* p_utility, game_t* game) 
{
    for(uint16_t i = 0; i < p_utility->nb_strat; i++) 
    {
        for(uint16_t j = 0; j < p_utility->nb_strat; j++) 
        {
            p_utility->utility[i * p_utility->nb_strat + j] = game->result(game, i, j);
        }
    }
}

static inline int8_t get_utility(utility_t* p_utility, uint16_t i, uint16_t j) 
{
    return p_utility->utility[i * p_utility->nb_strat + j];
}

void accumulate_regret(player_t* p_player, player_t* p_player_other, utility_t* p_utility) 
{
    uint16_t action = p_player->action;
    uint16_t action_other = p_player_other->action;

    for(uint16_t i = 0; i < p_utility->nb_strat; i++) 
    {
        p_player->cumulative_regret[i] += (get_utility(p_utility, i, action_other) - get_utility(p_utility, action, action_other));
        p_player_other->cumulative_regret[i] += (get_utility(p_utility, i, action) - get_utility(p_utility, action_other, action));
    }
}

void print_utilities(utility_t* p_utility) 
{
    printf("Utility Matrix :\nrows: player1 \\ columns: player2\n");
    for(uint16_t i = 0; i < p_utility->nb_strat+1; i++) 
    {
        for(uint16_t j = 0; j < p_utility->nb_strat+1; j++) 
        {
            if (!j && !i) printf("\t|");
            if (j && !i) printf("%d\t|", j-1);
            if (!j && i) printf("%d\t|", i-1);
            else if (i) printf("%d\t|", get_utility(p_utility, i-1,j-1));
        }
        printf("\n");
    }
    printf("\n\n");
}