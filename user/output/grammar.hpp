#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include<map>
#include<vector>

namespace Yamg{

        /* Classe do usuário */
    class Tree;

        /* Enums */
    enum class Rules {
        null = -1,
        registlamoi = 0,
    };

    enum class User_Symbols {
        null = -1,
        ADD = 4,
        ASSIGN = 24,
        BIT_AND = 11,
        BIT_OR = 10,
        BIT_XOR = 12,
        CONST = 31,
        DEC = 9,
        DIV = 6,
        EQUALS = 16,
        EXIT = 27,
        FOR = 2,
        GEQ = 21,
        GREAT = 19,
        IF = 3,
        INC = 8,
        LEQ = 20,
        LESS = 18,
        LOG_AND = 13,
        LOG_NOT = 15,
        LOG_OR = 14,
        LSHIFT = 23,
        MOD = 7,
        NOT_EQUALS = 17,
        PRINTF = 25,
        RETURN = 0,
        RSHIFT = 22,
        SCANF = 26,
        SUB = 5,
        SWITCH = 30,
        UNR_MINUS = 29,
        UNR_PLUS = 28,
        WHILE = 1,
        reg = 32,
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
    const int TERMINALS_END       = 31;
    const int NON_TERMINALS_START = 32;
    const int NON_TERMINALS_END   = 32;
    const std::map<Rules, int> Rules_Non_Terminals{
        { Rules::registlamoi, 32 },
    };

        /* Funções */
    // MyPair     isFinalState(int state, Tree=Tree());
    StateArray matchTree(Tree&, StateArray={0});
    int    action(Rules r, Tree &t);
    cost_t cost(Rules, Tree);
    void   generateCode(Tree&);

}

#endif
