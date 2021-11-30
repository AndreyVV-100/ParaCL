#include "driver.hpp"
#include <fstream>
#include <string>

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
        std::cout << "Pass the filename" << std::endl;
        return 1;
    }

    std::fstream program {argv[1], std::ios::in};

    if (!program.is_open())
    {
        std::cout << "Couldn't open file" << std::endl;
        return 1;
    }

    std::streambuf* cin_buff = std::cin.rdbuf();
    std::cin.rdbuf (program.rdbuf());

    yy::Driver driver{};

    try { driver.parse(); }  
    catch(std::string err) { std::cerr << err << std::endl; }
    
    driver.printout();
    std::cin.rdbuf (cin_buff);

    try { driver.interpretate(); }
    catch(interpretator::ERRORS e) 
    { 
        std::cerr << "Interpretator error: " 
                  << static_cast <int> (e) << '\n'; 
    }

    return 0;
}