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

AST::AbstractNode* BindNodes(AST::AbstractNode* lhs, AST::AbstractNode* rhs, 
                                                        AST::OpType op_type);
}
}

%token
    LOR             "||"
    LAND            "&&"
    EQU             "=="
    EQL             "<="
    EQG             ">="
    STL             "<"
    STG             ">"
    SUB             "-"
    ADD             "+"
    MUL             "*"
    DIV             "/"
    ASS             "="
    SCOLON          ";"
    OPEXPRBRACE     "("
    CLEXPRBRACE     ")"
    OPSCOPEBRACE    "{"
    CLSCOPEBRACE    "}"
    WHILE           "while"
    IF              "if"
    LEXERR
    <std::string> PRINT
    <std::string> SCAN
    <std::string> WORD
    <std::string> FUNC
    <int> NUMBER
    <int> ZERO
;

%left '+' '-' '*' '/'

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
    <AST::AbstractNode*> MATH_C
    <AST::AbstractNode*> MATH_Cw
    <AST::AbstractNode*> MATH_D
    <AST::AbstractNode*> MATH_Dw
    <AST::AbstractNode*> MATH_E
    <AST::AbstractNode*> MATH_Ew
    <AST::AbstractNode*> MATH_T
    <AST::AbstractNode*> MATH_Tw
    <AST::AbstractNode*> MATH_P
    <AST::AbstractNode*> MATH_Q
    <AST::AbstractNode*> SCOPE
    <AST::AbstractNode*> EXPRBRACE
    <AST::AbstractNode*> VAL
    <AST::AbstractNode*> VAR
    <AST::AbstractNode*> CALLFUNC
;

%start program

%%

program: STMS                                    { driver->insert($1); }

STMS: STM STMS                                   { $$ = AST::MakeORD($1, $2); }
    | %empty									 { $$ = nullptr; }
;

STM: ASSIGN                                      { $$ = $1; }
   | EXPR		                                 { $$ = $1; }
   | SCOPE                                       { $$ = $1; }
   | COND                                        { $$ = $1; }
   | CALLFUNC EXPR			                     { $$ = $1;
                                                   $1->left_ = $2; 
                                                   $2->prev_ = $$; }
   | CALLFUNC error								 { delete $1;
                                                   driver->push_error("expected arguments"); }
   | SCOLON                                      { $$ = nullptr; }
;

ASSIGN: VAR ASS EXPR         		             { $$ = AST::MakeOp($1, AST::OpType::ASS, $3); }
;

EXPR: MATH_C SCOLON                              { $$ = $1; }
    | MATH_C error								 { delete $1;
                                                   driver->push_error("expected ';' at the end of input"); }
;

MATH_C: MATH_D MATH_Cw                           { $$ = BindNodes ($1, $2); }
;

MATH_Cw: LOR MATH_D MATH_Cw                      { $$ = BindNodes ($2, $3, AST::OpType::LOR); }
       | LOR error                               { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_D: MATH_A MATH_Dw                           { $$ = BindNodes ($1, $2); }
;

MATH_Dw: LAND MATH_A MATH_Dw                     { $$ = BindNodes ($2, $3, AST::OpType::LAND); }
       | LAND error                              { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_A: MATH_B MATH_Aw                           { $$ = BindNodes ($1, $2); }
;

MATH_Aw: EQU MATH_B MATH_Aw                      { $$ = BindNodes ($2, $3, AST::OpType::EQU); }
       | EQU error                               { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_B: MATH_E MATH_Bw                           { $$ = BindNodes ($1, $2); }
;

MATH_Bw: STL MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, AST::OpType::STL); }
       | STG MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, AST::OpType::STG); }
       | EQL MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, AST::OpType::EQL); }
       | EQG MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, AST::OpType::EQG); }
       | STL error                               { driver->push_err_text("expeced primary-expression before "); }
       | STG error                               { driver->push_err_text("expeced primary-expression before "); }
       | EQL error                               { driver->push_err_text("expeced primary-expression before "); }
       | EQG error                               { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_E: MATH_T MATH_Ew                           { $$ = BindNodes ($1, $2); }
;

MATH_Ew: ADD MATH_T MATH_Ew                      { $$ = BindNodes ($2, $3, AST::OpType::ADD); }
       | SUB MATH_T MATH_Ew                      { $$ = BindNodes ($2, $3, AST::OpType::SUB); }
       | ADD error                               { driver->push_err_text("expeced primary-expression before "); }
       | SUB error                               { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_T: MATH_P MATH_Tw                           { $$ = BindNodes ($1, $2); }
;

MATH_Tw: MUL MATH_P MATH_Tw                      { $$ = BindNodes ($2, $3, AST::OpType::MUL); }
       | DIV MATH_P MATH_Tw                      { $$ = BindNodes ($2, $3, AST::OpType::DIV); }
       | MUL error 								 { driver->push_err_text("expeced primary-expression before "); }
       | DIV error                               { driver->push_err_text("expeced primary-expression before "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_P: MATH_Q									 { $$ = $1; }
      | ADD MATH_Q								 { $$ = AST::MakeOp(AST::MakeVal(0), AST::OpType::ADD, $2); }
      | SUB MATH_Q								 { $$ = AST::MakeOp(AST::MakeVal(0), AST::OpType::SUB, $2); }
      | SUB error								 { driver->push_err_text("expeced primary-expression before "); }
      | ADD error								 { driver->push_err_text("expeced primary-expression before "); }

;

MATH_Q: EXPRBRACE                                { $$ = $1; }
      | VAL                                      { $$ = $1; }
      | VAR                                      { $$ = $1; }
      | SCAN                                     { $$ = AST::MakeFunc($1); }


VAL: NUMBER                                      { $$ = AST::MakeVal($1); }
   | ZERO                                        { $$ = AST::MakeVal($1); }
;

VAR: WORD                                        { $$ = AST::MakeVar($1); }
;

CALLFUNC: FUNC                                   { $$ = AST::MakeFunc($1); }
        | PRINT                                  { $$ = AST::MakeFunc($1); }
;

SCOPE: OPSCOPEBRACE STMS CLSCOPEBRACE            { $$ = $2; }
     | OPSCOPEBRACE STMS error					 { driver->push_err_text("expeced ')' before "); }
;

EXPRBRACE: OPEXPRBRACE MATH_C CLEXPRBRACE        { $$ = $2; }
         | OPEXPRBRACE MATH_C error		         { driver->push_err_text("expeced '}' before "); }
;

COND: IF    EXPRBRACE SCOPE                      { $$ = AST::MakeCond($2, AST::CondType::IF,    $3);  }
    | WHILE EXPRBRACE SCOPE                      { $$ = AST::MakeCond($2, AST::CondType::WHILE, $3);  }
    | IF	error						 		 { driver->push_error("wrong syntax in 'if'");    }
    | WHILE	error								 { driver->push_error("wrong syntax in 'while'"); }
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

        if (lhs)
        {
            while (tmp->left_) tmp = tmp->left_;
            tmp->left_ = lhs;
            lhs->prev_ = tmp;
        }
    
        return rhs;
    }

    return lhs;
}

AST::AbstractNode* BindNodes(AST::AbstractNode* lhs, AST::AbstractNode* rhs, AST::OpType op_type)
{
    AST::AbstractNode* tmp = AST::MakeOp(nullptr, op_type, lhs);
    tmp = BindNodes(tmp, rhs);
    return tmp;
}

} /* namespace yy */
