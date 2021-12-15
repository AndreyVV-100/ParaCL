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
                                                     AST::OpType op_type, int lineno);
}
}

%token
    SCOLON                  ";"
    OPEXPRBRACE             "("
    CLEXPRBRACE             ")"
    OPSCOPEBRACE            "{"
    CLSCOPEBRACE            "}"
    WHILE                   "while"
    IF                      "if"
    <AST::OpType> ASS       "="
    <AST::OpType> LOR       "||"
    <AST::OpType> LAND      "&&"
    <AST::OpType> EQU       "=="
    <AST::OpType> EQL       "<="
    <AST::OpType> EQG       ">="
    <AST::OpType> STL       "<"
    <AST::OpType> STG       ">"
    <AST::OpType> SUB       "-"
    <AST::OpType> ADD       "+"
    <AST::OpType> MUL       "*"
    <AST::OpType> DIV       "/"
    <std::string> PRINT
    <std::string> SCAN
    <std::string> WORD
    <std::string> FUNC
    <int> NUMBER
    <int> ZERO
    LEXERR
;

%left '+' '-' '*' '/'

%nterm 
    <AST::AbstractNode*> STMS
    <AST::AbstractNode*> STM
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
    <AST::AbstractNode*> MATH_Z
    <AST::AbstractNode*> MATH_Zw
    <AST::AbstractNode*> SCOPE
    <AST::AbstractNode*> EXPRBRACE
    <AST::AbstractNode*> VAL
    <AST::AbstractNode*> VAR
    <AST::AbstractNode*> CALLFUNC
;

%start program

/* %destructor { delete $$; } <AST::AbstractNode*> */

%%

program: STMS                                    { driver->insert($1); }

STMS: STM STMS                                   { $$ = AST::MakeORD($1, $2, driver->getlineno()); }
    | %empty                                     { $$ = nullptr; }
;

STM: EXPR		                                 { $$ = $1; }
   | SCOPE                                       { $$ = $1; }
   | COND                                        { $$ = $1; }
   | CALLFUNC EXPR                               { $$ = $1;
                                                   $1->left_ = $2; 
                                                   $2->prev_ = $$; }
   | CALLFUNC error                              { driver->push_error("expected arguments in function"); 
                                                   delete $1;      }
   | SCOLON                                      { $$ = nullptr;   }
;

EXPR: MATH_Z SCOLON                              { $$ = $1; }
    | MATH_Z error                               { delete $1;
                                                   driver->push_err_text("expected ';' "); }
;

MATH_Z: MATH_C MATH_Zw                           { $$ = BindNodes ($1, $2); }
;

MATH_Zw: ASS MATH_C MATH_Zw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | ASS error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_C: MATH_D MATH_Cw                           { $$ = BindNodes ($1, $2); }
;

MATH_Cw: LOR MATH_D MATH_Cw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | LOR error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_D: MATH_A MATH_Dw                           { $$ = BindNodes ($1, $2); }
;

MATH_Dw: LAND MATH_A MATH_Dw                     { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | LAND error                              { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_A: MATH_B MATH_Aw                           { $$ = BindNodes ($1, $2); }
;

MATH_Aw: EQU MATH_B MATH_Aw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | EQU error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_B: MATH_E MATH_Bw                           { $$ = BindNodes ($1, $2); }
;

MATH_Bw: STL MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | STG MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | EQL MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | EQG MATH_E MATH_Bw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | STL error                               { driver->push_err_text("expected primary-expression "); }
       | STG error                               { driver->push_err_text("expected primary-expression "); }
       | EQL error                               { driver->push_err_text("expected primary-expression "); }
       | EQG error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_E: MATH_T MATH_Ew                           { $$ = BindNodes ($1, $2); }
;

MATH_Ew: ADD MATH_T MATH_Ew                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | SUB MATH_T MATH_Ew                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | ADD error                               { driver->push_err_text("expected primary-expression "); }
       | SUB error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_T: MATH_P MATH_Tw                           { $$ = BindNodes ($1, $2); }
;

MATH_Tw: MUL MATH_P MATH_Tw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | DIV MATH_P MATH_Tw                      { $$ = BindNodes ($2, $3, $1, driver->getlineno()); }
       | MUL error                               { driver->push_err_text("expected primary-expression "); }
       | DIV error                               { driver->push_err_text("expected primary-expression "); }
       | %empty                                  { $$ = nullptr; }
;

MATH_P: MATH_Q                                   { $$ = $1; }
      | ADD MATH_Q                               { $$ = AST::MakeOp(AST::MakeVal(0, driver->getlineno()), $1, $2, driver->getlineno()); }
      | SUB MATH_Q                               { $$ = AST::MakeOp(AST::MakeVal(0, driver->getlineno()), $1, $2, driver->getlineno()); }
      | SUB error                                { driver->push_err_text("expected primary-expression "); }
      | ADD error                                { driver->push_err_text("expected primary-expression "); }
      | ASS error                                { driver->push_error("expected primary-expression before token '='" ); }
      | LOR error                                { driver->push_error("expected primary-expression before token '||'"); }
      | EQU error                                { driver->push_error("expected primary-expression before token '=='"); }
      | EQL error                                { driver->push_error("expected primary-expression before token '<='"); }
      | EQG error                                { driver->push_error("expected primary-expression before token '>='"); }
      | STL error                                { driver->push_error("expected primary-expression before token '<'" ); }
      | STG error                                { driver->push_error("expected primary-expression before token '>'" ); }
      | MUL error                                { driver->push_error("expected primary-expression before token '*'" ); }
      | DIV error                                { driver->push_error("expected primary-expression before token '/'" ); }
      | LAND error                               { driver->push_error("expected primary-expression before token '&&'"); }
;

MATH_Q: EXPRBRACE                                { $$ = $1; }
      | VAL                                      { $$ = $1; }
      | VAR                                      { $$ = $1; }
      | SCAN                                     { $$ = AST::MakeFunc($1, driver->getlineno()); }
;

VAL: NUMBER                                      { $$ = AST::MakeVal($1, driver->getlineno()); }
   | ZERO                                        { $$ = AST::MakeVal($1, driver->getlineno()); }
;

VAR: WORD                                        { $$ = AST::MakeVar($1, driver->getlineno()); }
;

CALLFUNC: FUNC                                   { $$ = AST::MakeFunc($1, driver->getlineno()); }
        | PRINT                                  { $$ = AST::MakeFunc($1, driver->getlineno()); }
;

SCOPE: OPSCOPEBRACE STMS CLSCOPEBRACE            { $$ = $2; }
  /* | CLSCOPEBRACE error                        { driver->push_error("expected primary-expression before token '}'"); } */
     | OPSCOPEBRACE STMS error                   { driver->push_err_text("expected '}' ");
                                                   delete $2; }
;

EXPRBRACE: OPEXPRBRACE MATH_Z CLEXPRBRACE        { $$ = $2; }
         | CLEXPRBRACE error                     { driver->push_error("expected primary-expression before token ')'"); }
         | OPEXPRBRACE MATH_Z error              { driver->push_err_text("expected ')' "); 
                                                   delete $2; }
;

COND: IF    EXPRBRACE SCOPE                      { $$ = AST::MakeCond($2, AST::CondType::IF,    $3, driver->getlineno());  }
    | WHILE EXPRBRACE SCOPE                      { $$ = AST::MakeCond($2, AST::CondType::WHILE, $3, driver->getlineno());  }
    | IF	error                                { driver->push_error("wrong syntax in 'if'");    }
    | WHILE	error                                { driver->push_error("wrong syntax in 'while'"); }
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

AST::AbstractNode* BindNodes(AST::AbstractNode* lhs, AST::AbstractNode* rhs, AST::OpType op_type, int lineno)
{
    AST::AbstractNode* tmp = AST::MakeOp(nullptr, op_type, lhs, lineno);
    tmp = BindNodes(tmp, rhs);
    return tmp;
}
} /* namespace yy */
