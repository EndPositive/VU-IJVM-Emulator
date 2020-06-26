#include <ijvm.h>
#include <parser.h>
#include <string.h>
#include <stdlib.h>
#include <util.h>
#include <machine.h>


int get_blocks() {
    // Parse constants
    buffer->constant_size = swap_uint32(buffer->data[2]) / 4;
    buffer->constants = (word_t *) malloc(buffer->constant_size * 4);
    if (buffer->constants == NULL) {
        return -1;
    }
    for (unsigned int i = 0; i < buffer->constant_size; i++) {
        if (3 + i >= buffer->data_size) return -1;
        buffer->constants[i] = (word_t) swap_uint32(buffer->data[3 + i]);
    }

    // Parse instructions
    buffer->text_size = swap_uint32(buffer->data[4 + buffer->constant_size]);
    if (buffer->text_size + buffer->constant_size + 5 >= buffer->data_size) return -1;
    buffer->text = (byte_t *) malloc(buffer->text_size);
    if (buffer->text == NULL) return -1;
    memcpy(buffer->text, (byte_t *) &buffer->data[5 + buffer->constant_size], buffer->text_size);
    return 1;
}

int init_buffer(char *binary_file) {
    FILE *fp;

    buffer = (buffer_t *)malloc(sizeof(buffer_t));
    if (buffer == NULL) {
        free(buffer);
        return -1;
    }

    // Open file
    fp = fopen(binary_file, "rb");
    if (fp == NULL) {
        destroy_buffer();
        fclose(fp);
        return -1;
    }

    // Get file length
    fseek(fp, 0, SEEK_END);
    buffer->data_size = (unsigned int) ftell(fp);
    if (buffer->data_size == 0) return -1;
    buffer->data = (unsigned int*)malloc(buffer->data_size * sizeof(unsigned int));
    if (buffer->data == NULL) {
        destroy_buffer();
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    // Copy file into buffer
    if (fread(buffer->data, sizeof(unsigned int), buffer->data_size, fp) <= 0) {
        destroy_buffer();
        fclose(fp);
        return -1;
    }

    // Check magic number
    if (swap_uint32(buffer->data[0]) != MAGIC_NUMBER) {
        destroy_buffer();
        fclose(fp);
        return -1;
    }

    // Load data blocks;
    if (get_blocks() < 0) {
        destroy_buffer();
        fclose(fp);
        return -1;
    }

    fclose(fp);

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
    return (int) buffer->text_size;
}

word_t get_constant(unsigned int i) {
    if (i >= buffer->constant_size) doERR("Getting constant out of bounds");
    return buffer->constants[i];
}
