// to compile: gcc -o fth fth.c fth_thread.c
#include <stdio.h>
#include <stddef.h>

#include "fth_cfg.h"
#include "fth_thread.h"
#include "fth_reader.h"

#define STATE_IMMEDIATE 0 // todo: this should probably be in dict.h instead?
#define STATE_COMPILE   1 //

/* globals */
cell state = STATE_IMMEDIATE;
static thread_state_t* current_thread = NULL;

static void run(
    void**  ip,     cell*   ds,     void*** rs, 
    reader_state_t* inputstate,     FILE* outp, 
    int argc,       char** argv
) {
    static int initialized = 0;

    register cell tmp; // placeholder var used in bytecode



}

int main(int argc, char** argv) {
    cell    datastack[DS_SIZE]      = {0};
    void**  returnstack[RS_SIZE]    = {0};


}