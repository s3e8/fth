// to compile: gcc -o fth fth.c fth_thread.c
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "fth_cfg.h"
// #include "fth_dict.h"
#include "fth_reader.h"
#include "fth_thread.h"

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
}