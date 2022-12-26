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
    fstream out_file_cpp;
    fstream out_file_h;
    map<string, int> user_symbols{};
    vector<final_state_t> final_states{};


        /* Funções locais */

    // int calculateCost(BasicTree& tree){
    //     int cost = 0 + tree.cost.integer_part;
    //     for( int i: tree.cost.cost_directives ){
    //         BasicTree t = tree.getChild(i).value();
    //         cost += calculateCost(t);
    //     }
    //     return cost;
    // }

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
    
    void startHeader(string output_file_name){
        string include_name{};
        output_file_name = output_file_name.substr(output_file_name.find_last_of("/") + 1);
        include_name = output_file_name;
        
        /* Includes */
        out_file_cpp << "#include<map>\n"
                 << "#include<limits>\n"
                 << "#include\"" << include_name << ".hpp\"\n"
                 << "#include\"../../src/Tree.hpp\"\n\n";


        /* Enum de regras */
        out_file_cpp << "enum class Rules {\n"
                 << "    null = -1,\n";
        for( Rule r: Helper::getRules() ){
            out_file_cpp << printTab(1) << r.getName() << " = " << r.getRuleNumber() << ",\n";
        }
        out_file_cpp << "};\n\n";


        /* map de (regra, não-terminal) */
        out_file_cpp << "const std::map<Rules, int> Rules_Non_Terminals{\n";
        for( Rule r: Helper::getRules() ){
            int num{ user_symbols.at(r.getSymbol()) };
            out_file_cpp << printTab(1) << "{ Rules::" << r.getName() << ", " << num << " },\n";
        }
        out_file_cpp << "};\n\n";


        /* Variáveis de delimitação do enum */
        out_file_cpp << "const int TERMINALS_START     = " << 0 << ";\n";
        out_file_cpp << "const int TERMINALS_END       = " << Helper::getTerms().size() - 1 << ";\n";
        out_file_cpp << "const int NON_TERMINALS_START = " << Helper::getTerms().size() << ";\n";
        out_file_cpp << "const int NON_TERMINALS_END   = " << user_symbols.size() - 1 << ";\n";

        /* Enum de não-terminais */
        out_file_cpp << "enum class User_Symbols {\n";
        out_file_cpp << printTab(1) << "null = -1,\n";
        for( auto symbol: user_symbols ){
            out_file_cpp << printTab(1) << symbol.first << " = " << symbol.second << ",\n";
        }
        out_file_cpp << "};\n\n";
        

        /* Header do usuário */
        out_file_cpp << Helper::getHeader() << "\n\n";


        /* Abre o namespace */
        out_file_cpp << "namespace Code_Generator{\n\n";


        /* Aliases de tipos */
        out_file_cpp << "    using cost_t = int;\n"
                 << "    using rule_number_t = Rules;\n"
                 << "    using MyPair = std::pair<rule_number_t, cost_t>;\n"
                 << '\n'
                 << "    const int infinity{ std::numeric_limits<int>::max() };\n\n";

        /* Declarações de funções */
        out_file_cpp << "    cost_t cost(Rules, Tree);\n\n";
    }
    
    void endHeaderStartUserCode(){
        /* Fecha o namespace */
        out_file_cpp << "}\n\n";

        /* Código do usuário */
        out_file_cpp << '\n' << Helper::getCode() << '\n';
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

        out_file_cpp << "    const int recognition_table[" << rows << "][" << columns << "]{\n";
        for( int r=0; r<rows; r++ ){
            out_file_cpp << "        {" << std::setw(4) << (*table)[r][0];
            for( int c=1; c<columns; c++ ){
                out_file_cpp << ", " << std::setw(4) << (*table)[r][c];
            }
            out_file_cpp << "},\n";
        }
        out_file_cpp << "    };\n\n";


        out_file_cpp << "    MyPair isFinalState(int state, Tree t){\n"
                     << "        switch(state){\n";
        for( auto f: final_states ){
            out_file_cpp << "            case " << std::setw(4) << f.first << ": return MyPair{ Rules::"
                << f.second.first << ", cost(Rules::" << f.second.first << ", t) };\n";
        }
        out_file_cpp << "            default: return MyPair{ Rules::null, -1 };\n"
                     << "        }\n"
                     << "    }\n\n";

        out_file_cpp << "    bool isFinalState(int state){\n"
                     << "        switch(state){\n";
        for( auto f: final_states ) 
            out_file_cpp << "            case " << std::setw(4) << f.first << ": return true;\n";
        out_file_cpp << "            default: return false;\n"
                     << "        }\n"
                     << "    }\n\n";
    }
    void printCost(){
        out_file_cpp << "    int cost(Rules r, Tree t){\n"
                 << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            string costStr = r.getCost();
            costStr = regex_replace( costStr, regex( "\\$cost\\[" ), "t.getChildCost(" );
            costStr = regex_replace( costStr, regex( "\\]" ), ")" );

            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
                     << printTab(4) << costStr << '\n'
                     << printTab(4) << "break;\n";
        }
        out_file_cpp << "            default:\n"
                 << "                t.getChildCost(0) = infinity;\n"
                 << "                return infinity;\n"
                 << "        }\n"
                 << "    }\n\n";
    }
    void printMatchTree(){
        out_file_cpp
        << "    using SymbolArray = std::vector<int>; \n"
        << "    using StateArray  = std::vector<int>; \n\n"

        << "    StateArray matchTree(Tree& t, StateArray& final_states, StateArray start_states=StateArray{0}){\n"
        << "        SymbolArray symbols{ (int)t.getSymbol() };\n"
        << "        for( int rule: t.non_terminal ) symbols.push_back(rule);\n"
        << "        StateArray states{};\n"
        << "        for( int symbol: symbols ){\n"
        << "            for( int state: start_states ){\n"
        << "                state = recognition_table[state][symbol];\n"
        << "                if( state != 0 ) states.push_back(state);\n"
        << "                if( isFinalState(state) ) final_states.push_back(state);\n"
        << "            }\n"
        << "        }\n"
        << "        for( Tree c: t.getChildren() ){\n"
        << "            StateArray newStates{ matchTree(c, final_states, states) };\n"
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
        out_file_cpp
        << "    void label(Tree& t){\n"
        << "        for( int i=0; i<t.children_size; i++ ){\n"
        << "            Tree& c{ t.getChildReference(i) };\n"
        << "            label(c);\n"
        << "        }\n"
        << "        StateArray final_states{};\n"
        << "        matchTree(t, final_states);\n"
        << "        for( int state: final_states ){\n"
        << "            MyPair s{ isFinalState(state, t) };\n"
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
        out_file_cpp << "    int action(Rules r){\n"
                 << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
                     << printTab(4) << r.getAction() << '\n'
                     << printTab(4) << "break;\n";
        }
        out_file_cpp << "            default: break;\n"
                 << "        }\n"
                 << "        return 0;"
                 << "    }\n\n";
    }
    
    int printReduceAux(BasicTree& t, int count=0){
        if( Helper::isNonTerm(t.getName())  ||  (count==0 && t.children_size==0) ){
        // if( t.children_size == 0 ){
            out_file_cpp << "                RuleLimit_t{ " << count << ", User_Symbols::" << t.getName() << " },\n";
        }
        
        for( BasicTree c: t.getChildren() ){
            count = printReduceAux(c, count+1);
        }
        return count;
    }
    void printReduce(){
        out_file_cpp << "    using RuleLimit_t = std::pair<int, User_Symbols>;\n"
                 << "    const map<Rules, std::vector<RuleLimit_t>> RulesLimitsMap{\n";
                //  << "        { Rules::addReg, \n"
                //  << "            {\n"
                //  << "                Par{ 1, User_Symbols::reg },\n"
                //  << "                Par{ 2, User_Symbols::CONST },\n"
                //  << "            }\n"
                //  << "        },\n"
                //  << "        { Rules::addConst, \n"
                //  << "            {\n"
                //  << "                Par{ 1, User_Symbols::CONST },\n"
                //  << "            }\n"
                //  << "        },\n"
                //  << "        { Rules::regist, \n"
                //  << "            {\n"
                //  << "                Par{ 0, User_Symbols::CONST },\n"
                //  << "            }\n"
                //  << "        },\n"
        for( Rule r: Helper::getRules() ){
            out_file_cpp << "        { Rules::" << r.getName() << ",\n"
                     << "            {\n";
            BasicTree t{ r.getPattern() };
            printReduceAux(t);
            out_file_cpp << "            },\n"
                     << "        },\n";
        }
        out_file_cpp << "    };\n\n"

                 << "    void reduce(Tree&);\n"
                 << "    void reduceAux(Tree& t, std::vector<RuleLimit_t>& limit, int& count){\n"
                 << "        if( count == limit.at(0).first ){\n"
                 << "            limit.erase(limit.begin());\n"
                 << "            if( count != 0 ) reduce(t);\n"
                 << "            return;\n"
                 << "        }\n\n"
                 
                 << "        for( Tree c: t.getChildren() ){\n"
                 << "            count++;\n"
                 << "            reduceAux(c, limit, count);\n"
                 << "            if( limit.empty() ) break;\n"
                 << "        }\n"
                 << "    }\n\n"

                 << "    void reduce(Tree& t){\n"
                 << "        Rules r{ t.matched_rules.at(0).first };\n"
                 << "        vector<RuleLimit_t> limit{ RulesLimitsMap.at(r) };\n"
                 << "        int count{ 0 };\n"
                 << "        reduceAux(t, limit, count);\n"
                 << "        action(r);\n"
                 << "    }\n\n";
    }



    void printHpp(string output_file_name){
        string define_name{};
        output_file_name = output_file_name.substr(output_file_name.find_last_of("/") + 1);
        for( char c: output_file_name ) define_name += toupper(c);
        define_name += "_HPP";

        out_file_h << "#ifndef " << define_name << "\n"
                   << "#define " << define_name << "\n\n"
                   
                   << "#include<map>\n"
                   << "#include<vector>\n\n"
                   
                   << "namespace Yamg{\n\n"
                   
                   << "        /* Classe do usuário */\n"
                   << "    class Tree;\n\n"
                   
                   << "        /* Enums */\n"
                   << "    enum class Rules: int;\n"
                   << "    enum class User_Symbols: int;\n\n"
                   
                   << "        /* Aliases */\n"
                   << "    using cost_t = int;\n"
                   << "    using rule_number_t = Rules;\n"
                   << "    using MyPair = std::pair<rule_number_t, cost_t>;\n"
                   << "    using SymbolArray = std::vector<int>; \n"
                   << "    using StateArray  = std::vector<int>; \n"
                   << "    using RuleLimit_t = std::pair<int, User_Symbols>;\n\n"
                   
                   << "        /* Variáveis */\n"
                   << "    extern const int infinity;\n"
                   << "    extern const int TERMINALS_START;\n"
                   << "    extern const int TERMINALS_END;\n"
                   << "    extern const int NON_TERMINALS_START;\n"
                   << "    extern const int NON_TERMINALS_END;\n"
                   << "    extern const int recognition_table[17][8];\n"
                   << "    extern const std::map<Rules, int> Rules_Non_Terminals;\n"
                   << "    extern const std::map<Rules, std::vector<RuleLimit_t>> RulesLimitsMap;\n\n"
                   
                   << "        /* Funções */\n"
                   << "    // MyPair     isFinalState(int state, Tree=Tree());\n"
                   << "    StateArray matchTree(Tree&, StateArray={0});\n"
                   << "    int    action(Rules r);\n"
                   << "    cost_t cost(Rules, Tree);\n"
                   << "    void   label(Tree&);\n"
                   << "    void   reduce(Tree&);\n\n"
                   
                   << "}\n\n"
                   
                   << "#endif\n";

    }
}


    /* Função principal */
void CodeGenerator::generate(string output_file_name){
    
    if(output_file_name.empty()) output_file_name = "yamg";
    out_file_cpp.open(output_file_name+".cpp", ios::out | ios::trunc);
    out_file_h.open(output_file_name+".hpp", ios::out | ios::trunc);

    generateNonTermsMap();

    startHeader(output_file_name);

    MyArray<Rule> rules{ Helper::getRules() };
    unique_ptr<table_t> table = generateTable(rules);

    printTableAndFinalStates(table.get());
    printCost();
    printMatchTree();
    printLabel();
    printAction();
    printReduce();

    endHeaderStartUserCode();


    printHpp(output_file_name);
}
