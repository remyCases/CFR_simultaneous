// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include "../include/player.h"

void init_player(player_t* p_player, uint16_t nb_pure_strategy) {
    p_player->nb_cases = nb_pure_strategy;
    p_player->cumulative_regret = calloc(nb_pure_strategy, sizeof(int32_t));
    p_player->strategy = calloc(nb_pure_strategy, sizeof(float));
    p_player->sum_strategy = calloc(nb_pure_strategy, sizeof(float));
    p_player->avg_strategy = calloc(nb_pure_strategy, sizeof(float));

    p_player->action = 0;
    p_player->sum_regret = 0;
}

void free_player(player_t* p_player) {
    free(p_player->cumulative_regret);
    free(p_player->strategy);
    free(p_player->sum_strategy);
    free(p_player->avg_strategy);
}

void sum_regret(player_t* p_player) {
    p_player->sum_regret = 0;
    for(int i = 0; i < p_player->nb_cases; i++) {
        p_player->sum_regret += p_player->cumulative_regret[i] > 0 ? p_player->cumulative_regret[i] : 0;
    }
}

void get_strategy(player_t* p_player) {
    sum_regret(p_player);
    for(int i = 0; i < p_player->nb_cases; i++) {
        if(p_player->sum_regret > 0) {
            p_player->strategy[i] = p_player->cumulative_regret[i] > 0 ? (float)p_player->cumulative_regret[i] / (float)p_player->sum_regret : 0.0f;
        } else {
            p_player->strategy[i] = (1.0f / (float)p_player->nb_cases);
        }
        p_player->sum_strategy[i] += p_player->strategy[i];
    }
}

void get_strategies(player_t* p_player, player_t* p_player_other) {
    get_strategy(p_player);
    get_strategy(p_player_other);
}

void get_action(player_t* p_player) {
    float cumulative_distribution = 0.0f;
    float r = (float)to_double(next_xorshiro256());

    for(uint16_t i = 0; i < p_player->nb_cases; i++) {
        cumulative_distribution += p_player->strategy[i];
        if(r < cumulative_distribution) {
            p_player->action = i;
            break;
        }
    }
}

void get_actions(player_t* p_player, player_t* p_player_other) {
    get_action(p_player);
    get_action(p_player_other);
}

void get_average_strategy(player_t* p_player) {
    float normalizing_sum = 0.0f;

    for(int i = 0; i < p_player->nb_cases; i++) {
        normalizing_sum += p_player->sum_strategy[i];
    }

    for(int i = 0; i < p_player->nb_cases; i++) {
        if(normalizing_sum > 0.0f) {
            p_player->avg_strategy[i] = p_player->sum_strategy[i] / normalizing_sum;
        } else {
            p_player->avg_strategy[i] = (1.0f / (float)p_player->nb_cases);
        }
    }
}

void get_average_strategies(player_t* p_player, player_t* p_player_other) {
    get_average_strategy(p_player);
    get_average_strategy(p_player_other);
}

void print_avg_strategies(uint16_t* pure_strat, player_t* p_player, player_t* p_player_other) {
    printf("Nash strategies :\n");
    printf("Action\t:player 1 && player 2\n");
    for(int i = 0; i < p_player->nb_cases; i++) {
        printf("%d\t:%.7f && %.7f\n", pure_strat[i], p_player->avg_strategy[i], p_player_other->avg_strategy[i]);
    }
}