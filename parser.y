%require "3.7.4"
%language "C++"
%defines "parser.hpp"
%output "parser.cpp"

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

// std::vector<Rule> rules;

%}

%type <std::string> token
/* %type <Rule> rule */
%type <Tree> tree
%type <std::vector<Tree>> tree_list
/* %type <Pattern> pattern */
/* %type <Pattern> pattern_rule */
%type <std::vector<std::string>> register
%type <std::string> register_name

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
        {  }
    | rule DL_RULE  IDENTIFIER  DL_PATTERN  pattern  replace  TREE_PATTERN_SEPARATOR  tree  DL_PATTERN  cost  DL_RULE
        {  }
;

pattern: L_SBRACKET  pattern_rule  R_SBRACKET {}
;

pattern_rule: pattern_rule  SEMI_COLON  token  DL_L_REGISTER  register  DL_R_REGISTER  
                {
                    if(!Helper::isTerm($3)){
                        Helper::semanticError("Symbol \"" + $3 + "\" not declared as terminal");
                    } 
                }
            | token  DL_L_REGISTER  register  DL_R_REGISTER 
                {
                    if(!Helper::isTerm($1)){
                        Helper::semanticError("Symbol \"" + $1 + "\" not declared as terminal");
                    } 
                }
;

register: register  COMMA  register_name { $1.push_back($3); $$ = $1; }
        | register_name                { $$.push_back($1); }
;

register_name: IDENTIFIER { $$ = $1; }
;

    /* Primeiro casamento: DEVE ser um terminal */
    /* Segundo casamento : checar se é um terminal ou não-terminal */
tree: token  L_BRACKET  action  tree_list  R_BRACKET
        { 
            if(!Helper::isNonTerm($1)){
                Helper::semanticError("Symbol \"" + $1 + "\" not declared as non-terminal");
            }

            $$ = Tree{$1};
            for( Tree t: $4 ){
                if( !$$.insertChild(t) ){
                    Helper::semanticError("Child \"" + $1 + "\" could not be inserted, node has too many children");
                }
            }
        }
    | token     { $$ = Tree{$1}; }
;

tree_list: tree_list  COMMA  tree { $1.push_back($3); $$ = $1; }
         | tree           { $$.push_back($1); }
;

action: CPP_CODE {}
      | %empty   {}
;
cost: CPP_CODE {}
;

replace: INTEGER {};
token: IDENTIFIER { $$ = $1; };


%%


void generator::parser::error(const generator::location &c, const std::string& msg) {
    std::cerr << msg << '\n';
}



int main()
{
    
    yyscan_t scanner;
    yylex_init(&scanner);
    generator::parser parser{ scanner };
    parser.parse();
    yylex_destroy(scanner);

    return 0;
}
