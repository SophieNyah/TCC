{
    #include"../src/Tree.h"
    #include<iostream>
    
    bool isRegister(std::string);
    bool isTempReg(std::string);
    bool isArgumentReg(std::string);
    void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable=false);
    void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable=false);
}

%alg MAXIMAL_MUNCH
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

_var <- reg: VARIABLE {} = {
    std::cout << "_var\n";
};
_const <- reg: CONST {} = {
    std::string reg = newTempRegister();
    RegAlloc::newInstruction({"li " + reg + ", " + $[0]}, {}); 
    $node[0].setName(reg);
    std::cout << "_const " << reg << " = " << $[0] << "\n";
};
_statement <- stmt: reg {} = {
    std::cout << "_statement\n";
};

_addConst <- reg: ADD(reg,CONST) {} = {
    regConstInstruction("addi", $[0], $[1], $[2]);
    std::cout << "_addConst\n";
};
_addReg <- reg: ADD(reg,reg) {} = {
    doubleRegisterInstruction("add", $[0], $[1], $[2]);
    std::cout << "_addReg\n";
};
_subConst <- reg: SUB(reg,CONST) {} = {
    regConstInstruction("subi", $[0], $[1], $[2]);
    std::cout << "_subConst\n";
};
_subReg <- reg: SUB(reg,reg) {} = {
    doubleRegisterInstruction("sub", $[0], $[1], $[2]);
    std::cout << "_subReg\n";
};
_mul <- reg: MUL(reg,reg) {} = {
    doubleRegisterInstruction("mul", $[0], $[1], $[2]);
    std::cout << "_mul\n";
};

_equals <- reg: EQUALS(reg,reg) {} = {
    std::cout << "_equals\n";
};

_if <- stmt: IF(reg,stmt) {} = {
    std::cout << "_if\n";
};
_ifElse <- stmt: IF(reg,stmt,stmt) {} = {
    std::cout << "_ifElse\n";
};
_return <- stmt: RETURN(reg) {} = {
    std::cout << "_return\n";
};
_assignConst <- stmt: ASSIGN(reg,CONST) {} = {
    RegAlloc::newInstruction({"li %o, %c"}, { {$[1], YAMG_WRITEABLE_OPERAND} }, {$[2]});
    std::cout << "_assignConst\n";
};
_assign <- stmt: ASSIGN(reg,reg) {} = {
    std::string templ{"move %o, "};
    std::vector<Instruction::OperandType> operands{ {$[1], YAMG_WRITEABLE_OPERAND} };
    if(isTempReg($[2])) templ += $[2];
    else {
        templ += "%o";
        operands.emplace_back(Instruction::OperandType{$[2]});
    }
    RegAlloc::newInstruction(templ, operands);
    std::cout << "_assign\n";
};
_function <- reg: FUNCTION_CALL {} = {
    std::cout << "_functionCall\n";
};

_command <- stmt: COMMAND(stmt,stmt) {} = {
    std::cout << "_command\n";
};
_lastCommand <- stmt: COMMAND(stmt) {} = {
    std::cout << "_lastCommand\n";
};

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
bool isRegister(std::string reg){
    if(reg[0] == '$') return true;
    return false;
}
bool isTempReg(std::string reg) {
    if(isRegister(reg) && (reg[1] == 't')) return true;
    return false;
}
bool isArgumentReg(std::string reg) {
    if(isRegister(reg) && (reg[1] == 'a')) return true;
    return false;
}

void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(isRegister(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(isRegister(reg1)) templ += reg1 + ", ";
    else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{reg1});
    }
    if(isRegister(reg2)) templ += reg2;
    else {
        templ += "%o";
        operands.emplace_back(Instruction::OperandType{reg2});
    }
    RegAlloc::newInstruction(templ, operands);
}

void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(isRegister(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(isRegister(reg)) templ += reg + ", ";
    else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{reg});
    }
    templ += "%c";
    RegAlloc::newInstruction(templ, operands, {cnst});
}
}
