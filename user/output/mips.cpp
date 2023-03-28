#include<map>
#include<limits>
#include<cassert>
#include<algorithm>
#include"mips.hpp"
#include"../../src/RegAlloc.hpp"
#include"../../src/Tree.hpp"

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

namespace Yamg{

    using cost_t = int;
    using rule_number_t = Rules;
    using MyPair = std::pair<rule_number_t, cost_t>;
    typedef int (*costFunction_t)(Rules, Tree);

    const int infinity{ std::numeric_limits<int>::max() };

    cost_t cost(Rules, Tree);

    const int recognition_table[62][40]{
        {  59,   42,    0,   37,   33,    2,    0,    1,   54,   48,    0,   45,    0,   50,    4,    8,    0,   12,    0,    0,    0,   15,    0,    0,    0,    0,    0,    0,   17,    0,    0,    0,   21,    0,   25,    0,   29,    0,    3,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    5,    0},
        {   0,    0,    0,    0,    0,    6,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    7,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    9,    0},
        {   0,    0,    0,    0,    0,   10,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   11,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   13,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   14,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   16,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   18,    0},
        {   0,    0,    0,    0,    0,   19,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   20,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   22,    0},
        {   0,    0,    0,    0,    0,   23,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   24,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   26,    0},
        {   0,    0,    0,    0,    0,   27,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   28,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   30,    0},
        {   0,    0,    0,    0,    0,   32,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   31,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   34,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   35},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   36},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   38},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   39,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   40,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   41},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,   43,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   44,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,   46,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,   47,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,   49,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   51,    0},
        {   0,    0,    0,    0,    0,   52,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   53,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   55,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   56,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   57,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   58,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   60},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   61},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    };

    MyPair isFinalState(int state, Tree t, costFunction_t cost){
        switch(state){
            case    1: return MyPair{ Rules::_var, cost(Rules::_var, t) };
            case    2: return MyPair{ Rules::_const, cost(Rules::_const, t) };
            case    3: return MyPair{ Rules::_statement, cost(Rules::_statement, t) };
            case    6: return MyPair{ Rules::_addConst, cost(Rules::_addConst, t) };
            case    7: return MyPair{ Rules::_addReg, cost(Rules::_addReg, t) };
            case   10: return MyPair{ Rules::_subConst, cost(Rules::_subConst, t) };
            case   11: return MyPair{ Rules::_subReg, cost(Rules::_subReg, t) };
            case   14: return MyPair{ Rules::_mul, cost(Rules::_mul, t) };
            case   16: return MyPair{ Rules::_increment, cost(Rules::_increment, t) };
            case   19: return MyPair{ Rules::_subscriptConst, cost(Rules::_subscriptConst, t) };
            case   20: return MyPair{ Rules::_subscript, cost(Rules::_subscript, t) };
            case   23: return MyPair{ Rules::_equalsConst, cost(Rules::_equalsConst, t) };
            case   24: return MyPair{ Rules::_equalsReg, cost(Rules::_equalsReg, t) };
            case   27: return MyPair{ Rules::_lessThanConst, cost(Rules::_lessThanConst, t) };
            case   28: return MyPair{ Rules::_lessThan, cost(Rules::_lessThan, t) };
            case   31: return MyPair{ Rules::_leqConst, cost(Rules::_leqConst, t) };
            case   32: return MyPair{ Rules::_leq, cost(Rules::_leq, t) };
            case   35: return MyPair{ Rules::_if, cost(Rules::_if, t) };
            case   36: return MyPair{ Rules::_ifElse, cost(Rules::_ifElse, t) };
            case   41: return MyPair{ Rules::_for, cost(Rules::_for, t) };
            case   43: return MyPair{ Rules::_returnConst, cost(Rules::_returnConst, t) };
            case   44: return MyPair{ Rules::_return, cost(Rules::_return, t) };
            case   46: return MyPair{ Rules::_print, cost(Rules::_print, t) };
            case   47: return MyPair{ Rules::_printVar, cost(Rules::_printVar, t) };
            case   49: return MyPair{ Rules::_exit, cost(Rules::_exit, t) };
            case   52: return MyPair{ Rules::_assignConst, cost(Rules::_assignConst, t) };
            case   53: return MyPair{ Rules::_assign, cost(Rules::_assign, t) };
            case   54: return MyPair{ Rules::_functionZero, cost(Rules::_functionZero, t) };
            case   55: return MyPair{ Rules::_functionOne, cost(Rules::_functionOne, t) };
            case   56: return MyPair{ Rules::_functionTwo, cost(Rules::_functionTwo, t) };
            case   57: return MyPair{ Rules::_functionThree, cost(Rules::_functionThree, t) };
            case   58: return MyPair{ Rules::_functionFour, cost(Rules::_functionFour, t) };
            case   61: return MyPair{ Rules::_command, cost(Rules::_command, t) };
            case   60: return MyPair{ Rules::_lastCommand, cost(Rules::_lastCommand, t) };
            default: return MyPair{ Rules::null, infinity };
        }
    }

    bool isFinalState(int state){
        switch(state){
            case    1: return true;
            case    2: return true;
            case    3: return true;
            case    6: return true;
            case    7: return true;
            case   10: return true;
            case   11: return true;
            case   14: return true;
            case   16: return true;
            case   19: return true;
            case   20: return true;
            case   23: return true;
            case   24: return true;
            case   27: return true;
            case   28: return true;
            case   31: return true;
            case   32: return true;
            case   35: return true;
            case   36: return true;
            case   41: return true;
            case   43: return true;
            case   44: return true;
            case   46: return true;
            case   47: return true;
            case   49: return true;
            case   52: return true;
            case   53: return true;
            case   54: return true;
            case   55: return true;
            case   56: return true;
            case   57: return true;
            case   58: return true;
            case   61: return true;
            case   60: return true;
            default: return false;
        }
    }

    bool isTerm(int user_symbol){
        if(user_symbol >= TERMINALS_START && user_symbol <= TERMINALS_END) return true;
        else return false;
    }
    bool isNonTerm(int user_symbol){
        if(user_symbol >= NON_TERMINALS_START && user_symbol <= NON_TERMINALS_END) return true;
        else return false;
    }

    int ruleSize(Rules r) {
        switch(r) {
            case Rules::_var: return 1;
            case Rules::_const: return 1;
            case Rules::_statement: return 1;
            case Rules::_addConst: return 3;
            case Rules::_addReg: return 3;
            case Rules::_subConst: return 3;
            case Rules::_subReg: return 3;
            case Rules::_mul: return 3;
            case Rules::_increment: return 2;
            case Rules::_subscriptConst: return 3;
            case Rules::_subscript: return 3;
            case Rules::_equalsConst: return 3;
            case Rules::_equalsReg: return 3;
            case Rules::_lessThanConst: return 3;
            case Rules::_lessThan: return 3;
            case Rules::_leqConst: return 3;
            case Rules::_leq: return 3;
            case Rules::_if: return 3;
            case Rules::_ifElse: return 4;
            case Rules::_for: return 5;
            case Rules::_returnConst: return 2;
            case Rules::_return: return 2;
            case Rules::_print: return 2;
            case Rules::_printVar: return 3;
            case Rules::_exit: return 2;
            case Rules::_assignConst: return 3;
            case Rules::_assign: return 3;
            case Rules::_functionZero: return 1;
            case Rules::_functionOne: return 2;
            case Rules::_functionTwo: return 3;
            case Rules::_functionThree: return 4;
            case Rules::_functionFour: return 5;
            case Rules::_command: return 3;
            case Rules::_lastCommand: return 2;
        }
    }

    int costMinMunch(Rules r, Tree t) {
        switch(r) {
            case Rules::_var :
                return 1;
                break;
            case Rules::_const :
                return 1;
                break;
            case Rules::_statement :
                return 1;
                break;
            case Rules::_addConst :
                return 3;
                break;
            case Rules::_addReg :
                return 3;
                break;
            case Rules::_subConst :
                return 3;
                break;
            case Rules::_subReg :
                return 3;
                break;
            case Rules::_mul :
                return 3;
                break;
            case Rules::_increment :
                return 2;
                break;
            case Rules::_subscriptConst :
                return 3;
                break;
            case Rules::_subscript :
                return 3;
                break;
            case Rules::_equalsConst :
                return 3;
                break;
            case Rules::_equalsReg :
                return 3;
                break;
            case Rules::_lessThanConst :
                return 3;
                break;
            case Rules::_lessThan :
                return 3;
                break;
            case Rules::_leqConst :
                return 3;
                break;
            case Rules::_leq :
                return 3;
                break;
            case Rules::_if :
                return 3;
                break;
            case Rules::_ifElse :
                return 4;
                break;
            case Rules::_for :
                return 5;
                break;
            case Rules::_returnConst :
                return 2;
                break;
            case Rules::_return :
                return 2;
                break;
            case Rules::_print :
                return 2;
                break;
            case Rules::_printVar :
                return 3;
                break;
            case Rules::_exit :
                return 2;
                break;
            case Rules::_assignConst :
                return 3;
                break;
            case Rules::_assign :
                return 3;
                break;
            case Rules::_functionZero :
                return 1;
                break;
            case Rules::_functionOne :
                return 2;
                break;
            case Rules::_functionTwo :
                return 3;
                break;
            case Rules::_functionThree :
                return 4;
                break;
            case Rules::_functionFour :
                return 5;
                break;
            case Rules::_command :
                return 3;
                break;
            case Rules::_lastCommand :
                return 2;
                break;
            default:
                return infinity;
            }
    }

    int costMaxMunch(Rules r, Tree t) {
        switch(r) {
            case Rules::_var :
                return -1;
                break;
            case Rules::_const :
                return -1;
                break;
            case Rules::_statement :
                return -1;
                break;
            case Rules::_addConst :
                return -3;
                break;
            case Rules::_addReg :
                return -3;
                break;
            case Rules::_subConst :
                return -3;
                break;
            case Rules::_subReg :
                return -3;
                break;
            case Rules::_mul :
                return -3;
                break;
            case Rules::_increment :
                return -2;
                break;
            case Rules::_subscriptConst :
                return -3;
                break;
            case Rules::_subscript :
                return -3;
                break;
            case Rules::_equalsConst :
                return -3;
                break;
            case Rules::_equalsReg :
                return -3;
                break;
            case Rules::_lessThanConst :
                return -3;
                break;
            case Rules::_lessThan :
                return -3;
                break;
            case Rules::_leqConst :
                return -3;
                break;
            case Rules::_leq :
                return -3;
                break;
            case Rules::_if :
                return -3;
                break;
            case Rules::_ifElse :
                return -4;
                break;
            case Rules::_for :
                return -5;
                break;
            case Rules::_returnConst :
                return -2;
                break;
            case Rules::_return :
                return -2;
                break;
            case Rules::_print :
                return -2;
                break;
            case Rules::_printVar :
                return -3;
                break;
            case Rules::_exit :
                return -2;
                break;
            case Rules::_assignConst :
                return -3;
                break;
            case Rules::_assign :
                return -3;
                break;
            case Rules::_functionZero :
                return -1;
                break;
            case Rules::_functionOne :
                return -2;
                break;
            case Rules::_functionTwo :
                return -3;
                break;
            case Rules::_functionThree :
                return -4;
                break;
            case Rules::_functionFour :
                return -5;
                break;
            case Rules::_command :
                return -3;
                break;
            case Rules::_lastCommand :
                return -2;
                break;
            default:
                return infinity;
            }
    }

    using SymbolArray = std::vector<int>; 
    using StateArray  = std::vector<int>; 

    StateArray matchTree(Tree& t, StateArray& final_states, StateArray start_states=StateArray{0}){
        SymbolArray symbols{ (int)t.getSymbol() };
        for( int rule: t.non_terminal ) symbols.push_back(rule);
        StateArray states{};
        for( int symbol: symbols ){
            for( int state: start_states ){
                state = recognition_table[state][symbol];
                if( state != 0 ) states.push_back(state);
                if( isFinalState(state) ) final_states.push_back(state);
            }
        }
        for( Tree c: t.getChildren() ){
            StateArray newStates{ matchTree(c, final_states, states) };
            if(newStates.empty()){
                return states;
            }else{
                states = newStates;
            }
        }
        return states;
    }

    void transitoryRules(Tree &t, StateArray &final_states, costFunction_t cost) {
        SymbolArray symbols{ t.getSymbol() };
        for(int rule: t.non_terminal) symbols.push_back(rule);
        for(int symbol: symbols) {
            if(!isNonTerm(symbol)) continue;
            int state = recognition_table[0][symbol];
            if(isFinalState(state)) {
                if (std::find(final_states.begin(), final_states.end(), state) == final_states.end()) {
                    final_states.emplace_back(state);
                    MyPair s{isFinalState(state, t, cost)};
                    t.non_terminal.emplace_back(Rules_Non_Terminals.at(s.first));
                }
            }
        }
    }

    void _label(Tree& t, costFunction_t cost){
        for( int i=0; i<t.children_size; i++ ){
            Tree& c{ t.getChildReference(i) };
            _label(c, cost);
        }
        StateArray final_states{};
        matchTree(t, final_states);
        for (int state: final_states){
            MyPair s{isFinalState(state, t, cost)};
            t.non_terminal.push_back(Rules_Non_Terminals.at(s.first));
        }
        transitoryRules(t, final_states, cost);
        for( int state: final_states ){
            MyPair s{ isFinalState(state, t, cost) };
            auto exists = std::find_if(t.matched_rules.begin(), t.matched_rules.end(), [s](std::pair<Yamg::Rules, int> r)->bool { return s.first == r.first; });
            if(exists == t.matched_rules.end()) {
                auto pos = std::find_if(t.matched_rules.begin(), t.matched_rules.end(), [s](std::pair<Yamg::Rules, int> r) -> bool { return r.second > s.second; });
                t.matched_rules.insert(pos, s);
            }
        }
    }

    using RuleLimit_t = std::pair<int, User_Symbols>;
    const std::map<Rules, std::vector<RuleLimit_t>> RulesLimitsMap{
        { Rules::_var,
            {
                RuleLimit_t{ 0, User_Symbols::VARIABLE },
            },
        },
        { Rules::_const,
            {
                RuleLimit_t{ 0, User_Symbols::CONST },
            },
        },
        { Rules::_statement,
            {
                RuleLimit_t{ 0, User_Symbols::reg },
            },
        },
        { Rules::_addConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_addReg,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_subConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_subReg,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_mul,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_increment,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_subscriptConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_subscript,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_equalsConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_equalsReg,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_lessThanConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_lessThan,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_leqConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_leq,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_if,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::stmt },
            },
        },
        { Rules::_ifElse,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::stmt },
                RuleLimit_t{ 3, User_Symbols::stmt },
            },
        },
        { Rules::_for,
            {
                RuleLimit_t{ 1, User_Symbols::stmt },
                RuleLimit_t{ 2, User_Symbols::reg },
                RuleLimit_t{ 3, User_Symbols::reg },
                RuleLimit_t{ 4, User_Symbols::stmt },
            },
        },
        { Rules::_returnConst,
            {
            },
        },
        { Rules::_return,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_print,
            {
            },
        },
        { Rules::_printVar,
            {
            },
        },
        { Rules::_exit,
            {
            },
        },
        { Rules::_assignConst,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_assign,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_functionZero,
            {
                RuleLimit_t{ 0, User_Symbols::FUNCTION_CALL },
            },
        },
        { Rules::_functionOne,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
        { Rules::_functionTwo,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
            },
        },
        { Rules::_functionThree,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
                RuleLimit_t{ 3, User_Symbols::reg },
            },
        },
        { Rules::_functionFour,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
                RuleLimit_t{ 2, User_Symbols::reg },
                RuleLimit_t{ 3, User_Symbols::reg },
                RuleLimit_t{ 4, User_Symbols::reg },
            },
        },
        { Rules::_command,
            {
                RuleLimit_t{ 1, User_Symbols::stmt },
                RuleLimit_t{ 2, User_Symbols::stmt },
            },
        },
        { Rules::_lastCommand,
            {
                RuleLimit_t{ 1, User_Symbols::stmt },
            },
        },
    };

    int action(Rules r, Tree &t, int subrule){
        switch(r){
            case Rules::_var :
                switch(subrule) {
                    case -1:
                        {
                            Logger::Log("Var", t.getChildName(1, RulesLimitsMap.at(Rules::_var)));
                        }
                        break;
                    }
                break;
            case Rules::_const :
                switch(subrule) {
                    case -1:
                        {
                            std::string reg = newTempRegister();
                            RegAlloc::newInstruction({"li " + reg + ", " + t.getChildName(0, RulesLimitsMap.at(Rules::_const))}, {}); 
                            t.getChild(0, RulesLimitsMap.at(Rules::_const)).setName(reg);
                            Logger::Log("Const", reg + " = " + t.getChildName(0, RulesLimitsMap.at(Rules::_const)));
                        }
                        break;
                    }
                break;
            case Rules::_statement :
                switch(subrule) {
                    case -1:
                        {
                            Logger::Log("Statement", t.getChildName(1, RulesLimitsMap.at(Rules::_statement)));
                        }
                        break;
                    }
                break;
            case Rules::_addConst :
                switch(subrule) {
                    case -1:
                        {
                            regConstInstruction("addi", t.getChildName(0, RulesLimitsMap.at(Rules::_addConst)), t.getChildName(1, RulesLimitsMap.at(Rules::_addConst)), t.getChildName(2, RulesLimitsMap.at(Rules::_addConst)));
                            Logger::Log("Add Const", t.getChildName(1, RulesLimitsMap.at(Rules::_addConst)) + " + " + t.getChildName(2, RulesLimitsMap.at(Rules::_addConst)));
                        }
                        break;
                    }
                break;
            case Rules::_addReg :
                switch(subrule) {
                    case -1:
                        {
                            doubleRegisterInstruction("add", t.getChildName(0, RulesLimitsMap.at(Rules::_addReg)), t.getChildName(1, RulesLimitsMap.at(Rules::_addReg)), t.getChildName(2, RulesLimitsMap.at(Rules::_addReg)));
                            Logger::Log("Add Reg", t.getChildName(1, RulesLimitsMap.at(Rules::_addReg)) + " + " + t.getChildName(2, RulesLimitsMap.at(Rules::_addReg)));
                        }
                        break;
                    }
                break;
            case Rules::_subConst :
                switch(subrule) {
                    case -1:
                        {
                            regConstInstruction("subi", t.getChildName(0, RulesLimitsMap.at(Rules::_subConst)), t.getChildName(1, RulesLimitsMap.at(Rules::_subConst)), t.getChildName(2, RulesLimitsMap.at(Rules::_subConst)));
                            Logger::Log("Sub Const", t.getChildName(1, RulesLimitsMap.at(Rules::_subConst)) + " - " + t.getChildName(2, RulesLimitsMap.at(Rules::_subConst)));
                        }
                        break;
                    }
                break;
            case Rules::_subReg :
                switch(subrule) {
                    case -1:
                        {
                            doubleRegisterInstruction("sub", t.getChildName(0, RulesLimitsMap.at(Rules::_subReg)), t.getChildName(1, RulesLimitsMap.at(Rules::_subReg)), t.getChildName(2, RulesLimitsMap.at(Rules::_subReg)));
                            Logger::Log("Sub Reg", t.getChildName(1, RulesLimitsMap.at(Rules::_subReg)) + " - " + t.getChildName(2, RulesLimitsMap.at(Rules::_subReg)));
                        }
                        break;
                    }
                break;
            case Rules::_mul :
                switch(subrule) {
                    case -1:
                        {
                            // doubleRegisterInstruction("mul", t.getChildName(0, RulesLimitsMap.at(Rules::_mul)), t.getChildName(1, RulesLimitsMap.at(Rules::_mul)), t.getChildName(2, RulesLimitsMap.at(Rules::_mul)));
                            std::string templ{ "mult " };
                            std::vector<Instruction::OperandType> operands{};
                            if(isNonAllocable(t.getChildName(1, RulesLimitsMap.at(Rules::_mul)))){
                                templ += t.getChildName(1, RulesLimitsMap.at(Rules::_mul)) + ", ";
                            } else {
                                templ += "%o, ";
                                operands.emplace_back(Instruction::OperandType{t.getChildName(1, RulesLimitsMap.at(Rules::_mul))});
                            }
                            if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_mul)))) {
                                templ += t.getChildName(2, RulesLimitsMap.at(Rules::_mul));
                            } else {
                                templ += "%o";
                                operands.emplace_back(Instruction::OperandType{t.getChildName(2, RulesLimitsMap.at(Rules::_mul))});
                            }
                            RegAlloc::newInstruction(templ, operands);
                            RegAlloc::newInstruction({"mflo " + t.getChildName(0, RulesLimitsMap.at(Rules::_mul))}, {});
                            Logger::Log("Mul", t.getChildName(1, RulesLimitsMap.at(Rules::_mul)) + " * " + t.getChildName(2, RulesLimitsMap.at(Rules::_mul)));
                        }
                        break;
                    }
                break;
            case Rules::_increment :
                switch(subrule) {
                    case -1:
                        {
                            std::string templ{"addi "};
                            std::vector<Instruction::OperandType> operands;
                            if(isNonAllocable(t.getChildName(1, RulesLimitsMap.at(Rules::_increment)))) {
                                templ += t.getChildName(1, RulesLimitsMap.at(Rules::_increment)) + ", " + t.getChildName(1, RulesLimitsMap.at(Rules::_increment)) + ", 1";
                            } else {
                                templ += "%o, %o, 1";
                                operands.emplace_back(Instruction::OperandType{t.getChildName(1, RulesLimitsMap.at(Rules::_increment))});
                                operands.emplace_back(Instruction::OperandType{t.getChildName(1, RulesLimitsMap.at(Rules::_increment))});
                            }
                            RegAlloc::newInstruction(templ, operands);
                            Logger::Log("Increment", t.getChildName(1, RulesLimitsMap.at(Rules::_increment)) + "++");
                        }
                        break;
                    }
                break;
            case Rules::_subscriptConst :
                switch(subrule) {
                    case -1:
                        {
                            RegAlloc::newInstruction({"lw " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscriptConst)) + ", " + std::to_string(std::stoi(t.getChildName(2, RulesLimitsMap.at(Rules::_subscriptConst)))*4) + "(%o)"}, { Instruction::OperandType{t.getChildName(1, RulesLimitsMap.at(Rules::_subscriptConst))} } );
                            Logger::Log("Subscript Const", t.getChildName(1, RulesLimitsMap.at(Rules::_subscriptConst)) + "[" + t.getChildName(2, RulesLimitsMap.at(Rules::_subscriptConst)) + "])");
                        }
                        break;
                    }
                break;
            case Rules::_subscript :
                switch(subrule) {
                    case -1:
                        {
                            std::string templ{"lw " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", "};
                            if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_subscript)))) {
                                std::string subscriptor = t.getChildName(2, RulesLimitsMap.at(Rules::_subscript));
                                if(isNumber(t.getChildName(2, RulesLimitsMap.at(Rules::_subscript)))) {
                                    subscriptor = std::to_string(4 * std::stoi(subscriptor));
                                    RegAlloc::newInstruction({"li " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", " + subscriptor}, {});
                                } else {
                                    RegAlloc::newInstruction({"sll " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", " + subscriptor + ", 2"}, {});
                                }
                                std::string addi = (isNumber(subscriptor) ? "i " : " ");
                                RegAlloc::newInstruction({"add " + addi + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", %o, " + t.getChildName(2, RulesLimitsMap.at(Rules::_subscript))}, { {t.getChildName(1, RulesLimitsMap.at(Rules::_subscript))} });
                                templ += "(" + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ")";
                            } else {
                                RegAlloc::newInstruction({"sll " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", %o, 2"}, { {t.getChildName(2, RulesLimitsMap.at(Rules::_subscript))} });
                                RegAlloc::newInstruction({"add " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ", %o, " + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript))}, { {t.getChildName(1, RulesLimitsMap.at(Rules::_subscript))} });
                                templ += "(" + t.getChildName(0, RulesLimitsMap.at(Rules::_subscript)) + ")";
                            }
                            RegAlloc::newInstruction(templ, {});
                            Logger::Log("Subscript", t.getChildName(1, RulesLimitsMap.at(Rules::_subscript)) + "[" + t.getChildName(2, RulesLimitsMap.at(Rules::_subscript)) + "])");
                        }
                        break;
                    }
                break;
            case Rules::_equalsConst :
                switch(subrule) {
                    case -1:
                        {
                            regConstInstruction("seq", t.getChildName(0, RulesLimitsMap.at(Rules::_equalsConst)), t.getChildName(1, RulesLimitsMap.at(Rules::_equalsConst)), t.getChildName(2, RulesLimitsMap.at(Rules::_equalsConst)));
                            Logger::Log("Equals Const", t.getChildName(1, RulesLimitsMap.at(Rules::_equalsConst)) + " == " + t.getChildName(2, RulesLimitsMap.at(Rules::_equalsConst)));
                        }
                        break;
                    }
                break;
            case Rules::_equalsReg :
                switch(subrule) {
                    case -1:
                        {
                            doubleRegisterInstruction("seq", t.getChildName(0, RulesLimitsMap.at(Rules::_equalsReg)), t.getChildName(1, RulesLimitsMap.at(Rules::_equalsReg)), t.getChildName(2, RulesLimitsMap.at(Rules::_equalsReg)));
                            Logger::Log("Equals", t.getChildName(1, RulesLimitsMap.at(Rules::_equalsReg)) + " == " + t.getChildName(2, RulesLimitsMap.at(Rules::_equalsReg)));
                        }
                        break;
                    }
                break;
            case Rules::_lessThanConst :
                switch(subrule) {
                    case -1:
                        {
                            regConstInstruction("slt", t.getChildName(0, RulesLimitsMap.at(Rules::_lessThanConst)), t.getChildName(1, RulesLimitsMap.at(Rules::_lessThanConst)), t.getChildName(2, RulesLimitsMap.at(Rules::_lessThanConst)));
                            Logger::Log("Less Than Const", t.getChildName(1, RulesLimitsMap.at(Rules::_lessThanConst)) + " < " + t.getChildName(2, RulesLimitsMap.at(Rules::_lessThanConst)));
                        }
                        break;
                    }
                break;
            case Rules::_lessThan :
                switch(subrule) {
                    case -1:
                        {
                            doubleRegisterInstruction("slt", t.getChildName(0, RulesLimitsMap.at(Rules::_lessThan)), t.getChildName(1, RulesLimitsMap.at(Rules::_lessThan)), t.getChildName(2, RulesLimitsMap.at(Rules::_lessThan)));
                            Logger::Log("Less Than", t.getChildName(1, RulesLimitsMap.at(Rules::_lessThan)) + " < " + t.getChildName(2, RulesLimitsMap.at(Rules::_lessThan)));
                        }
                        break;
                    }
                break;
            case Rules::_leqConst :
                switch(subrule) {
                    case -1:
                        {
                            doubleRegisterInstruction("sle", t.getChildName(0, RulesLimitsMap.at(Rules::_leqConst)), t.getChildName(1, RulesLimitsMap.at(Rules::_leqConst)), t.getChildName(2, RulesLimitsMap.at(Rules::_leqConst)));
                            Logger::Log("Leq Const", t.getChildName(1, RulesLimitsMap.at(Rules::_leqConst)) + " <= " + t.getChildName(2, RulesLimitsMap.at(Rules::_leqConst)));
                        }
                        break;
                    }
                break;
            case Rules::_leq :
                switch(subrule) {
                    case -1:
                        {
                            regConstInstruction("sle", t.getChildName(0, RulesLimitsMap.at(Rules::_leq)), t.getChildName(1, RulesLimitsMap.at(Rules::_leq)), t.getChildName(2, RulesLimitsMap.at(Rules::_leq)));
                            Logger::Log("Leq", t.getChildName(1, RulesLimitsMap.at(Rules::_leq)) + " <= " + t.getChildName(2, RulesLimitsMap.at(Rules::_leq)));
                        }
                        break;
                    }
                break;
            case Rules::_if :
                switch(subrule) {
                    case 2:
{
                                                 std::string index = std::to_string(labelIndex(true));
                                                 RegAlloc::newInstruction({"beq "+t.getChildName(1, RulesLimitsMap.at(Rules::_if))+", $zero, if_end_"+index}, {});
                                             }
                        break;
                    case -1:
                        {
                            std::string index = std::to_string(labelIndex());
                            RegAlloc::newInstruction({"if_end_"+index+":"}, {});
                            Logger::Log("If");
                        }
                        break;
                    }
                break;
            case Rules::_ifElse :
                switch(subrule) {
                    case 2:
{
                                                std::string index = std::to_string(labelIndex(true));
                                                RegAlloc::newInstruction({"beq "+t.getChildName(1, RulesLimitsMap.at(Rules::_ifElse))+", $zero, if_else_"+index}, {});
                                            }
                        break;
                    case 3:
{
                                                std::string index = std::to_string(labelIndex());
                                                RegAlloc::newInstruction({"j if_end_"+index}, {});
                                                RegAlloc::newInstruction({"if_else_"+index+":"}, {});
                                            }
                        break;
                    case -1:
                        {
                            std::string index = std::to_string(labelIndex());
                            RegAlloc::newInstruction({"if_end_"+index+":"}, {});
                            Logger::Log("_ifElse");
                        }
                        break;
                    }
                break;
            case Rules::_for :
                switch(subrule) {
                    case 1:
{
                                              std::string index = std::to_string(labelIndex(true));
                                              RegAlloc::newInstruction({"for_init_" + index + ":"}, {});
                                          }
                        break;
                    case 2:
{
                                              std::string index = std::to_string(labelIndex());
                                              RegAlloc::newInstruction({"for_check_" + index + ":"}, {});
                                          }
                        break;
                    case 3:
{
                                              std::string index = std::to_string(labelIndex());
                                              std::string templ{"beq "};
                                              std::vector<Instruction::OperandType> operands{};
                                              if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_for)))) templ += t.getChildName(2, RulesLimitsMap.at(Rules::_for)) + ", $zero, for_end_" + index;
                                              else {
                                                  templ += "%o, $zero, for_end_" + index;
                                                  operands.emplace_back(Instruction::OperandType{t.getChildName(2, RulesLimitsMap.at(Rules::_for))});
                                              }
                                              RegAlloc::newInstruction(templ, operands);
                                              RegAlloc::newInstruction({"j for_body_" + index}, {});
                                              RegAlloc::newInstruction({"for_update_" + index + ":"}, {});
                                          }
                        break;
                    case 4:
{
                                              std::string index = std::to_string(labelIndex());
                                              RegAlloc::newInstruction({"j for_check_" + index}, {});
                                              RegAlloc::newInstruction({"for_body_" + index + ":"}, {});
                                          }
                        break;
                    case -1:
                        {
                            std::string index = std::to_string(labelIndex());
                            RegAlloc::newInstruction({"j for_update_" + index}, {});
                            RegAlloc::newInstruction({"for_end_" + index + ":"}, {});
                            Logger::Log("For");
                        }
                        break;
                    }
                break;
            case Rules::_returnConst :
                switch(subrule) {
                    case -1:
                        {
                            returnInstruction(t.getChildName(1, RulesLimitsMap.at(Rules::_returnConst)));
                            Logger::Log("Return Const", t.getChildName(1, RulesLimitsMap.at(Rules::_returnConst)));
                        }
                        break;
                    }
                break;
            case Rules::_return :
                switch(subrule) {
                    case -1:
                        {
                            returnInstruction(t.getChildName(1, RulesLimitsMap.at(Rules::_return)));
                            Logger::Log("Return", t.getChildName(1, RulesLimitsMap.at(Rules::_return)));
                        }
                        break;
                    }
                break;
            case Rules::_print :
                switch(subrule) {
                    case -1:
                        {
                            printInstruction(t.getChildName(1, RulesLimitsMap.at(Rules::_print)));
                            Logger::Log("Printf", AstSymbols::Programa::getLiteralVar(t.getChildName(1, RulesLimitsMap.at(Rules::_print))).value() + ": " + t.getChildName(1, RulesLimitsMap.at(Rules::_print)));
                        }
                        break;
                    }
                break;
            case Rules::_printVar :
                switch(subrule) {
                    case -1:
                        {
                            printInstruction(t.getChildName(1, RulesLimitsMap.at(Rules::_printVar)), t.getChildName(2, RulesLimitsMap.at(Rules::_printVar)));
                            Logger::Log("Printf", t.getChildName(2, RulesLimitsMap.at(Rules::_printVar)));
                        }
                        break;
                    }
                break;
            case Rules::_exit :
                switch(subrule) {
                    case -1:
                        {
                            RegAlloc::newInstruction("li $v0, 10", {});
                            RegAlloc::newInstruction("li $a0, %c", {}, {t.getChildName(1, RulesLimitsMap.at(Rules::_exit))});
                            RegAlloc::newInstruction("syscall", {});
                            Logger::Log("Exit", t.getChildName(1, RulesLimitsMap.at(Rules::_exit)));
                        }
                        break;
                    }
                break;
            case Rules::_assignConst :
                switch(subrule) {
                    case -1:
                        {
                            if(t.getChild(1, RulesLimitsMap.at(Rules::_assignConst)).getSymbol() == (int)Yamg::User_Symbols::SUBSCRIPT) {
                                std::string subscripted = t.getImmediateChild(1).getName();
                                std::string subscriptor = t.getImmediateChild(2).getName();
                                std::string tempReg = t.getChildName(1, RulesLimitsMap.at(Rules::_assignConst));
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
                                if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst)))) {
                                    RegAlloc::newInstruction({"sw " + t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst)) + ", (" + tempReg + ")"}, {});
                                } else {
                                    RegAlloc::newInstruction({"sw %o, (" + tempReg + ")"}, { Instruction::OperandType{t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst))} });
                                }
                            } else {
                                if(isNonAllocable(t.getChildName(1, RulesLimitsMap.at(Rules::_assignConst)))) RegAlloc::newInstruction({"li " + t.getChildName(1, RulesLimitsMap.at(Rules::_assignConst)) +", %c"}, {}, {t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst))});
                                else RegAlloc::newInstruction({"li %o, %c"}, { {t.getChildName(1, RulesLimitsMap.at(Rules::_assignConst)), YAMG_WRITEABLE_OPERAND} }, {t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst))});
                            }
                            Logger::Log("Assign Const", t.getChildName(1, RulesLimitsMap.at(Rules::_assignConst)) + " = " + t.getChildName(2, RulesLimitsMap.at(Rules::_assignConst)));
                        }
                        break;
                    }
                break;
            case Rules::_assign :
                switch(subrule) {
                    case -1:
                        {
                            if(t.getChild(1, RulesLimitsMap.at(Rules::_assign)).getSymbol() == (int)Yamg::User_Symbols::SUBSCRIPT) {
                                std::string subscripted = t.getChild(1, RulesLimitsMap.at(Rules::_assign)).getImmediateChild(1).getName();
                                std::string subscriptor = t.getChild(1, RulesLimitsMap.at(Rules::_assign)).getImmediateChild(2).getName();
                                std::string tempReg = t.getChildName(1, RulesLimitsMap.at(Rules::_assign));
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
                                if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_assign)))) {
                                    RegAlloc::newInstruction({"sw " + t.getChildName(2, RulesLimitsMap.at(Rules::_assign)) + ", (" + tempReg + ")"}, {});
                                } else {
                                    RegAlloc::newInstruction({"sw %o, (" + tempReg + ")"}, { Instruction::OperandType{t.getChildName(2, RulesLimitsMap.at(Rules::_assign))} });
                                }
                            } else {
                                std::string templ{"move "};
                                std::vector<Instruction::OperandType> operands{};
                                if(isNonAllocable(t.getChildName(1, RulesLimitsMap.at(Rules::_assign)))) templ += t.getChildName(1, RulesLimitsMap.at(Rules::_assign)) + ", ";
                                else {
                                    templ += "%o, ";
                                    operands.emplace_back(Instruction::OperandType{t.getChildName(1, RulesLimitsMap.at(Rules::_assign)), YAMG_WRITEABLE_OPERAND});
                                }
                                if(isNonAllocable(t.getChildName(2, RulesLimitsMap.at(Rules::_assign)))) templ += t.getChildName(2, RulesLimitsMap.at(Rules::_assign));
                                else {
                                    templ += "%o";
                                    operands.emplace_back(Instruction::OperandType{t.getChildName(2, RulesLimitsMap.at(Rules::_assign))});
                                }
                                RegAlloc::newInstruction(templ, operands);
                            }
                            Logger::Log("Assign", t.getChildName(1, RulesLimitsMap.at(Rules::_assign)) + " = " + t.getChildName(2, RulesLimitsMap.at(Rules::_assign)));
                        }
                        break;
                    }
                break;
            case Rules::_functionZero :
                switch(subrule) {
                    case -1:
                        {
                            functionInstruction(t.getChildName(0, RulesLimitsMap.at(Rules::_functionZero)));
                            t.getChild(0, RulesLimitsMap.at(Rules::_functionZero)).setName("$v0");
                            Logger::Log("Function Call", "no parameters");
                        }
                        break;
                    }
                break;
            case Rules::_functionOne :
                switch(subrule) {
                    case -1:
                        {
                            functionInstruction(t.getChildName(0, RulesLimitsMap.at(Rules::_functionOne)), t.getChildName(1, RulesLimitsMap.at(Rules::_functionOne)));
                            t.getChild(0, RulesLimitsMap.at(Rules::_functionOne)).setName("$v0");
                            Logger::Log("Function Call", "1 parameter");
                        }
                        break;
                    }
                break;
            case Rules::_functionTwo :
                switch(subrule) {
                    case -1:
                        {
                            functionInstruction(t.getChildName(0, RulesLimitsMap.at(Rules::_functionTwo)), t.getChildName(1, RulesLimitsMap.at(Rules::_functionTwo)), t.getChildName(2, RulesLimitsMap.at(Rules::_functionTwo)));
                            t.getChild(0, RulesLimitsMap.at(Rules::_functionTwo)).setName("$v0");
                            Logger::Log("Function Call", "2 parameters");
                        }
                        break;
                    }
                break;
            case Rules::_functionThree :
                switch(subrule) {
                    case -1:
                        {
                            functionInstruction(t.getChildName(0, RulesLimitsMap.at(Rules::_functionThree)), t.getChildName(1, RulesLimitsMap.at(Rules::_functionThree)), t.getChildName(2, RulesLimitsMap.at(Rules::_functionThree)), t.getChildName(3, RulesLimitsMap.at(Rules::_functionThree)));
                            t.getChild(0, RulesLimitsMap.at(Rules::_functionThree)).setName("$v0");
                            Logger::Log("Function Call", "3 parameters");
                        }
                        break;
                    }
                break;
            case Rules::_functionFour :
                switch(subrule) {
                    case -1:
                        {
                            functionInstruction(t.getChildName(0, RulesLimitsMap.at(Rules::_functionFour)), t.getChildName(1, RulesLimitsMap.at(Rules::_functionFour)), t.getChildName(2, RulesLimitsMap.at(Rules::_functionFour)), t.getChildName(3, RulesLimitsMap.at(Rules::_functionFour)), t.getChildName(4, RulesLimitsMap.at(Rules::_functionFour)));
                            t.getChild(0, RulesLimitsMap.at(Rules::_functionFour)).setName("$v0");
                            Logger::Log("Function Call", "4 parameters");
                        }
                        break;
                    }
                break;
            case Rules::_command :
                switch(subrule) {
                    case -1:
                        {
                            Logger::Log("Commands");
                        }
                        break;
                    }
                break;
            case Rules::_lastCommand :
                switch(subrule) {
                    case -1:
                        {
                            Logger::Log("Last Command");
                        }
                        break;
                    }
                break;
            default: break;
        }
        return 0;
    }

    bool checkRule(Tree &t, std::vector<RuleLimit_t> limit) {
        int last_index;
        if(t.children_size+1 != ruleSize(t.matched_rules[0].first)) return false;
//        for(RuleLimit_t l: limit) {
//            if(t.children_size < l.first) return false;
//            if(!isTerm((int)l.second)) {
//                if (t.getImmediateChild(l.first).getNonTerms()[0] != (int) l.second) return false;
//            }
//            last_index = l.first+1;
//        }
//        for(int i=last_index; i<=t.children_size; i++) {
//            Tree child = t.getImmediateChild(i);
//            if(isNonTerm(child.getNonTerms()[0])) return false;
//        }
        return true;
    }

    void _reduce(Tree&);
    void _reduceAux(Tree& originalTree, Tree& t, std::vector<RuleLimit_t>& limit, int& count, const Rules &rule){
        if( !limit.empty() && count == limit.at(0).first ){
            limit.erase(limit.begin());
            action(rule, originalTree, count);
            if( count != 0 ) _reduce(t);
            return;
        } else {
            action(rule, originalTree, count);
        }

        for( Tree &c: t.getChildren() ){
            count++;
            _reduceAux(originalTree, c, limit, count, rule);
            if( limit.empty() ) break;
        }
    }

    void _reduce(Tree& t){
        Rules r{ t.matched_rules.at(0).first };
        std::vector<RuleLimit_t> limit{ RulesLimitsMap.at(r) };
        while(!checkRule(t, limit)){
            t.matched_rules.erase(t.matched_rules.begin());
            assert(t.matched_rules[0].first!=Rules::null && "NENHUMA REGRA ENCONTRADA\n");
            r = t.matched_rules.at(0).first;
            limit = RulesLimitsMap.at(r);
        }
        int count{ 0 };
        _reduceAux(t, t, limit, count, r);
        action(r, t, -1);
    }

    void _setRegisters() {
        RegAlloc::_newReg({"$s0"});
        RegAlloc::_newReg({"$s1"});
        RegAlloc::_newReg({"$s2"});
        RegAlloc::_newReg({"$s3"});
        RegAlloc::_newReg({"$s4"});
        RegAlloc::_newReg({"$s5"});
        RegAlloc::_newReg({"$s6"});
        RegAlloc::_newReg({"$s7"});
        RegAlloc::_newSpillReg({"$t0"});
        RegAlloc::_newSpillReg({"$t1"});
        RegAlloc::_newSpillReg({"$t2"});
        RegAlloc::_setReadInstruction(Instruction{ {"lw %o, %o($sp)"}, {} });
        RegAlloc::_setWriteInstruction(Instruction{ {"sw %o, %o($sp)"}, {} });
    }

    void match(Tree&t, costFunction_t cost) {
        _setRegisters();
        _label(t, cost);
        _reduce(t);
    }

    void matchMinimalMunch(Tree &t) {
        match(t, costMinMunch);
    }
    void matchMaximalMunch(Tree &t) {
        match(t, costMaxMunch);
    }
}


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
