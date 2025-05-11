// to-compile: gcc -o fth_thread fth_thread.c
#include "fth.h"

// todo: 
// round-robin scheduler bytecode?
// change name from thread to... fiber/routine/task/context? idk
// safe mode that checks for under/overflow
// check direction of "s0 <= ds < s0 + 64" etc

thread_state_t* current_thread = NULL;

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

thread_state_t* create_thread(int ds_size, int rs_size, int ts_size, void** entrypoint) {
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
void print_stack(cell* s0, cell* ds) {
    printf("DS: [ ");
    for (cell* p = ds; p < s0 + 64; ++p) printf("%ld ", *p);
    printf("] ds-ok\n");
}

void print_return_stack(void*** r0, void*** rs) {
    printf("RS: [ ");
    for (void*** p = rs; p < r0 + 64; ++p) if(p) printf("%p ", *p);
    printf("] rs-ok\n");
}