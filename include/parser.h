#ifndef PARSER_H
#define PARSER_H

#include <ijvm.h>

typedef struct buffer {
    unsigned int data_size;
    unsigned int *data;
    unsigned int constant_size;
    word_t *constants;
    unsigned int text_size;
    byte_t *text;
} buffer_t;

buffer_t *buffer;

int get_blocks(void);

int init_buffer(char *binary_file);

void destroy_buffer(void);

#endif
