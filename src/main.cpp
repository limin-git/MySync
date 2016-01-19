#include "StdAfx.h"
#include "CommandLineHelper.h"
#include "MySync.h"


void main(int argc, char* argv[])
{
    CommandLineHelper command_line_helper( argc, argv );
    ParameterPtr parm = command_line_helper.get_parameter();

    if ( parm )
    {
        try
        {
            MySync( parm->m_src, parm->m_dst ).sync();
        }
        catch ( std::exception& e )
        {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
}
