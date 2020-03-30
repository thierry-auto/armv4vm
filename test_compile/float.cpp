#include <cstdio>
#include <cstdlib>
#include <cstring>


extern unsigned char uart_base;

int main(void)
{
    int i1 = 123456789;
    float f1 = 1234.56789;
    double d1 = 123456.78912345;
    long l1 = 123448912;
    unsigned char c1 = 0xDF;
    long long ll1 = 12344891234589423;

    unsigned char * uart = &uart_base;

    memcpy(uart, &i1, sizeof(int));
    uart += sizeof(int);
    memcpy(uart, &f1, sizeof(float));
    uart += sizeof(float);
    memcpy(uart, &d1, sizeof(double));
    uart += sizeof(double);
    memcpy(uart, &l1, sizeof(long));
    uart += sizeof(long);
    memcpy(uart, &c1, sizeof(unsigned char));
    uart += sizeof(unsigned char);
    memcpy(uart, &ll1, sizeof(long long));

    return EXIT_SUCCESS;
}
