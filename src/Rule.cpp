#include"Rule.hpp"

    /* Patern */
Pattern::Pattern(string name, vector<string> variables): name{name},variables{variables}{}
ostream& operator<<(ostream& out, Pattern& s){
    out << s.name;
    out << "(";
    for( auto v: s.variables ){ 
        out << v; 
        out << ", "; 
    }
    out << ")";
    return out;
}

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


        /* Overload de operadores */
ostream& operator<<(ostream& out, Rule& r){
    out << "Rule: " << r.name << '\n';
    out << "Tree: " << r.tree_pattern << '\n';
    out << "Patterns: ";
    for( auto p: r.patterns ){
        out << "\n " << p;
    }
    out << "\nReplace with pattern " << r.replace << '\n';
    out << "Cost: " << r.cost;
    return out;
}
