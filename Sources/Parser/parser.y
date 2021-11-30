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

ParaCL::AbstractNode* BindNodes(ParaCL::AbstractNode* lhs, 
                                         ParaCL::AbstractNode* rhs);
}
}

%token
    EQU             "=="
    STL             "<"
    STG             ">"
    SUB             "-"
    ADD             "+"
    MUL             "*"
    DIV             "/"
    ASS             "="
    SCOLON       ";"
    OPEXPRBRACE  "("
    CLEXPRBRACE  ")"
      OPSCOPEBRACE "{"
    CLSCOPEBRACE "}"
    WHILE         "while"
    IF              "if"
    ERR
    <std::string> PRINT
    <std::string> SCAN
    <std::string> WORD
    <std::string> FUNC
    <int> NUMBER
    <int> ZERO
;

%nterm 
    <ParaCL::AbstractNode*> STMS
    <ParaCL::AbstractNode*> STM
    <ParaCL::AbstractNode*> ASSIGN
    <ParaCL::AbstractNode*> EXPR
    <ParaCL::AbstractNode*> COND
    <ParaCL::AbstractNode*> MATH_A
    <ParaCL::AbstractNode*> MATH_Aw
    <ParaCL::AbstractNode*> MATH_B
    <ParaCL::AbstractNode*> MATH_Bw
    <ParaCL::AbstractNode*> MATH_E
    <ParaCL::AbstractNode*> MATH_Ew
    <ParaCL::AbstractNode*> MATH_T
    <ParaCL::AbstractNode*> MATH_Tw
    <ParaCL::AbstractNode*> MATH_P
    <ParaCL::AbstractNode*> SCOPE
    <ParaCL::AbstractNode*> VAL
    <ParaCL::AbstractNode*> VAR
    <ParaCL::AbstractNode*> CALLFUNC
;

%left '+' '-' '*' '/'

%start program

%%

program: STMS                                    { driver->insert($1); }

STMS: STM                                        { $$ = ParaCL::MakeOp($1, ParaCL::OpType::ORD, nullptr); }
    | STM STMS                                   { $$ = ParaCL::MakeOp($1, ParaCL::OpType::ORD, $2); }
;

STM: ASSIGN                                      { $$ = $1; }
   | EXPR SCOLON                                 { $$ = $1; }
   | COND                                        { $$ = $1; }
   | CALLFUNC EXPR SCOLON                        { $$ = $1; 
                                                   $1->left_ = $2; 
                                                   $2->prev_ = $$; }
   | SCOLON                                      { $$ = nullptr; }
;

ASSIGN: VAR ASS EXPR SCOLON                      { $$ = ParaCL::MakeOp($1, ParaCL::OpType::ASS, $3); }
;

EXPR: MATH_A                                     { $$ = $1; }
;
                                                //TODO return pair(AbstractNode*, AbstractNode*);
MATH_A: MATH_B MATH_Aw                           { $$ = BindNodes ($1, $2); }
;
                                                //TODO copypaste?
MATH_Aw: EQU MATH_B    MATH_Aw                   { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::EQU, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_B: MATH_E MATH_Bw                           { $$ = BindNodes ($1, $2); }
;

MATH_Bw: STL MATH_E MATH_Bw                      { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::STL, $2);  $$ = BindNodes ($$, $3); }
       | STG MATH_E MATH_Bw                      { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::STG, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_E: MATH_T MATH_Ew                           { $$ = BindNodes ($1, $2); }
;

MATH_Ew: ADD MATH_T MATH_Ew                      { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::ADD, $2);  $$ = BindNodes ($$, $3); }
       | SUB MATH_T    MATH_Ew                   { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::SUB, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_T: MATH_P MATH_Tw                           { $$ = BindNodes ($1, $2); }
;

MATH_Tw: MUL MATH_P MATH_Tw                      { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::MUL, $2);  $$ = BindNodes ($$, $3); }
       | DIV MATH_P    MATH_Tw                   { $$ = ParaCL::MakeOp(nullptr, ParaCL::OpType::DIV, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_P: OPEXPRBRACE EXPR CLEXPRBRACE             { $$ = $2; }
       | VAL                                     { $$ = $1; }
      | VAR                                      { $$ = $1; }
      | SCAN                                     { $$ = ParaCL::MakeFunc($1); }
;

VAL: SUB NUMBER                                  { $$ = ParaCL::MakeOp(ParaCL::MakeVal(0), ParaCL::OpType::SUB, ParaCL::MakeVal($2)); }
   | ADD NUMBER                                  { $$ = ParaCL::MakeOp(ParaCL::MakeVal(0), ParaCL::OpType::ADD, ParaCL::MakeVal($2)); }
   | NUMBER                                      { $$ = ParaCL::MakeVal($1); }
   | ZERO                                        { $$ = ParaCL::MakeVal($1); }
;

VAR: WORD                                        { $$ = ParaCL::MakeVar($1); }

CALLFUNC: FUNC                                   { $$ = ParaCL::MakeFunc($1); }
        | PRINT                                  { $$ = ParaCL::MakeFunc($1); }
;

SCOPE: OPSCOPEBRACE STMS CLSCOPEBRACE            { $$ = $2; }
;

COND: IF     OPEXPRBRACE EXPR CLEXPRBRACE SCOPE  { $$ = ParaCL::MakeCond($3, ParaCL::CondType::IF,    $5); }
    | WHILE OPEXPRBRACE EXPR CLEXPRBRACE SCOPE   { $$ = ParaCL::MakeCond($3, ParaCL::CondType::WHILE, $5); }

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, Driver* driver)
{
    return driver->yylex(yylval);
}

void parser::error(const std::string&) {}

ParaCL::AbstractNode* BindNodes(ParaCL::AbstractNode* lhs, ParaCL::AbstractNode* rhs)
{
    if (rhs)
    {
        ParaCL::AbstractNode* tmp = rhs;

        while (tmp->left_) tmp = tmp->left_;

        tmp->left_ = lhs;
        lhs->prev_ = tmp;
        return rhs;
    }

    return lhs;
}

}
