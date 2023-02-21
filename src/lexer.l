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

%%

    /* Delimitadores */
"%%" { return parser::token::DL_INPUT; }
"$!" { return parser::token::DL_RULE; }
"$%" { return parser::token::DL_PATTERN; }
"$[" { return parser::token::DL_L_REGISTER; }
"$]" { return parser::token::DL_R_REGISTER; }

    /* Símbolos Miscelânia */
"<-" { return parser::token::TREE_PATTERN_SEPARATOR; }
"("  { return parser::token::L_BRACKET; }
")"  { return parser::token::R_BRACKET; }
"["  { return parser::token::L_SBRACKET; }
"]"  { return parser::token::R_SBRACKET; }
";"  { return parser::token::SEMI_COLON; }
","  { return parser::token::COMMA; }
":"  { return parser::token::COLON; }
"="  { return parser::token::EQUALS; }

    /* Símbolos declarativos */
"%term"      { return parser::token::NEW_TERM; }
"%nonterm"   { return parser::token::NEW_NON_TERM; }
"%register"  { return parser::token::NEW_REGISTER; }
"%set_read"  { return parser::token::SET_READ_INSTRUCTION; }
"%set_write" { return parser::token::SET_WRITE_INSTRUCTION; }
"%spill_register" { return parser::token::NEW_SPILL_REGISTER; }

    /* Código CPP */
"{" {
    std::string text = yytext;
    int c;
    int bracket_count = 0;

    while(true){
        c = yyinput(yyscanner);
        text += c;

        if (c == '}') {
            if(bracket_count > 0) {
                bracket_count--;
            } else {
                break;
            }
        }
        
        if (c == EOF){
            REJECT;
            break;
        } 
        
        if (c == '{') {
            bracket_count++;
        }
    }

    yylval->emplace<std::string>(text);
    return parser::token::CPP_CODE;
}

[0-9]+  { yylval->emplace<int>(std::atoi(yytext)); return parser::token::INTEGER; }

    /* Identificador */
$?[_a-zA-Z][_a-zA-Z0-9]* { yylval->emplace<std::string>(yytext); return parser::token::IDENTIFIER; }
    /* String literal */
\"(\\.|[^\\"])*\" { yylval->emplace<std::string>(yytext); return parser::token::STRING_LITERAL; }

	/* misc */
[ \r\t\f]	{  }
[\n]		{  }
<<EOF>>		{ return parser::token::END_OF_FILE; }
.			{ return parser::token::UNIDENTIFIED; }

%%
