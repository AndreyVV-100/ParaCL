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

    parser::token_type yylex(parser::semantic_type *yylval, yy::parser::location_type* location);

    void insert(AST::AbstractNode* other) { tree.ChangeTop (other); }

    void push_error (std::string error, yy::location location);

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

    void interpretate ();

    void printout() const { tree.PrintTree ("graph.dot"); }

    ~Driver() = default;
};

} // namespace yy
