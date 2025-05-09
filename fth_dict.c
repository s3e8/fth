#include <stddef.h>
#include "fth_dict.h"

static void*        here;
static void*        here0;
static cell         here_size;
struct word_hdr_t*  latest = NULL;

static word_hdr_t* create_word(const char* name, cell flags) {
    if(!name) name="\0";

    word_hdr_t* new = (word_hdr_t*)here;
    here += sizeof(word_hdr_t);

    strncpy(new->name, name, WORD_NAME_MAX_LEN);
    new->flags = flags;
    new->next = latest;
    latest = new;

    return new;
}

static word_hdr_t* find(const char* name) {
    if(!name) return NULL;

    word_hdr_t* hdr = latest;
    while(hdr) {
        if(!strncmp(hdr->name, name, WORD_NAME_MAX_LEN)) return hdr;
        hdr = hdr->next;
    }
    return NULL;
}

static void** cfa(word_hdr_t* word) {
    return (void**)(word + 1);
}

// comma operator stores a value (usually a number or an address) into the next available spot in memory (typically here)
static void comma(cell val) {
    *(cell*)here = val;
    here += sizeof(cell);
}

// fetches the cfa/execution token (single quote, not actual "`")
static void* tick(const char* name) {
    word_hdr_t* hdr = find(name);
    return *(cfa(hdr));
}

// utility stuff
static void assemble_word(const char* name, cell flags, void** code, cell codesize) {
    create_word(name, flags);

    int i;
    for(i = 0; i < codesize/sizeof(void*); i++) {
        comma((cell)code[i]);
    }

    comma((cell)tick("eow"));
}

// todo: if assemble already adds 'tick(eow)', do we need it here?
static void create_constant(const char* name, cell val) {
    void* flagdef[] = { tick("lit"), (void*)val, tick("exit") };
    assemble_word(name, FLAG_INLINE, flagdef, sizeof(flagdef));
}

// Core words and helpers
static void create_builtin(builtin_word_t* b) {
    create_word(b->name, b->flags | FLAG_BUILTIN);
    comma((cell)b->code);
    // comma(0); // ??
}