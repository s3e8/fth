// fth_dict.h
#ifndef FTH_DICT_H
#define FTH_DICT_H

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

word_hdr_t*  create_word(const char* name, cell flags);
word_hdr_t*  find_word(const char* name);
void**       cfa(word_hdr_t* word);
void         comma(cell val);
void*        tick(const char* name);
// Dictionary helpers //
void assemble_word(const char* name, cell flags, void** code, cell codesize);
void create_constant(const char* name, cell val);
void create_builtin(builtin_word_t* b);

#endif // FTH_DICT_H