// fth_thread.h
#ifndef FTH_THREAD_H
#define FTH_THREAD_H

// todo: does order matter?
// "green" cooperative thread
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

//
#define NEXT()      goto **ip++
#define PUSH(x)     *--ds = (cell)(x)
#define POP()       (*ds++)
#define FPUSH(x)    *--fs = (float)(x)
#define FPOP()      (*fs++)
#define PUSHRS(x)   *--rs = (void**)(x)
#define POPRS()     (*rs++)
#define INTARG()    ((cell)(*ip++))
#define FLOATARG()  (*(float*)ip)
#define ARG()       (*ip++)
#define TOP()       (*ds) // todo: TOP -- vs: "(*(ds))"? // always returns mot recent/last pushed value
#define FTOP()      (*fs)
#define AT(x)       (*(ds+(x)))
#define FAT(x)      (*(fs+(x)))

thread_state_t* init_thread(cell* s0, void*** r0, cell* t0, void** entrypoint);
thread_state_t* create_thread(int ds_size, int rs_size, int ts_size, void** entrypoint);
void kill_thread();
// helper stuff
void print_stack(cell* s0, cell* ds);
void print_return_stack(void*** r0, void*** rs);
void print_thread_state(thread_state_t* t);

#endif // FTH_THREAD_H