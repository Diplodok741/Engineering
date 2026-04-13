#include <sys/stat.h>
#include <stdint.h>
#include <stddef.h>

extern uint8_t _end; // символ из линкера
static uint8_t *heap_end;

void* _sbrk(ptrdiff_t incr)
{
    if (heap_end == 0) {
        heap_end = &_end;
    }

    uint8_t *prev = heap_end;
    heap_end += incr;

    return prev;
}

int _write(int file, char *ptr, int len)
{
    // пока просто "съедаем"
    return len;
}

int _close(int file) { return -1; }

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) { return 1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _read(int file, char *ptr, int len) { return 0; }
