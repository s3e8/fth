#ifndef CELL
#include <stdint.h>
typedef uintptr_t cell;
#endif

// todo: does order matter?
// "green" thread
typedef struct thread_state_t {
    cell                    killed;    // ?
    struct thread_state_t*  next;

    // We simulate control flow by moving an instruction pointer (ip), 
    // which points into a block of bytecode (really just a void*[] array.
    void**  ip;
    cell*   ds;
    cell*   s0;
    void*** rs;
    void*** r0;
    float*  fs;
    float*  f0;
    cell*   ts; // ?
    cell*   t0; // ?
} thread_state_t;

static thread_state_t* current_thread = NULL;

static thread_state_t* init_thread(cell* s0, void*** r0, cell* t0, void** entrypoint);
static thread_state_t* create_thread(int ds_size, int rs_size, int ts_size, void** entrypoint);
static void kill_thread();