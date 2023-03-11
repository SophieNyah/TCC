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
        registlamoi = 0,
    };

    enum class User_Symbols {
        null = -1,
        ADD = 5,
        ASSIGN = 25,
        BIT_AND = 12,
        BIT_OR = 11,
        BIT_XOR = 13,
        COMMAND = 0,
        CONST = 32,
        DEC = 10,
        DIV = 7,
        EQUALS = 17,
        EXIT = 28,
        FOR = 3,
        GEQ = 22,
        GREAT = 20,
        IF = 4,
        INC = 9,
        LEQ = 21,
        LESS = 19,
        LOG_AND = 14,
        LOG_NOT = 16,
        LOG_OR = 15,
        LSHIFT = 24,
        MOD = 8,
        NOT_EQUALS = 18,
        PRINTF = 26,
        RETURN = 1,
        RSHIFT = 23,
        SCANF = 27,
        SUB = 6,
        SWITCH = 31,
        UNR_MINUS = 30,
        UNR_PLUS = 29,
        WHILE = 2,
        reg = 33,
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
    const int TERMINALS_END       = 32;
    const int NON_TERMINALS_START = 33;
    const int NON_TERMINALS_END   = 33;
    const std::map<Rules, int> Rules_Non_Terminals{
        { Rules::registlamoi, 33 },
    };

        /* Funções */
    // MyPair     isFinalState(int state, Tree=Tree());
    void matchDynamicProgramming(Tree&);
    void matchMaximalMunch(Tree&);
    void matchMinimalMunch(Tree&);

}

#endif
