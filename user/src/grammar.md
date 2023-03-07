{
    #include"../src/Tree.h"
}

%term RETURN
%term WHILE
%term FOR
%term IF

%term ADD
%term SUB
%term DIV
%term MOD
%term INC
%term DEC
%term BIT_OR
%term BIT_AND
%term BIT_XOR
%term LOG_AND
%term LOG_OR
%term LOG_NOT
%term EQUALS
%term NOT_EQUALS
%term LESS
%term GREAT
%term LEQ
%term GEQ
%term RSHIFT
%term LSHIFT
%term ASSIGN
%term PRINTF
%term SCANF
%term EXIT
%term UNR_PLUS
%term UNR_MINUS
%term SWITCH
%term CONST

%nonterm reg

%%

registlamoi <- reg: ADD(reg,CONST) {} = {};

%%

%register $s0
%register $s1
%register $s2
%register $s3
%register $s4
%register $s5
%register $s6
%register $s7

%spill_register $t0
%spill_register $t1
%spill_register $t2

%set_read "lw %o, %o($sp)"
%set_write "sw %o, %o($sp)"

%%

{

}
