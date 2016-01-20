#include "StdAfx.h"
#include "CommandLineHelper.h"


CommandLineHelper::CommandLineHelper( int argc, char* argv[] )
{
    boost::program_options::options_description desc( "Options", 100 );
    desc.add_options()
        ( "help,?", "produce help message" )
        ( "config,C", boost::program_options::value<std::string>(),  "config file" )
        ( "src,S", boost::program_options::value<std::string>(),  "source path" )
        ( "dests,D", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "destination path list" )
        ( "include-files", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "filter include files" )
        ( "exclude-files", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "filter exclude files" )
        ( "include-folders", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "filter include folders" )
        ( "exclude-folders", boost::program_options::value< std::vector<std::string> >()->multitoken(),  "filter exclude folders" )
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
    std::vector<std::string> dests;

    if ( vm.count( "src" ) )
    {
        src = vm["src"].as<std::string>();
    }

    if ( vm.count( "dests" ) )
    {
        dests = vm["dests"].as<std::vector<std::string> >();
    }

    if ( src.empty() || dests.empty() )
    {
        return;
    }

    m_parameter.reset( new Parameter(src, dests) );

    if ( vm.count( "include-files" ) )
    {
        m_parameter->m_include_files = vm["include-files"].as<std::vector<std::string> >();
    }

    if ( vm.count( "exclude-files" ) )
    {
        m_parameter->m_exclude_files = vm["exclude-files"].as<std::vector<std::string> >();
    }

    if ( vm.count( "include-folders" ) )
    {
        m_parameter->m_include_folders = vm["include-folders"].as<std::vector<std::string> >();
    }

    if ( vm.count( "exclude-folders" ) )
    {
        m_parameter->m_exclude_folders = vm["exclude-folders"].as<std::vector<std::string> >();
    }
}
