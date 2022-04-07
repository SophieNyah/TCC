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

%token NEW_TERM
%token NEW_NON_TERM

%token TREE_PATTERN_SEPARATOR
%token L_BRACKET
%token R_BRACKET
%token L_SBRACKET
%token R_SBRACKET
%token SEMI_COLON
%token COMMA

%token <std::string> CPP_CODE
%token <int> INTEGER
%token <std::string> CONSTANT_ID
%token <std::string> REGISTER_ID
%token <std::string> SPECIFIC_ID
%token <std::string> IDENTIFIER

%token END_OF_FILE
%token UNIDENTIFIED

    /* não-terminal inicial */
%start start

    /* Tipos das regras */
%type <std::string> token
%type <std::string> cost
%type <std::string> action
%type <int> replace

%type <Rule> rule

%type <std::pair<std::string, Node_type>> tree_token
%type <BasicTree> tree
%type <std::vector<BasicTree>> tree_list

%type <vector<Pattern>> pattern
%type <vector<Pattern>> pattern_rule

%type <std::vector<std::string>> register
%type <std::string> register_name

%%

start: header  DL_INPUT  rule  DL_INPUT  CPP_CODE  END_OF_FILE  {
            code_t code{ Helper::trim($5.substr(1, $5.size()-2)) };
            Helper::setCode(code);
            return Helper::getError() ? 0 /*false*/ : 1 /*true*/;
        }
;

header: header  header_new_token {}
      | header CPP_CODE          {
            code_t code{ Helper::trim($2.substr(1, $2.size()-2)) };
            Helper::setHeader(code);
        }
      | header_new_token         {}
;
header_new_token: NEW_TERM  IDENTIFIER     { Helper::newTerm($2); }
                | NEW_NON_TERM  IDENTIFIER { Helper::newNonTerm($2); }
;

rule: DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE
        { Helper::newRule(Rule{ $2, $4, $7, $5, Helper::trim($9.substr(1, $9.size()-2)) }); }
    | rule DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE
        { Helper::newRule(Rule{ $3, $5, $8, $6, Helper::trim($10.substr(1, $10.size()-2)) }); }
;

pattern: L_SBRACKET  pattern_rule  R_SBRACKET { $$ = $2; }
;

pattern_rule: pattern_rule  SEMI_COLON  token  DL_L_REGISTER  register  DL_R_REGISTER  
                {
                    if(!Helper::isTerm($3)){
                        Helper::semanticError("Symbol \"" + $3 + "\" not declared as terminal");
                    }

                    Pattern p{$3, $5};
                    $1.push_back(p);
                    $$ = $1;
                    // std::cout << "+++ " << $$.at(0) << " +++";
                }
            | token  DL_L_REGISTER  register  DL_R_REGISTER 
                {
                    if(!Helper::isTerm($1)){
                        Helper::semanticError("Symbol \"" + $1 + "\" not declared as terminal");
                    }

                    Pattern p{$1, $3};
                    $$.push_back(p);
                }
;

register: register  COMMA  register_name { $1.push_back($3); $$ = $1; }
        | register_name                { $$.push_back($1); }
;

register_name: IDENTIFIER { $$ = $1; }
;

tree: tree_token  L_BRACKET  action  tree_list  R_BRACKET
        { 
            if(!Helper::isNonTerm($1.first)){
                Helper::semanticError("Symbol \"" + $1.first + "\" not declared as non-terminal");
            }

            $$ = BasicTree{$1.first, 0, Node_type{ $1.second }, $3};
            for( BasicTree t: $4 ){ $$.insertChild(t); }
        }
    | tree_token     { $$ = BasicTree{$1.first, 0, Node_type{ $1.second }}; }
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

replace: INTEGER { $$ = $1; }
       | %empty  { $$ = -1; };
token: IDENTIFIER { $$ = $1; };


%%


void generator::parser::error(const generator::location &c, const std::string& msg) {
    std::cerr << msg << '\n';
    Helper::setError();
}



int main()
{

    yyscan_t scanner;
    yylex_init(&scanner);
    generator::parser parser{ scanner };
    parser.parse();
    yylex_destroy(scanner);

    if(!Helper::getError()){
        CodeGenerator::generate();
    }

    return 0;
}
