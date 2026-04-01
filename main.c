// Copyright (C) 2023 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "./include/xorshiro256.h"
#include "./include/player.h"
#include "./include/utility.h"
#include "./include/combinatory.h"
#include "./include/game.h"

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

int main(int argc, char *argv[]) 
{
    // file where to export strategies
    FILE* f = fopen("export_avg_strategy.csv", "w");
    game_t game;

    if (argc >= 2) 
    {
        if (!strcmp(argv[1], "RPS"))
        {
            game.type = GAME_RPS;
            game.nb_pure_strategies = NUMBER_CASES;
            game.result = result_game_rps;
        } 
        else if (!strcmp(argv[1], "Blotto")) 
        {
            game.type = GAME_BLT;
            game.blt.nb_battlefield = 3;
            game.blt.nb_soldier = 5;
            if(argc >= 4)
            {
                game.blt.nb_battlefield = atol(argv[3]);
            }
            if(argc >= 5)
            {
                game.blt.nb_soldier = atol(argv[4]);
            }
            game.blt.z = game.blt.nb_battlefield + game.blt.nb_soldier - 1;
            if (game.blt.z > MAX_Z) 
            {
                printf("Can't encode combinations if Z>18 since they are encoded using an uint16. Currently the Z value is %u\n", game.blt.z);
                return EXIT_SUCCESS;
            }
            printf("Colonel Blotto with %u battlefields and %u soldiers\n", game.blt.nb_battlefield, game.blt.nb_soldier);
            game.nb_pure_strategies = (uint16_t)binomial_coefficient_ym(game.blt.z, game.blt.nb_battlefield - 1);
            game.result = result_game_blotto;
        } 
        else 
        {
            printf("Incorrect choice\n");
            return EXIT_FAILURE;
        }
    }

    player_t X;
    player_t Y;
    init_player(&X, game.nb_pure_strategies);
    init_player(&Y, game.nb_pure_strategies);

    utility_t utilities;
    init_utility(&utilities, game.nb_pure_strategies);
    compute_all_utilities(&utilities, &game);

    if (game.nb_pure_strategies < 15) 
    {
        print_utilities(&utilities);
    }

    set_seed(20);

    uint64_t N = 100;
    if (argc >= 3) 
    {
        N = atol(argv[2]);
    }
    printf("Executing Monte-Carlo CRF for %llu steps\n", N);
    export_pure_strategies(f, game.nb_pure_strategies);
    for(uint64_t j = 0; j < N; j++) 
    {
        compute_strategies(&X, &Y);
        compute_actions(&X, &Y);
        accumulate_regret(&X, &Y, &utilities);
        
        compute_average_strategies(&X, &Y);
        export_avg_strategies(f, &X);
    }

    compute_average_strategies(&X, &Y);

    print_avg_strategies(&X, &Y, &game);
    fflush(stdout);

    free_player(&X);
    free_player(&Y);
    free_utility(&utilities);

    if (f) {
        fclose(f);
    }
    return EXIT_SUCCESS;
}