#ifndef TREE_H__
#define TREE_H__

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef enum TREE_STATUS {
    OK,
    ERROR_CAPACITY,
    PARENT_NOT_FOUND,
} TREE_STATUS;

typedef struct node_s {
    uint8_t data;
    struct node_s *left, *right, *middle;
} node_t;

typedef struct tree_s {
    size_t capacity;
    size_t size;
    node_t** array;
} tree_t;

TREE_STATUS build_tree(tree_t* tree, size_t deepth);
void read_tree(tree_t* tree, uint16_t* codes, size_t* number_codes);

#endif // TREE_H__