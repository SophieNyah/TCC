#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"CodeGen.hpp"
#include"Helper.hpp"
#include"Tree.hpp"
#include"Rule.hpp"

using namespace std;
fstream out_file{ "saida.cpp" };

string printTab(int n){
    string str{""};
    for(int i=0; i<n; i++){
        str+="    ";
    }
    return str;
}


void generateRuleCondition(Tree t, int& tabs){
    int children_size = t.getChildren().size();
    for(int i=0; i<children_size; i++){
        out_file << printTab(tabs) << "Tree t" << i << "{ t.getChild(" << i << ").value() };\n";
    }

    out_file << printTab(tabs) << "if(t0.getName() == \"" << t.getChild(0).value().getName() << "\"";
    for(int i=1; i<children_size; i++){
        out_file << " && t" << i << ".getName() == \"" << t.getChild(i).value().getName() << "\"";
    }
    out_file << "){\n";
    tabs++;

    for(int i=0; i<children_size; i++){
        if(t.getChild(i).value().getChildren().size() > 0){
            generateRuleCondition(t.getChild(i).value(), tabs);
        }
    }

    tabs--;
    out_file << printTab(tabs) << "}\n";
}


void generateRule(Rule& r){
    Tree t = r.tree_pattern;
    int tabs = 0;

    out_file << "MyPair match_" << t.getName() << "(Tree& t){\n";
    tabs++;

        out_file << printTab(tabs) << "if(t.getName() == \"" << t.getName() << "\"){\n";
        tabs++;

            generateRuleCondition(t, tabs);
        
        tabs--;
        out_file << printTab(tabs) << "}\n";

    tabs--;
    out_file << "}\n\n";
}

void CodeGenerator::generate(){
    vector<Rule> rules{ Helper::getRules() };

    out_file << "#include\"Tree.hpp\"\n\n";

    out_file << "using cost_t = int;\n";
    out_file << "using rule_number_t = int;\n";
    out_file << "using MyPair = std::pair<rule_number_t, cost_t>;\n\n";

    for( int i=0; i<rules.size(); i++ ){
        Rule r = rules.at(i);
        generateRule(r);
    }
}
