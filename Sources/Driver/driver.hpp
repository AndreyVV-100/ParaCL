#pragma once

#include <iostream>
#include <numeric>

#include "parser.tab.hh"
#include <FlexLexer.h>
#include "interpretator.hpp"
#include <cstring>

namespace yy {

class Driver final {
    FlexLexer *plex_;
    AST::Tree tree;
    std::vector<std::string> errors;
public:
    Driver(): plex_(new yyFlexLexer), tree{} {}

    parser::token_type yylex(parser::semantic_type *yylval) // ToDo: move to driver.cpp?
    {
        parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());

        switch (tt)
        {
            case yy::parser::token_type::LOR:
                yylval->build<AST::OpType>() = AST::OpType::LOR;
                break;
            case yy::parser::token_type::LAND:
                yylval->build<AST::OpType>() = AST::OpType::LAND;
                break;
            case yy::parser::token_type::EQU:
                yylval->build<AST::OpType>() = AST::OpType::EQU;
                break;
            case yy::parser::token_type::EQL:
                yylval->build<AST::OpType>() = AST::OpType::EQL;
                break;
            case yy::parser::token_type::EQG:
                yylval->build<AST::OpType>() = AST::OpType::EQG;
                break;
            case yy::parser::token_type::STL:
                yylval->build<AST::OpType>() = AST::OpType::STL;
                break;
            case yy::parser::token_type::STG:
                yylval->build<AST::OpType>() = AST::OpType::STG;
                break;
            case yy::parser::token_type::SUB:
                yylval->build<AST::OpType>() = AST::OpType::SUB;
                break;
            case yy::parser::token_type::ADD:
                yylval->build<AST::OpType>() = AST::OpType::ADD;
                break;
            case yy::parser::token_type::MUL:
                yylval->build<AST::OpType>() = AST::OpType::MUL;
                break;
            case yy::parser::token_type::DIV:
                yylval->build<AST::OpType>() = AST::OpType::DIV;
                break;
            case yy::parser::token_type::ASS:
                yylval->build<AST::OpType>() = AST::OpType::ASS;
                break;
            case yy::parser::token_type::NUMBER:
            case yy::parser::token_type::ZERO:			
                yylval->build<int>() = std::stoi(plex_->YYText());
                break;
            case yy::parser::token_type::WORD:
            case yy::parser::token_type::PRINT:
            case yy::parser::token_type::SCAN:
            case yy::parser::token_type::FUNC:
                yylval->build<std::string>() = plex_->YYText();
                break;
            case yy::parser::token_type::LEXERR:
                push_err_text (std::string("unrecognized symbol: ") + plex_->YYText());
        }

        return tt;
    }

    void parse()
    {
        parser parser(this);
        parser.parse();

        if (errors.size()) 
            throw errors;
    }

    void insert(AST::AbstractNode* other) { tree.top_ = other; }

    void push_error (std::string error)
    {
        errors.push_back (
               "line: " 	+ std::to_string(plex_->lineno()) +
               " | error: " + error
        );
    }

    void push_err_text(std::string error)
    {
        if (std::strlen(plex_->YYText()))
            push_error (error + "before \"" + plex_->YYText() + "\"");
        else 
            push_error (error + "at the end of input");
    }

    int getlineno() { return plex_->lineno(); }

    void interpretate ()
    {
        interpretator::start_interpretate (tree);
    }

    void printout() const { tree.PrintTree ("graph.dot"); }

    ~Driver() { delete plex_; }
};

} // namespace yy
