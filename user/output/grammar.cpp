#include<map>
#include<limits>
#include"grammar.hpp"
#include"../../src/RegAlloc.hpp"
#include"../../src/Tree.hpp"

#include"../src/Tree.h"

namespace Yamg{

    using cost_t = int;
    using rule_number_t = Rules;
    using MyPair = std::pair<rule_number_t, cost_t>;

    const int infinity{ std::numeric_limits<int>::max() };

    cost_t cost(Rules, Tree);

    const int recognition_table[4][33]{
        {   0,    0,    0,    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    2},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,    0},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    };

    MyPair isFinalState(int state, Tree t){
        switch(state){
            case    3: return MyPair{ Rules::registlamoi, cost(Rules::registlamoi, t) };
            default: return MyPair{ Rules::null, -1 };
        }
    }

    bool isFinalState(int state){
        switch(state){
            case    3: return true;
            default: return false;
        }
    }

    int cost(Rules r, Tree t){
        switch(r){
            case Rules::registlamoi :
                
                break;
            default:
                t.getChildCost(0) = infinity;
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

    void _label(Tree& t){
        for( int i=0; i<t.children_size; i++ ){
            Tree& c{ t.getChildReference(i) };
            _label(c);
        }
        StateArray final_states{};
        matchTree(t, final_states);
        for( int state: final_states ){
            MyPair s{ isFinalState(state, t) };
            if( s.second <= t.matched_rules.at(0).second ){
                t.matched_rules.insert(t.matched_rules.begin(), s);
            }else{
                t.matched_rules.push_back(s);
            }
            t.non_terminal.push_back(Rules_Non_Terminals.at(s.first));
        }
    }

    int action(Rules r, Tree &t){
        switch(r){
            case Rules::registlamoi :
                {}
                break;
            default: break;
        }
        return 0;    }

    using RuleLimit_t = std::pair<int, User_Symbols>;
    const map<Rules, std::vector<RuleLimit_t>> RulesLimitsMap{
        { Rules::registlamoi,
            {
                RuleLimit_t{ 1, User_Symbols::reg },
            },
        },
    };

    void _reduce(Tree&);
    void _reduceAux(Tree& t, std::vector<RuleLimit_t>& limit, int& count){
        if( count == limit.at(0).first ){
            limit.erase(limit.begin());
            if( count != 0 ) _reduce(t);
            return;
        }

        for( Tree c: t.getChildren() ){
            count++;
            _reduceAux(c, limit, count);
            if( limit.empty() ) break;
        }
    }

    void _reduce(Tree& t){
        Rules r{ t.matched_rules.at(0).first };
        vector<RuleLimit_t> limit{ RulesLimitsMap.at(r) };
        int count{ 0 };
        _reduceAux(t, limit, count);
        action(r, t);
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

    void generateCode(Tree &t) {
        _setRegisters();
        _label(t);
        _reduce(t);
    }

}



