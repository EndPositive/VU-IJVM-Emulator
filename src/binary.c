#include <ijvm.h>
#include <binary.h>
#include <string.h>
#include <stdlib.h>

word_t btow(byte_t *data, word_t pos) {
    return data[pos] << 24 | data[pos + 1] << 16 | (data[pos + 2] << 8) | data[pos + 3];
}

int get_blocks(buffer_t *buffer) {
    // Parse constants
    buffer->constant_pos =  btow(buffer->data, 4);
    buffer->constant_size = btow(buffer->data, 8) / 4;
    buffer->constants =     (word_t *) malloc(buffer->constant_size);
    for (int i = 0; i < buffer->constant_size; i++) {
        buffer->constants[i] = btow(buffer->data, 12 + i * 4);
    }

    // Parse instructions
    buffer->text_pos = btow(buffer->data, 12 + buffer->constant_size * 4);
    buffer->text_size = btow(buffer->data, 16 + buffer->constant_size * 4);
    buffer->text = (byte_t *) malloc(buffer->text_size);
    memcpy(buffer->text, buffer->data + 20 + buffer->constant_size * 4, buffer->text_size);
    return 1;
}

int check_magic(byte_t *data) {
    // Check magic number
    word_t number = btow(data, 0);
    if (number != MAGIC_NUMBER) {
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
    buffer->data = (byte_t *)malloc(buffer->data_size);
    fseek(fp, 0, SEEK_SET);

    // Copy file into buffer
    size_t ret = fread(buffer->data, 4, buffer->data_size, fp);
    if (ret <= 0) return -1;

    // Check magic number
    if (!check_magic(buffer->data)) return -1;

    // Load data blocks;
    if (!get_blocks(buffer)) return -1;

    return 1;
}
