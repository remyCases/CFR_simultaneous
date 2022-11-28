#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "./include/xorshiro256.h"
#include "./include/tree.h"

// Colonel Blotto is a 2-players 1-turn game in which each player deploy N soldiers among M battlefields.
// After deployment, for each battlefield, the player who deployed the bigger amount of soldiers wins it.
// The player who won the most battlefield wins the game.
// For instance, a following deployment lets player 1 win :
// 2 - 1 for battlefield 1
// 2 - 1 for battlefield 2
// 0 - 2 for battlefield 3

// A usefull deployment representation is a M-array, which represents the M different battlefield.
// Each element of the array can have N different values, which represents the N available soldiers,
// with the condition than Sum(array) = N.
// Since each soldier can be deployed independantly, the number of pure strategy are then M^N.
// Instead of using N-array, we will convert it into an hexa int.
// Thus, the previous example will lead to the following array : 
// Player 1 : [2, 2, 0] => 2 + 2*3 + 0*3^2 = 8
// Player 2 : [1, 1, 2] => 1 + 1*3 + 2*3^2 = 22
// With this representation, one can show the number of strategy are K = (M+N-1 M-1)
#define NUMBER_BATTLEFIELD 3
#define NUMBER_SOLDIER 5

static size_t NUMBER_PURE_STRATEGY = (size_t)(pow(NUMBER_BATTLEFIELD, NUMBER_SOLDIER) + 0.5f);

// strategy and utility are then K arrays
typedef uint16_t* strategy_t;
typedef int8_t* utility_t;

typedef struct player_s {
    int32_t* cumulative_regret;
    float* strategy;

    int action;
    int32_t sum_regret;
} player_t;

void init_player(player_t* p_player) {
    p_player->cumulative_regret = calloc(NUMBER_PURE_STRATEGY, sizeof(int32_t));
    p_player->strategy = calloc(NUMBER_PURE_STRATEGY, sizeof(float));

    p_player->action = 0;
    p_player->sum_regret = 0;
}

// compute sum of regrets
void sum_regret(player_t* p_player) {
    p_player->sum_regret = 0;
    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        p_player->sum_regret += (p_player->cumulative_regret[i] > 0 ? p_player->cumulative_regret[i] : 0);
    }
}

void get_strategy(player_t* p_player) {
    sum_regret(p_player);
    if(p_player->sum_regret > 0) {
        for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
            p_player->strategy[i] = ((p_player->cumulative_regret[i] > 0 ? p_player->cumulative_regret[i] : 0) / 
            (float)p_player->sum_regret);
        }
    } else {
        for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
            p_player->strategy[i] = (1.0f / (float)NUMBER_PURE_STRATEGY);
        }
    }
}

void get_strategies(player_t* p_player, player_t* p_player_other) {
    get_strategy(p_player);
    get_strategy(p_player_other);
}

static inline double to_double(uint64_t x) {
    const union { uint64_t i; double d; } u = { .i = UINT64_C(0x3FF) << 52 | x >> 12 };
    return u.d - 1.0;
}

void get_action(player_t* p_player) {
    float cumulative_distribution = 0.0f;
    float r = to_double(next_xorshiro256());

    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
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

int utility_end(int battlefield[NUMBER_BATTLEFIELD], int battlefield_other[NUMBER_BATTLEFIELD]) {
    int sum_battlefield = 0;
    int sum_battlefield_other = 0;

    for(int i = 0; i < NUMBER_BATTLEFIELD; i++) {
        if(battlefield[i] > battlefield_other[i]) {
            sum_battlefield++;
        } else if(battlefield[i] < battlefield_other[i]) {
            sum_battlefield_other++;
        }
    }

    if(sum_battlefield > sum_battlefield_other) {
        return 1;
    } else if(sum_battlefield < sum_battlefield_other) {
        return -1;
    } else {
        return 0;
    }
}

void get_average_strategy(player_t* p_player) {
    p_player->sum_regret = 0;
    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        p_player->sum_regret += p_player->cumulative_regret[i];
    }

    if(p_player->sum_regret > 0) {
        for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
            p_player->strategy[i] = (p_player->cumulative_regret[i] / (float)p_player->sum_regret);
        }
    } else {
        for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
            p_player->strategy[i] = (1.0f / (float)NUMBER_PURE_STRATEGY);
        }
    }
}

void get_average_strategies(player_t* p_player, player_t* p_player_other) {
    get_average_strategy(p_player);
    get_average_strategy(p_player_other);
}

static inline void uncoded_battlefield(int coded_battlefield, int battlefield[NUMBER_BATTLEFIELD]) {
    for(int j = 0; j < NUMBER_BATTLEFIELD; j++) {
        battlefield[j] = 0;
    }

    for(int j = 0; j < NUMBER_SOLDIER; j++) {
        battlefield[coded_battlefield%10] += 1;
        coded_battlefield /= 10;
    }
}

void accumulate_regret(player_t* p_player, player_t* p_player_other, strategy_t pure_strategy) 
{
    utility_t utility = calloc(NUMBER_PURE_STRATEGY, sizeof(uint8_t));
    int battlefield_tmp[NUMBER_BATTLEFIELD];
    int battlefield[NUMBER_BATTLEFIELD];
    int battlefield_other[NUMBER_BATTLEFIELD];

    uncoded_battlefield(pure_strategy[p_player->action], battlefield);
    uncoded_battlefield(pure_strategy[p_player_other->action], battlefield_other);

    // for all pure strategy, compute the utility
    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        uncoded_battlefield(pure_strategy[i], battlefield_tmp);
        
        // compute the utility against the battlefield previously computed
        utility[i] = utility_end(battlefield_tmp, battlefield_other);
    }

    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        p_player->cumulative_regret[i] += (utility[i] - utility[p_player_other->action]);
    }

    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        uncoded_battlefield(pure_strategy[i], battlefield_tmp);
        
        // compute the utility against the battlefield previously computed
        utility[i] = utility_end(battlefield_tmp, battlefield);
    }

    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        p_player_other->cumulative_regret[i] += (utility[i] - utility[p_player->action]);
    }
}

int main(void) {
    player_t X;
    player_t Y;
    uint64_t N = 1000;

    set_seed(0);
    init_player(&X); 
    init_player(&Y); 

    tree_t tree;
    strategy_t pure_strat = calloc(NUMBER_PURE_STRATEGY, sizeof(uint16_t));
    int nb_strat = 0;

    size_t ntree = (size_t)((pow(NUMBER_BATTLEFIELD, 6) - 1) / 2 + 0.5f); // ???
    build_tree(&tree, ntree);
    read_tree(&tree, pure_strat, &nb_strat);

    for(uint64_t j = 0; j < N; j++) {
        get_strategies(&X, &Y);
        get_actions(&X, &Y);
        accumulate_regret(&X, &Y, pure_strat);
    }

    get_average_strategies(&X, &Y);

    int max_x = 0;
    float max_strat_x = 0;
    int max_y = 0;
    float max_strat_y = 0;

    printf("Nash strategies :\n");
    for(int i = 0; i < NUMBER_PURE_STRATEGY; i++) {
        printf("%d\t:%.7f && %.7f\n", pure_strat[i], X.strategy[i], Y.strategy[i]);

        if (X.strategy[i] > max_strat_x) {
            max_strat_x = X.strategy[i];
            max_x = pure_strat[i];
        }
        if (Y.strategy[i] > max_strat_y) {
            max_strat_y = Y.strategy[i];
            max_y = pure_strat[i];
        }
    }

    printf("max strat x\t:%d\n", max_x);
    printf("max strat y\t:%d\n", max_y);
    fflush(stdout);

    return EXIT_SUCCESS;
}