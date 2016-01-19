#include "StdAfx.h"
#include "CommandLineHelper.h"


CommandLineHelper::CommandLineHelper( int argc, char* argv[] )
{
    std::string src;
    std::string dst;

    if ( argc == 3 )
    {
        src = argv[1];
        dst = argv[2];

        if ( src == dst )
        {
            std::cout << "<src> <dst> are same." << std::endl;
        }
        else
        {
            if ( boost::filesystem::exists( src ) && boost::filesystem::exists( dst ) )
            {
                m_parameter = ParameterPtr( new Parameter( src, dst ) );
            }
            else
            {
                std::cout
                    << "invalid path: " << src << "\n"
                    << "              " << dst << "\n"
                    ;
            }
        }
    }
    else
    {
        display_usage();
    }
}


void CommandLineHelper::display_usage()
{
    std::cout << "Usage:\n";
    std::cout << "\tMySync <src> <dst>\n";
    std::cout << std::flush;
}
