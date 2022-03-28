#include"Rule.hpp"

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


Rule::Rule(){}
Rule::Rule(string name, vector<Pattern> patterns, Tree root, int replace, code_t cost):
    name{ name },
    tree_pattern{ root },
    patterns{ patterns },
    replace{ replace },
    cost{ cost }
    {}


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
