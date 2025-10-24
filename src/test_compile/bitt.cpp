#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdint>

extern unsigned char uart_base;

extern "C" uint32_t myadd(uint32_t a, uint32_t b);
extern "C" uint32_t mycount(uint32_t a);


int main(void) {

    uint32_t a = 0xF48910;
    uint32_t b = 0x249762;
    uint32_t r = 0;
    uint32_t c = 0;
    uint32_t d = 0;
    uint64_t e = 0;
    uint64_t f = 0;

    std::ios_base::Init __unspecified_name__;

    r = myadd(a, b);
    c = mycount(r);
    d = mycount(r);
    e = 0xF7861B1D54AB7;
    f = mycount(e);
    std::cout << "r=" << r <<
                 " c=" << c <<
                 " d=" << d <<
                 " e=" << e <<
                 " f=" << f <<
                 std::endl;

    return EXIT_SUCCESS;
}
