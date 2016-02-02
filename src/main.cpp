#include "StdAfx.h"
#include "CommandLineHelper.h"
#include "SyncMgr.h"
#include "Folder.h"


void main(int argc, char* argv[])
{
    CommandLineHelper helper( argc, argv );
    ParameterPtr parm = helper.get_parameter();

    if ( parm )
    {
        //Folder folder;
        //folder.initialize( parm->m_src );
        //PathSet folders, files;
        //folder.collect_folders( folders );
        //folder.collect_files( files );
        //return;

        try
        {
            SyncMgr( parm ).sync();
            std::cout << std::flush;
        }
        catch ( std::exception& e )
        {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
}
