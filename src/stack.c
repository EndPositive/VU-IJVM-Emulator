#include <stack.h>
#include <ijvm.h>
#include <util.h>
#include <string.h>
#include <stdlib.h>

stack_t *stack;

int init_stack(int max_size) {
    stack = (stack_t *)malloc(sizeof(stack_t));
    stack->max_size = max_size;
    stack->size = -1;
    stack->data = (word_t *)malloc(stack->max_size * sizeof(word_t));
    return 1;
}

void destroy_stack() {
    free(stack->data);
    free(stack);
}

int push(word_t data) {
    if (stack->size < stack->max_size) {
        stack->size++;
        stack->data[stack->size] = data;
        return 1;
    }
    return -1;
}

int pop() {
    if (stack->size > -1) {
        stack->data[stack->size] = 0;
        stack->size--;
        return 1;
    }
    return -1;
}

word_t tos() {
    return (int8_t) stack->data[stack->size];
}

word_t *get_stack() {
    return stack->data;
}

int stack_size() {
    return stack->size + 1;
}

int print_stack(FILE *fp) {
    byte_t buff[stack->size];
    memcpy(buff, (byte_t *) &stack->data[0], stack->size + 1);
    return print_hex(buff, stack->size + 1, fp);
}
