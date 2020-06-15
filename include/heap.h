#ifndef IJVM_EMULATOR_HEAP_H
#define IJVM_EMULATOR_HEAP_H

#include "ijvm.h"
#include "frame.h"

typedef struct array {
    int size;
    word_t *data;
    word_t ref;
    bool wipe;
} array_t;

typedef struct arrays {
    int size;
    array_t **arrays;
} arrays_t;

int init_arrays();

void destroy_arrays();

word_t new_array(int size);

array_t *get_array(word_t ref);

bool find_array_in_frame(frame_t *current_frame, array_t *array_to_find);

bool find_array_in_frame_data(int size, const word_t *data, array_t *array_to_find);

void detect_garbage(void);

#endif
