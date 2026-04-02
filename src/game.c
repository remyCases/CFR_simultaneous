// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include "../include/game.h"
#include "../include/combinatory.h"

// cant encode using combinatorial_number_system on a uint16 with Z exceeding that value
#define MAX_Z 18

int8_t result_game_rps(game_t* game, uint16_t a, uint16_t b) {
    if (a == b) return 0;
    if (a == (b + 1) % NUMBER_CASES) return 1;
    return -1;
}

// a is player 1, b is player 2
int8_t result_game_blotto(game_t* game, uint16_t coded_strat_a, uint16_t coded_strat_b) {
    int8_t sum_a = 0;
    int8_t sum_b = 0;
    uint16_t* strat_a = decode_blotto_configuration(coded_strat_a, game->blt.nb_battlefield, game->blt.nb_soldier);
    uint16_t* start_b = decode_blotto_configuration(coded_strat_b, game->blt.nb_battlefield, game->blt.nb_soldier);

    uint16_t a;
    uint16_t b;

    for(uint16_t i = 0; i < game->blt.nb_battlefield; i++) 
    {
        a = strat_a[i];
        b = start_b[i];
        if(a > b) {
            sum_a++;
        } else if(a < b) {
            sum_b++;
        }
    }

    free(strat_a);
    free(start_b);

    if(sum_a > sum_b) {
        return 1;
    } else if(sum_a < sum_b) {
        return -1;
    } else {
        return 0;
    }
}

int init_game_rps(game_t* game)
{
    game->type = GAME_RPS;
    game->nb_pure_strategies = NUMBER_CASES;
    game->result = result_game_rps;

    return EXIT_SUCCESS;
}

int init_game_blt(game_t* game, uint16_t nb_battlefield, uint16_t nb_soldier)
{
    game->type = GAME_BLT;
    game->blt.nb_battlefield = nb_battlefield;
    game->blt.nb_soldier = nb_soldier;
    game->blt.z = game->blt.nb_battlefield + game->blt.nb_soldier - 1;

    if (game->blt.z > MAX_Z) 
    {
        printf("Can't encode combinations if Z>18 since they are encoded using an uint16. Currently the Z value is %u\n", game->blt.z);
        return EXIT_FAILURE;
    }
    printf("Colonel Blotto with %u battlefields and %u soldiers\n", game->blt.nb_battlefield, game->blt.nb_soldier);
    game->nb_pure_strategies = (uint16_t)binomial_coefficient_ym(game->blt.z, game->blt.nb_battlefield - 1);
    game->result = result_game_blotto;

    return EXIT_SUCCESS;
}