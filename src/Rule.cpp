#include"Rule.hpp"

    /* Patern */
Pattern::Pattern(string name, vector<string> variables): name{name},variables{variables}{}

    /* Rule */

        /* Membros estáticos */
int Rule::rules_quantity{ 0 };


        /* Construtores */
Rule::Rule(){}
Rule::Rule(string name, vector<Pattern> patterns, BasicTree root, int replace, code_t cost):
    name{ name },
    tree_pattern{ root },
    patterns{ patterns },
    replace{ replace },
    cost{ cost }
    {
        this->rule_no = Rule::rules_quantity;
        Rule::rules_quantity++;
    }


        /* Getters e Setter */
string Rule::getName()      { return this->name; }
int    Rule::getRuleNumber(){ return this->rule_no; }
BasicTree   Rule::getPattern()   { return this->tree_pattern; }
code_t Rule::getCost()      { return this->cost; }
