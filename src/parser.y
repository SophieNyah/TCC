%require "3.7.4"
%language "C++"

%define parse.error verbose
%define api.parser.class {parser}
%define api.namespace {generator}
%define api.value.type variant
%param {yyscan_t scanner}
%locations
 
%code provides
{
    #define YY_DECL \
        int yylex(generator::parser::semantic_type *yylval, generator::parser::location_type *loc, yyscan_t yyscanner)
    YY_DECL;
    // void analyzeCode()
}

%code requires{
#include"Tree.hpp"
#include"Rule.hpp"
}

%{

#include<iostream>
#include<string>
#include<list>
#include<algorithm>
#include"scanner.hpp"
#include"Helper.hpp"
#include"RegAlloc.hpp"
#include"CodeGen.hpp"

%}

    /* declaração de tokens */
%token DL_INPUT
%token DL_RULE
%token DL_PATTERN
%token DL_L_REGISTER
%token DL_R_REGISTER
%token DL_L_CBRACKET
%token DL_R_CBRACKET

%token ALGORITHM
%token NEW_TERM
%token NEW_NON_TERM
%token NEW_REGISTER
%token SET_READ_INSTRUCTION
%token SET_WRITE_INSTRUCTION
%token NEW_SPILL_REGISTER

%token TREE_PATTERN_SEPARATOR
%token L_BRACKET
%token R_BRACKET
%token L_SBRACKET
%token R_SBRACKET
%token SEMI_COLON
%token COMMA
%token COLON
%token EQUALS

%token <std::string> CPP_CODE
%token <int> INTEGER
%token <std::string> CONSTANT_ID
%token <std::string> REGISTER_ID
%token <std::string> SPECIFIC_ID
%token <std::string> IDENTIFIER
%token <std::string> STRING_LITERAL

%token END_OF_FILE
%token UNIDENTIFIED

    /* não-terminal inicial */
%start start

    /* Tipos das regras */
%type <std::string> cost
%type <std::string> action

%type <Rule> rule

%type <std::pair<std::string, Node_type>> tree_token
%type <BasicTree> tree
%type <std::vector<BasicTree>> tree_list

%%

start: header  DL_INPUT  rule  DL_INPUT  registers_declaration CPP_CODE END_OF_FILE  {
            code_t code{ Helper::trim($6.substr(1, $6.size()-2)) };
            Helper::setCode(code);
            return Helper::getError() ? 0 /*false*/ : 1 /*true*/;
        }
;

header: header  header_new_token {}
      | header CPP_CODE          {
            code_t code{ Helper::trim($2.substr(1, $2.size()-2)) };
            Helper::setHeader(code);
        }
      | CPP_CODE                 {
            code_t code{ Helper::trim($1.substr(1, $1.size()-2)) };
            Helper::setHeader(code);
      }
      | header_new_token         {}
;
header_new_token: NEW_TERM  IDENTIFIER     { Helper::newTerm($2); }
                | NEW_NON_TERM  IDENTIFIER { Helper::newNonTerm($2); }
                | ALGORITHM IDENTIFIER     {
                    std::string alg = $2;
                    if(alg != "DYNAMIC_PROGRAMMING" && alg != "MINIMAL_MUNCH" && alg != "MAXIMAL_MUNCH") {
                        Helper::semanticError("%alg is not one of \"DYNAMIC_PROGRAMMING\", \"MINIMAL_MUNCH\", \"MAXIMAL_MUNCH\".");
                    }
                    else if(alg == "DYNAMIC_PROGRAMMING") { Helper::setAlgorithm(Helper::AlgorithmValues::DYNAMIC_PROGRAMMING); }
                    else if(alg == "MINIMAL_MUNCH") { Helper::setAlgorithm(Helper::AlgorithmValues::MINIMAL_MUNCH); }
                    else if(alg == "MAXIMAL_MUNCH") { Helper::setAlgorithm(Helper::AlgorithmValues::MAXIMAL_MUNCH); }
                }
;

rule: IDENTIFIER TREE_PATTERN_SEPARATOR IDENTIFIER COLON tree cost EQUALS action SEMI_COLON
        {
            std::string name{ $1 };
            std::vector<Pattern> patterns{ std::vector<Pattern>{} };
            BasicTree root{ $5 };
            string non_term{ $3 };
            code_t action{ $8 };
            int replace{ -1 };
            std::string cost{ Helper::trim($6.substr(1, $6.size()-2)) };
            if(!Helper::isNonTerm(non_term)){
                Helper::semanticError("Symbol \"" + non_term + "\" not declared");
            }

            Helper::newRule(Rule{ name, patterns, root, non_term, action, replace, cost });
        }
       | rule IDENTIFIER TREE_PATTERN_SEPARATOR IDENTIFIER COLON tree cost EQUALS action SEMI_COLON
        {
            std::string name{ $2 };
            std::vector<Pattern> patterns{ std::vector<Pattern>{} };
            BasicTree root{ $6 };
            string non_term{ $4 };
            code_t action{ $9 };
            int replace{ -1 };
            std::string cost{ Helper::trim($7.substr(1, $7.size()-2)) };
            if(!Helper::isNonTerm(non_term)){
                Helper::semanticError("Symbol \"" + non_term + "\" not declared");
            }

            Helper::newRule(Rule{ name, patterns, root, non_term, action, replace, cost });
        }
;

registers_declaration: registers_block DL_INPUT { RegAlloc::_setAllocator(); }
                     | %empty {}
;
registers_block: registers_block register_declare {}
               | register_declare                 {}
;
register_declare: NEW_REGISTER IDENTIFIER { RegAlloc::_newReg($2); }
                | NEW_SPILL_REGISTER IDENTIFIER { RegAlloc::_newSpillReg($2); }
                | SET_READ_INSTRUCTION STRING_LITERAL { RegAlloc::_setReadInstruction(Instruction($2, {})); }
                | SET_WRITE_INSTRUCTION STRING_LITERAL { RegAlloc::_setWriteInstruction(Instruction($2, {})); }
;

tree: action tree_token  L_BRACKET  tree_list  R_BRACKET
        { 
            if(!Helper::isNonTerm($2.first) && !Helper::isTerm($2.first)){
                Helper::semanticError("Symbol \"" + $2.first + "\" not declared");
            }

            $$ = BasicTree{$2.first, 0, Node_type{ $2.second }, $1};
            for( BasicTree t: $4 ){ $$.insertChild(t); }
        }
    | action tree_token     { $$ = BasicTree{$2.first, 0, Node_type{ $2.second }, $1}; }
;

tree_list: tree_list  COMMA  tree { $1.push_back($3); $$ = $1; }
         | tree           { $$.push_back($1); }
;
tree_token: IDENTIFIER  { $$ = std::pair{ $1, Node_type::operacao }; }
          | REGISTER_ID { $$ = std::pair{ $1, Node_type::registrador }; }
          | CONSTANT_ID { $$ = std::pair{ $1, Node_type::constante }; }
          | SPECIFIC_ID { $$ = std::pair{ $1, Node_type::especifico }; }

action: CPP_CODE { $$ = $1; }
      | %empty   {}
;
cost: CPP_CODE { $$ = $1; }
;


%%


void generator::parser::error(const generator::location &c, const std::string& msg) {
    std::cerr << msg << '\n';
    Helper::setError();
}



int main(int argc, char *argv[])
{

    yyscan_t scanner;
    yylex_init(&scanner);
    generator::parser parser{ scanner };
    parser.parse();
    yylex_destroy(scanner);

    if(!Helper::getError()){
        string out_name{};
        if(argc > 1) out_name = argv[1];
        CodeGenerator::generate(out_name);
    }

    return 0;
}
