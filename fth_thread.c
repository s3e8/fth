// to-compile: gcc -o fth_thread fth_thread.c
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "fth_thread.h"

// todo: 
// round-robin scheduler bytecode?
// change name from thread to... fiber/routine/task/context? idk
// safe mode that checks for under/overflow
// check direction of "s0 <= ds < s0 + 64" etc


static thread_state_t *current_thread = NULL;

static thread_state_t* init_thread(cell* s0, void*** r0, cell* t0, void** entrypoint) {
    thread_state_t* new_thread = malloc(sizeof(thread_state_t));

    new_thread->killed = 0;
    new_thread->ip = entrypoint;
    new_thread->s0 = s0;
    new_thread->r0 = r0;
    new_thread->t0 = t0;
    new_thread->s0 = new_thread->s0;
    new_thread->r0 = new_thread->r0;
    new_thread->t0 = new_thread->t0;

    if (!current_thread) {
        current_thread   = new_thread;
        new_thread->next = new_thread;
    } else {
        new_thread->next     = current_thread->next;
        current_thread->next = new_thread;
    }

    return new_thread;
}

static thread_state_t* create_thread(int ds_size, int rs_size, int ts_size, void** entrypoint) {
    return init_thread(
        (cell*)     malloc(ds_size * sizeof(cell)),
        (void***)   malloc(rs_size * sizeof(void*)),
        (cell*)     malloc(ts_size * sizeof(cell)),
        entrypoint
    );
}

// todo?
static void kill_thread() {
    if (!current_thread) return;
    if (current_thread->next == current_thread) return; // todo:?

    current_thread->killed = 1;
    thread_state_t* i = current_thread;

    // increment to latest thread
    while (i->next != current_thread) i = i->next;
    i->next = current_thread->next;
    current_thread = i;
}

// temp
// static void scheduler() {
//     if (!current_thread) return;
    
//     thread_state_t* thread = current_thread;
//     do {
//         // Execute the current thread's program until it yields or exits
//         void** ip = thread->ip;
//         goto **ip++;  // Execute the next instruction
        
//         // Move to the next thread in the round-robin queue
//         current_thread = thread->next;
//     } while (current_thread != thread);
// }

//
#define NEXT()      goto **ip++
//
#define PUSH(x)     *--ds = (cell)(x)
#define POP()       (*ds++)
//
#define FPUSH(x)    *--fs = (float)(x)
#define FPOP()      (*fs++)
//
#define PUSHRS(x)   *--rs = (void**)(x)
#define POPRS()     (*rs++)
//
#define INTARG()    ((cell)(*ip++))
#define FLOATARG()  (*(float*)ip)
#define ARG()       (*ip++)
//
// TOP -- vs: "(*(ds))"?
#define TOP()       (*ds) // always returns mot recent/last pushed value
#define FTOP()      (*fs)
//
#define AT(x)       (*(ds+(x)))
#define FAT(x)      (*(fs+(x)))
//
// #define CHECK_OVERFLOW()                        \
//     if (ds >= s0 + ds_size) {                   \
//         printf("Stack overflow detected!\n");   \
//         kill_thread();                          \
//     }
#define CHECK_OVERFLOW()                \
    if (ds <= s0) {                     \
        printf("Stack overflow\n");     \
        kill_thread();                  \
        return;                         \
    }
//
#define SAFE_PUSH(x) do { CHECK_OVERFLOW(); PUSH(x); } while(0)
//
//
// tmp primitives
#define DUP()        { PUSH(TOP()); }
#define IF()         { cell condition = POP(); if (!condition) { goto **ip++; } }
#define YIELD()      { scheduler(); return; }

// tmp idk
#define LIT(n)     &&lit, (void*)(n)
#define ADD        &&add
#define EXIT       &&exit_
#define CALL(addr) &&call, (void*)(addr)

// todo: 
// make generic
// add print_ds, print_rs, print_etc
// add print_thread_state or something
static void print_stack(cell* s0, cell* ds) {
    printf("DS: [ ");
    for (cell* p = ds; p < s0 + 64; ++p) printf("%ld ", *p);
    printf("] ds-ok\n");
}

static void print_return_stack(void*** r0, void*** rs) {
    printf("RS: [ ");
    for (void*** p = rs; p < r0 + 64; ++p) if(p) printf("%p ", *p);
    printf("] rs-ok\n");
}

// void run_thread_test() {
//     void* program[] = {
//         &&lit,  (void*)42,
//         &&lit,  (void*)100,
//         &&add,
//         &&exit_
//     };

//     void* subroutine[] = {
//         &&lit,  (void*)10,
//         &&add,
//         &&exit_
//     };

//     void* main_program[] = {
//         &&lit, (void*)32,
//         &&lit, (void*)100,
//         &&call, subroutine,
//     };

//     // Allocate and initialize thread
//     // thread_state_t* t = create_thread(64, 64, 0, program);
//     thread_state_t* t = create_thread(64, 64, 0, main_program);

//     cell* ds = t->s0 + 64; // stack grows downward
//     cell* s0 = t->s0;
//     void*** rs = t->r0;
//     void*** r0 = t->r0;

//     void** ip = t->ip;

//     goto **ip++;

//     lit:
//         printf("Before LIT:\n");
//         print_stack(s0, ds);
//         PUSH(INTARG());
//         printf("After LIT:\n");
//         print_stack(s0, ds);
//         NEXT();

//     add: {
//         printf("Before ADD:\n");
//         print_stack(s0, ds);
//         cell a = POP();
//         cell b = POP();
//         PUSH(a + b);
//         print_stack(s0, ds);
//         NEXT();
//     }

//     call: {
//         void *fn = ARG();
//         PUSHRS(ip);
//         print_return_stack(r0, rs);
//         ip = fn;
//         NEXT(); // todo: rm
//     }


//     exit_:
//         // print_return_stack(r0, rs);
//         printf("Top of stack: %ld\n", TOP());
//         // print_return_stack(r0, rs);
//         return;
// }

// int main() {
//     run_thread_test();
// }