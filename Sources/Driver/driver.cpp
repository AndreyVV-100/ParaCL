#include "driver.hpp"
#include <fstream>
#include <string>

void parse (yy::Driver& driver);

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
        std::cout << "Pass the filename." << std::endl; // ToDo: help?
        return 0;
    }

    std::fstream program {argv[1], std::ios::in};

    if (!program.is_open())
    {
        std::cout << "Couldn't open file." << std::endl;
        return 0;
    }

    std::streambuf* cin_buff = std::cin.rdbuf(); // ToDo: Bad crutch, but my idea. How to fix it?
    std::cin.rdbuf (program.rdbuf());

    yy::Driver driver{argv[1]};

    try { parse (driver); }  
    catch (std::runtime_error e)
    { 
        std::cout << e.what() << '\n';
        return 0;
    }

    std::cin.rdbuf (cin_buff);

    if (!driver.err())
        driver.interpretate();

    driver.print_errors();

    return 0;
}

void parse (yy::Driver& driver)
{
    yy::parser parser (&driver);
    parser.parse();
}

namespace yy
{
    parser::token_type Driver :: yylex(parser::semantic_type *yylval, yy::parser::location_type* location)
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

    void Driver :: push_error (std::string error, yy::location location)
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

    void Driver :: interpretate ()
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

} // end of namespace yy

namespace semantic
{
    
} // end of namespace semantic
