#pragma once

#include <iostream>
#include <numeric>
#include <cstring>
#include <fstream>
#include <memory>

#include <FlexLexer.h>
#include "parser.tab.hh"

#include "interpreter.hpp"
#include "lexer.hpp"

namespace yy {

class Driver final {
    std::unique_ptr<ParaCLlexer> plex;
    AST::Tree tree;
    interpreter::interpreter interpreter_;
    std::vector<std::string> program;
    std::vector<std::string> errors;

public:
    Driver(const char* filename): plex{new ParaCLlexer}, tree{}, interpreter_{}
    {
        std::fstream inputfile{filename, std::ios_base::in};

        while (inputfile)
        {
            std::string line;
            std::getline(inputfile, line);
            program.push_back(line);
        }
    }

    parser::token_type yylex(parser::semantic_type *yylval, yy::parser::location_type* location) // ToDo: move to driver.cpp?
    {
        *location = plex->getlocation();

        parser::token_type tt = static_cast<parser::token_type>(plex->yylex());

        switch (tt)
        {
            case yy::parser::token_type::LOR:
                yylval->build<AST::OpType>() = AST::OpType::LOR;
                break;
            case yy::parser::token_type::LAND:
                yylval->build<AST::OpType>() = AST::OpType::LAND;
                break;
            case yy::parser::token_type::EQN:
                yylval->build<AST::OpType>() = AST::OpType::EQN;
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
                yylval->build<int>() = std::stoi(plex->YYText());
                break;
            case yy::parser::token_type::WORD:
            case yy::parser::token_type::PRINT:
            case yy::parser::token_type::SCAN:
            case yy::parser::token_type::FUNC:
                yylval->build<std::string>() = plex->YYText();
                break;
            case yy::parser::token_type::LEXERR:
                throw std::runtime_error(std::string("unrecognized symbol: ") + plex->YYText());
        }

        return tt;
    }

    void insert(AST::AbstractNode* other) { tree.ChangeTop (other); }

    void push_error (std::string error, yy::location location)
    {
        std::string column{"\n"};
        column.insert(1, location.begin.column + 9, '~');
        column.push_back('^');

        errors.push_back (
               "line: " 	+ std::to_string(plex->lineno()) +
               " | error: " + error + "\n\t| " + 
               program[plex->lineno() - 1] + column
        );
    }

    void push_err_text(std::string error, yy::location location)
    {
        if (std::strlen(plex->YYText()))
            push_error (error + "before \"" + plex->YYText() + "\"", location);
        else 
            push_error (error + "at the end of input", location);
    }

    bool err() { return !errors.empty(); }

    void print_errors() 
    {
        for (auto&& err: errors)
            std::cout << err << '\n';
    }

    int getlineno() { return plex->lineno(); }

    void interpretate ()
    {
        try { interpreter_.interpretate (interpreter_.global, tree.GetTop()); }
        catch (std::runtime_error e)
        { 
            std::cout << e.what() << '\n';
            errors.push_back (
               "line: " 	+ std::to_string(interpreter_.error_line_number) +
               " | error: " + interpreter_.error_message + "\t| " + 
               program[interpreter_.error_line_number - 1]
            );
        }
    }

    void printout() const { tree.PrintTree ("graph.dot"); }

    ~Driver() = default;
};

} // namespace yy
