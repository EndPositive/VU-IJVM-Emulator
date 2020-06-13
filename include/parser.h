#ifndef PARSER_H
#define PARSER_H

#include <ijvm.h>

typedef struct buffer {
    int data_size;
    word_t *data;
    int constant_pos;
    int constant_size;
    word_t *constants;
    int text_pos;
    int text_size;
    byte_t *text;
} buffer_t;

/**
 * @param buffer object to fill
 * Returns  0 on success
 *         -1 on failure
 **/
int get_blocks();

/**
 * @param data object to check for magic number
 * Returns  0 on success
 *         -1 on failure
 **/
int check_magic(word_t *data);

/**
 * @param binary_file path to binary (.ijvm) file to init_buffer
 * Returns  0 on success
 *         -1 on failure
 **/
int init_buffer(char *binary_file);

void destroy_buffer(void);

#endif
