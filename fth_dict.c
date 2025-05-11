// to compile: gcc -o fth_dict fth_dict.c
#include "fth.h"

cell state = STATE_IMMEDIATE;
void*        here;
void*        here0;
cell         here_size;
word_hdr_t*  latest = NULL;

word_hdr_t* create_word(const char* name, cell flags) {
    if(!name) name="\0";

    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);

    strncpy(new->name, name, WORD_NAME_MAX_LEN);
    new->flags = flags;
    new->next = latest;
    latest = new;

    return new;
}

word_hdr_t* find_word(const char* name) {
    if(!name) return NULL;

    word_hdr_t* hdr = latest;
    while(hdr) {
        if(!strncmp(hdr->name, name, WORD_NAME_MAX_LEN)) return hdr;
        hdr = hdr->next;
    }
    return NULL;
}

void** cfa(word_hdr_t* word) {
    return (void**)(word + 1);
}

// comma operator stores a value (usually a number or an address) into the next available spot in memory (typically here)
void comma(cell val) {
    *(cell*)here = val;
    here += sizeof(cell);
}

// fetches the cfa/execution token (single quote, not actual "`")
void* tick(const char* name) {
    word_hdr_t* hdr = find_word(name);
    return *(cfa(hdr));
}

// utility stuff
void assemble_word(const char* name, cell flags, void** code, cell codesize) {
    create_word(name, flags);

    int i;
    for(i = 0; i < codesize/sizeof(void*); i++) {
        comma((cell)code[i]);
    }

    comma((cell)tick("eow"));
}

// todo: if assemble already adds 'tick(eow)', do we need it here?
void create_constant(const char* name, cell val) {
    void* flagdef[] = { tick("lit"), (void*)val, tick("exit") };
    assemble_word(name, FLAG_INLINE, flagdef, sizeof(flagdef));
}

// Core words and helpers
void create_builtin(builtin_word_t* b) {
    create_word(b->name, b->flags | FLAG_BUILTIN);
    comma((cell)b->code);
    // comma(0); // ??
}

// // misc tests...
// void test_create_word() {
//     printf("Creating: 'created-word'...\n");
//     create_word("created-word", 0);  // Use consistent name
//     word_hdr_t* res = find_word("created-word");
//     if (res) {
//         if (strncmp(res->name, "created-word", WORD_NAME_MAX_LEN) == 0) {
//             printf("Created word found!\n");
//         } else {
//             printf("Created word found but name mismatch!\n");
//         }
//     } else {
//         printf("Created word NOT found!\n");
//     }

//     printf("created-word cfa: %p\n", cfa(res));
// }

// void test_init_dict() {

// }

// void test_assemble_word() {
//     printf("Creating: 'assembled-word'...\n");
//     assemble_word("assembled-word", 0, (void**)20, 0);  // Use consistent name
//     word_hdr_t* res = find_word("assembled-word");
//     if (res) {
//         if (strncmp(res->name, "assembled-word", WORD_NAME_MAX_LEN) == 0) {
//             printf("Assembled word found!\n");
//         } else {
//             printf("Assembled word found but name mismatch!\n");
//         }
//     } else {
//         printf("Assembled word NOT found!\n");
//     }
// }

// void test_dict() {
//     here_size   = HERE_SIZE;
//     here0       = malloc(here_size);
//     here        = here0;
//     if (!here0) {
//         fprintf(stderr, "Failed to allocate dictionary memory!\n");
//         exit(1);
//     }
//     test_create_word();
//     // test_assemble_word();
// }

// void main() {
//     printf("starting dictionary test...\n");
//     test_dict();
// }