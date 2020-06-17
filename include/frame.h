#ifndef STACK_H
#define STACK_H

#include <ijvm.h>

typedef struct frame {
    unsigned short local_size;
    word_t *local_data;
    struct frame *prev_frame;
    unsigned short stack_size;
    word_t *stack_data;
    int prev_pc;
} frame_t;

frame_t *frame;

frame_t *init_frame(frame_t *prev, unsigned short local_size, int pc);

void destroy_frame(void);

int push(word_t data);

int pop(void);

#endif
