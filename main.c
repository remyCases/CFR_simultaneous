#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "./include/xorshiro256.h"
#include "./include/player.h"
#include "./include/utility.h"
#include "./include/combinatory.h"

typedef enum RPS {
    ROCK,
    PAPER,
    SCISSOR,
    NUMBER_CASES,
} RPS;

static uint16_t number_battlefield  = 3;
static uint16_t number_soldier = 5;
// Z is the number of gaps in the stars-and-bars problem
#define Z (number_battlefield + number_soldier - 1)
// cant encode using combinatorial_number_system on a uint16 with Z exceeding that value
#define MAX_Z 18

int8_t result_game_rps(uint16_t a, uint16_t b) {
    if (a == b) return 0;
    if (a == (b + 1) % NUMBER_CASES) return 1;
    return -1;
}

// a is player 1, b is player 2
int8_t result_game_blotto(uint16_t coded_strat_a, uint16_t coded_strat_b) {
    int8_t sum_a = 0;
    int8_t sum_b = 0;
    uint16_t base = number_battlefield <= 3 ? Z : Z + 3 - number_battlefield;

    uint16_t a = coded_strat_a%base;
    uint16_t a_prev = a;
    uint16_t b = coded_strat_b%base;
    uint16_t b_prev = b;
    coded_strat_a = coded_strat_a/base;
    coded_strat_b = coded_strat_b/base;
    base++;

    for(uint16_t i = 0; i < number_battlefield; i++) {
        if(a > b) {
            sum_a++;
        } else if(a < b) {
            sum_b++;
        }
        
        if (i < number_battlefield - 2) {
            a = coded_strat_a%base - a_prev - 1;
            b = coded_strat_b%base - b_prev - 1;
        } else if (i == number_battlefield - 2) {
            a = Z - a_prev - 1;
            b = Z - b_prev - 1;
        }
        a_prev = coded_strat_a%base;
        b_prev = coded_strat_b%base;
        coded_strat_a = coded_strat_a/base;
        coded_strat_b = coded_strat_b/base;
        base++;
    }

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

    uint16_t nb_pure_strategies = 0;
    uint16_t* pure_strat = NULL;
    // temporary array used in the algo T for representing one combination
    uint16_t* c = NULL;
    int8_t (*result_game)(uint16_t, uint16_t) = NULL;

    if (argc >= 2) 
    {
        if (!strcmp(argv[1], "RPS"))
        {
            nb_pure_strategies = NUMBER_CASES;
            pure_strat = calloc(nb_pure_strategies, sizeof(uint16_t));
            pure_strat[0] = ROCK;
            pure_strat[1] = PAPER;
            pure_strat[2] = SCISSOR;
            result_game = result_game_rps;
        } 
        else if (!strcmp(argv[1], "Blotto")) 
        {
            if(argc >= 4)
            {
                number_battlefield = atol(argv[3]);
            }
            if(argc >= 5)
            {
                number_soldier = atol(argv[4]);
            }
            if (Z > MAX_Z) 
            {
                printf("Can't encode combinations if Z>18 since they are encoded using an uint16. Currently the Z value is %u\n", Z);
                return EXIT_SUCCESS;
            }
            printf("Colonel Blotto with %u battlefields and %u soldiers\n", number_battlefield, number_soldier);
            nb_pure_strategies = (uint16_t)binomial_coefficient_ym(Z, number_battlefield - 1);
            c = calloc(number_battlefield + 1, sizeof(uint16_t));
            pure_strat = calloc(nb_pure_strategies, sizeof(uint16_t));
            lexicographic_combinations(Z, number_battlefield - 1, c, pure_strat);
            result_game = result_game_blotto;
        } 
        else 
        {
            printf("Incorrect choice\n");
            return EXIT_FAILURE;
        }
    }

    player_t X;
    player_t Y;
    init_player(&X, nb_pure_strategies);
    init_player(&Y, nb_pure_strategies);

    utility_t utilities;
    init_utility(&utilities, nb_pure_strategies);
    compute_all_utilities(pure_strat, &utilities, result_game);

    if (nb_pure_strategies < 15) 
    {
        print_utilities(pure_strat, &utilities);
    }

    set_seed(10);

    uint64_t N = 100;
    if (argc >= 3) 
    {
        N = atol(argv[2]);
    }
    printf("Executing Monte-Carlo CRF for %llu steps\n", N);
    export_pure_strategies(f, pure_strat, nb_pure_strategies);
    for(uint64_t j = 0; j < N; j++) 
    {
        compute_strategies(&X, &Y);
        compute_actions(&X, &Y);
        accumulate_regret(&X, &Y, &utilities);
        
        compute_average_strategies(&X, &Y);
        export_avg_strategies(f, &X);
    }

    compute_average_strategies(&X, &Y);

    print_avg_strategies(pure_strat, &X, &Y);
    fflush(stdout);

    free_player(&X);
    free_player(&Y);
    free_utility(&utilities);
    free(c);
    free(pure_strat);

    if (f) {
        fclose(f);
    }
    return EXIT_SUCCESS;
}