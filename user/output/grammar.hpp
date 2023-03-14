#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

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
        _equals = 8,
        _if = 9,
        _ifElse = 10,
        _return = 11,
        _assign = 12,
        _function = 13,
        _command = 14,
        _lastCommand = 15,
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
        EQUALS = 31,
        EXIT = 9,
        FOR = 3,
        FUNCTION_CALL = 8,
        GEQ = 36,
        GREAT = 34,
        IF = 4,
        INC = 21,
        LEQ = 35,
        LESS = 33,
        LOG_AND = 28,
        LOG_NOT = 30,
        LOG_OR = 29,
        LSHIFT = 20,
        MOD = 18,
        MUL = 17,
        NOT_EQUALS = 32,
        PRINTF = 11,
        RETURN = 1,
        RSHIFT = 19,
        SCANF = 10,
        STRING = 6,
        SUB = 15,
        SWITCH = 12,
        UNR_MINUS = 24,
        UNR_PLUS = 23,
        VARIABLE = 7,
        WHILE = 2,
        reg = 37,
        stmt = 38,
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
    const int TERMINALS_END       = 36;
    const int NON_TERMINALS_START = 37;
    const int NON_TERMINALS_END   = 38;
    const std::map<Rules, int> Rules_Non_Terminals{
        { Rules::_var, 37 },
        { Rules::_const, 37 },
        { Rules::_statement, 38 },
        { Rules::_addConst, 37 },
        { Rules::_addReg, 37 },
        { Rules::_subConst, 37 },
        { Rules::_subReg, 37 },
        { Rules::_mul, 37 },
        { Rules::_equals, 37 },
        { Rules::_if, 38 },
        { Rules::_ifElse, 38 },
        { Rules::_return, 38 },
        { Rules::_assign, 38 },
        { Rules::_function, 37 },
        { Rules::_command, 38 },
        { Rules::_lastCommand, 38 },
    };

        /* Funções */
    void matchDynamicProgramming(Tree&);
    void matchMaximalMunch(Tree&);
    void matchMinimalMunch(Tree&);

}

#endif
