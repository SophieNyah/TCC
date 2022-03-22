%require "3.7.4"
%language "C++"
%defines "parser.hpp"
%output "parser.cpp"

%define api.parser.class {parser}
%define api.namespace {generator}
%define api.value.type variant
/* %define api.token.constructor */
%param {yyscan_t scanner}
 
%code provides
{
    #define YY_DECL \
        int yylex(generator::parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%{

#include<iostream>
#include<string>
#include<list>
#include<algorithm>
#include"scanner.hpp"
#include"Helper.hpp"

%}

%type <std::string> token

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

%token CPP_CODE
%token INTEGER
%token <std::string> IDENTIFIER

%token END_OF_FILE
%token UNIDENTIFIED

/* não-terminal inicial */
%start start

%%

start: header  DL_INPUT  rule  DL_INPUT  CPP_CODE  END_OF_FILE  {
            if(!Helper::error()){
                std::cout << "SUCESSFUL COMPILATION" << '\n';
                return 0;
            }else{
                return 1;
            }
        }
;

header: header  header_new_token {}
      | header_new_token        {}
;
header_new_token: NEW_TERM  IDENTIFIER     { Helper::newTerm($2); }
                | NEW_NON_TERM  IDENTIFIER { Helper::newNonTerm($2); }
;

rule: DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE
        {}
    | rule DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE
        {}
;

pattern: L_SBRACKET  pattern_rule  R_SBRACKET {}
;

pattern_rule: pattern_rule  SEMI_COLON  token  DL_L_REGISTER  register  DL_R_REGISTER  
                {
                    if(!Helper::isTerm($3)){
                        Helper::semanticError("Symbol \"" + $3 + "\" not declared as terminal or non-terminal");
                        // YYERROR;
                    } 
                }
            | token  DL_L_REGISTER  register  DL_R_REGISTER 
                { 
                    if(!Helper::isTerm($1)){
                        Helper::semanticError("Symbol \"" + $1 + "\" not declared as terminal or non-terminal");
                        // YYERROR;
                    } 
                }
;

register: register  COMMA  register_name {}
        | register_name                {}
;

register_name: IDENTIFIER {}
;

    /* Primeiro casamento: DEVE ser um terminal */
    /* Segundo casamento : checar se é um terminal ou não-terminal */
tree: token  L_BRACKET  action  tree_list  R_BRACKET
        { 
            if(!Helper::isTerm($1)){
                Helper::semanticError("Symbol \"" + $1 + "\" not declared as terminal");
                // YYERROR;
            }
        }
    | token
        {
            if(!Helper::isTerm($1) && !Helper::isNonTerm($1)){
                Helper::semanticError("Symbol \"" + $1 + "\" not declared as terminal or non-terminal");
                // YYERROR;
            }
        }
;

tree_list: tree_list  COMMA  tree {}
         | tree                 {}
;
/* child: tree {}
     | any  {}
; */
/* action: DL_L_CBRACKET  DL_R_CBRACKET {} */
action: CPP_CODE {}
      | %empty   {}
;
cost: CPP_CODE {}
;

replace: INTEGER {};
token: IDENTIFIER { $$ = $1; };


/* any: any any_token | any_token;
any_token: %empty; */

%%


void generator::parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}



int main()
{
    
    yyscan_t scanner;
    yylex_init(&scanner);
    generator::parser parser{ scanner };
    parser.parse();
    yylex_destroy(scanner);

    /* std::cout << "***";
    for(auto  &i: terminals){
        std::cout  << i << " ";
    }
    for(auto  &i: non_terminals){
        std::cout << i << " ";
    }

    std::string a{"hewooUWU"};
    std::cout << findInList(terminals, a) << " | " << findInList(non_terminals, a); */

    return 0;
}
