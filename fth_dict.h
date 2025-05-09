#include "fth_cfg.h"
#ifndef CELL
#include <stdint.h>
typedef uintptr_t cell;
#endif

#define STATE_IMMEDIATE 0 // todo: this should probably be in dict.h instead?
#define STATE_COMPILE   1 //

#define BIT(x) (1<<(x))
#define FLAG_HIDDEN      BIT(0)
#define FLAG_IMMED       BIT(1)
#define FLAG_BUILTIN     BIT(2)
#define FLAG_HASARG      BIT(3)
#define FLAG_INLINE      BIT(4)
#define FLAG_DEFERRED    BIT(5)

typedef struct word_hdr_t {
    cell                flags;
    struct word_hdr_t*  next;
    char                name[WORD_NAME_MAX_LEN];
} word_hdr_t;

typedef struct builtin_word_t {
    char* name;
    void* code;
    cell flags;
} builtin_word_t;
static void create_builtin(builtin_word_t* b);

cell state = STATE_IMMEDIATE;
static void*        here;
static void*        here0;
static cell         here_size;
struct word_hdr_t*  latest = NULL;

// static word_hdr_t*  create(const char* name, cell flags);
// static word_hdr_t*  find(const char* name);
// static void**       cfa(word_hdr_t* word);
// static void         comma(cell val);
// static void*        tick(const char* name);
// // Dictionary helpers //
// static void assemble(const char* name, cell flags, void** code, cell codesize);
// static void create_constant(const char* name, cell val);