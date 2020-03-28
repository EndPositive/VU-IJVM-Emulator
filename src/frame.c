#include <frame.h>
#include <stdlib.h>

int init_frame(frame_t *frame, frame_t *prev) {
    frame->data = (word_t *)malloc(10000);
    frame->prev = prev;
    return 1;
}
