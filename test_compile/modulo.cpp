#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <cstdio>

int modulo() {

    volatile int i = 7;
    return i % 7;
}

int main() {

    volatile int j = 7;

    j = modulo();

    return j;
}
