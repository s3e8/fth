#include "fth_reader.h"

static void init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp) {
    state->stream           = fp;
    state->linebuf          = linebuf;
    state->linebuf[0]       = '\0';
    state->linebuf_size     = linebuf_size;
    state->remaining_chars  = linebuf;   
}

//
static reader_state_t* open_file(const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) return NULL;

    char *lbuf = malloc(1024);
    if (!lbuf) goto err_exit;
  
    setvbuf(fp, NULL, _IONBF, 0);  // disable input buffering, we have our own
  
    reader_state_t *state = (reader_state_t*)malloc(sizeof(reader_state_t));
    if(!state) goto err_exit;

    init_reader_state(state, lbuf, 1024, fp);
    return state;
  
    err_exit:
        free(lbuf);
        fclose(fp);
        return NULL;
}

//
static void close_file(reader_state_t *fp) {
    if(fp->stream) fclose(fp->stream);
    free(fp->linebuf);
    free(fp);
}

static void skip_whitespace(reader_state_t* state) {
    while(isspace(*state->remaining_chars)) state->remaining_chars++;
}

static void* get_next_line(reader_state_t* state) {
    char* tmp = fgets(state->linebuf, state->linebuf_size, state->stream);
    if(!tmp) return NULL;

    state->remaining_chars = tmp;
    return tmp;
}

static void* read_word(reader_state_t* state, char* tobuf) {
    char* buf = tobuf; // This is a common C pattern for writing into a buffer while keeping track of both the start and current position.

    // skip any preceding whitespace
    skipws:
        skip_whitespace(state);

    // get_next_line if buffer is empty
    if (*state->remaining_chars == '\0') {
        if (!get_next_line(state)) return NULL;
        goto skipws;
    }

    // copy until next whitespace
    while(*state->remaining_chars != '\0' && !isspace(*state->remaining_chars)) {
        *buf++ = *state->remaining_chars++;
    }

    state->remaining_chars++;
    *buf = '\0';

    return tobuf;
}

static int read_key(reader_state_t *state) {
    if (*state->remaining_chars=='\0') {
        if (!get_next_line(state)) return -1;
    }
    return *state->remaining_chars++;
}

static cell is_eol(reader_state_t *state) {
    skip_whitespace(state);
    return *state->remaining_chars=='\0';
}
  
static cell is_eof(reader_state_t *fp) {
    return *fp->remaining_chars=='\0' && feof(fp->stream);
}

static void emit_char(int c, FILE *fp) {
    fputc(c, fp);
}

static char *prompt_line(const char *prompt, reader_state_t *state) {
    char *tmp = readline(prompt);
    if(!tmp) return NULL;

    add_history(tmp);
    strncpy(state->linebuf, tmp, state->linebuf_size);
    free(tmp);
    state->remaining_chars = state->linebuf;
    return state->remaining_chars;
}