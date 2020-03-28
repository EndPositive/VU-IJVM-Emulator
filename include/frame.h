#ifndef FRAME_H
#define FRAME_H

#include <ijvm.h>

typedef struct frame {
    int data_size;
    word_t *data;
    struct frame *prev;
} frame_t;

int init_frame(frame_t *frame, frame_t *prev);

#endif
