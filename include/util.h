#ifndef UTIL_H
#define UTIL_H
#include <ijvm.h>

/* debug print */
#ifdef DEBUG

#define dprintf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define dprintf(...)
#endif

int print_hex(byte_t *data, int length, FILE *fp);

short to_short(word_t A, word_t B);

#endif
