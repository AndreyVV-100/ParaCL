%option c++
%option yylineno
%option yyclass="ParaCLlexer"

%{ 
#include "parser.tab.hh"
#include "lexer.hpp"
#define YY_USER_ACTION setlocation();
int yyFlexLexer::yywrap() { return 1; }
%}

WS      		[ \t\n\v]+
DIGIT   		[0-9]
DIGIT1  		[1-9]
WORD  			[_a-zA-Z]+{DIGIT}*


%%

"//".*
{WS}                        /* skip blanks and tabs */
"||"                    { return yy::parser::token_type::LOR;          }
"&&"                    { return yy::parser::token_type::LAND;         }
"!="                    { return yy::parser::token_type::EQN;          }
"=="                    { return yy::parser::token_type::EQU;          }
"<="                    { return yy::parser::token_type::EQL;          }
">="                    { return yy::parser::token_type::EQG;          }
"<"                     { return yy::parser::token_type::STL;          }
">"                     { return yy::parser::token_type::STG;          }
"+"                     { return yy::parser::token_type::ADD;          }
"-"                     { return yy::parser::token_type::SUB;          }
"*"                     { return yy::parser::token_type::MUL;          }
"/"                     { return yy::parser::token_type::DIV;          }
"="                     { return yy::parser::token_type::ASS;          }
";"                     { return yy::parser::token_type::SCOLON;       }
"{"		                { return yy::parser::token_type::OPSCOPEBRACE; }
"}"		                { return yy::parser::token_type::CLSCOPEBRACE; }
"("		                { return yy::parser::token_type::OPEXPRBRACE;  }
")"		                { return yy::parser::token_type::CLEXPRBRACE;  }
"0"                     { return yy::parser::token_type::ZERO;         }
"print"                 { return yy::parser::token_type::PRINT;        }
"?"                     { return yy::parser::token_type::SCAN;         }
"while"			        { return yy::parser::token_type::WHILE;        }
"if"	    	        { return yy::parser::token_type::IF;           }
{WORD}                  { return yy::parser::token_type::WORD;         }
{DIGIT1}{DIGIT}*        { return yy::parser::token_type::NUMBER;       }
{DIGIT}+{WORD}          { return yy::parser::token_type::LEXERR;       }
"0"+{DIGIT}+            { return yy::parser::token_type::LEXERR;       }
.			            { return yy::parser::token_type::LEXERR;       }

%%

// nothing
