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
%nterm <ParaCL::AbstractNode*> expr
%left '+' '-'

%start program

%%

program: expr               { driver->insert($1); }

expr: NUMBER                { $$ = ParaCL::MakeVal($1); }
    | expr PLUS NUMBER      { $$ = ParaCL::MakeOp($1, ParaCL::OpType::ADD, ParaCL::MakeVal($3)); }
    | expr MINUS NUMBER     { $$ = ParaCL::MakeOp($1, ParaCL::OpType::SUB, ParaCL::MakeVal($3)); }
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
