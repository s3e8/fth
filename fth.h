// fth.h
#ifndef FTH_H
#define FTH_H
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef uintptr_t cell;

#include "fth_cfg.h"
#include "fth_dict.h"
#include "fth_thread.h"

/* Dictionary Globals */
extern cell                 state;      // compiler state -- This is a word
extern void*                here;
extern void*                here0;
extern cell                 here_size;
extern struct word_hdr_t*   latest;

/* Thread State Globals */
extern thread_state_t* current_thread; // todo: can change to static later I think

#endif // FTH_H