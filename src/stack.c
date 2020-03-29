#include <stack.h>
#include <ijvm.h>
#include <util.h>
#include <string.h>
#include <stdlib.h>



int init_frame(frame_t *prev, int max_stack_size, int max_local_size, int pc, int n_args) {
    frame_t *new_frame = (frame_t *)malloc(sizeof(frame_t));

    new_frame->local_data = (word_t *)malloc(max_local_size);
    new_frame->prev_frame = prev;

    new_frame->max_stack_size = max_stack_size;
    new_frame->stack_size = -1;
    new_frame->stack_data = (word_t *)malloc(new_frame->max_stack_size * sizeof(word_t));

    new_frame->prev_pc = pc;

    new_frame->n_args = n_args;

    frame = new_frame;

    return 1;
}

void destroy_frame() {
    word_t tos_back = tos();
    int n_args = frame->n_args;

    frame_t *prev = frame->prev_frame;
    free(frame->local_data);
    free(frame->stack_data);
    free(frame);
    frame = prev;

    if (frame == NULL) {
        return;
    }

    for (int i = 0; i < n_args; ++i) {
        pop();
    }
    push(tos_back);
}

int push(word_t data) {
    if (frame->stack_size < frame->max_stack_size) {
        frame->stack_size++;
        frame->stack_data[frame->stack_size] = data;
        return 1;
    }
    return -1;
}

int pop() {
    if (frame->stack_size > -1) {
        frame->stack_data[frame->stack_size] = 0;
        frame->stack_size--;
        return 1;
    }
    return -1;
}

word_t tos() {
    return (int8_t) frame->stack_data[frame->stack_size];
}

word_t *get_stack() {
    return frame->stack_data;
}

int stack_size() {
    return frame->stack_size + 1;
}

int print_stack(FILE *fp) {
    byte_t buff[frame->stack_size];
    memcpy(buff, (byte_t *) &frame->stack_data[0], frame->stack_size + 1);
    return print_hex(buff, frame->stack_size + 1, fp);
}
