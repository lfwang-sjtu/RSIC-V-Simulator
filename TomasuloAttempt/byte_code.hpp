//
// Created by Snoopy on 2021/7/9.
//

#ifndef RISC_V_SIMULATOR_BYTE_CODE_HPP
#define RISC_V_SIMULATOR_BYTE_CODE_HPP

unsigned get(unsigned x, int low, int high) {
    x >>= low;
    return x & ((1u << (high - low + 1)) - 1);
}

void set(unsigned &x, int low, int high, unsigned y) {
    x &= ~(((1u << (high - low + 1)) - 1) << low);
    x += (y & ((1u << (high - low + 1)) - 1)) << low;
}

unsigned sext(unsigned x, unsigned n) {
    if (x & (1u << (n - 1))) {
        return x - (1u << n);
    } else return x;
}

#endif //RISC_V_SIMULATOR_BYTE_CODE_HPP
