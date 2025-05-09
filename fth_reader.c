#include "fth_reader.h"

static void emit_char(int c, FILE *fp) {
    fputc(c, fp);
}