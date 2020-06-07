#ifndef STACK_H
#define STACK_H

#include <ijvm.h>

typedef struct frame {
    int local_size;
    word_t *local_data;
    struct frame *prev_frame;
    int stack_size;
    word_t *stack_data;
    int prev_pc;
    int n_args;
} frame_t;

frame_t *frame;

int init_frame(frame_t *prev, unsigned short, int pc, unsigned short n_args);

void destroy_frame(void);

int push(word_t data);

int pop(void);

int print_stack(FILE *fp);

#endif
