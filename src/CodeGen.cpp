#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<memory>
#include<iomanip>
#include<algorithm>
#include<regex>
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
    map<string, int> user_symbols{};
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
        int i=0;
        MyArray<string> terms = Helper::getTerms();
        for( ; i<terms.size(); i++ ) user_symbols.insert(pair<string, int>{ terms.at(i), i });

        MyArray<string> nonterms = Helper::getNonTerms();
        for( int j=0; j<nonterms.size(); j++, i++ ) { user_symbols.insert(pair<string, int>{ nonterms.at(j), i }); }
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
        out_file << "#include<map>\n"
                 << "#include<limits>\n"
                 << "#include\"./src/Tree.hpp\"\n\n";


        /* Enum de regras */
        out_file << "enum class Rules {\n"
                 << "    null = -1,\n";
        for( Rule r: Helper::getRules() ){
            out_file << printTab(1) << r.getName() << " = " << r.getRuleNumber() << ",\n";
        }
        out_file << "};\n\n";


        /* map de (regra, não-terminal) */
        out_file << "const std::map<Rules, int> Rules_Non_Terminals{\n";
        for( Rule r: Helper::getRules() ){
            int num{ user_symbols.at(r.getSymbol()) };
            out_file << printTab(1) << "{ Rules::" << r.getName() << ", " << num << " },\n";
        }
        out_file << "};\n\n";


        /* Variáveis de delimitação do enum */
        out_file << "const int TERMINALS_START     = " << 0 << ";\n";
        out_file << "const int TERMINALS_END       = " << Helper::getTerms().size() - 1 << ";\n";
        out_file << "const int NON_TERMINALS_START = " << Helper::getTerms().size() << ";\n";
        out_file << "const int NON_TERMINALS_END   = " << user_symbols.size() - 1 << ";\n";

        /* Enum de não-terminais */
        out_file << "enum class User_Symbols {\n";
        out_file << printTab(1) << "null = -1,\n";
        for( auto symbol: user_symbols ){
            out_file << printTab(1) << symbol.first << " = " << symbol.second << ",\n";
        }
        out_file << "};\n\n";
        

        /* Header do usuário */
        out_file << Helper::getHeader() << "\n\n";


        /* Abre o namespace */
        out_file << "namespace Code_Generator{\n\n";


        /* Aliases de tipos */
        out_file << "    using cost_t = int;\n"
                 << "    using rule_number_t = Rules;\n"
                 << "    using MyPair = std::pair<rule_number_t, cost_t>;\n"
                 << '\n'
                 << "    const int infinity{ std::numeric_limits<int>::max() };\n\n";

        /* Declarações de funções */
        out_file << "    cost_t cost(Rules, Tree);\n\n";
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
        int column{ user_symbols[tree.getName()] };

        if( (*table)[state][column] == 0 ){
            table->push_back(vector<int>( (int)user_symbols.size(), 0 ));
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
        unique_ptr<table_t> table{ new table_t(1, vector<int>( (int)user_symbols.size(), 0 )) };

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

        out_file << printTab(1) << "const int recognition_table[" << rows << "][" << columns << "]{\n";
        
        for( int r=0; r<rows; r++ ){
            out_file << printTab(2) << "{" << std::setw(4) << (*table)[r][0];
            for( int c=1; c<columns; c++ ){
                out_file << ", " << std::setw(4) << (*table)[r][c];
            }
            out_file << "},\n";
        }

        out_file << printTab(1) << "};\n\n";


        out_file << printTab(1) << "MyPair isFinalState(int state, Tree t={}){\n";
        out_file << printTab(2) << "switch(state){\n";

        for( auto f: final_states ){
            out_file << printTab(3) << "case " << std::setw(4) << f.first << ": return MyPair{ Rules::"
                << f.second.first << ", cost(Rules::" << f.second.first << ", t) };\n";
        }
        out_file << printTab(3) << "default: return MyPair{ Rules::null, -1 };\n";

        out_file << printTab(2) << "}\n";
        out_file << printTab(1) << "}\n\n";
    }
    void printCost(){
        out_file << "    int cost(Rules r, Tree t){\n"
                 << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            string costStr = r.getCost();
            costStr = regex_replace( costStr, regex( "\\$cost\\[" ), "t.getChildCost(" );
            costStr = regex_replace( costStr, regex( "\\]" ), ")" );

            out_file << printTab(3) << "case Rules::" << r.getName() << " :\n"
                     << printTab(4) << costStr << '\n'
                     << printTab(4) << "break;\n";
        }
        out_file << "            default:\n"
                 << "                t.getChildCost(0) = infinity;\n"
                 << "                return infinity;\n"
                 << "        }\n"
                 << "    }\n\n";
    }
    void printMatchTree(){
        out_file
        << "    using SymbolArray = std::vector<int>; \n"
        << "    using StateArray  = std::vector<int>; \n\n"

        << "    StateArray matchTree(Tree& t, StateArray start_states=StateArray{0}){\n"
        << "        SymbolArray symbols{ (int)t.getSymbol() };\n"
        << "        for( int rule: t.non_terminal ) symbols.push_back(rule);\n"
        << "        StateArray states{};\n"
        << "        for( int symbol: symbols ){\n"
        << "            for( int state: start_states ){\n"
        << "                state = recognition_table[state][symbol];\n"
        << "                if( state != 0 ) states.push_back(state);\n"
        << "            }\n"
        << "        }\n"
        << "        for( Tree c: t.getChildren() ){\n"
        << "            StateArray newStates{ matchTree(c, states) };\n"
        << "            if(newStates.empty()){\n"
        << "                return states;\n"
        << "            }else{\n"
        << "                states = newStates;\n"
        << "            }\n"
        << "        }\n"
        << "        return states;\n"
        << "    }\n\n";
    }
    void printLabel(){
        out_file
        << "    void label(Tree& t){\n"
        << "        for( int i=0; i<t.children_size; i++ ){\n"
        << "            Tree& c{ t.getChildReference(i) };\n"
        << "            label(c);\n"
        << "        }\n"
        << "        for( int state: matchTree(t) ){\n"
        << "            MyPair s{ isFinalState(state, t) };\n"
        // << "            std::cout << "Node " << t.getName() << ", Rule " << (int)s.first << ": " << s.second << endl;\n"
        << "            if( s.second <= t.matched_rules.at(0).second ){\n"
        << "                t.matched_rules.insert(t.matched_rules.begin(), s);\n"
        << "            }else{\n"
        << "                t.matched_rules.push_back(s);\n"
        << "            }\n"
        << "            t.non_terminal.push_back(Rules_Non_Terminals.at(s.first));\n"
        << "        }\n"
        << "    }\n\n";
    }
    void printAction(){
        out_file << "    int action(Rules r){\n"
                 << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            out_file << printTab(3) << "case Rules::" << r.getName() << " :\n"
                     << printTab(4) << r.getAction() << '\n'
                     << printTab(4) << "break;\n";
        }
        out_file << "            default: break;\n"
                 << "        }\n"
                 << "    }\n\n";
    }

}


    /* Função principal */
void CodeGenerator::generate(){
    generateNonTermsMap();

    startHeader();

    MyArray<Rule> rules{ Helper::getRules() };
    unique_ptr<table_t> table = generateTable(rules);

    printTableAndFinalStates(table.get());
    printCost();
    printMatchTree();
    printLabel();
    printAction();

    endHeaderStartUserCode();
}
