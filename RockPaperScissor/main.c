#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./include/xorshiro256.h"

#define STR(X) #X

typedef enum RPS {
    ROCK,
    PAPER,
    SCISSOR,
    NUMBER_CASES,
} RPS;

typedef struct player_s {
    int cumulative_regret[NUMBER_CASES];
    float strategy[NUMBER_CASES];

    int sum_regret;
    int action;
} player_t;

typedef int utility_t[NUMBER_CASES];

void init_player(player_t* p_player) {
    for(int i = 0; i < NUMBER_CASES; i++) {
        p_player->cumulative_regret[i] = 0;
        p_player->strategy[i] = 0.0f;
    }

    p_player->sum_regret = 0;
    p_player->action = NUMBER_CASES;
}

void sum_regret(player_t* p_player) {
    p_player->sum_regret = 0;
    for(int i = 0; i < NUMBER_CASES; i++) {
        p_player->sum_regret += p_player->cumulative_regret[i];
    }
}

void get_strategy(player_t* p_player) {
    sum_regret(p_player);
    if(p_player->sum_regret > 0) {
        for(int i = 0; i < NUMBER_CASES; i++) {
            p_player->strategy[i] = ((p_player->cumulative_regret[i] > 0 ? p_player->cumulative_regret[i] : 0) / 
            (float)p_player->sum_regret);
        }
    } else {
        for(int i = 0; i < NUMBER_CASES; i++) {
            p_player->strategy[i] = (1.0f / (float)NUMBER_CASES);
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

    for(int i = 0; i < NUMBER_CASES; i++) {
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

void accumulate_regret(player_t* p_player, player_t* p_player_other) {
    utility_t utility;

    int action = p_player->action;
    int action_other = p_player_other->action;

    utility[action_other] = 0; // playing the same action lead to a draw
    utility[action_other == SCISSOR ? ROCK: action_other + 1] = 1; // playing the next action lead to a win
    utility[action_other == ROCK ? SCISSOR: action_other - 1] = -1; // playing the previous action lead to a loose

    for(int i = 0; i < NUMBER_CASES; i++) {
        p_player->cumulative_regret[i] += (utility[i] - utility[action_other]);
    }

    utility[action] = 0; // playing the same action lead to a draw
    utility[action == SCISSOR ? ROCK: action + 1] = 1; // playing the next action lead to a win
    utility[action == ROCK ? SCISSOR: action - 1] = -1; // playing the previous action lead to a loose

    for(int i = 0; i < NUMBER_CASES; i++) {
        p_player_other->cumulative_regret[i] += (utility[i] - utility[action]);
    }
}

void get_average_strategy(player_t* p_player) {
    sum_regret(p_player);
    if(p_player->sum_regret > 0) {
        for(int i = 0; i < NUMBER_CASES; i++) {
            p_player->strategy[i] = (p_player->cumulative_regret[i] / (float)p_player->sum_regret);
        }
    } else {
        for(int i = 0; i < NUMBER_CASES; i++) {
            p_player->strategy[i] = (1.0f / (float)NUMBER_CASES);
        }
    }
}

void get_average_strategies(player_t* p_player, player_t* p_player_other) {
    get_average_strategy(p_player);
    get_average_strategy(p_player_other);
}

int main(void) {
    player_t X;
    player_t Y;
    uint64_t N = 1000000;

    set_seed(0);
    init_player(&X); 
    init_player(&Y); 

    for(uint64_t j = 0; j < N; j++) {
        get_strategies(&X, &Y);
        get_actions(&X, &Y);

        accumulate_regret(&X, &Y);
    }

    get_average_strategies(&X, &Y);

    printf("Nash X strategy :\n ");
    printf(STR(ROCK)":%.2f; ", X.strategy[ROCK]);
    printf(STR(PAPER)":%.2f; ", X.strategy[PAPER]);
    printf(STR(SCISSOR)":%.2f\n", X.strategy[SCISSOR]);

    printf("Nash Y strategy :\n ");
    printf(STR(ROCK)":%.2f; ", Y.strategy[ROCK]);
    printf(STR(PAPER)":%.2f; ", Y.strategy[PAPER]);
    printf(STR(SCISSOR)":%.2f\n", Y.strategy[SCISSOR]);

    fflush(stdout);
    return EXIT_SUCCESS;
}