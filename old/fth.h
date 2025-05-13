// fth.h
#ifndef FTH_H
#define FTH_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <editline/readline.h>

#define DEBUG 1
#ifdef DEBUG
    #define DBG_PRINT(fmt, ...) printf("DBG: " fmt, ##__VA_ARGS__)
#else
    #define DBG_PRINT(fmt, ...)
#endif

#ifdef USE_GC
    #include <gc.h>
    #define MALLOC(x) GC_MALLOC(x)
    #define MALLOC_ATOMIC(x) GC_MALLOC_ATOMIC(x)
    #define REALLOC(ptr,newlen) GC_REALLOC(ptr, newlen)
    #define RUNGC() GC_gcollect()
    #define FREE(x)
#else
    #define MALLOC(x) malloc(x)
    #define MALLOC_ATOMIC(x) malloc(x)
    #define REALLOC(ptr,newlen) realloc(ptr,newlen)
    #define RUNGC()
    #define FREE(x) free(x)
#endif

typedef uintptr_t cell;

#include "fth_cfg.h"
#include "fth_io.h"
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