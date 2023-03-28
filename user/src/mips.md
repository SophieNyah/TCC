{
    #include"../src/Tree.h"
    #include<iostream>

    bool isNumber(std::string);
    bool isLiteral(std::string);
    bool isRegister(std::string);
    bool isTempReg(std::string);
    bool isVariableRegister(std::string);
    bool isArgumentReg(std::string);
    bool isNonAllocable(std::string);
    void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable=false);
    void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable=false);
    void returnInstruction(std::string operand);
    void functionInstruction(std::string function, std::string var1="", std::string var2="", std::string var3="", std::string var4="");
    void printInstruction(std::string literal, std::string integer="");
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
%term SUBSCRIPT

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
    Logger::Log("Var", $[1]);
};
_const <- reg: CONST {} = {
    std::string reg = newTempRegister();
    RegAlloc::newInstruction({"li " + reg + ", " + $[0]}, {}); 
    $node[0].setName(reg);
    Logger::Log("Const", reg + " = " + $[0]);
};
_statement <- stmt: reg {} = {
    Logger::Log("Statement", $[1]);
};

_addConst <- reg: ADD(reg,CONST) {} = {
    regConstInstruction("addi", $[0], $[1], $[2]);
    Logger::Log("Add Const", $[1] + " + " + $[2]);
};
_addReg <- reg: ADD(reg,reg) {} = {
    doubleRegisterInstruction("add", $[0], $[1], $[2]);
    Logger::Log("Add Reg", $[1] + " + " + $[2]);
};
_subConst <- reg: SUB(reg,CONST) {} = {
    regConstInstruction("subi", $[0], $[1], $[2]);
    Logger::Log("Sub Const", $[1] + " - " + $[2]);
};
_subReg <- reg: SUB(reg,reg) {} = {
    doubleRegisterInstruction("sub", $[0], $[1], $[2]);
    Logger::Log("Sub Reg", $[1] + " - " + $[2]);
};
_mul <- reg: MUL(reg,reg) {} = {
    // doubleRegisterInstruction("mul", $[0], $[1], $[2]);
    std::string templ{ "mult " };
    std::vector<Instruction::OperandType> operands{};
    if(isNonAllocable($[1])){
        templ += $[1] + ", ";
    } else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{$[1]});
    }
    if(isNonAllocable($[2])) {
        templ += $[2];
    } else {
        templ += "%o";
        operands.emplace_back(Instruction::OperandType{$[2]});
    }
    RegAlloc::newInstruction(templ, operands);
    RegAlloc::newInstruction({"mflo " + $[0]}, {});
    Logger::Log("Mul", $[1] + " * " + $[2]);
};

_increment <- reg: INC(reg) {} = {
    std::string templ{"addi "};
    std::vector<Instruction::OperandType> operands;
    if(isNonAllocable($[1])) {
        templ += $[1] + ", " + $[1] + ", 1";
    } else {
        templ += "%o, %o, 1";
        operands.emplace_back(Instruction::OperandType{$[1]});
        operands.emplace_back(Instruction::OperandType{$[1]});
    }
    RegAlloc::newInstruction(templ, operands);
    Logger::Log("Increment", $[1] + "++");
};
_subscriptConst <- reg: SUBSCRIPT(reg, CONST) {} = {
    RegAlloc::newInstruction({"lw " + $[0] + ", " + std::to_string(std::stoi($[2])*4) + "(%o)"}, { Instruction::OperandType{$[1]} } );
    Logger::Log("Subscript Const", $[1] + "[" + $[2] + "])");
};
_subscript <- reg: SUBSCRIPT(reg, reg) {} = {
    std::string templ{"lw " + $[0] + ", "};
    if(isNonAllocable($[2])) {
        std::string subscriptor = $[2];
        if(isNumber($[2])) {
            subscriptor = std::to_string(4 * std::stoi(subscriptor));
            RegAlloc::newInstruction({"li " + $[0] + ", " + subscriptor}, {});
        } else {
            RegAlloc::newInstruction({"sll " + $[0] + ", " + subscriptor + ", 2"}, {});
        }
        std::string addi = (isNumber(subscriptor) ? "i " : " ");
        RegAlloc::newInstruction({"add " + addi + $[0] + ", %o, " + $[2]}, { {$[1]} });
        templ += "(" + $[0] + ")";
    } else {
        RegAlloc::newInstruction({"sll " + $[0] + ", %o, 2"}, { {$[2]} });
        RegAlloc::newInstruction({"add " + $[0] + ", %o, " + $[0]}, { {$[1]} });
        templ += "(" + $[0] + ")";
    }
    RegAlloc::newInstruction(templ, {});
    Logger::Log("Subscript", $[1] + "[" + $[2] + "])");
};


_equalsConst <- reg: EQUALS(reg,CONST) {} = {
    regConstInstruction("seq", $[0], $[1], $[2]);
    Logger::Log("Equals Const", $[1] + " == " + $[2]);
};
_equalsReg <- reg: EQUALS(reg,reg) {} = {
    doubleRegisterInstruction("seq", $[0], $[1], $[2]);
    Logger::Log("Equals", $[1] + " == " + $[2]);
};
_lessThanConst <- reg: LESS(reg, CONST) {} = {
    regConstInstruction("slt", $[0], $[1], $[2]);
    Logger::Log("Less Than Const", $[1] + " < " + $[2]);
};
_lessThan <- reg: LESS(reg, reg) {} = {
    doubleRegisterInstruction("slt", $[0], $[1], $[2]);
    Logger::Log("Less Than", $[1] + " < " + $[2]);
};
_leqConst <- reg: LEQ(reg, reg) {} = {
    doubleRegisterInstruction("sle", $[0], $[1], $[2]);
    Logger::Log("Leq Const", $[1] + " <= " + $[2]);
};
_leq <- reg: LEQ(reg, CONST) {} = {
    regConstInstruction("sle", $[0], $[1], $[2]);
    Logger::Log("Leq", $[1] + " <= " + $[2]);
};

_if <- stmt: IF(reg, {
                         std::string index = std::to_string(labelIndex(true));
                         RegAlloc::newInstruction({"beq "+$[1]+", $zero, if_end_"+index}, {});
                     } stmt) {} = {
    std::string index = std::to_string(labelIndex());
    RegAlloc::newInstruction({"if_end_"+index+":"}, {});
    Logger::Log("If");
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
    Logger::Log("_ifElse");
};

_for <- stmt: FOR({
                      std::string index = std::to_string(labelIndex(true));
                      RegAlloc::newInstruction({"for_init_" + index + ":"}, {});
                  } stmt, {
                      std::string index = std::to_string(labelIndex());
                      RegAlloc::newInstruction({"for_check_" + index + ":"}, {});
                  } reg, {
                      std::string index = std::to_string(labelIndex());
                      std::string templ{"beq "};
                      std::vector<Instruction::OperandType> operands{};
                      if(isNonAllocable($[2])) templ += $[2] + ", $zero, for_end_" + index;
                      else {
                          templ += "%o, $zero, for_end_" + index;
                          operands.emplace_back(Instruction::OperandType{$[2]});
                      }
                      RegAlloc::newInstruction(templ, operands);
                      RegAlloc::newInstruction({"j for_body_" + index}, {});
                      RegAlloc::newInstruction({"for_update_" + index + ":"}, {});
                  } reg, {
                      std::string index = std::to_string(labelIndex());
                      RegAlloc::newInstruction({"j for_check_" + index}, {});
                      RegAlloc::newInstruction({"for_body_" + index + ":"}, {});
                  } stmt) {} =
{
    std::string index = std::to_string(labelIndex());
    RegAlloc::newInstruction({"j for_update_" + index}, {});
    RegAlloc::newInstruction({"for_end_" + index + ":"}, {});
    Logger::Log("For");
};

_returnConst <- stmt: RETURN(CONST) {} = {
    returnInstruction($[1]);
    Logger::Log("Return Const", $[1]);
};
_return <- stmt: RETURN(reg) {} = {
    returnInstruction($[1]);
    Logger::Log("Return", $[1]);
};

_print <- stmt: PRINTF(STRING) {} = {
    printInstruction($[1]);
    Logger::Log("Printf", AstSymbols::Programa::getLiteralVar($[1]).value() + ": " + $[1]);
};
_printVar <- stmt: PRINTF(STRING, VARIABLE) {} = {
    printInstruction($[1], $[2]);
    Logger::Log("Printf", $[2]);
};
_exit <- stmt: EXIT(CONST) {} = {
    RegAlloc::newInstruction("li $v0, 10", {});
    RegAlloc::newInstruction("li $a0, %c", {}, {$[1]});
    RegAlloc::newInstruction("syscall", {});
    Logger::Log("Exit", $[1]);
};

_assignConst <- stmt: ASSIGN(reg,CONST) {} = {
    if($node[1].getSymbol() == (int)Yamg::User_Symbols::SUBSCRIPT) {
        std::string subscripted = t.getImmediateChild(1).getName();
        std::string subscriptor = t.getImmediateChild(2).getName();
        std::string tempReg = $[1];
        if(isNonAllocable(subscriptor)) {
            if(isNumber(subscriptor)){
                subscriptor = std::to_string(4 * std::stoi(subscriptor));
                RegAlloc::newInstruction({"li " + tempReg + ", " + subscriptor}, {});
            } else {
                RegAlloc::newInstruction({"sll " + tempReg + ", " + subscriptor + ", 2"}, {});
            }
            std::string addi = (isNumber(subscriptor) ? "i " : " ");
            RegAlloc::newInstruction({"add" + addi + tempReg + ", %o, " + subscriptor}, { Instruction::OperandType{subscripted} });
    } else {
        RegAlloc::newInstruction({"sll " + tempReg + ", %o, 2"}, { Instruction::OperandType{subscriptor} });
        RegAlloc::newInstruction({"add " + tempReg + ", %o, %o"}, { Instruction::OperandType{subscriptor}, Instruction::OperandType{subscripted} });
    }
        if(isNonAllocable($[2])) {
            RegAlloc::newInstruction({"sw " + $[2] + ", (" + tempReg + ")"}, {});
        } else {
            RegAlloc::newInstruction({"sw %o, (" + tempReg + ")"}, { Instruction::OperandType{$[2]} });
        }
    } else {
        if(isNonAllocable($[1])) RegAlloc::newInstruction({"li " + $[1] +", %c"}, {}, {$[2]});
        else RegAlloc::newInstruction({"li %o, %c"}, { {$[1], YAMG_WRITEABLE_OPERAND} }, {$[2]});
    }
    Logger::Log("Assign Const", $[1] + " = " + $[2]);
};
_assign <- stmt: ASSIGN(reg,reg) {} = {
    if($node[1].getSymbol() == (int)Yamg::User_Symbols::SUBSCRIPT) {
        std::string subscripted = $node[1].getImmediateChild(1).getName();
        std::string subscriptor = $node[1].getImmediateChild(2).getName();
        std::string tempReg = $[1];
        if(isNonAllocable(subscriptor)) {
            if(isNumber(subscriptor)){
                subscriptor = std::to_string(4 * std::stoi(subscriptor));
                RegAlloc::newInstruction({"li " + tempReg + ", " + subscriptor}, {});
            } else {
                RegAlloc::newInstruction({"sll " + tempReg + ", " + subscriptor + ", 2"}, {});
            }
            std::string addi = (isNumber(subscriptor) ? "i " : " ");
            RegAlloc::newInstruction({"add" + addi + tempReg + ", %o, " + subscriptor}, { Instruction::OperandType{subscripted} });
        } else {
            Logger::Log("SLL TA AQUI PORRA");
            RegAlloc::newInstruction({"sll " + tempReg + ", %o, 2"}, { Instruction::OperandType{subscriptor} });
            RegAlloc::newInstruction({"add " + tempReg + ", %o, %o"}, { Instruction::OperandType{subscripted}, Instruction::OperandType{subscriptor} });
        }
        if(isNonAllocable($[2])) {
            RegAlloc::newInstruction({"sw " + $[2] + ", (" + tempReg + ")"}, {});
        } else {
            RegAlloc::newInstruction({"sw %o, (" + tempReg + ")"}, { Instruction::OperandType{$[2]} });
        }
    } else {
        std::string templ{"move "};
        std::vector<Instruction::OperandType> operands{};
        if(isNonAllocable($[1])) templ += $[1] + ", ";
        else {
            templ += "%o, ";
            operands.emplace_back(Instruction::OperandType{$[1], YAMG_WRITEABLE_OPERAND});
        }
        if(isNonAllocable($[2])) templ += $[2];
        else {
            templ += "%o";
            operands.emplace_back(Instruction::OperandType{$[2]});
        }
        RegAlloc::newInstruction(templ, operands);
    }
    Logger::Log("Assign", $[1] + " = " + $[2]);
};
_functionZero <- reg: FUNCTION_CALL {} = {
    functionInstruction($[0]);
    $node[0].setName("$v0");
    Logger::Log("Function Call", "no parameters");
};
_functionOne <- reg: FUNCTION_CALL(reg) {} = {
    functionInstruction($[0], $[1]);
    $node[0].setName("$v0");
    Logger::Log("Function Call", "1 parameter");
};
_functionTwo <- reg: FUNCTION_CALL(reg,reg) {} = {
    functionInstruction($[0], $[1], $[2]);
    $node[0].setName("$v0");
    Logger::Log("Function Call", "2 parameters");
};
_functionThree <- reg: FUNCTION_CALL(reg,reg,reg) {} = {
    functionInstruction($[0], $[1], $[2], $[3]);
    $node[0].setName("$v0");
    Logger::Log("Function Call", "3 parameters");
};
_functionFour <- reg: FUNCTION_CALL(reg,reg,reg,reg) {} = {
    functionInstruction($[0], $[1], $[2], $[3], $[4]);
    $node[0].setName("$v0");
    Logger::Log("Function Call", "4 parameters");
};

_command <- stmt: COMMAND(stmt,stmt) {} = {
    Logger::Log("Commands");
};
_lastCommand <- stmt: COMMAND(stmt) {} = {
    Logger::Log("Last Command");
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
bool isNumber(std::string reg)           { return (std::isdigit(reg[0])); }
bool isLiteral(std::string reg)          { return (reg[0] == '\"' || reg[0] == '\'' || std::isdigit(reg[0])); }
bool isTempReg(std::string reg)          { return (isRegister(reg) && (reg[1] == 't')); }
bool isRegister(std::string reg)         { return (reg[0] == '$'); }
bool isArgumentReg(std::string reg)      { return (isRegister(reg) && (reg[1] == 'a')); }
bool isVariableRegister(std::string reg) { return (isRegister(reg) && reg[1] == 's'); }
bool isReturnReg(std::string reg)        { return (isRegister(reg) && reg[1] == 'v'); }
bool isNonAllocable(std::string reg) {
    return isNumber(reg)    || 
           isLiteral(reg)   || 
           isTempReg(reg)   || 
           isReturnReg(reg) ||
           isArgumentReg(reg);
}

void doubleRegisterInstruction(std::string instruction, std::string root, std::string reg1, std::string reg2, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(!isNonAllocable(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(isNonAllocable(reg1)) {
        templ += reg1 + ", ";
    } else {
        templ += "%o, ";
        operands.emplace_back(Instruction::OperandType{reg1});
    }
    if(isNonAllocable(reg2)) {
        templ += reg2;
    } else {
        templ += "%o";
        operands.emplace_back(Instruction::OperandType{reg2});
    }
    RegAlloc::newInstruction(templ, operands);
}

void regConstInstruction(std::string instruction, std::string root, std::string reg, std::string cnst, bool isRootWriteable){
    std::string templ{ instruction + " " + root + ", " };
    std::vector<Instruction::OperandType> operands{};
    if(!isNonAllocable(root)){
        operands.emplace_back(Instruction::OperandType{root, isRootWriteable});
    }
    if(isNonAllocable(reg)) {
        templ += reg + ", ";
    } else {
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
        if(isNonAllocable(operand)) {
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
    std::vector<std::string> vars{};
    if(!var1.empty()) vars.emplace_back(var1);
    if(!var2.empty()) vars.emplace_back(var2);
    if(!var3.empty()) vars.emplace_back(var3);
    if(!var4.empty()) vars.emplace_back(var4);

    RegAlloc::storeStack();

    RegAlloc::newInstruction({"subi $sp, $sp, " + std::to_string(vars.size() * 4)}, {});
    for(int i=0; i<vars.size(); i++) {
        std::string store{"sw $a" + std::to_string(i) + ", " + std::to_string(i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
        if(isLiteral(vars[i])) {
            if(isNumber(vars[i])) {
                RegAlloc::newInstruction({"li $a" + std::to_string(i) + ", %c"}, {}, { vars[i] });
            } else {
                throw std::runtime_error(std::to_string(i) + "º argumento da função \"" + function + "\" não é um inteiro nem uma variável");
                // RegAlloc::newInstruction({"move $a"+std::to_string(i)+", " + vars[i]}, {});
            }
        } else {
            RegAlloc::newInstruction({"move $a"+std::to_string(i)+", %o"}, { {vars[i] } });
        }
    }

    RegAlloc::newInstruction({"jal " + function}, {});

    for(int i=vars.size()-1; i>=0; i--) {
        std::string store{"lw $a" + std::to_string(i) + ", " + std::to_string(i*4) + "($sp)"};
        RegAlloc::newInstruction(store, {});
    }
    RegAlloc::newInstruction({"addi $sp, $sp, " + std::to_string(vars.size()*4)}, {});
    RegAlloc::retrieveStack();
}

void printInstruction(std::string literal, std::string integer) {
    RegAlloc::newInstruction({"subi $sp, $sp, 4"}, {});
    RegAlloc::newInstruction({"sw $a0, 0($sp)"}, {});
    if(integer.empty()) {
        std::string var = AstSymbols::Programa::getLiteralVar(literal).value();
        RegAlloc::newInstruction({"li $v0, 4"}, {});
        RegAlloc::newInstruction({"la $a0, " + var}, {});
    } else {
        RegAlloc::newInstruction({"li $v0, 1"}, {});
        std::string templ{"la $a0, ("};
        std::vector<Instruction::OperandType> operands{};
        if(isNonAllocable(integer)) {
            templ += integer;
        } else {
            templ += "%o";
            operands.emplace_back(Instruction::OperandType{integer});
        }
        RegAlloc::newInstruction(templ + ")", operands);
    }
    RegAlloc::newInstruction({"syscall"}, {});
    RegAlloc::newInstruction({"lw $a0, 0($sp)"}, {});
    RegAlloc::newInstruction({"addi $sp, $sp, 4"}, {});
}

}
