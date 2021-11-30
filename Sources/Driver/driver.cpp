#include "driver.hpp"

int yyFlexLexer::yywrap() { return 1; }

int main() 
{
    yy::Driver driver{};
    
    try
    {
        driver.parse();
    }
    catch(std::string err)
    {
        std::cerr << err << std::endl;
    }
    
//    driver.interpretate();

    return 0;
}