// fth_io.h
#ifndef FTH_IO_H
#define FTH_IO_H
// todo: rename to fth_io and include emit_char etc?
typedef struct reader_state_t {
    FILE*   stream;
    char*   linebuf;
    cell    linebuf_size;
    char*   remaining_chars;
} reader_state_t;

void init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp);
void* get_next_line(reader_state_t* state);
void* read_word(reader_state_t* state, char* tobuf);
reader_state_t* read_string_as_file(const char* input);
int read_key(reader_state_t *state);
cell is_eol(reader_state_t *state);
cell is_eof(reader_state_t *fp);
void emit_char(int c, FILE *fp);
char *prompt_line(const char* prompt, reader_state_t* state);

reader_state_t* open_file(const char* filename, const char* mode);
void close_file(reader_state_t *fp);

#endif // FTH_IO_H