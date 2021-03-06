#include "StdAfx.h"
#include "CommandLineHelper.h"


CommandLineHelper::CommandLineHelper( int argc, char* argv[] )
{
    boost::program_options::options_description desc( "Options", 100 );
    desc.add_options()
        ( "help,?", "produce help message" )
        ( "config,C", boost::program_options::value<std::string>(),  "config file" )
        ( "src,S", boost::program_options::value<std::string>(),  "source path" )
        ( "dst,D", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "destination path list" )
        ( "include-files", boost::program_options::value<std::string>(), "filter include files" )
        ( "exclude-files", boost::program_options::value<std::string>(), "filter exclude files" )
        ( "include-folders", boost::program_options::value<std::string>(), "filter include folders" )
        ( "exclude-folders", boost::program_options::value<std::string>(), "filter exclude folders" )
        ;

    boost::program_options::variables_map vm;
    store( boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm );
    notify( vm );

    if ( vm.count( "help" ) )
    {
        std::cout << desc << std::endl;
        return;
    }

    if ( vm.count( "config" ) )
    {
        std::string config_file = vm["config"].as<std::string>();

        if ( boost::filesystem::exists( config_file ) )
        {
            store( boost::program_options::parse_config_file<char>( config_file.c_str(), desc, true ), vm );
            notify( vm );
        }
    }

    std::string src;
    std::vector<std::string> dst;

    if ( vm.count( "src" ) )
    {
        src = vm["src"].as<std::string>();
    }

    if ( vm.count( "dst" ) )
    {
        dst = vm["dst"].as<std::vector<std::string> >();
    }

    if ( src.empty() || dst.empty() )
    {
        return;
    }

    m_parameter.reset( new Parameter(src, dst) );

    if ( vm.count( "include-files" ) )
    {
        boost::split( m_parameter->m_include_files, vm["include-files"].as<std::string>(), boost::is_any_of(" \t,:;" ), boost::token_compress_on );
    }

    if ( vm.count( "exclude-files" ) )
    {
        boost::split( m_parameter->m_exclude_files, vm["exclude-files"].as<std::string>(), boost::is_any_of(" \t,:;"), boost::token_compress_on );
    }

    if ( vm.count( "include-folders" ) )
    {
        boost::split( m_parameter->m_include_folders, vm["include-folders"].as<std::string>(), boost::is_any_of(" \t,:;"), boost::token_compress_on );
    }

    if ( vm.count( "exclude-folders" ) )
    {
        boost::split( m_parameter->m_exclude_folders, vm["exclude-folders"].as<std::string>(), boost::is_any_of(" \t,:;"), boost::token_compress_on );
    }
}
