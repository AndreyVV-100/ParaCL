#include "driver.hpp"
#include <fstream>
#include <string>

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

    try { driver.parse(); }  
    catch (std::runtime_error e)
    { 
        std::cout << e.what() << '\n';
        return 0;
    }

    std::cin.rdbuf (cin_buff);

    if (driver.parse_err())
        driver.print_errors();
    else
    {
        try { driver.interpretate(); }  
        catch(std::runtime_error e)
        { 
            std::cout << e.what() << '\n';
            driver.print_errors();
        }
    }

    return 0;
}
