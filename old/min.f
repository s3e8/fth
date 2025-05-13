\ Stage 1: Basic dictionary and stack operations
: test-dup dup ;
: test-drop drop ;
: test-swap swap ;
: test-over over ;

\ Stage 2: Arithmetic
: test-add + ;
: test-sub - ;
: test-mul * ;
: test-div / ;

\ Stage 3: Memory operations
: test-! ! ;
: test-@ @ ;
: test-c! c! ;
: test-c@ c@ ;

\ Stage 4: Control flow
: test-if if then ;
: test-begin-until begin until ;
: test-begin-while-repeat begin while repeat ;

\ Stage 5: String operations (basic)
: test-emit emit ;
: test-key key ;

\ Test runner
: run-tests
  cr ." Running tests..." cr
  1 2 test-dup .s cr
  1 test-drop .s cr
  1 2 test-swap .s cr
  1 2 test-over .s cr
  2 3 test-add . cr
  5 3 test-sub . cr
  2 3 test-mul . cr
  6 3 test-div . cr
  here 42 test-! here @ . cr
  here 65 test-c! here c@ emit cr
  ." Tests completed" cr
;

run-tests