// to compile: gcc -o fth fth.c fth_dict.c fth_thread.c fth_io.c -ledit
#include "fth.h"

// misc tests...
void test_create_word() {
    printf("Creating: 'created-word'...\n");
    create_word("created-word", 0);  // Use consistent name
    word_hdr_t* res = find_word("created-word");
    if (res) {
        if (strncmp(res->name, "created-word", WORD_NAME_MAX_LEN) == 0) {
            printf("Created word found!\n");
        } else {
            printf("Created word found but name mismatch!\n");
        }
    } else {
        printf("Created word NOT found!\n");
    }

    printf("created-word cfa: %p\n", cfa(res));
}

void test_init_dict() {

}

void test_assemble_word() {
    printf("Creating: 'assembled-word'...\n");
    assemble_word("assembled-word", 0, (void**)20, 0);  // Use consistent name
    word_hdr_t* res = find_word("assembled-word");
    if (res) {
        if (strncmp(res->name, "assembled-word", WORD_NAME_MAX_LEN) == 0) {
            printf("Assembled word found!\n");
        } else {
            printf("Assembled word found but name mismatch!\n");
        }
    } else {
        printf("Assembled word NOT found!\n");
    }
}

void test_dict() {
    here_size   = HERE_SIZE;
    here0       = MALLOC(here_size);
    here        = here0;
    if (!here0) {
        fprintf(stderr, "Failed to allocate dictionary memory!\n");
        exit(1);
    }
    test_create_word();
    // test_assemble_word();
}

void test_thread() {
    void* program[] = {
        &&lit,  (void*)42,
        &&lit,  (void*)100,
        &&add,
        &&exit_
    };

    void* subroutine[] = {
        &&lit,  (void*)10,
        &&add,
        &&exit_
    };

    void* main_program[] = {
        &&lit, (void*)32,
        &&lit, (void*)100,
        &&call, subroutine,
    };

    // Allocate and initialize thread
    // thread_state_t* t = create_thread(64, 64, 0, program);
    thread_state_t* t = create_thread(64, 64, 0, main_program);

    cell* ds = t->s0 + 64; // stack grows downward
    cell* s0 = t->s0;
    void*** rs = t->r0;
    void*** r0 = t->r0;

    void** ip = t->ip;

    // cell    ds[DS_SIZE] = {0};
    // void**  ds[RS_SIZE] = {0};

    // cell*   s0 = ds;
    // cell*   t0 = NULL;
    // float*  f0 = NULL;
    // cell*   ts = NULL;
    // float*  fs = NULL;
    // void*** r0 = rs;


    // init_thread(s0, r0, t0, ip);

    goto **ip++;

    lit:
        printf("Before LIT:\n");
        print_stack(s0, ds);
        PUSH(INTARG());
        printf("After LIT:\n");
        print_stack(s0, ds);
        NEXT();

    add: {
        printf("Before ADD:\n");
        print_stack(s0, ds);
        cell a = POP();
        cell b = POP();
        PUSH(a + b);
        print_stack(s0, ds);
        NEXT();
    }

    call: {
        void *fn = ARG();
        PUSHRS(ip);
        print_return_stack(r0, rs);
        ip = fn;
        NEXT(); // todo: rm
    }


    exit_:
        // print_return_stack(r0, rs);
        printf("Top of stack: %ld\n", TOP());
        // print_return_stack(r0, rs);
        return;
}

/* utilies for calculating branch offsets in inline bytecode and referencing bytecodes */
#define OFFSET(x) (void*)((x)*sizeof(cell))
#define WORD(name) &&l_##name

// static void interpret(void **ip, cell *ds, void ***rs, reader_state_t *inputstate, FILE *outp, int argc, char **argv)
static void run(void** ip, cell* ds, void*** rs, reader_state_t* inputstate, FILE* outp, int argc, char** argv) {
    printf("DBG: running interpreter...\n");
    static int initialized = 0;
    printf("DBG: initialization flag set to 0...\n");

    printf("DBG: creating up temporary buffer cell...\n");
    // temp buffer used by bytecodes
    register cell tmp;
    printf("DBG: temporary buffer cell created and initialized successfully.n");

    printf("DBG: initializing STATE and BASE...\n");
    cell state = STATE_IMMEDIATE;   // initialize STATE word
    printf("DBG: STATE initialized.\n");
    printf("DBG: initializing BASE.\n");
    cell base  = 10;                // initialize BASE  word
    printf("DBG: BASE initialized.\n");
    printf("DBG: STATE and BASE successfully initialized.n");

    printf("DBG: initializing stack...");
    cell*   s0 = ds;
    cell*   t0 = NULL;
    float*  f0 = NULL;
    cell*   ts = NULL;
    float*  fs = NULL;
    void*** r0 = rs;
    for (cell*   p = ds; p < s0 + DS_SIZE; ++p) p = 0;
    for (void*** p = rs; p < r0 + RS_SIZE; ++p) p = 0;
    print_stack(s0, ds);

    // ...
    void**  nestingstack_space[NESTINGSTACK_MAX_DEPTH];
    void*** nestingstack = nestingstack_space + NESTINGSTACK_MAX_DEPTH;

    // ...
    void **debugger_vector = NULL;

    void* builtin_immediatebuf[2]   = { NULL, WORD(IRETURN) };              // is this 2 or 1?
    void* word_immediatebuf[3]      = { WORD(CALL), NULL, WORD(IRETURN) };  //

    char wordbuf[WORD_NAME_MAX_LEN];
    char linebuf[WORD_NAME_MAX_LEN];

    char stdinbuf[1024];
    reader_state_t stdin_state;

    // init dyncall...
    // DCCallVM* callvm = dcNewCallVM((DCsize)4096);

    /* trick: include bytecodes.h with a macro for BYTECODE that produces builtin
    * list elements */
    static builtin_word_t builtins[] = {
        #define BYTECODE(label, name, nargs, nfargs, flags, code) { name, &&l_##label, flags },
        #include "fth_opcode.h"
        #undef BYTECODE
        { NULL, NULL, 0 }
    };

    /* one time init: install the builtins into dictionary and define some essential variables */
    if (!initialized) {
        initialized = 1;
        
        // disable input buffering, we have our own
        // init reader state

        // build core (builtins and constants)

        // build builtins...
        builtin_word_t* b = builtins;
        while(b->name) create_builtin(b++);

        // build constants...

        init_thread(s0, r0, t0, ip);

        // init topmost quitcode interpreter loop:
        // QUIT is the topmost interpreter loop: interpret forever
        // better version implemented in forth later that supports eof etc
        void* program[] = {
            &&lit,  (void*)42,
            &&lit,  (void*)100,
            &&add,
            &&exit_
        };

        void* subroutine[] = {
            &&lit,  (void*)10,
            &&add,
            &&exit_
        };

        void* main_program[] = {
            &&lit, (void*)32,
            &&lit, (void*)100,
            &&call, subroutine,
        };

        ip = main_program;
    }

    // calling NEXT/goto starts the loop...
    NEXT();

    // todo: idky we do this twice
    // #define BYTECODE(label, name, nargs, nfargs, flags, code) l_##label: CHECKSTACK(name, nargs) CHECKFSTACK(name, nfargs) code NEXT();
    #define BYTECODE(label, name, nargs, nfargs, flags, code) l_##label: code NEXT();
    #include "fth_opcode.h"

    lit:
        printf("Before LIT:\n");
        print_stack(s0, ds);
        PUSH(INTARG());
        printf("After LIT:\n");
        print_stack(s0, ds);
        NEXT();

    add: {
        printf("Before ADD:\n");
        print_stack(s0, ds);
        cell a = POP();
        cell b = POP();
        PUSH(a + b);
        print_stack(s0, ds);
        NEXT();
    }

    call: {
        void *fn = ARG();
        PUSHRS(ip);
        print_return_stack(r0, rs);
        ip = fn;
        NEXT(); // todo: rm
    }


    exit_:
        // print_return_stack(r0, rs);
        printf("Top of stack: %ld\n", TOP());
        // print_return_stack(r0, rs);
        return;
}

int main(int argc, char** argv) {
    // // Initialize ds/rs here for speed. Init the rest later:
    // // ... todo: etc
    // // ... todo: etc
    cell    datastack[DS_SIZE]      = {0};  // todo: change to 1024 default
    void**  returnstack[RS_SIZE]    = {0};  // todo: change to  512 default


    // here_size   = HERE_SIZE;
    // here0       = MALLOC(here_size);
    // here        = here0;
    // test_dict();
    // test_thread();

    reader_state_t *fp = open_file("bootstrap.f", "r");
    if(!fp) {
        fprintf(stderr, "Cannot open bootstrap file forth.f!\n");
        return 1;
    }
    printf("DBG: bootstrap file opened successfully...\n");

    run(NULL, datastack + DS_SIZE, returnstack + RS_SIZE, fp, stdout, argc, argv);

    return 0;
}