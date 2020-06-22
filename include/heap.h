#ifndef HEAP_H
#define HEAP_H

#include "ijvm.h"
#include "frame.h"

typedef struct array {
    unsigned int size;
    word_t *data;
    word_t ref;
    bool wipe;
} array_t;

typedef struct arrays {
    unsigned int size;
    array_t **arrays;
} arrays_t;

arrays_t *arrays;

int init_arrays(void);

void destroy_arrays(void);

word_t new_array(unsigned int size);

array_t *get_array(word_t ref);

bool find_array_in_frame(frame_t *current_frame, array_t *array_to_find);

bool find_array_in_frame_data(int size, const word_t *data, array_t *array_to_find);

void detect_garbage(void);

#endif
