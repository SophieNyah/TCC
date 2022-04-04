#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"CodeGen.hpp"
#include"Helper.hpp"
#include"Tree.hpp"
#include"Rule.hpp"
#include"types.hpp"

using namespace std;
fstream out_file{ "saida.cpp" };

string printTab(int n){
    string str{""};
    for(int i=0; i<n; i++){
        str+="    ";
    }
    return str;
}

void printHeader(){
    /* Includes */
    out_file << "#include\"./src/Tree.hpp\"\n\n";

    /* Aliases de tipos */
    out_file << "using cost_t = int;\n";
    out_file << "using rule_number_t = int;\n";
    out_file << "using MyPair = std::pair<rule_number_t, cost_t>;\n\n";

    /* Enum de regras */
    out_file << "enum Rules {\n";
    for( Rule r: Helper::getRules() ){
        out_file << printTab(1) << r.getName() << " = " << r.getRuleNumber() << ",\n";
    }
    out_file << "};\n\n";

    /* Enum de não-terminais */
    out_file << "enum Non_terminals{\n";
    array<string> nt{ Helper::getNonTerms() };
    for( auto it=nt.begin(); it!=nt.end(); it++ ){
        out_file << printTab(1) << *it << " = " << it - nt.begin() << ",\n";
    }
    out_file << "};\n\n";
}


void generateRuleCondition(Tree t, int& tabs){
    int children_size = t.getChildren().size();

        /* if */
    switch ( t.getChild(0).value().getType() ){
        // default: out_file << printTab(tabs) << "if(t0.getNonTerm() == " << t.getChild(0).value().getName(); break;
        case operacao:
            out_file << printTab(tabs) << "if(t0.getNonTerm() == Non_terminals::" << t.getChild(0).value().getName();
            break;
        case registrador:
            out_file << printTab(tabs) << "if(t0.getType() == Node_type::registrador";
            break;
        case constante:
            out_file << printTab(tabs) << "if(t0.getType() == Node_type::constante";
            break;
        case especifico:
            out_file << printTab(tabs) << "if(t0.getType() == Node_type::especifico";
            break;
    }
    for(int i=1; i<children_size; i++){
        switch ( t.getChild(i).value().getType() ){
            // default: out_file << " && t" << i << ".getNonTerm() == " << t.getChild(i).value().getName(); break;
            case operacao:
                out_file << " && t" << i << ".getNonTerm() == Non_terminals::" << t.getChild(i).value().getName();
                break;
            case registrador:
                out_file << " && t" << i << ".getType() == Node_type::registrador";
                break;
            case constante:
                out_file << " && t" << i << ".getType() == Node_type::constante";
                break;
            case especifico:
                out_file << " && t" << i << ".getType() == Node_type::especifico";
                break;
        }
    }
    out_file << "){\n";
    tabs++;

        /* comparação recursiva */
    for(int i=0; i<children_size; i++){

        int c_size = t.getChild(i).value().getChildren().size();
            /* novas variáveis de árvore */
        if( i==0 && c_size > 0 ){
            for(int i=0; i<children_size; i++){
                out_file << printTab(tabs) << "Tree t" << i << "{ t.getChild(" << i << ").value() };\n";
            }
            out_file << '\n';
        }

        if(c_size > 0){
            generateRuleCondition(t.getChild(i).value(), tabs);
        }

        if(i == children_size-1 && c_size == 0){
            out_file << printTab(tabs) << "mp.first  = 1;\n";
            out_file << printTab(tabs) << "mp.second = 2;\n";
        }

    }

        /* else */
    tabs--;
    out_file << printTab(tabs) << "} else {\n";
    tabs++;
    out_file << printTab(tabs) << "mp.first  = -1;\n";
    out_file << printTab(tabs) << "mp.second = -1;\n";
    tabs --;
    out_file << printTab(tabs) << "}\n\n";
}


void generateRule(Rule& r){
    Tree t = r.getPattern();
    int tabs = 0;

        /* header da função */
    out_file << "MyPair match_" << t.getName() << "(Tree& t){\n\n";
    tabs++;

            /* variáveis da função */
        out_file << printTab(tabs) << "MyPair mp{-1, -1};\n";
        for(int i=0; i<t.getChildren().size(); i++){
            out_file << printTab(tabs) << "Tree t" << i << "{ t.getChild(" << i << ").value() };\n";
        }
        out_file << '\n';

        generateRuleCondition(t, tabs);

            /* retorno da função */
        out_file << printTab(tabs) << "return mp;\n";

        /* fim do escopo da função */
    tabs--;
    out_file << "}\n\n";
}

void CodeGenerator::generate(){
    vector<Rule> rules{ Helper::getRules() };

    printHeader();

    for( int i=0; i<rules.size(); i++ ){
        Rule r = rules.at(i);
        generateRule(r);
    }

    out_file << '\n' << Helper::getCode() << '\n';
}
