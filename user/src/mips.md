{
    #include"../src/Tree.h"
    #include<iostream>
    
    bool isRegister(std::string);
    bool isTempReg(std::string);
    bool isVariableRegister(std::string);
    bool isArgumentReg(std::string);
    void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable=false);
    void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable=false);
    void returnInstruction(std::string operand);
    void functionInstruction(std::string function, std::string var1="", std::string var2="", std::string var3="", std::string var4="");
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

_equalsConst <- reg: EQUALS(reg,CONST) {} = {
    std::string tempReg = newTempRegister();
    regConstInstruction("seq", tempReg, $[1], $[2]);
    std::cout << "_equalsConst\n";
};
_equalsReg <- reg: EQUALS(reg,reg) {} = {
    std::string tempReg = newTempRegister();
    doubleRegisterInstruction("seq", tempReg, $[1], $[2]);
    std::cout << "_equals\n";
};

_if <- stmt: IF(reg, {
                         std::string index = std::to_string(labelIndex(true));
                         RegAlloc::newInstruction({"beq "+$[1]+", $zero, if_end_"+index}, {});
                     } stmt) {} = {
    std::string index = std::to_string(labelIndex());
    RegAlloc::newInstruction({"if_end_"+index+":"}, {});
    std::cout << "_if\n";
};
_ifElse <- stmt: IF(reg, {
                        std::string index = std::to_string(labelIndex(true));
                        RegAlloc::newInstruction({"beq "+$[1]+", $zero, if_else_"+index}, {});
                    } stmt, {
                        std::string index = std::to_string(labelIndex());
                        RegAlloc::newInstruction({"j if_end_"+index}, {});
                        RegAlloc::newInstruction({"if_else_"+index+":"}, {});
                    }stmt) {} = {
    std::string index = std::to_string(labelIndex());
    RegAlloc::newInstruction({"if_end_"+index+":"}, {});
    std::cout << "_ifElse\n";
};
_returnConst <- stmt: RETURN(CONST) {} = {
    returnInstruction($[1]);
    std::cout << "_returnConst\n";
};
_return <- stmt: RETURN(reg) {} = {
    returnInstruction($[1]);
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
_function0 <- reg: FUNCTION_CALL {} = {
    functionInstruction($[0]);
    std::cout << "_functionCall0\n";
};
_function1 <- reg: FUNCTION_CALL(reg) {} = {
    functionInstruction($[0], $[1]);
    std::cout << "_functionCall1\n";
};
_function2 <- reg: FUNCTION_CALL(reg,reg) {} = {
    functionInstruction($[0], $[1], $[2]);
    std::cout << "_functionCall2\n";
};
_function3 <- reg: FUNCTION_CALL(reg,reg,reg) {} = {
    functionInstruction($[0], $[1], $[2], $[3]);
    std::cout << "_functionCall3\n";
};
_function4 <- reg: FUNCTION_CALL(reg,reg,reg,reg) {} = {
    functionInstruction($[0], $[1], $[2], $[3], $[4]);
    std::cout << "_functionCall4\n";
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
bool isVariableRegister(std::string reg) {
    if(isRegister(reg) && reg[1] == 's') return true;
    return false;
}

void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(isVariableRegister(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(!isVariableRegister(reg1)) templ += reg1 + ", ";
    else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{reg1});
    }
    if(!isVariableRegister(reg2)) templ += reg2;
    else {
        templ += "%o";
        operands.emplace_back(Instruction::OperandType{reg2});
    }
    RegAlloc::newInstruction(templ, operands);
}

void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(isVariableRegister(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(!isVariableRegister(reg)) templ += reg + ", ";
    else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{reg});
    }
    templ += "%c";
    RegAlloc::newInstruction(templ, operands, {cnst});
}

void returnInstruction(std::string operand){
    std::string mvTempl{};
    std::vector<Instruction::OperandType> mvOperands{};
    std::vector<std::string> mvConst{};
    if(isRegister(operand)){
        mvTempl = "move $v0, ";
        if(!isVariableRegister(operand)) {
            mvTempl += operand;
        } else {
            mvTempl += "%o";
            mvOperands.emplace_back(Instruction::OperandType{operand});
        }
    } else {
        mvTempl = "li $v0, %c";
        mvConst.emplace_back(operand);
    }
    RegAlloc::newInstruction(mvTempl, mvOperands, mvConst);
    RegAlloc::clearStack();
    RegAlloc::newInstruction({"lw $ra, 0($sp)"}, {});
    RegAlloc::newInstruction({"addi $sp, $sp, 4"}, {});
    RegAlloc::newInstruction({"jr $ra"}, {});
}

void functionInstruction(std::string function, std::string var1, std::string var2, std::string var3, std::string var4){
    RegAlloc::newInstruction({"subi $sp, $sp, 48"}, {});
    for(int i=0; i<8; i++) {
        std::string store{"sw $s" + std::to_string(i) + ", " + std::to_string(i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
    }
    for(int i=0; i<4; i++) {
        std::string store{"sw $a" + std::to_string(i) + ", " + std::to_string(32 + i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
    }

    if(!var1.empty()) {
        if(!isVariableRegister(var1)) {
            RegAlloc::newInstruction({"move $a0, "+var1}, {});
        } else {
            RegAlloc::newInstruction({"move $a0, %o"}, { {var1} });
        }
    }
    if(!var2.empty()) {
        if(!isVariableRegister(var2)) {
            RegAlloc::newInstruction({"move $a1, "+var2}, {});
        } else {
            RegAlloc::newInstruction({"move $a1, %o"}, { {var2} });
        }
    }
    if(!var3.empty()) {
        if(!isVariableRegister(var3)) {
            RegAlloc::newInstruction({"move $a2, "+var3}, {});
        } else {
            RegAlloc::newInstruction({"move $a2, %o"}, { {var3} });
        }
    }
    if(!var4.empty()) {
        if(!isVariableRegister(var4)) {
            RegAlloc::newInstruction({"move $a3, "+var4}, {});
        } else {
            RegAlloc::newInstruction({"move $a3, %o"}, { {var4} });
        }
    }

    RegAlloc::newInstruction({"j " + function}, {});
    
     for(int i=7; i>=0; i--) {
        std::string store{"lw $s" + std::to_string(i) + ", " + std::to_string(i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
    }
    for(int i=3; i>=0; i--) {
        std::string store{"lw $a" + std::to_string(i) + ", " + std::to_string(32 + i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
    }   
}

}
