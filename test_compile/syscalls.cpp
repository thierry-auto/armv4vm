#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char uart_base;
  unsigned char * UART0_ADDR = (unsigned char*) &uart_base;

enum {
    UART_FR_RXFE = 0x10,
    UART_FR_TXFF = 0x20,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {

    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {

    return 1;
}

int _lseek(int file, int ptr, int dir) {

    return 0;
                                       }

int _open(const char *name, int flags, int mode) {

    return -1;
}

int _read(int file, char *ptr, int len) {

    int todo;
    if (len == 0)
        return 0;
    while (UART_FR(UART0_ADDR) & UART_FR_RXFE);
    *ptr++ = UART_DR(UART0_ADDR);
    for (todo = 1; todo < len; todo++) {
        if (UART_FR(UART0_ADDR) & UART_FR_RXFE) {
            break;
        }
        *ptr++ = UART_DR(UART0_ADDR);
    }
    return todo;
}

char *ld_heap_end = 0;

caddr_t _sbrk(int incr) {

    extern char ld_heap_low; /* Defined by the linker */
    extern char ld_heap_top; /* Defined by the linker */
    char *prev_heap_end;

    if (ld_heap_end == 0) {
        ld_heap_end = &ld_heap_low;
    }
    prev_heap_end = ld_heap_end;

    if (ld_heap_end + incr > &ld_heap_top) {

        /* Heap and stack collision */
        return (caddr_t)0;
    }

    ld_heap_end += incr;

    return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {

    int todo;
    for (todo = 0; todo < len; todo++) {
        UART_DR(&uart_base) = *ptr++;
    }
    return len;
}


int _kill(int pid, int sig) {

    (void) pid;
    (void) sig;
    return -1;
}

int __exidx_start;
int __exidx_end;

int _getpid(void) {

    return 1;
}

void _exit() {

}

#ifdef __cplusplus
}
#endif
