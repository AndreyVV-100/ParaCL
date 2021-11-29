/* ------------------------------------------------------------------------- **
 *
 *  Grammar with shift/reduce, but correct
 *  EQL -> EQ; EQL | empty
 *  EQ -> E = E
 *  E -> number | E + number | E - number
 *
 * ------------------------------------------------------------------------- */

%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {yy::Driver* driver}

%code requires
{
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include "AST.hpp"

// forward decl of argument to parser
namespace yy { class Driver; }
}

%code
{
#include "driver.hpp"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         Driver* driver);
}
}

%token
  EQUAL   "="
  MINUS   "-"
  PLUS    "+"
  SCOLON  ";"
  ERR
;

%token <int> NUMBER
%nterm <AST::AbstractNode*> expr
%left '+' '-'

%start program

%%

program: expr               { driver->insert($1); }

expr: NUMBER                { $$ = AST::MakeVal($1); }
    | expr PLUS NUMBER      { $$ = AST::MakeOp($1, AST::OpType::ADD, AST::MakeVal($3)); }
    | expr MINUS NUMBER     { $$ = AST::MakeOp($1, AST::OpType::SUB, AST::MakeVal($3)); }
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval,                         
                         Driver* driver)
{
  return driver->yylex(yylval);
}

void parser::error(const std::string&){}
}
