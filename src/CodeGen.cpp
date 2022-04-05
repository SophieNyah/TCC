#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include"CodeGen.hpp"
#include"Helper.hpp"
#include"Tree.hpp"
#include"Rule.hpp"
#include"types.hpp"

using namespace std;

namespace{

        /* Aliases */
    using table_t = vector<vector<int>>;


        /* Variáveis locais */
    fstream out_file{ "saida.cpp" };
    map<string, int> nonTerms{};
    vector<pair<int, pair<int, code_t>>> final_states{};


        /* Funções locais */
    void generateNonTermsMap(){
        MyArray<string> nt = Helper::getNonTerms();
        for( int i=0; i<nt.size(); i++ ) nonTerms.insert(pair<string, int>{ nt.at(i), i });
    }
    string printTab(int n){
        string str{""};
        for(int i=0; i<n; i++){
            str+="    ";
        }
        return str;
    }
    void printHeader(){
        /* Header do usuário */
        out_file << Helper::getHeader() << '\n';

        /* Includes */
        out_file << "#include\"./src/Tree.hpp\"\n\n";

        /* Namespace para evitar conflite de nomes */
        out_file << "namespace Code_Generator{\n\n";

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
        MyArray<string> nt{ Helper::getNonTerms() };
        for( auto nt: nonTerms ){
            out_file << printTab(1) << nt.first << " = " << nt.second << ",\n";
        }
        out_file << "};\n\n";
    }
    void printFooter(){
        /* Fecha o namespace */
        out_file << "}\n\n";

        /* Código do usuário */
        out_file << '\n' << Helper::getCode() << '\n';
    }

    void traverseTree(table_t* table, int& state, Tree& tree){
        MyArray<Tree> children = tree.getChildren();
        table_t tab = *table;
        int column{ tree.getType() == operacao  ?  nonTerms[tree.getName()]  :  (int)nonTerms.size() + tree.getType() - 1 };

        if( (*table)[state][column] == 0 ){
            table->push_back(vector<int>( (int)nonTerms.size() + 3, 0 ));
            int newState = table->size() - 1;
            (*table)[state][column] = newState;
            state = newState;
        }else{
            state = (*table)[state][column];
        }

        for( Tree t: children ){
            traverseTree(table, state, t);
        }
    }
    table_t* generateTable(MyArray<Rule>& rules){
            /* As colunas representam os não-terminais, seguidos dos Node_type registrador, constante, e especifico */
        table_t *table{ new table_t(1, vector<int>( (int)nonTerms.size() + 3, 0 )) };

        for( Rule r: rules ){
            Tree tree{ r.getPattern() };

            int state{ 0 };
            traverseTree(table, state, tree);

            final_states.push_back(pair<int, pair<int, code_t>>{ state, pair<int, code_t>{ r.getRuleNumber(), r.getCost() } });
        }

        return table;
    }
    void printTableAndFinalStates(table_t* table){
        int rows{ (int)table->size() };
        int columns{ (int)(*table)[0].size() };

        out_file << "int recognition_table[" << rows << "][" << columns << "]{\n";
        
        for( int r=0; r<rows; r++ ){
            out_file << printTab(1) << "{" << (*table)[r][0];
            for( int c=1; c<columns; c++ ){
                out_file << ", " << (*table)[r][c];
            }
            out_file << "},\n";
        }

        out_file << "};\n\n";


        out_file << "MyPair isFinalState(int state){\n";
        out_file << printTab(1) << "switch(state){\n";

        for( auto f: final_states ){
            out_file << printTab(2) << "case " << f.first << ": return MyPair{" << f.second.first << ", " << f.second.second << "};\n";
        }
        out_file << printTab(2) << "default: return MyPair{-1, -1};\n";

        out_file << printTab(1) << "}\n";
        out_file << "}\n\n";
    }

}


    /* Função principal */
void CodeGenerator::generate(){
    generateNonTermsMap();

    printHeader();

    MyArray<Rule> rules{ Helper::getRules() };
    table_t *table = generateTable(rules);
    printTableAndFinalStates(table);

    printFooter();

    delete(table);
}
