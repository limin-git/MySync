#include "StdAfx.h"
#include "CommandLineHelper.h"
#include "MySync.h"


void main(int argc, char* argv[])
{
    CommandLineHelper command_line_helper( argc, argv );
    ParameterPtr parm = command_line_helper.get_parameter();

    if ( parm )
    {
        MySync my( parm->m_src, parm->m_dst );
        my.sync();
    }

    //if ( parm )
    //{
    //    std::cout << parm->m_src << std::endl;
    //    std::cout << parm->m_dst << std::endl;
    //}
}
