#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <cstdint>

int main() {

    uint8_t *memA = (uint8_t *)malloc(64);
    memset(memA, 0xaa, 64);

    uint8_t *memB = (uint8_t *)malloc(32);
    memset(memB, 0xbb, 32);

    uint8_t *memC = (uint8_t *)malloc(16);
    memset(memC, 0xcc, 16);

    free(memB);

    uint8_t *memD = (uint8_t *)malloc(16);
    memset(memD, 0xdd, 16);

    std::string s("hello world");
    std::ios_base::Init ss;

    std::cout << s << std::endl;

    free(memA);
    free(memC);
    free(memD);

    return EXIT_SUCCESS;
}
