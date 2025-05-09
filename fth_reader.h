#include <stdio.h>
// todo: rename to fth_io and include emit_char etc?

#ifndef CELL
#include <stdint.h>
typedef uintptr_t cell;
#endif

typedef struct reader_state_t {
    FILE*   stream;
    char*   linebuf;
    cell    linebuf_size;
    char*   remaining_chars;
} reader_state_t;

void  init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp);
void* read(reader_state_t* state, char* tobuf);