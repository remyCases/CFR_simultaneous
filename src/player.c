// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include "../include/player.h"

void init_player(player_t* p_player, uint16_t nb_pure_strategy) 
{
    p_player->nb_cases = nb_pure_strategy;
    p_player->cumulative_regret = calloc(nb_pure_strategy, sizeof(int32_t));
    p_player->strategy = calloc(nb_pure_strategy, sizeof(float));
    p_player->sum_strategy = calloc(nb_pure_strategy, sizeof(float));
    p_player->avg_strategy = calloc(nb_pure_strategy, sizeof(float));

    p_player->action = 0;
    p_player->sum_regret = 0;
}

void free_player(player_t* p_player) 
{
    free(p_player->cumulative_regret);
    free(p_player->strategy);
    free(p_player->sum_strategy);
    free(p_player->avg_strategy);
}

void sum_regret(player_t* p_player) 
{
    p_player->sum_regret = 0;
    for(int i = 0; i < p_player->nb_cases; i++) 
    {
        p_player->sum_regret += p_player->cumulative_regret[i] > 0 ? p_player->cumulative_regret[i] : 0;
    }
}

static void compute_strategy(player_t* p_player) {
    sum_regret(p_player);
    for(int i = 0; i < p_player->nb_cases; i++) 
    {
        if(p_player->sum_regret > 0) 
        {
            p_player->strategy[i] = p_player->cumulative_regret[i] > 0 ? (float)p_player->cumulative_regret[i] / (float)p_player->sum_regret : 0.0f;
        }
        else 
        {
            p_player->strategy[i] = (1.0f / (float)p_player->nb_cases);
        }
        p_player->sum_strategy[i] += p_player->strategy[i];
    }
}

void compute_strategies(player_t* p_player, player_t* p_player_other) 
{
    compute_strategy(p_player);
    compute_strategy(p_player_other);
}

static void compute_action(player_t* p_player) 
{
    float cumulative_distribution = 0.0f;
    float r = (float)to_double(next_xorshiro256());

    for(uint16_t i = 0; i < p_player->nb_cases; i++) 
    {
        cumulative_distribution += p_player->strategy[i];
        if(r < cumulative_distribution) 
        {
            p_player->action = i;
            break;
        }
    }
}

void compute_actions(player_t* p_player, player_t* p_player_other) 
{
    compute_action(p_player);
    compute_action(p_player_other);
}

static void compute_average_strategy(player_t* p_player) 
{
    float normalizing_sum = 0.0f;

    for(int i = 0; i < p_player->nb_cases; i++) 
    {
        normalizing_sum += p_player->sum_strategy[i];
    }

    for(int i = 0; i < p_player->nb_cases; i++) 
    {
        if(normalizing_sum > 0.0f) {
            p_player->avg_strategy[i] = p_player->sum_strategy[i] / normalizing_sum;
        } else {
            p_player->avg_strategy[i] = (1.0f / (float)p_player->nb_cases);
        }
    }
}

void compute_average_strategies(player_t* p_player, player_t* p_player_other) 
{
    compute_average_strategy(p_player);
    compute_average_strategy(p_player_other);
}

void print_blotto_strategy(uint16_t coded_strat, uint16_t nb_battlefield, uint16_t n_gaps) {
    uint16_t base = nb_battlefield <= 3 ? n_gaps : n_gaps + 3 - nb_battlefield;
    uint16_t a = coded_strat%base;
    uint16_t a_prev = a;
    coded_strat = coded_strat/base;
    base++;

    for(uint16_t i = 0; i < nb_battlefield; i++) {
        printf("%d, ", a);
        if (i < nb_battlefield - 2) {
            a = coded_strat%base - a_prev - 1;
        } else if (i == nb_battlefield - 2) {
            a = n_gaps - a_prev - 1;
        }
        a_prev = coded_strat%base;
        coded_strat = coded_strat/base;
        base++;
    }
}

void print_avg_strategies(player_t* p_player, player_t* p_player_other, uint16_t b, uint16_t s) 
{
    printf("Nash strategies :\n");
    printf("Action:\tplayer 1\tplayer 2\n");
    for(int i = 0; i < p_player->nb_cases; i++) 
    {
        // TODO : make it not specific to blotto
        uint16_t* d = decode_blotto_configuration(i, b, s);
        for (int j = 0; j < b; j++)
        {
            printf("%u, ", d[j]);
        }
        free(d);
        printf(
            ":\t%.7f\t%.7f\n",
            p_player->avg_strategy[i],
            p_player_other->avg_strategy[i]
        );
    }
}

void export_pure_strategies(FILE* f, uint16_t nstrat) 
{
    fprintf(f, "%d", 0);
    for(int i = 1; i < nstrat; i++) 
    {
        fprintf(f, ";%d", i);
    }
    fprintf(f, "\n");
}

void export_avg_strategies(FILE* f, player_t* p_player) 
{
    fprintf(f, "%.7f", p_player->avg_strategy[0]);
    for(int i = 1; i < p_player->nb_cases; i++) 
    {
        fprintf(f, ";%.7f", p_player->avg_strategy[i]);
    }
    fprintf(f, "\n");
}