%{
#include <cstdlib>
#include "parser.hpp"
using namespace generator;
%}

%option reentrant
%option interactive
%option noyywrap
%option noyylineno
%option nodefault
%option outfile="scanner.cpp"
%option header="scanner.hpp"

%%

    /* Delimitadores */
"$!" { return Parser::token::DL_RULE; }
"$%" { return Parser::token::DL_PATTERN; }
"$[" { return Parser::token::DL_L_REGISTER; }
"$]" { return Parser::token::DL_R_REGISTER; }
"${" { return Parser::token::DL_L_CBRACKET; }
"$}" { return Parser::token::DL_R_CBRACKET; }

    /* Símbolos Miscelânia */
"<-" { return Parser::token::TREE_PATTERN_SEPARATOR; }
"("  { return Parser::token::L_BRACKET; }
")"  { return Parser::token::R_BRACKET; }
"["  { return Parser::token::L_SBRACKET; }
"]"  { return Parser::token::R_SBRACKET; }
";"  { return Parser::token::SEMI_COLON; }
","  { return Parser::token::COMMA; }

"${.*$}"  { return Parser::token::CPP_CODE; }
[_a-zA-Z] { return Parser::token::IDENTIFIER; }

	/* misc */
[ \r\t\f]	{  }
[\n]		{  }
<<EOF>>		{ return Parser::token::END_OF_FILE; }
.			{ return Parser::token::UNIDENTIFIED; }

%%
