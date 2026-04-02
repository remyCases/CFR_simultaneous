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

int main(int argc, char *argv[]) 
{
    // file where to export strategies
    FILE* f = fopen("export_avg_strategy.csv", "w");
    game_t game;

    uint64_t seed = 10;
    if (argc >= 3)
    {
        seed = atoll(argv[2]);
    }

    uint64_t N = 100;
    if (argc >= 4) 
    {
        N = atoll(argv[3]);
    }

    if (argc >= 2) 
    {
        if (!strcmp(argv[1], "RPS"))
        {
            init_game_rps(&game);
        } 
        else if (!strcmp(argv[1], "Blotto")) 
        {
            uint16_t nb_battlefield = 3;
            uint16_t nb_soldier = 5;
            if(argc >= 5)
            {
                nb_battlefield = atoi(argv[4]);
            }
            if(argc >= 6)
            {
                nb_soldier = atoi(argv[5]);
            }
            if (init_game_blt(&game, nb_battlefield, nb_soldier) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
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

    set_seed(seed);
    printf("Executing Monte-Carlo CFR for %llu steps with seed %llu\n", N, seed);
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