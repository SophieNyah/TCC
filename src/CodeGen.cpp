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
#include"RegAlloc.hpp"
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
                        "#include<limits>\n"
                        "#include<cassert>\n"
                        "#include<algorithm>\n"
                        "#include\"" << include_name << ".hpp\"\n"
                     << (RegAlloc::_isAllocatorSet() ? "#include\"../../src/RegAlloc.hpp\"\n" : "")
                     << "#include\"../../src/Tree.hpp\"\n\n";


        /* Header do usuário */
        out_file_cpp << Helper::getHeader() << "\n\n";


        /* Abre o namespace */
        out_file_cpp << "namespace Yamg{\n\n";


        /* Aliases de tipos */
        out_file_cpp << "    using cost_t = int;\n"
                 << "    using rule_number_t = Rules;\n"
                 << "    using MyPair = std::pair<rule_number_t, cost_t>;\n"
                 << "    typedef int (*costFunction_t)(Rules, Tree);\n"
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
    
    string findAndReplace(string search_in, string replace_with[2], string seach_expression) {
        string replaced{};
        try {
            regex expr(seach_expression);
            auto itr = search_in.begin();
            std::match_results<std::string::iterator> match;

            while(regex_search(itr, search_in.end(), match, expr)) {
                replaced.append(itr, itr+match.position());
                replaced.append(replace_with[0] + match[1].str() + replace_with[1]);
                itr += match.position() + match.length();
            }
            if(replaced.length() > 0) replaced.append(itr, search_in.end());
        } catch(...) {
            return search_in;
        }
        return replaced.length() > 0 ? replaced : search_in;
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


        out_file_cpp << "    MyPair isFinalState(int state, Tree t, costFunction_t cost){\n"
                     << "        switch(state){\n";
        for( auto f: final_states ){
            out_file_cpp << "            case " << std::setw(4) << f.first << ": return MyPair{ Rules::"
                << f.second.first << ", cost(Rules::" << f.second.first << ", t) };\n";
        }
        out_file_cpp << "            default: return MyPair{ Rules::null, infinity };\n"
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
    void printTermsUtil() {
        out_file_cpp << "    bool isTerm(int user_symbol){\n"
                        "        if(user_symbol >= TERMINALS_START && user_symbol <= TERMINALS_END) return true;\n"
                        "        else return false;\n"
                        "    }\n"
                        "    bool isNonTerm(int user_symbol){\n"
                        "        if(user_symbol >= NON_TERMINALS_START && user_symbol <= NON_TERMINALS_END) return true;\n"
                        "        else return false;\n"
                        "    }\n\n";
    }
    void printRulesSize() {
        out_file_cpp <<     "    int ruleSize(Rules r) {\n"
                            "        switch(r) {\n";
        for(Rule r: Helper::getRules()){
            // FIXME: essa função precisa considerar possíveis sub-árvores
            out_file_cpp << "            case Rules::" << r.getName() << ": return " << r.getPattern().children_size+1 << ";\n";
        }
        out_file_cpp <<     "        }\n"
                            "    }\n\n";
    }
    void _printCostDP(){
        if(!Helper::getAlgorithms().dynamicProgramming) return;
        out_file_cpp << "    int costDP(Rules r, Tree t){\n"
                     << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            string costStr = r.getCost();
            string replace_with[2]{"t.getChildCost(", ")"};
            costStr = findAndReplace(costStr, replace_with, "\\$cost\\[(\\d+)\\]");
            replace_with[0] = "\n"; replace_with[1] = "                ";
            costStr = findAndReplace(costStr, replace_with, "\n");

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
    void _printCostMinMunch() {
        if(!Helper::getAlgorithms().minMunch) return;
        out_file_cpp << "    int costMinMunch(Rules r, Tree t) {\n"
                     << "        switch(r) {\n";

        for( Rule r: Helper::getRules() ) {
            int patternCost = r.getPattern().nonTermsSize(Helper::isNonTerm) + r.getPattern().nonTermsSize(Helper::isTerm);
//            int patternCost = r.getPattern().size();
            std::string nonTermsCost{};
//            for(int nonTerm: r.getPattern().nonTermsIndexes(Helper::isNonTerm)) {
//                nonTermsCost += " + t.getChildCost(" + std::to_string(nonTerm) + ")";
//            }
            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
                         << printTab(4) << "return " << patternCost << nonTermsCost << ";\n"
                         << printTab(4) << "break;\n";
        }
        out_file_cpp << "            default:\n"
                     << "                return infinity;\n"
                     << "            }\n"
                     << "    }\n\n";
    }
    void _printCostMaxMunch() {
        if(!Helper::getAlgorithms().maxMunch) return;
        out_file_cpp << "    int costMaxMunch(Rules r, Tree t) {\n"
                     << "        switch(r) {\n";

        for( Rule r: Helper::getRules() ) {
//            int patternCost = r.getPattern().nonTermsSize(Helper::isNonTerm) + 1;
            int patternCost = r.getPattern().size();
            std::string nonTermsCost{};
//            for(int nonTerm: r.getPattern().nonTermsIndexes(Helper::isNonTerm)) {
//                nonTermsCost += " - t.getChildCost(" + std::to_string(nonTerm) + ")";
//            }
            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
                         << printTab(4) << "return -" << patternCost << nonTermsCost << ";\n"
                         << printTab(4) << "break;\n";
        }
        out_file_cpp << "            default:\n"
                     << "                return infinity;\n"
                     << "            }\n"
//                     << "        switch(r) {\n";
//
//        for( Rule r: Helper::getRules() ) {
//            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
//                         << printTab(4) << "return " << -r.getPattern().size() << ";\n"
//                         << printTab(4) << "break;\n";
//        }
//        out_file_cpp << "            default:\n"
//                     << "                return infinity;\n"
//                     << "            }\n"
//                     << "        return -costMinMunch(r, t);\n"
                     << "    }\n\n";
    }
    void printCost() {
        _printCostDP();
        _printCostMinMunch();
        _printCostMaxMunch();
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
        out_file_cpp <<
           "    void transitoryRules(Tree &t, StateArray &final_states, costFunction_t cost) {\n"
           "        SymbolArray symbols{ t.getSymbol() };\n"
           "        for(int rule: t.non_terminal) symbols.push_back(rule);\n"
           "        for(int symbol: symbols) {\n"
           "            if(!isNonTerm(symbol)) continue;\n"
           "            int state = recognition_table[0][symbol];\n"
           "            if(isFinalState(state)) {\n"
           "                if (std::find(final_states.begin(), final_states.end(), state) == final_states.end()) {\n"
           "                    final_states.emplace_back(state);\n"
           "                    MyPair s{isFinalState(state, t, cost)};\n"
           "                    t.non_terminal.emplace_back(Rules_Non_Terminals.at(s.first));\n"
           "                }\n"
           "            }\n"
           "        }\n"
           "    }\n\n"
           "    void _label(Tree& t, costFunction_t cost){\n"
           "        for( int i=0; i<t.children_size; i++ ){\n"
           "            Tree& c{ t.getChildReference(i) };\n"
           "            _label(c, cost);\n"
           "        }\n"
           "        StateArray final_states{};\n"
           "        matchTree(t, final_states);\n"
           "        for (int state: final_states){\n"
           "            MyPair s{isFinalState(state, t, cost)};\n"
           "            t.non_terminal.push_back(Rules_Non_Terminals.at(s.first));\n"
           "        }\n"
           "        transitoryRules(t, final_states, cost);\n"
           "        for( int state: final_states ){\n"
           "            MyPair s{ isFinalState(state, t, cost) };\n"
           "            auto exists = std::find_if(t.matched_rules.begin(), t.matched_rules.end(), [s](std::pair<Yamg::Rules, int> r)->bool { return s.first == r.first; });\n"
           "            if(exists == t.matched_rules.end()) {\n"
           "                auto pos = std::find_if(t.matched_rules.begin(), t.matched_rules.end(), [s](std::pair<Yamg::Rules, int> r) -> bool { return r.second > s.second; });\n"
           "                t.matched_rules.insert(pos, s);\n"
           "            }\n"
           "        }\n"
           "    }\n\n";
    }
    void printAction(){
        out_file_cpp << "    int action(Rules r, Tree &t, int subrule){\n"
                     << "        switch(r){\n";
        for( Rule r: Helper::getRules() ){
            BasicTree &pattern = r.getPattern();
            string replace_with[2]{"t.getChildName(", ")"};
            string act = findAndReplace(r.getAction(), replace_with, "\\$\\[(\\d+)\\]");
            pattern.map(
                    [&replace_with](TemplateTree<BasicTree> *t, auto findReplace) -> void {
                        t->setAction(findReplace(t->getAction(), replace_with, "\\$\\[(\\d+)\\]"));
                    }, findAndReplace);
            replace_with[0] = "t.getChild(";
            act = findAndReplace(act, replace_with, "\\$node\\[(\\d+)\\]");
            pattern.map(
                [&replace_with](TemplateTree<BasicTree> *t, auto findReplace) -> void {
                    t->setAction(findReplace(t->getAction(), replace_with, "\\$node\\[(\\d+)\\]"));
                }, findAndReplace);
            replace_with[0] = "\n"; replace_with[1] = "                        ";
            act = findAndReplace(act, replace_with, "\n");
            pattern.map(
                    [&replace_with](TemplateTree<BasicTree> *t, auto findReplace) -> void {
                        t->setAction(findReplace(t->getAction(), replace_with, "\n"));
                    }, findAndReplace);
            string subrules{};
            int location{ 0 };
            pattern.map([](TemplateTree<BasicTree> *t, string *subrules, int *location)->void {
                if(t->getAction() == ""){
                    (*location)++;
                    return;
                }
                subrules->append(printTab(5) + "case " + std::to_string(*location) + ":\n" + t->getAction() + "\n" + printTab(6) + "break;\n");
                (*location)++;
            }, &subrules, &location);

            out_file_cpp << printTab(3) << "case Rules::" << r.getName() << " :\n"
                         << printTab(4) << "switch(subrule) {\n"
                         << subrules
                         << printTab(5) << "case -1:\n"
                         << printTab(6) << act << '\n'
                         << printTab(6) << "break;\n"
                         << printTab(5) << "}\n"
                         << printTab(4) << "break;\n";
//                         << "br"
        }
        out_file_cpp << "            default: break;\n"
                     << "        }\n"
                     << "        return 0;\n"
                     << "    }\n\n";
    }

    void printCheckRule() {
        out_file_cpp << "    bool checkRule(Tree &t, std::vector<RuleLimit_t> limit) {\n"
                        "        int last_index;\n"
                        "        if(t.children_size+1 != ruleSize(t.matched_rules[0].first)) return false;\n"
                        "//        for(RuleLimit_t l: limit) {\n"
                        "//            if(t.children_size < l.first) return false;\n"
                        "//            if(!isTerm((int)l.second)) {\n"
                        "//                if (t.getImmediateChild(l.first).getNonTerms()[0] != (int) l.second) return false;\n"
                        "//            }\n"
                        "//            last_index = l.first+1;\n"
                        "//        }\n"
                        "//        for(int i=last_index; i<=t.children_size; i++) {\n"
                        "//            Tree child = t.getImmediateChild(i);\n"
                        "//            if(isNonTerm(child.getNonTerms()[0])) return false;\n"
                        "//        }\n"
                        "        return true;\n"
                        "    }\n\n";
    }
    int printReduceAux(BasicTree& t, int count=0){
        if( Helper::isNonTerm(t.getName())  ||  (count==0 && t.children_size==0) ){
            out_file_cpp << "                RuleLimit_t{ " << count << ", User_Symbols::" << t.getName() << " },\n";
        }
        
        for( BasicTree c: t.getChildren() ){
            count = printReduceAux(c, count+1);
        }
        return count;
    }
    void printReduce(){
        out_file_cpp << "    using RuleLimit_t = std::pair<int, User_Symbols>;\n"
                 << "    const std::map<Rules, std::vector<RuleLimit_t>> RulesLimitsMap{\n";
        for( Rule r: Helper::getRules() ){
            out_file_cpp << "        { Rules::" << r.getName() << ",\n"
                     << "            {\n";
            BasicTree t{ r.getPattern() };
            printReduceAux(t);
            out_file_cpp << "            },\n"
                     << "        },\n";
        }
        out_file_cpp << "    };\n\n"

                        "    void _reduce(Tree&);\n"
                        "    void _reduceAux(Tree& originalTree, Tree& t, std::vector<RuleLimit_t>& limit, int& count, const Rules &rule){\n"
                        "        if( !limit.empty() && count == limit.at(0).first ){\n"
                        "            limit.erase(limit.begin());\n"
                        "            action(rule, originalTree, count);\n"
                        "            if( count != 0 ) _reduce(t);\n"
                        "            return;\n"
                        "        } else {\n"
                        "            action(rule, originalTree, count);\n"
                        "        }\n\n"

                        "        for( Tree c: t.getChildren() ){\n"
                        "            count++;\n"
                        "            _reduceAux(originalTree, c, limit, count, rule);\n"
                        "            if( limit.empty() ) break;\n"
                        "        }\n"
                        "    }\n\n"

                        "    void _reduce(Tree& t){\n"
                        "        Rules r{ t.matched_rules.at(0).first };\n"
                        "        std::vector<RuleLimit_t> limit{ RulesLimitsMap.at(r) };\n"
                        "        while(!checkRule(t, limit)){\n"
                        "            t.matched_rules.erase(t.matched_rules.begin());\n"
                        "            assert(t.matched_rules[0].first!=Rules::null && \"NENHUMA REGRA ENCONTRADA\\n\");\n"
                        "            r = t.matched_rules.at(0).first;\n"
                        "            limit = RulesLimitsMap.at(r);\n"
                        "        }\n"
                        "        int count{ 0 };\n"
                        "        _reduceAux(t, t, limit, count, r);\n"
                        "        action(r, t, -1);\n"
                        "    }\n\n";
    }
    void printGenerateCode(){
        if(RegAlloc::_isAllocatorSet()) {
            optional<Instruction> read  = RegAlloc::_getReadInstruction();
            optional<Instruction> write = RegAlloc::_getWriteInstruction();
            out_file_cpp << "    void _setRegisters() {\n";
            for(string reg: RegAlloc::_getRegs())        { out_file_cpp << "        RegAlloc::_newReg({\"" + reg + "\"});\n"; }
            for(string spill: RegAlloc::_getSpillRegs()) { out_file_cpp << "        RegAlloc::_newSpillReg({\"" + spill + "\"});\n"; }
            if(read.has_value())  out_file_cpp << "        RegAlloc::_setReadInstruction(Instruction{ {" << read.value().template_instruction << "}, {} });\n";
            if(write.has_value()) out_file_cpp << "        RegAlloc::_setWriteInstruction(Instruction{ {" << write.value().template_instruction << "}, {} });\n";
            out_file_cpp << "    }\n\n";
        }

        out_file_cpp << "    void match(Tree&t, costFunction_t cost) {\n"
                     << (RegAlloc::_isAllocatorSet() ? "        _setRegisters();\n" : "")
                     << "        _label(t, cost);\n"
                        "        _reduce(t);\n"
                        "    }\n\n";

        auto alg = Helper::getAlgorithms();
        if(alg.dynamicProgramming) out_file_cpp
            << "    void matchDynamicProgramming(Tree &t) {\n"
            << "        match(t, costDP);\n"
            << "    }\n";
        if(alg.minMunch) out_file_cpp
            << "    void matchMinimalMunch(Tree &t) {\n"
            << "        match(t, costMinMunch);\n"
            << "    }\n";
        if(alg.maxMunch) out_file_cpp
            << "    void matchMaximalMunch(Tree &t) {\n"
            << "        match(t, costMaxMunch);\n"
            << "    }\n";
    }


    void printHpp(string output_file_name){
        auto alg = Helper::getAlgorithms();
        string define_name{};
        output_file_name = output_file_name.substr(output_file_name.find_last_of("/") + 1);
        for( char c: output_file_name ) define_name += toupper(c);
        define_name += "_HPP";

        out_file_h << "#ifndef " << define_name << "\n"
                   << "#define " << define_name << "\n\n"
                   
                   << "#include<map>\n"
                   << "#include<vector>\n\n"

                   << "    /* Classe do usuário */\n"
                   << "class Tree;\n\n"
                   
                   << "namespace Yamg{\n\n"
                   
                   << "        /* Enums */\n";
                    /* Enum de regras */
                    out_file_h << printTab(1) << "enum class Rules {\n"
                                 << "        null = -1,\n";
                    for( Rule r: Helper::getRules() ){
                        out_file_h << printTab(2) << r.getName() << " = " << r.getRuleNumber() << ",\n";
                    }
                    out_file_h << printTab(1) << "};\n\n";

                    /* Enum de não-terminais */
                    out_file_h << printTab(1) << "enum class User_Symbols {\n";
                    out_file_h << printTab(2) << "null = -1,\n";
                    for( auto symbol: user_symbols ){
                        out_file_h << printTab(2) << symbol.first << " = " << symbol.second << ",\n";
                    }
                    out_file_h << printTab(1) << "};\n\n";
                   
                   out_file_h << "        /* Aliases */\n"
                   << "    using cost_t = int;\n"
                   << "    using rule_number_t = Rules;\n"
                   << "    using MyPair = std::pair<rule_number_t, cost_t>;\n"
                   << "    using SymbolArray = std::vector<int>; \n"
                   << "    using StateArray  = std::vector<int>; \n"
                   << "    using RuleLimit_t = std::pair<int, User_Symbols>;\n\n"
                   
                   << "        /* Variáveis */\n"
                   << "    extern const int infinity;\n";


                    /* Variáveis de delimitação do enum */
                    out_file_h << printTab(1) << "const int TERMINALS_START     = " << 0 << ";\n";
                    out_file_h << printTab(1) << "const int TERMINALS_END       = " << Helper::getTerms().size() - 1 << ";\n";
                    out_file_h << printTab(1) << "const int NON_TERMINALS_START = " << Helper::getTerms().size() << ";\n";
                    out_file_h << printTab(1) << "const int NON_TERMINALS_END   = " << user_symbols.size() - 1 << ";\n";

                    out_file_h << printTab(1) << "const std::map<Rules, int> Rules_Non_Terminals{\n";
                    for( Rule r: Helper::getRules() ){
                        int num{ user_symbols.at(r.getSymbol()) };
                        out_file_h << printTab(2) << "{ Rules::" << r.getName() << ", " << num << " },\n";
                    }
                    out_file_h << printTab(1) << "};\n\n";
                   
                   out_file_h << "        /* Funções */\n"
                   << (alg.dynamicProgramming ? "    void matchDynamicProgramming(Tree&);\n" : "")
                   << (alg.maxMunch ?           "    void matchMaximalMunch(Tree&);\n" : "")
                   << (alg.minMunch ?           "    void matchMinimalMunch(Tree&);\n" : "")
                   << "\n"

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
    printTermsUtil();
    printRulesSize();
    printCost();
    printMatchTree();
    printLabel();
    printAction();
    printCheckRule();
    printReduce();
    printGenerateCode();

    endHeaderStartUserCode();


    printHpp(output_file_name);
}
