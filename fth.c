// to compile: gcc -o gcc -o fth fth.c -ledit
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <editline/readline.h>
// #include <readline/readline.h>
// #include <readline/history.h>

#include "fth_cfg.h"

/* ThE C3Ll t7P3 */
typedef uintptr_t cell;

/* Dictionary */
typedef struct word_hdr_t {
    cell                flags;
    struct word_hdr_t*  next;
    char                name[WORD_NAME_MAX_LEN];
} word_hdr_t;

typedef struct builtin_word_t {
    char*   name;
    void*   code;
    cell    flags;
} builtin_word_t;

typedef struct reader_state_t {
    FILE*   stream;
    char*   linebuf;
    cell    linebuf_size;
    char*   remaining_chars;
} reader_state_t;

// /* Thread State */
// // todo: does order matter?
// // "green" thread
// typedef struct thread_state_t {
//     cell                    killed;    // ?
//     struct thread_state_t*  next;

//     // We simulate control flow by moving an instruction pointer (ip), 
//     // which points into a block of bytecode (really just a void*[] array.
//     void**  ip;
//     cell*   ds;
//     cell*   s0;
//     void*** rs;
//     void*** r0;
//     float*  fs;
//     float*  f0;
//     cell*   ts; // ?
//     cell*   t0; // ?
// } thread_state_t;

/* Dictionary Globals */
cell state = STATE_IMMEDIATE;
static void*        here;
static void*        here0;
static cell         here_size;
struct word_hdr_t*  latest = NULL;

// /* Thread State Globals */
// static thread_state_t* current_thread = NULL;

/* Dictionary Impl */
static word_hdr_t* create_word(const char* name, cell flags) {
    if(!name) name="\0";

    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);

    strncpy(new->name, name, WORD_NAME_MAX_LEN);
    new->flags = flags;
    new->next = latest;
    latest = new;

    return new;
}

static word_hdr_t* find_word(const char* name) {
    if(!name) return NULL;

    word_hdr_t* hdr = latest;
    while(hdr) {
        if(!strncmp(hdr->name, name, WORD_NAME_MAX_LEN)) return hdr;
        hdr = hdr->next;
    }
    return NULL;
}

static void** cfa(word_hdr_t* word) {
    return (void**)(word + 1);
}

// comma operator stores a value (usually a number or an address) into the next available spot in memory (typically here)
static void comma(cell val) {
    *(cell*)here = val;
    here += sizeof(cell);
}

// fetches the cfa/execution token (single quote, not actual "`")
static void* tick(const char* name) {
    word_hdr_t* hdr = find_word(name);
    return *(cfa(hdr));
}

// utility stuff
static void assemble_word(const char* name, cell flags, void** code, cell codesize) {
    create_word(name, flags);

    int i;
    for(i = 0; i < codesize/sizeof(void*); i++) {
        comma((cell)code[i]);
    }

    comma((cell)tick("eow"));
}

// todo: if assemble already adds 'tick(eow)', do we need it here?
static void create_constant(const char* name, cell val) {
    void* flagdef[] = { tick("lit"), (void*)val, tick("exit") };
    assemble_word(name, FLAG_INLINE, flagdef, sizeof(flagdef));
}

// Core words and helpers
static void create_builtin(builtin_word_t* b) {
    create_word(b->name, b->flags | FLAG_BUILTIN);
    comma((cell)b->code);
    // comma(0); // ??
}

/* Reader Stuff */

static void init_reader_state(reader_state_t* state, char* linebuf, cell linebuf_size, FILE* fp) {
    state->stream           = fp;
    state->linebuf          = linebuf;
    state->linebuf[0]       = '\0';
    state->linebuf_size     = linebuf_size;
    state->remaining_chars  = linebuf;   
}

//
static reader_state_t* open_file(const char *filename, const char* mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) return NULL;

    char *lbuf = malloc(1024);
    if (!lbuf) goto err_exit;
  
    setvbuf(fp, NULL, _IONBF, 0);  // disable input buffering, we have our own
  
    reader_state_t* state = (reader_state_t*)malloc(sizeof(reader_state_t));
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

static int read_key(reader_state_t* state) {
    if (*state->remaining_chars=='\0') {
        if (!get_next_line(state)) return -1;
    }
    return *state->remaining_chars++;
}

static cell is_eol(reader_state_t* state) {
    skip_whitespace(state);
    return *state->remaining_chars=='\0';
}
  
static cell is_eof(reader_state_t* fp) {
    return *fp->remaining_chars=='\0' && feof(fp->stream);
}

static void emit_char(int c, FILE* fp) {
    fputc(c, fp);
}

static char *prompt_line(const char* prompt, reader_state_t* state) {
    char *tmp = readline(prompt);
    if(!tmp) return NULL;

    add_history(tmp);
    strncpy(state->linebuf, tmp, state->linebuf_size);
    free(tmp);
    state->remaining_chars = state->linebuf;
    return state->remaining_chars;
}

// toy interpret!
//
static void interpret(reader_state_t* state, char* wordbuf) {
//     // Read the next word into `word`
//     char* word = read();
//     // early-exit if no word was read
//     // if(!word) NEXT();

//     // Try to find the word in the dictionary
//     word_hdr_t* entry = find_word(wordbuf);

//     if (!entry) {
//         // Attempt to interpret it as a literal number
//         char* endptr = NULL;
//         // cell val = (cell)strtol(word, &endptr, base);

//         // Check if the entire string was not consumed => invalid number
//         // (Note: This line is potentially buggy and should probably be:
//         // if (*endptr != '\0') )
//         if (!endptr != "\0") printf("ERROR: No such word: %s\n", word);
//         else {
//             // Valid number literal
//             if (state == STATE_COMPILE) {
//                 // In compile state: compile a literal onto the dictionary
//             // e.g., emit a `lit` instruction + value
//                 // comma((cell) ...?) .. tick('lit')?
//                 comma(val);
//             } else {
//                 // In interpret state: just push the value or print it
//                 printf("val: %d", val);
//             }
//         }
//         // Continue to next instruction
//         // NEXT();
//     }

//     if (STATE_COMPILE && !(entry->flags & FLAG_IMMED)) {
//         if (entry->flags & FLAG_BUILTIN) {

//         }
//     }

    // Get word...
    word_hdr_t* word = read_word(state, wordbuf);
    printf("Word: %s\n", wordbuf);

    // Try to find word in dictionary...
    word_hdr_t* entry = find_word(wordbuf);

    //
    // If word is not an entry in dictionary...
    if (!entry) {
        // Try to interpret as literal number
        char* endptr = NULL;
        // todo: cell val = (cell)strtol(word, &endptr, base);
        // todo: impl "base"
        cell val = (cell)strtol(wordbuf, &endptr, 10);

        // Check if the entire string was not consumed => invalid number
        if (*endptr != '\0') { // note: single quotes matters for char apparently...
            printf("ERROR: No such word: %s\n", wordbuf);
        } else {
            printf("'%s' is number.\n", wordbuf);

            // Valid number literal:
            if (state == STATE_COMPILE) {
                printf("Compiling number: %s...\n", wordbuf);
                // In compile state: compile a literal onto the dictionary
                // e.g., emit a `lit` instruction + value
                comma(val);
            } else {
                // In interpret state: just push the value
                printf("value: %s\n", wordbuf);
            }
        }
        // Continue to next instruction
        // todo: NEXT();
        return;
    }

    //
    // If in compile state (1) and the word is *not* an IMMEDIATE word...
    if (state == STATE_COMPILE && !(entry->flags & FLAG_IMMED)) {
        // Check if it's a BUILTIN word (native C function)
        if (entry->flags & FLAG_BUILTIN) {
            printf("TODO: Compiling builtin '%s' at cfa: %p\n", entry->name, cfa(entry));
            // Emit (emit?) (compile) the built-in function directly into the instruction stream
            // by placing its function pointer (cast as a cell) at the current memory location.
            // comma((cell)(*cfa(entry)));
        } else {
            // Otherwise, it's a user-defined word.
            // Emit the l_CALL opcode followed by the word's code field address.
            // comma((cell) &&l_CALL);           // compile-time token that means "call next address"
            // comma((cell) cfa(entry));         // compile the address of the word to be called
            printf("User-defined word: '%s'\n", entry->name);
        }
    } else {
        // Otherwise (not compiling, or it *is* IMMEDIATE), we execute it immediately.

        // Get code field address of the word
        void **code = cfa(entry);
        // printf("Executing: '%s' with cfa: %p\n", entry->name, cfa);
        // while(*code) { *code++; }

        printf("Evaling: '%s' with cfa: %p\n", entry->name, cfa);
        // eval(entry->name);

        // Push the current instruction pointer onto the nesting stack
        // so we can return here after the word finishes executing.
        // *--nestingstack = ip;

        // If the word is a built-in, patch and jump to it via a temporary buffer
        if(entry->flags & FLAG_BUILTIN) {
            // Place the built-in function pointer in the immediate buffer
            // builtin_immediatebuf[0] = *code;

            // Set instruction pointer to point to the buffer, so the function runs immediately
            // ip = builtin_immediatebuf;
        } else {
            // Otherwise, prepare the call to a normal (non-builtin) Forth word

            // Place the word's code address into the immediate buffer
            // word_immediatebuf[1] = (void*)code;

            // Set instruction pointer to that buffer, which acts like a trampoline
            // ip = word_immediatebuf;
        }
    }
    // eval(entry->name);
    printf("\n");
}

static void toy_run() {
    // static int initialized = 0;

    char input[1024];
    char wordbuf[WORD_NAME_MAX_LEN];
    char linebuf[WORD_NAME_MAX_LEN];

    char stdin_buf[1024];
    reader_state_t stdin_state;

    init_reader_state(&stdin_state, stdin_buf, 1024, stdin);

    while (1) {
        printf("forth> ");
        fflush(stdout);

        interpret(&stdin_state, linebuf);
    }
}

// /* Interpreter */
// static void run(
//     void**  ip,     cell*   ds,     void*** rs, 
//     reader_state_t* inputstate,     FILE* outp, 
//     int argc,       char** argv
// ) {
//     static int initialized = 0;

//     register cell tmp; // temp buffer var used in bytecode

//     // todo: why initialize like this, rather than init/create_thread?
//     cell*   s0 = ds;
//     void*** r0 = rs;
//     cell*   t0 = NULL;
//     float*  f0 = NULL;
//     cell*   ts = NULL;
//     float*  fs = NULL;

//     // void**  nestingstack_space[NESTINGSTACK_MAX_DEPTH];
//     // void*** nestingstack = nestingstack_space + NESTINGSTACK_MAX_DEPTH;

//     // void** debuggervector = NULL;

//     // void* builtin_immediatebuf[2]   = { NULL,       WORD(IRETURN) };
//     // void* word_immediatebuf[3]      = { WORD(CALL), NULL, WORD(IRETURN) };

//     char wordbuf[WORD_NAME_MAX_LEN];    // like tmp, these buffers are used in the bytecode
//     char linebuf[WORD_NAME_MAX_LEN];    // --

//     char stdinbuf[1024]; // todo: make #define
//     reader_state_t stdin_state;

//     if(!initialized) {
//         initialized = 1;

//     }

// }

int main(int argc, char** argv) {
    // cell    datastack[DS_SIZE]      = {0};
    // void**  returnstack[RS_SIZE]    = {0};
    // todo: why don't we need to free after this?

    here_size   = HERE_SIZE;
    here0       = malloc(here_size);
    here        = here0;
    if (!here0) {
        fprintf(stderr, "Failed to allocate dictionary memory!\n");
        exit(1);
    }

    create_word("test-word", 0);
    printf("Creating: 'test-word-1'...\n");
    create_word("test-word-1", 0);  // Use consistent name
    word_hdr_t* res = find_word("test-word-1");
    if (res) {
        if (strncmp(res->name, "test-word-1", WORD_NAME_MAX_LEN) == 0) {
            printf("word found!\n");
        } else {
            printf("word found but name mismatch!\n");
        }
    } else {
        printf("word NOT found!\n");
    }

    toy_run();
}