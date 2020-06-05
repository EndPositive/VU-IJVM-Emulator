#include <ijvm.h>

int print_hex(byte_t *data, int length, FILE *fp) {
    fprintf(fp, "printing hex... %i bytes", length);
    for (int i = 0; i < length; i++) {
        if (i % 16 == 0) {
            fprintf(fp, "\n");
        }
        fprintf(fp, "%02hhx", data[i]);
        if (i % 2 == 1) {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, "\n");
    return 1;
}

short to_short(word_t A, word_t B) {
    return (A << 8) | B;
}
