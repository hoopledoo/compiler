/********************************************************
 * Tyler Wolf scanner for C- 04/10/2020
 * 
 * This file defines the lexer for our compiler project
 * To build, we use `make` -- see the Makefile
 ********************************************************/

%{
#include "Node.h"
#include "parser.tab.hpp"

#include <iostream>
#include <stdbool.h>

bool in_comment = false;

// Keep track of our location in the parse
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for(int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 0; \
        } \
        else { \
            yylloc.last_column++; \
        } \
    }
%}

%option noyywrap
%option yylineno

/* definitions */
digit       [0-9]
letter      [a-zA-Z]
num         {digit}+
id          {letter}+
newline     \n
whitespace  [ \t\r]

%%

    /* REs and associated actions */

{num}         {if(!in_comment){ /* std::cout << "NUM: " << yytext << std::endl; */  yylval.integer=atoi(yytext); return NUM; }}
"else"        {if(!in_comment){ /* std::cout << "ELSE" << std::endl; */ return ELSE; }}
"if"          {if(!in_comment){ /* std::cout << "IF" << std::endl; */ return IF; }}
"int"         {if(!in_comment){ /* std::cout << "INT" << std::endl; */ return INT; }}
"return"      {if(!in_comment){ /* std::cout << "RETURN" << std::endl; */ return RETURN; }}
"void"        {if(!in_comment){ /* std::cout << "VOID" << std::endl; */ return VOID; }}
"while"       {if(!in_comment){ /* std::cout << "WHILE" << std::endl; */ return WHILE; }}
"+"           {if(!in_comment){ /* std::cout << "ADD" << std::endl; */ return ADD; }}
"-"           {if(!in_comment){ /* std::cout << "SUB" << std::endl; */ return SUB; }}
"/*"          {in_comment = true;}
"*/"          {in_comment = false;}
"*"           {if(!in_comment){ /* std::cout << "MULT" << std::endl; */ return MULT; }}
"/"           {if(!in_comment){ /* std::cout << "DIV" << std::endl; */ return DIV; }}
"<="          {if(!in_comment){ /* std::cout << "LTE" << std::endl; */ return LTE; }}
"<"           {if(!in_comment){ /* std::cout << "LT" << std::endl; */ return LT; }}
">="          {if(!in_comment){ /* std::cout << "GTE" << std::endl; */ return GTE; }}
">"           {if(!in_comment){ /* std::cout << "GT" << std::endl; */ return GT; }}
"=="          {if(!in_comment){ /* std::cout << "EQ" << std::endl; */ return EQ; }}
"!="          {if(!in_comment){ /* std::cout << "NEQ" << std::endl; */ return NEQ; }}
"="           {if(!in_comment){ /* std::cout << "ASSIGN" << std::endl; */ return ASSIGN; }}
";"           {if(!in_comment){ /* std::cout << "SEMI " << yytext << std::endl; */ return SEMI; }}
","           {if(!in_comment){ /* std::cout << "COMMA" << std::endl; */ return COMMA; }}
"("           {if(!in_comment){ /* std::cout << "LPAREN" << std::endl; */ return LPAREN; }}
")"           {if(!in_comment){ /* std::cout << "RPAREN" << std::endl; */ return RPAREN; }}
"["           {if(!in_comment){ /* std::cout << "LBRACKET" << std::endl; */ return LBRACKET; }}
"]"           {if(!in_comment){ /* std::cout << "RBRACKET" << std::endl; */ return RBRACKET; }}
"{"           {if(!in_comment){ /* std::cout << "LBRACE" << std::endl; */ return LBRACE; }}
"}"           {if(!in_comment){ /* std::cout << "RBRACE" <<std::endl; */ return RBRACE; }}
{id}          {if(!in_comment){ /* std::cout << "ID: " << yytext << std::endl; */ yylval.string=strdup(yytext); return ID;}}
{newline}     
{whitespace}+ 
<<EOF>>       {yyterminate();}
.             {std::cerr << "Unrecognized character in scanner.\n" << std::endl;}

%%

