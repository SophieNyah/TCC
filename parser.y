%require "3.7.4"
%language "C++"
%defines "parser.hpp"
%output "parser.cpp"

%{

#include<iostream>
#include<string>
#include "scanner.hpp"

// extern int yylex();
// extern char* yytext;
// void yyerror(char *s);
// FILE* in_file = NULL;

%}
 
%define api.parser.class {Parser}
%define api.namespace {generator}
%define api.value.type variant
%param {yyscan_t scanner}
 
%code provides
{
    #define YY_DECL \
        int yylex(generator::Parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

/* declaração de tokens */
%token DL_RULE
%token DL_PATTERN
%token DL_L_REGISTER
%token DL_R_REGISTER
%token DL_L_CBRACKET
%token DL_R_CBRACKET

%token TREE_PATTERN_SEPARATOR
%token L_BRACKET
%token R_BRACKET
%token L_SBRACKET
%token R_SBRACKET
%token SEMI_COLON
%token COMMA

%token CPP_CODE
%token IDENTIFIER

%token END_OF_FILE
%token UNIDENTIFIED

/* não-terminal inicial */
%start rule

%%

rule: DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE END_OF_FILE
        { std::cout << "SUCCESSFUL COMPILATION.\n" ; return 0; }
;

pattern: L_SBRACKET pattern_rule R_SBRACKET {}
;

pattern_rule: pattern_rule SEMI_COLON terminal DL_L_REGISTER register DL_R_REGISTER {}
            | terminal DL_L_REGISTER register DL_R_REGISTER {}
;

register: register COMMA register_name {}
        | register_name {}
;

register_name: IDENTIFIER {}
;

tree: terminal L_BRACKET action tree_list R_BRACKET {}
    | terminal L_BRACKET tree_list R_BRACKET {}
    | terminal {}
    | non_terminal {}
;

tree_list: tree_list COMMA child {}
         | child {}
;
child: tree {}
     | any {}
;
/* action: DL_L_CBRACKET  DL_R_CBRACKET {} */
action: CPP_CODE {}
;
cost: CPP_CODE {}
;

replace: IDENTIFIER {};
terminal: IDENTIFIER {};
non_terminal: IDENTIFIER {};


any: any any_token | any_token;
any_token: %empty;
%%

void generator::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}

int main()
{
	/* in_file = stdin;
	yyparse(); */

    std::cout << "Rodou\n";

    return 0;
}
