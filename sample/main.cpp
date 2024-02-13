#include "version.h"
#include "server.h"
#include "error.h"
#include <boost/asio.hpp>
#include <stdlib.h>
#include <iostream>
#include <cstdint> //uint8_t, uint32_t

using boost::system::error_code;

namespace
{

    void printHelp(const std::string& programName)
    {
        std::cout << "Usage: " << programName << " -s/--secret <secret> [-p/--port <port>] [-h/--help] [-v/--version]\n"
                  << "\t --secret, -s <secret> - shared secret for password encryption by client and server;\n"
                  << "\t --port, -p <port>     - port number for the socket;\n"
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
    uint16_t port = 1812;

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
        else if (arg == "--version" || arg == "-v")
        {
            printVersion(argv[0]);
            return 0;
        }
        else if (arg == "--port" || arg == "-p")
        {
            if (i + 1 == argc)
            {
                std::cerr << arg << " required argument - port number.\n";
                return 1;
            }
            port = atoi(argv[++i]);
        }
        else if (arg == "--secret" || arg == "-s")
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
        Server server(io_service, secret, port, "/usr/share/freeradius/dictionary");
        io_service.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() <<"\n";
    }
    return 0;
}
