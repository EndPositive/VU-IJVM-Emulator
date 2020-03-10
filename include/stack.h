#ifndef STACK_H
#define STACK_H

#include <ijvm.h>

typedef struct stack {
    int max_size;
    int size;
    word_t *data;
} stack_t;

int init_stack(int max_size);

void destroy_stack(void);

int push(word_t data);

int pop(void);

int print_stack(FILE *fp);

#endif