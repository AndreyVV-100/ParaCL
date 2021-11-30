#include "driver.hpp"
#include <fstream>

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) 
{
    // if (argc != 2)
    // {
    //     std::cout << "Not enough arguments" << std::endl;
    //     return 0;
    // }

    // std::fstream code_file(argv[1]);

    yy::Driver driver{};

    try
    {
        driver.parse();
    }  
    catch(std::string err)
    {
        std::cerr << err << std::endl;
    }
    
    driver.printout();

    try
    {
        driver.interpretate();
    }
    catch(interpretator::ERRORS e)
    {
        std::cerr << static_cast <int> (e) << '\n';
    }

    return 0;
}