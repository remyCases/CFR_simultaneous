#include "../include/tree.h"

static TREE_STATUS new_tree(tree_t* tree, size_t capacity, uint8_t default_value) {
    tree->array = calloc(capacity, sizeof(node_t*));
    tree->capacity = capacity;
    tree->size = 0;

    for(size_t i = 0; i < capacity; i++) {
        tree->array[i] = calloc(1, sizeof(node_t*));
        tree->array[i]->left = NULL;
        tree->array[i]->right = NULL;
        tree->array[i]->middle = NULL;
        tree->array[i]->data = default_value;
    }

    return OK;
}

static int is_leaf(node_t* root)
{
    return !(root->left) && !(root->right) && !(root->middle);
}

TREE_STATUS build_tree(tree_t* tree, size_t capacity)
{
    TREE_STATUS new_status = new_tree(tree, capacity, -1);
    if(new_status) return ERROR_CAPACITY;
    tree->size += 1;

    for(size_t i = 0; i < tree->capacity; i++) {
        if(is_leaf(tree->array[i])) 
        {
            tree->array[i]->left = tree->array[tree->size];
            tree->array[tree->size]->data = 0;
            tree->size += 1;
            if(tree->size == tree->capacity) break;

            tree->array[i]->right = tree->array[tree->size];
            tree->array[tree->size]->data = 1;
            tree->size += 1;
            if(tree->size == tree->capacity) break;

            tree->array[i]->middle = tree->array[tree->size];
            tree->array[tree->size]->data = 2;
            tree->size += 1;
            if(tree->size == tree->capacity) break;
        }
    }
    return OK;
}

TREE_STATUS find_parent(tree_t* tree, size_t index_child, size_t* index_parent) {
    if(index_child >= tree->capacity) return ERROR_CAPACITY;
    *index_parent = 0;

    for(size_t i = 0; i < tree->capacity; i++) {
        if(tree->array[i]->left) {
            if (tree->array[i]->left == tree->array[index_child]) {
                *index_parent = i;
                return OK;
            }
        }
        if(tree->array[i]->right) {
            if (tree->array[i]->right == tree->array[index_child]) {
                *index_parent = i;
                return OK;
            }
        }
        if(tree->array[i]->middle) {
            if (tree->array[i]->middle == tree->array[index_child]) {
                *index_parent = i;
                return OK;
            }
        }
    }
    return PARENT_NOT_FOUND;
}

void read_tree(tree_t* tree, uint16_t* codes, size_t* number_codes) 
{
    size_t index_parent = 0;
    size_t index_child = 0;
    *number_codes = 0;

    TREE_STATUS parent_found_status = ERROR_CAPACITY;
    int code = 0;
    int inc = 1;

    for(size_t i = 0; i < tree->capacity; i++) {
        if(is_leaf(tree->array[i])) {
            index_child = i;
            code = 0;
            inc = 1;
            do {
                parent_found_status = find_parent(tree, index_child, &index_parent);
                code += tree->array[index_child]->data * inc;
                index_child = index_parent;
                inc *= 10;
            } while(index_parent);

            codes[*number_codes] = code;
            *number_codes += 1;
        }
    }
}