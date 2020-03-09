#include <ijvm.h>
#include <binary.h>
#include <string.h>
#include <stdlib.h>

static uint32_t swap_uint32(uint32_t num) {
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

int get_blocks(buffer_t *buffer) {
    // Parse constants
    buffer->constant_pos = swap_uint32(buffer->data[1]);
    buffer->constant_size = swap_uint32(buffer->data[2] / 4);
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

int parse(buffer_t *buffer, char *binary_file) {
    // Open file
    FILE *fp = fopen(binary_file, "rb");
    if (fp == NULL) return -1;

    // Get file length
    fseek(fp, 0, SEEK_END);
    buffer->data_size = (int)ftell(fp);
    buffer->data = (word_t *)malloc(buffer->data_size);
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
