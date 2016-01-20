#include "StdAfx.h"
#include "CommandLineHelper.h"
#include "SyncMgr.h"


void main(int argc, char* argv[])
{
    CommandLineHelper helper( argc, argv );
    ParameterPtr parm = helper.get_parameter();

    if ( parm )
    {
        try
        {
            SyncMgr( parm ).sync();
        }
        catch ( std::exception& e )
        {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
}
