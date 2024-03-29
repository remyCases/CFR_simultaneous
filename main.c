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
#define Z (number_battlefield + number_soldier - 1)

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

void print_blotto_strategy(uint16_t coded_strat) {
    uint16_t base = number_battlefield <= 3 ? Z : Z + 3 - number_battlefield;
    uint16_t a = coded_strat%base;
    uint16_t a_prev = a;
    coded_strat = coded_strat/base;
    base++;

    for(uint16_t i = 0; i < number_battlefield; i++) {
        printf("%d, ", a);
        if (i < number_battlefield - 2) {
            a = coded_strat%base - a_prev - 1;
        } else if (i == number_battlefield - 2) {
            a = Z - a_prev - 1;
        }
        a_prev = coded_strat%base;
        coded_strat = coded_strat/base;
        base++;
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    
    uint16_t nb_pure_strategies = 0;
    uint16_t* pure_strat = NULL;
    uint16_t* c = NULL;
    int8_t (*result_game)(uint16_t, uint16_t) = NULL;

    if (argc >= 2) {
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
            if(argc >= 4) {
                number_battlefield = atol(argv[3]);
            }
            if(argc >= 5) {
                number_soldier = atol(argv[4]);
            }
            nb_pure_strategies = (uint16_t)binomial_coefficient_ym(Z, number_battlefield - 1);
            c = calloc(number_battlefield - 1 + 2, sizeof(uint16_t));
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

    printf("Actions available\n");
    for(uint16_t i = 0; i < nb_pure_strategies; i++) {
        if (!strcmp(argv[1], "Blotto")) {
            printf("%d:\t", pure_strat[i]);
            print_blotto_strategy(pure_strat[i]);
        } else { 
            printf("%d\n", pure_strat[i]);
        }
    }
    printf("\n");

    player_t X;
    player_t Y;
    init_player(&X, nb_pure_strategies);
    init_player(&Y, nb_pure_strategies);

    utility_t utilities;
    init_utility(&utilities, nb_pure_strategies);
    compute_all_utilities(pure_strat, &utilities, result_game);

    if (nb_pure_strategies < 15) {
        print_utilities(pure_strat, &utilities);
    }

    set_seed(10);

     uint64_t N = 100;
    if (argc >= 3) {
        N = atol(argv[2]);
        printf("Executing Monte-Carlo CRF for %llu steps\n", N);
    }
    for(uint64_t j = 0; j < N; j++) {
        get_strategies(&X, &Y);
        get_actions(&X, &Y);
        accumulate_regret(&X, &Y, &utilities);
    }

    get_average_strategies(&X, &Y);

    print_avg_strategies(pure_strat, &X, &Y);
    fflush(stdout);

    free_player(&X);
    free_player(&Y);
    free_utility(&utilities);
    free(c);
    free(pure_strat);
    return EXIT_SUCCESS;
}