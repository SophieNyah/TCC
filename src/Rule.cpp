#include"Rule.hpp"

    /* Patern */
Pattern::Pattern(string name, vector<string> variables): name{name},variables{variables}{}

    /* Rule */

        /* Membros estáticos */
int Rule::rules_quantity{ 0 };


        /* Construtores */
Rule::Rule(){}
Rule::Rule(string name, vector<Pattern> patterns, BasicTree root, string user_symbol, code_t action, int replace, code_t cost):
    name{ name },
    tree_pattern{ root },
    patterns{ patterns },
    user_symbol{ user_symbol },
    action{ action },
    replace{ replace },
    cost{ cost }
    {
        this->rule_no = Rule::rules_quantity;
        Rule::rules_quantity++;
    }


        /* Getters e Setter */
int    Rule::getRuleNumber(){ return this->rule_no; }
string Rule::getName()      { return this->name; }
string Rule::getSymbol()    { return this->user_symbol; }
BasicTree& Rule::getPattern(){ return this->tree_pattern; }
code_t Rule::getCost()      { return this->cost; }
code_t Rule::getAction()    { return this->action; }
void Rule::setAction(code_t action){ this->action = action; }
