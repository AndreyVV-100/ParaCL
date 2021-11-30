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

AST::AbstractNode* BindNodes(AST::AbstractNode* lhs, 
                                         AST::AbstractNode* rhs);
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
    <AST::AbstractNode*> STMS
    <AST::AbstractNode*> STM
    <AST::AbstractNode*> ASSIGN
    <AST::AbstractNode*> EXPR
    <AST::AbstractNode*> COND
    <AST::AbstractNode*> MATH_A
    <AST::AbstractNode*> MATH_Aw
    <AST::AbstractNode*> MATH_B
    <AST::AbstractNode*> MATH_Bw
    <AST::AbstractNode*> MATH_E
    <AST::AbstractNode*> MATH_Ew
    <AST::AbstractNode*> MATH_T
    <AST::AbstractNode*> MATH_Tw
    <AST::AbstractNode*> MATH_P
    <AST::AbstractNode*> SCOPE
    <AST::AbstractNode*> VAL
    <AST::AbstractNode*> VAR
    <AST::AbstractNode*> CALLFUNC
;

%left '+' '-'

%start program

%%

program: STMS                                    { driver->insert($1); }

STMS: STM                                        { $$ = AST::MakeORD($1, nullptr); }
    | STM STMS                                   { $$ = AST::MakeORD($1, $2); }
;

STM: ASSIGN                                      { $$ = $1; }
   | EXPR SCOLON                                 { $$ = $1; }
   | COND                                        { $$ = $1; }
   | CALLFUNC EXPR SCOLON                        { $$ = $1; 
                                                   $1->left_ = $2; 
                                                   $2->prev_ = $$; }
   | SCOLON                                      { $$ = nullptr; }
;

ASSIGN: VAR ASS EXPR SCOLON                      { $$ = AST::MakeOp($1, AST::OpType::ASS, $3); }
;

EXPR: MATH_A                                     { $$ = $1; }
;
                                                //TODO return pair(AbstractNode*, AbstractNode*);
MATH_A: MATH_B MATH_Aw                           { $$ = BindNodes ($1, $2); }
;
                                                //TODO copypaste?
MATH_Aw: EQU MATH_B    MATH_Aw                   { $$ = AST::MakeOp(nullptr, AST::OpType::EQU, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_B: MATH_E MATH_Bw                           { $$ = BindNodes ($1, $2); }
;

MATH_Bw: STL MATH_E MATH_Bw                      { $$ = AST::MakeOp(nullptr, AST::OpType::STL, $2);  $$ = BindNodes ($$, $3); }
       | STG MATH_E MATH_Bw                      { $$ = AST::MakeOp(nullptr, AST::OpType::STG, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_E: MATH_T MATH_Ew                           { $$ = BindNodes ($1, $2); }
;

MATH_Ew: ADD MATH_T MATH_Ew                      { $$ = AST::MakeOp(nullptr, AST::OpType::ADD, $2);  $$ = BindNodes ($$, $3); }
       | SUB MATH_T    MATH_Ew                   { $$ = AST::MakeOp(nullptr, AST::OpType::SUB, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_T: MATH_P MATH_Tw                           { $$ = BindNodes ($1, $2); }
;

MATH_Tw: MUL MATH_P MATH_Tw                      { $$ = AST::MakeOp(nullptr, AST::OpType::MUL, $2);  $$ = BindNodes ($$, $3); }
       | DIV MATH_P    MATH_Tw                   { $$ = AST::MakeOp(nullptr, AST::OpType::DIV, $2);  $$ = BindNodes ($$, $3); }
       | %empty                                  { $$ = nullptr; }
;

MATH_P: OPEXPRBRACE EXPR CLEXPRBRACE             { $$ = $2; }
       | VAL                                     { $$ = $1; }
      | VAR                                      { $$ = $1; }
      | SCAN                                     { $$ = AST::MakeFunc($1); }
;

VAL: SUB NUMBER                                  { $$ = AST::MakeOp(AST::MakeVal(0), AST::OpType::SUB, AST::MakeVal($2)); }
   | ADD NUMBER                                  { $$ = AST::MakeOp(AST::MakeVal(0), AST::OpType::ADD, AST::MakeVal($2)); }
   | NUMBER                                      { $$ = AST::MakeVal($1); }
   | ZERO                                        { $$ = AST::MakeVal($1); }
;

VAR: WORD                                        { $$ = AST::MakeVar($1); }

CALLFUNC: FUNC                                   { $$ = AST::MakeFunc($1); }
        | PRINT                                  { $$ = AST::MakeFunc($1); }
;

SCOPE: OPSCOPEBRACE STMS CLSCOPEBRACE            { $$ = $2; }
;

COND: IF     OPEXPRBRACE EXPR CLEXPRBRACE SCOPE  { $$ = AST::MakeCond($3, AST::CondType::IF,    $5); }
    | WHILE OPEXPRBRACE EXPR CLEXPRBRACE SCOPE   { $$ = AST::MakeCond($3, AST::CondType::WHILE, $5); }

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, Driver* driver)
{
    return driver->yylex(yylval);
}

void parser::error(const std::string&) {}

AST::AbstractNode* BindNodes(AST::AbstractNode* lhs, AST::AbstractNode* rhs)
{
    if (rhs)
    {
        AST::AbstractNode* tmp = rhs;

        while (tmp->left_) tmp = tmp->left_;

        tmp->left_ = lhs;
        lhs->prev_ = tmp;
        return rhs;
    }

    return lhs;
}

}
