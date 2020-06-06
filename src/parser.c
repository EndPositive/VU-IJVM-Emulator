#include <ijvm.h>
#include <parser.h>
#include <string.h>
#include <stdlib.h>
#include <util.h>

buffer_t *buffer;

int get_blocks() {
    // Parse constants
    buffer->constant_pos = swap_uint32(buffer->data[1]);
    buffer->constant_size = swap_uint32(buffer->data[2]) / 4;
    buffer->constants = (word_t *) malloc(buffer->constant_size * 4);
    for (int i = 0; i < buffer->constant_size; i++) {
        buffer->constants[i] = swap_uint32(buffer->data[3 + i]);
    }

    // Parse instructions
    buffer->text_pos = swap_uint32(buffer->data[3 + buffer->constant_size]);
    buffer->text_size = swap_uint32(buffer->data[4 + buffer->constant_size]);
    buffer->text = (byte_t *) malloc(buffer->text_size);
    memcpy(buffer->text, (byte_t *) &buffer->data[5 + buffer->constant_size], buffer->text_size);
    return 1;
}

int check_magic(word_t *data) {
    // Check magic number
    if (swap_uint32(data[0]) != MAGIC_NUMBER) {
        fprintf(stderr, "MAGIC NUMBERS DO NOT MATCH...");
        return -1;
    }
    return  1;
}

int init_buffer(char *binary_file) {
    buffer = (buffer_t *)malloc(sizeof(buffer_t));

    // Open file
    FILE *fp = fopen(binary_file, "rb");
    if (fp == NULL) return -1;

    // Get file length
    fseek(fp, 0, SEEK_END);
    buffer->data_size = (int)ftell(fp);
    buffer->data = (word_t *)malloc(buffer->data_size * sizeof(word_t));
    fseek(fp, 0, SEEK_SET);

    // Copy file into buffer
    size_t ret = fread(buffer->data, sizeof(word_t), buffer->data_size, fp);
    if (ret <= 0) return -1;

    // Check magic number
    if (!check_magic(buffer->data)) return -1;

    // Load data blocks;
    if (!get_blocks(buffer)) return -1;

    return 1;
}

void destroy_buffer() {
    free(buffer->data);
    free(buffer->constants);
    free(buffer->text);
    free(buffer);
}

byte_t *get_text() {
    return buffer->text;
}

int text_size() {
    return buffer->text_size;
}

word_t get_constant(int i) {
    return buffer->constants[i];
}