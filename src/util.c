#include <ijvm.h>
#include <util.h>

short read_short(int i) {
    word_t A = get_text()[i];
    word_t B = get_text()[i + 1];
    return (short) (A << 8) | (short) B;
}
unsigned short read_unsigned_short(int i) {
    word_t A = get_text()[i];
    word_t B = get_text()[i + 1];
    return (unsigned short) (A << 8) | (unsigned short) B;
}

uint32_t swap_uint32(uint32_t num) {
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

