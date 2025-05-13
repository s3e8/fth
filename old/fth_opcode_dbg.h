// BYTECODE(TELL, "tell", 1, 0, 0, { 
//     printf("TELL: str=%p ('%s')\n", (char*)POP(), (char*)TOP()); 
//     fputs((char*)POP(), outp); 
// })

// BYTECODE(DUMPDS, ".ds", 0, 0, 0, {
//     printf("DS: ");
//     for(cell* p = s0; p > ds; --p) printf("%ld ", *p);
//     printf("\n");
//     // print_stack(s0, ds);
//     print_ds(s0, ds);
// })
// BYTECODE(DUMPRS, ".rs", 0, 0, 0, {
//     printf("RS: ");
//     for(void*** p = r0; p > rs; --p) printf("%p ", *p);
//     printf("\n");
// })
// BYTECODE(DUMPDICT, ".dict", 0, 0, 0, {
//     word_hdr_t* word = latest;
//     while(word) {
//         printf("%p: %s (flags: %lx)\n", word, word->name, word->flags);
//         word = word->next;
//     }
// })