#include <frame.h>
#include <stdlib.h>

frame_t *init_frame(frame_t *prev, unsigned short local_size, int pc) {
    frame_t *new_frame = (frame_t *)malloc(sizeof(frame_t));

    new_frame->local_size = local_size;
    new_frame->local_data = (word_t *)calloc(new_frame->local_size, sizeof(word_t));
    new_frame->prev_frame = prev;

    new_frame->stack_size = 0;
    new_frame->stack_data = NULL;

    new_frame->prev_pc = pc;

    return new_frame;
}

void destroy_frame() {
    frame_t *prev = frame->prev_frame;
    free(frame->local_data);
    free(frame->stack_data);
    free(frame);
    frame = prev;

    if (frame == NULL) {
        return;
    }
}

int push(word_t data) {
    frame->stack_size++;
    frame->stack_data = (word_t *)realloc(frame->stack_data, frame->stack_size * sizeof(word_t));
    frame->stack_data[frame->stack_size - 1] = data;
    return 1;
}

word_t pop() {
    word_t top = tos();
    frame->stack_size--;
    frame->stack_data = (word_t *)realloc(frame->stack_data, frame->stack_size * sizeof(word_t));
    return top;
}

word_t tos() {
    return frame->stack_data[frame->stack_size - 1];
}

word_t get_local_variable(int i) {
    return frame->local_data[i];
}

void set_local_variable(unsigned short i, word_t value) {
    if (i >= frame->local_size) {
        frame->local_size = (unsigned short) (i + 1);
        frame->local_data = (word_t *)realloc(frame->local_data, frame->local_size * sizeof(word_t));
    }
    frame->local_data[i] = value;
}

word_t *get_stack() {
    return frame->stack_data;
}

int stack_size() {
    return frame->stack_size;
}
