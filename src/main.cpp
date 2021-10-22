#include "version.h"

#include <iostream>

namespace
{

void printHelp(const std::string& programName)
{
    std::cout << "Usage: " << programName << " [options]\n"
              << "\t --help, -h    - print this help;\n"
              << "\t --version, -v - print version.\n";
}

void printVersion(const std::string& programName)
{
    std::cout << programName << " " << RADIUSD::version << "\n";
}

}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h")
        {
            printHelp(argv[0]);
            return 0;
        }
        if (arg == "--version" || arg == "-v")
        {
            printVersion(argv[0]);
            return 0;
        }
        std::cerr << "Unknown option: " << arg << "\n";
    }
    return 0;
}
