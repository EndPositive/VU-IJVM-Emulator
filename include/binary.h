#ifndef BINARY_H
#define BINARY_H

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
int get_blocks(buffer_t *buffer);

/**
 * @param data object to check for magic number
 * Returns  0 on success
 *         -1 on failure
 **/
int check_magic(word_t *data);

/**
 * @param buffer to parse into
 * @param binary_file path to binary (.ijvm) file to parse
 * Returns  0 on success
 *         -1 on failure
 **/
int parse(buffer_t *buffer, char *binary_file);

#endif
