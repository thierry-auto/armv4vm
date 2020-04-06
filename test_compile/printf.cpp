#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(void)
{
    static std::ios_base::Init __unspecified_name__;

    printf("[printf] %d %c %s %f\n", 2, 'c', "hello", 41.123);
    std::cout << "[cout] " << 2 << " " << 'c' << " " << "hello " << 41.123 << std::endl;

    return EXIT_SUCCESS;
}
