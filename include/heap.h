#ifndef IJVM_EMULATOR_HEAP_H
#define IJVM_EMULATOR_HEAP_H

#include "ijvm.h"

typedef struct array {
    int size;
    word_t *data;
    word_t ref;
} array_t;

typedef struct arrays {
    int size;
    array_t **arrays;
} arrays_t;

int init_arrays();

void destroy_arrays();

word_t new_array(int size);

array_t *get_array(word_t ref);

#endif
