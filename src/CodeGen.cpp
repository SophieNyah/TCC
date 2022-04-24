#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<memory>
#include"CodeGen.hpp"
#include"Helper.hpp"
#include"Tree.hpp"
#include"Rule.hpp"
#include"types.hpp"

using namespace std;

namespace{

        /* Aliases */
    using table_t = vector<vector<int>>;
    using state_data_t = pair<string, code_t>;
    using final_state_t = pair<int, state_data_t>;


        /* Variáveis locais */
    fstream out_file{ "saida.cpp" };
    map<string, int> nonTerms{};
    vector<final_state_t> final_states{};


        /* Funções locais */

    int calculateCost(BasicTree& tree){
        int cost = 0 + tree.cost.integer_part;
        for( int i: tree.cost.cost_directives ){
            BasicTree t = tree.getChild(i).value();
            cost += calculateCost(t);
        }
    }

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
    
    void startHeader(){
        /* Includes */
        out_file << "#include\"./src/Tree.hpp\"\n\n";

        /* Enum de regras */
        out_file << "enum class Rules {\n";
        out_file << printTab(1) << "null = -1,\n";
        for( Rule r: Helper::getRules() ){
            out_file << printTab(1) << r.getName() << " = " << r.getRuleNumber() << ",\n";
        }
        out_file << "};\n\n";

        /* Enum de não-terminais */
        out_file << "enum class Non_terminals {\n";
        out_file << printTab(1) << "null = -1,\n";
        for( auto nt: nonTerms ){
            out_file << printTab(1) << nt.first << " = " << nt.second << ",\n";
        }
        out_file << "};\n\n";
        
        /* Header do usuário */
        out_file << Helper::getHeader() << "\n\n";

        /* Abre o namespace */
        out_file << "namespace Code_Generator{\n\n";

        /* Aliases de tipos */
        out_file << printTab(1) << "using cost_t = int;\n";
        out_file << printTab(1) << "using rule_number_t = Rules;\n";
        out_file << printTab(1) << "using MyPair = std::pair<rule_number_t, cost_t>;\n\n";

        /* Função do cálculo do custo */
        // out_file << "int calculateCost(Tree& tree){\n";
        // out_file << printTab(1) << "int cost = 0 + tree.cost.integer_part;\n";
        // out_file << printTab(1) << "for( int i: tree.cost.cost_directives ){\n";
        // out_file << printTab(2) << "Tree t = tree.getChild(i).value();\n";
        // out_file << printTab(2) << "cost += calculateCost(t);\n";
        // out_file << printTab(1) << "}\n";
        // out_file << "}\n\n";
    }
    
    void endHeaderStartUserCode(){
        /* Fecha o namespace */
        out_file << "}\n\n";

        /* Código do usuário */
        out_file << '\n' << Helper::getCode() << '\n';
    }


    void traverseTree(table_t* table, int& state, BasicTree& tree){
        MyArray<BasicTree> children = tree.getChildren();
        table_t tab = *table;
        int column{ tree.getType() == Node_type::operacao  ?  nonTerms[tree.getName()]  :  (int)nonTerms.size() + (int)tree.getType() - 1 };

        if( (*table)[state][column] == 0 ){
            table->push_back(vector<int>( (int)nonTerms.size() + 3, 0 ));
            int newState = table->size() - 1;
            (*table)[state][column] = newState;
            state = newState;
        }else{
            state = (*table)[state][column];
        }

        for( BasicTree t: children ){
            traverseTree(table, state, t);
        }
    }
    
    unique_ptr<table_t> generateTable(MyArray<Rule>& rules){
            /* As colunas representam os não-terminais, seguidos dos Node_type registrador, constante, e especifico */
        unique_ptr<table_t> table{ new table_t(1, vector<int>( (int)nonTerms.size() + 3, 0 )) };

        for( Rule r: rules ){
            BasicTree tree{ r.getPattern() };

            int state{ 0 };
            traverseTree(table.get(), state, tree);

            final_states.push_back(final_state_t{ state, state_data_t{ r.getName(), r.getCost() } });
        }

        return table;
    }
    
    void printTableAndFinalStates(table_t* table){
        int rows{ (int)table->size() };
        int columns{ (int)(*table)[0].size() };

        out_file << printTab(1) << "int recognition_table[" << rows << "][" << columns << "]{\n";
        
        for( int r=0; r<rows; r++ ){
            out_file << printTab(2) << "{" << (*table)[r][0];
            for( int c=1; c<columns; c++ ){
                out_file << ", " << (*table)[r][c];
            }
            out_file << "},\n";
        }

        out_file << printTab(1) << "};\n\n";


        out_file << printTab(1) << "MyPair isFinalState(int state){\n";
        out_file << printTab(2) << "switch(state){\n";

        for( auto f: final_states ){
            out_file << printTab(3) << "case " << f.first << ": return MyPair{ Rules::"
                << f.second.first << ", 3 };\n";
        }
        out_file << printTab(3) << "default: return MyPair{ Rules::null, -1 };\n";

        out_file << printTab(2) << "}\n";
        out_file << printTab(1) << "}\n\n";
    }

    void printMatchTree(){
        int t = 1;
        out_file << printTab(t) << "int matchTree(Tree& t, int start_state=0){\n";            t++;
        out_file << printTab(t) << "int node_type = t.getType() == Node_type::operacao ? t.getNonTerm() : 4 + (int)t.getType();\n";
        out_file << printTab(t) << "int node_type = (int)t.getType();\n";
        out_file << printTab(t) << "int state = recognition_table[start_state][node_type];\n";
        out_file << printTab(t) << "if(state == 0){\n";                                       t++;
        out_file << printTab(t) << "return false;\n";                                         t--;
        out_file << printTab(t) << "}else{\n";                                                t++;
        out_file << printTab(t) << "if(t.children_size > 0){\n";                              t++;
        out_file << printTab(t) << "vector<Tree> children{ t.getChildren() };\n";
        out_file << printTab(t) << "for( Tree c: children ){\n";                              t++;
        out_file << printTab(t) << "state = matchTree(c, state);\n";                          
        out_file << printTab(t) << "if(state == 0) return 0;\n";                              t--;
        out_file << printTab(t) << "}\n";                                                     t--;
        out_file << printTab(t) << "}\n";                                                     t--;
        out_file << printTab(t) << "}\n";                                                     
        out_file << printTab(t) << "return state;\n";                                         t--;
        out_file << printTab(t) << "}\n";
    }

}


    /* Função principal */
void CodeGenerator::generate(){
    generateNonTermsMap();

    startHeader();

    MyArray<Rule> rules{ Helper::getRules() };
    unique_ptr<table_t> table = generateTable(rules);
    printTableAndFinalStates(table.get());

    printMatchTree();

    endHeaderStartUserCode();
}
