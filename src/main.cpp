#include "version.h"
#include "server.h"
#include <boost/asio.hpp>
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

    try
    {
        boost::asio::io_service io_service;
        Server server(io_service);
        io_service.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() <<"\n";
    }
    return 0;
}
