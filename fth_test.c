// to compile: gcc -o fth_test fth_test.c fth_dict.c
#include <stdio.h>
#include <stddef.h>

#include "fth_cfg.h"
#include "fth_dict.h"
#include "fth_thread.h"
#include "fth_reader.h"

void test_dict_1() {
    printf("Creating: 'test-word-1'...\n");
    create_word("test-word1", 0);
    word_hdr_t* res = find_word("test-word-1");
    if (res && res->name == "test-word-1") printf("word found!\n");
}

static void run_test_1(
    void**  ip,     cell*   ds,     void*** rs, 
    reader_state_t* inputstate,     FILE* outp, 
    int argc,       char** argv
) {
    static int initialized = 0;

    register cell tmp; // temp buffer var used in bytecode

    // todo: why initialize like this, rather than init/create_thread?
    cell*   s0 = ds;
    void*** r0 = rs;
    cell*   t0 = NULL;
    float*  f0 = NULL;
    cell*   ts = NULL;
    float*  fs = NULL;

    // void**  nestingstack_space[NESTINGSTACK_MAX_DEPTH];
    // void*** nestingstack = nestingstack_space + NESTINGSTACK_MAX_DEPTH;

    // void** debuggervector = NULL;

    // void* builtin_immediatebuf[2]   = { NULL,       WORD(IRETURN) };
    // void* word_immediatebuf[3]      = { WORD(CALL), NULL, WORD(IRETURN) };

    char wordbuf[WORD_NAME_MAX_LEN];    // like tmp, these buffers are used in the bytecode
    char linebuf[WORD_NAME_MAX_LEN];    // --

    char stdinbuf[1024]; // todo: make #define
    reader_state_t stdin_state;

    if(!initialized) {
        initialized = 1;

    }

}

int main(int argc, char** argv) {
    // cell    datastack[DS_SIZE]      = {0};
    // void**  returnstack[RS_SIZE]    = {0};

    test_dict_1();

}