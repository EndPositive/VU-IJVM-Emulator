#include <ijvm.h>
#include <machine.h>
#include <util.h>
#include <limits.h>

short read_short(int i) {
    word_t A, B;
    if (i >= text_size() || i < 0) doERR("Reading text out of bounds");
    A = get_text()[i];
    B = get_text()[i + 1];
    return (short) (A << 8) | (short) B;
}
unsigned short read_unsigned_short(int i) {
    word_t A, B;
    if (i >= text_size() || i < 0) doERR("Reading text out of bounds");
    A = get_text()[i];
    B = get_text()[i + 1];
    return (unsigned short) (A << 8) | (unsigned short) B;
}

uint32_t swap_uint32(uint32_t num) {
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

unsigned short ushrt_safe_addition(unsigned short A, unsigned short B) {
    if (A > 0) {
        if (B > (USHRT_MAX - A)) doERR("Unsigned short addition overflow");
    } else if (B > 0) {
        if (A > (USHRT_MAX - B)) doERR("Unsigned short addition overflow");
    }
    return (unsigned short) (A + B);
}
