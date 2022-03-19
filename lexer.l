%{
#include<cstdlib>
#include<iostream>
#include "parser.hpp"
using namespace generator;
%}

%option reentrant
%option interactive
%option noyywrap
%option yylineno
%option nodefault
%option outfile="scanner.cpp"
%option header="scanner.hpp"

%%

    /* Delimitadores */
"$!" { std::cout << yytext << '\n'; return Parser::token::DL_RULE; }
"$%" { std::cout << yytext << '\n'; return Parser::token::DL_PATTERN; }
"$[" { std::cout << yytext << '\n'; return Parser::token::DL_L_REGISTER; }
"$]" { std::cout << yytext << '\n'; return Parser::token::DL_R_REGISTER; }

    /* Símbolos Miscelânia */
"<-" { std::cout << yytext << '\n'; return Parser::token::TREE_PATTERN_SEPARATOR; }
"("  { std::cout << yytext << '\n'; return Parser::token::L_BRACKET; }
")"  { std::cout << yytext << '\n'; return Parser::token::R_BRACKET; }
"["  { std::cout << yytext << '\n'; return Parser::token::L_SBRACKET; }
"]"  { std::cout << yytext << '\n'; return Parser::token::R_SBRACKET; }
";"  { std::cout << yytext << '\n'; return Parser::token::SEMI_COLON; }
","  { std::cout << yytext << '\n'; return Parser::token::COMMA; }

"${" {
    std::string text = yytext;
    int c;
    while(true){
        c = yyinput(yyscanner);
        text += c;
        if(c == '$') {
            c = yyinput(yyscanner);
            text += c;
            if(c == '}') {
                break;
            }
        }
        if (c == EOF){
            REJECT;
            break;
        } 
        
        /* Lida com o input */
        
    }
    std::cout << text << '\n';
    return Parser::token::CPP_CODE;
}
[_a-zA-Z]+ { std::cout << yytext << '\n'; return Parser::token::IDENTIFIER; }

	/* misc */
[ \r\t\f]	{  }
[\n]		{  }
<<EOF>>		{ std::cout << "END_OF_FILE" << '\n'; return Parser::token::END_OF_FILE; }
.			{ std::cout << "UNIDENTIFIED" << '\n'; return Parser::token::UNIDENTIFIED; }

%%
