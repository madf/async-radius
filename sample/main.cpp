#include "version.h"
#include "server.h"
#include "error.h"
#include <boost/asio.hpp>
#include <iostream>

using boost::system::error_code;

namespace
{

    void printHelp(const std::string& programName)
    {
        std::cout << "Usage: " << programName << " -s/--secret <secret> [-h/--help] [-v/--version]\n"
                  << "\t --secret, -s <secret> - shared secret for password encryption by client and server;\n"
                  << "\t --help, -h            - print this help;\n"
                  << "\t --version, -v         - print version.\n";
    }

    void printVersion(const std::string& programName)
    {
        std::cout << programName << "\n"
                  << "radproto" <<  " " << RADIUSD::version << "\n";
    }

}

int main(int argc, char* argv[])
{
    std::string secret;

    if (argc < 2)
    {
        std::cerr << "Needs a parameter secret - shared secret for password encryption by client and server.\n";
        return 1;
    }

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
        if (arg == "--secret" || arg == "-s")
        {
            if (i + 1 == argc)
            {
                std::cerr << arg << " needs an argument - a shared secret.\n";
                return 1;
            }
            secret = argv[++i];
        }
        else
        {
            std::cerr << "Unknown command line argument: " << arg << "\n";
            return 1;
        }
    }

    try
    {
        boost::asio::io_service io_service;
        Server server(io_service, secret, "/usr/share/freeradius/dictionary");
        io_service.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() <<"\n";
    }
    return 0;
}
