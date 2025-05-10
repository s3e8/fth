#define DS_SIZE 64
#define RS_SIZE 32

#define HERE_SIZE 10*1024*1024 // 10mb

#define NESTINGSTACK_MAX_DEPTH  16
#define WORD_NAME_MAX_LEN       32

#define STATE_IMMEDIATE 0 // todo: this should probably be in dict.h instead?
#define STATE_COMPILE   1 //

#define BIT(x) (1<<(x))
#define FLAG_HIDDEN      BIT(0)
#define FLAG_IMMED       BIT(1)
#define FLAG_BUILTIN     BIT(2)
#define FLAG_HASARG      BIT(3)
#define FLAG_INLINE      BIT(4)
#define FLAG_DEFERRED    BIT(5)