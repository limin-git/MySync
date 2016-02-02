#include "StdAfx.h"
#include "Folder.h"
#include "File.h"


void Folder::initialize( const Path& dir )
{
    m_name = dir.filename();
    //std::cout << m_name << std::endl;

    boost::filesystem::directory_iterator it( dir );
    boost::filesystem::directory_iterator end;

    for ( ; it != end; ++it )
    {
        const Path& p = it->path();

        if ( is_directory( p ) )
        {
            m_folders.insert( FolderMap::value_type( p.filename(), Folder::create_folder( p ) ) );
        }
        else if ( boost::filesystem::is_regular_file( p ) )
        {
            m_files.insert( FileMap::value_type( p.filename(), File::create_file( p ) ) );
        }
        else
        {
            NULL;
        }
    }
}


Folder* Folder::create_folder( const Path& dir )
{
    Folder* folder = new Folder;
    folder->initialize( dir );
    return folder;
}


void Folder::scan( const Path& base, PathSet& folders, PathSet& files, PathSet& invalid_folders, PathSet& invalid_files, const Filter& filter )
{
    BOOST_FOREACH( FileMap::value_type& v, m_files )
    {
        Path p = base / v.first;

        if ( filter.is_file_valid( p ) )
        {
            files.insert( p );
        }
        else
        {
            invalid_files.insert( p );
        }
    }

    BOOST_FOREACH( FolderMap::value_type& v, m_folders )
    {
        const Path& folder_path = v.first;
        Folder* folder = v.second;
        Path p = base / folder_path;

        if ( filter.is_folder_valid(p) )
        {
            folders.insert( p );
        }
        else
        {
            invalid_folders.insert( p );
        }

        folder->scan( p, folders, files, invalid_folders, invalid_files, filter );
    }
}
