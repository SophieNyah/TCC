{
    #include"../src/Tree.h"
    #include<iostream>
}

%alg MAXIMAL_MUNCH
%alg DYNAMIC_PROGRAMMING
%alg MINIMAL_MUNCH

%term COMMAND

%term RETURN
%term WHILE
%term FOR
%term IF

%term CONST
%term STRING
%term VARIABLE
%term FUNCTION_CALL

%term EXIT
%term SCANF
%term PRINTF
%term SWITCH

%term ASSIGN

%term ADD
%term SUB
%term DIV
%term MUL
%term MOD

%term RSHIFT
%term LSHIFT

%term INC
%term DEC
%term UNR_PLUS
%term UNR_MINUS
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

%nonterm reg
%nonterm stmt

%%

_var       <- reg: VARIABLE {} = { std::cout << "_var\n"; };
_const     <- reg: CONST    {} = { std::cout << "_const\n"; };
_statement <- stmt: reg     {} = { std::cout << "_statement\n"; };

_addConst <- reg: ADD(reg,CONST) {} = { std::cout << "_addConst\n"; };
_addReg   <- reg: ADD(reg,reg)   {} = { std::cout << "_addReg\n"; };
_subConst <- reg: SUB({std::cout<<"   teste1\n";}reg,{std::cout<<"    teste2\n";}CONST) {} = { std::cout << "_subConst\n"; };
_subReg   <- reg: SUB(reg,reg)   {} = { std::cout << "_subReg\n"; };
_mul      <- reg: MUL(reg,reg)   {} = { std::cout << "_mul\n"; };

_equals <- reg: EQUALS(reg,reg) {} = { std::cout << "_equals\n"; };

_if        <- stmt: IF(reg,stmt)      {} = { std::cout << "_if\n"; };
_ifElse    <- stmt: IF(reg,stmt,stmt) {} = { std::cout << "_ifElse\n"; };
_return    <- stmt: RETURN(reg)       {} = { std::cout << "_return\n"; };
_assign    <- stmt: ASSIGN(reg,reg)   {} = { std::cout << "_assign\n"; };
_function  <- reg: FUNCTION_CALL      {} = { std::cout << "_functionCall\n"; };

_command <- stmt: COMMAND(stmt,stmt) {} = { std::cout << "_command\n"; };
_lastCommand <- stmt: COMMAND(stmt)  {} = { std::cout << "_lastCommand\n"; };

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
