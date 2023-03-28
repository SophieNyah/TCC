#ifndef MIPS_HPP
#define MIPS_HPP

#include<map>
#include<vector>

    /* Classe do usuário */
class Tree;

namespace Yamg{

        /* Enums */
    enum class Rules {
        null = -1,
        _var = 0,
        _const = 1,
        _statement = 2,
        _addConst = 3,
        _addReg = 4,
        _subConst = 5,
        _subReg = 6,
        _mul = 7,
        _increment = 8,
        _subscriptConst = 9,
        _subscript = 10,
        _equalsConst = 11,
        _equalsReg = 12,
        _lessThanConst = 13,
        _lessThan = 14,
        _leqConst = 15,
        _leq = 16,
        _if = 17,
        _ifElse = 18,
        _for = 19,
        _returnConst = 20,
        _return = 21,
        _print = 22,
        _printVar = 23,
        _exit = 24,
        _assignConst = 25,
        _assign = 26,
        _functionZero = 27,
        _functionOne = 28,
        _functionTwo = 29,
        _functionThree = 30,
        _functionFour = 31,
        _command = 32,
        _lastCommand = 33,
    };

    enum class User_Symbols {
        null = -1,
        ADD = 14,
        ASSIGN = 13,
        BIT_AND = 26,
        BIT_OR = 25,
        BIT_XOR = 27,
        COMMAND = 0,
        CONST = 5,
        DEC = 22,
        DIV = 16,
        EQUALS = 32,
        EXIT = 9,
        FOR = 3,
        FUNCTION_CALL = 8,
        GEQ = 37,
        GREAT = 35,
        IF = 4,
        INC = 21,
        LEQ = 36,
        LESS = 34,
        LOG_AND = 29,
        LOG_NOT = 31,
        LOG_OR = 30,
        LSHIFT = 20,
        MOD = 18,
        MUL = 17,
        NOT_EQUALS = 33,
        PRINTF = 11,
        RETURN = 1,
        RSHIFT = 19,
        SCANF = 10,
        STRING = 6,
        SUB = 15,
        SUBSCRIPT = 28,
        SWITCH = 12,
        UNR_MINUS = 24,
        UNR_PLUS = 23,
        VARIABLE = 7,
        WHILE = 2,
        reg = 38,
        stmt = 39,
    };

        /* Aliases */
    using cost_t = int;
    using rule_number_t = Rules;
    using MyPair = std::pair<rule_number_t, cost_t>;
    using SymbolArray = std::vector<int>; 
    using StateArray  = std::vector<int>; 
    using RuleLimit_t = std::pair<int, User_Symbols>;

        /* Variáveis */
    extern const int infinity;
    const int TERMINALS_START     = 0;
    const int TERMINALS_END       = 37;
    const int NON_TERMINALS_START = 38;
    const int NON_TERMINALS_END   = 39;
    const std::map<Rules, int> Rules_Non_Terminals{
        { Rules::_var, 38 },
        { Rules::_const, 38 },
        { Rules::_statement, 39 },
        { Rules::_addConst, 38 },
        { Rules::_addReg, 38 },
        { Rules::_subConst, 38 },
        { Rules::_subReg, 38 },
        { Rules::_mul, 38 },
        { Rules::_increment, 38 },
        { Rules::_subscriptConst, 38 },
        { Rules::_subscript, 38 },
        { Rules::_equalsConst, 38 },
        { Rules::_equalsReg, 38 },
        { Rules::_lessThanConst, 38 },
        { Rules::_lessThan, 38 },
        { Rules::_leqConst, 38 },
        { Rules::_leq, 38 },
        { Rules::_if, 39 },
        { Rules::_ifElse, 39 },
        { Rules::_for, 39 },
        { Rules::_returnConst, 39 },
        { Rules::_return, 39 },
        { Rules::_print, 39 },
        { Rules::_printVar, 39 },
        { Rules::_exit, 39 },
        { Rules::_assignConst, 39 },
        { Rules::_assign, 39 },
        { Rules::_functionZero, 38 },
        { Rules::_functionOne, 38 },
        { Rules::_functionTwo, 38 },
        { Rules::_functionThree, 38 },
        { Rules::_functionFour, 38 },
        { Rules::_command, 39 },
        { Rules::_lastCommand, 39 },
    };

        /* Funções */
    void matchMaximalMunch(Tree&);
    void matchMinimalMunch(Tree&);

}

#endif
