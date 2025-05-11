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
    here0       = malloc(here_size);
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

// static void interpret(void **ip, cell *ds, void ***rs, reader_state_t *inputstate, FILE *outp, int argc, char **argv)
static void run(void** ip, cell* ds, void*** rs, int argc, char** argv) {
    static int initialized = 0;

    // temp buffer used by bytecodes
    // register cell tmp;

    // cell state = STATE_IMMEDIATE;

    // cell base = 10;

    cell*   s0 = ds;
    cell*   t0 = NULL;
    float*  f0 = NULL;
    cell*   ts = NULL;
    float*  fs = NULL;
    void*** r0 = rs;

    if (!initialized) {
        initialized = 1;
        
        // disable input buffering, we have our own
        // init reader state
        // build builtins
        // build constants

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
    memset(datastack,   0, sizeof(datastack));
    memset(returnstack, 0, sizeof(returnstack));

    // here_size   = HERE_SIZE;
    // here0       = malloc(here_size);
    // here        = here0;
    // test_dict();
    test_thread();

    run(NULL, datastack + DS_SIZE, returnstack + RS_SIZE, argc, argv);

    return 0;
}