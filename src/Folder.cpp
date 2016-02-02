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


PathStack Folder::get_path_stack( Path p )
{
    PathStack s;

    while ( p.has_parent_path() )
    {
        s.push( p.filename() );
        p = p.parent_path();
    }

    s.push( p.filename() );
    return s;   
}


Folder* Folder::remove_folder( const Path& p )
{
    std::pair<FolderMap*, FolderMap::iterator> pair = find_folder( p );

    if ( pair.first )
    {
        pair.first->erase( pair.second );
    }

    return pair.second->second;
}


File* Folder::remove_file( const Path& p )
{
    std::pair<FileMap*, FileMap::iterator> pair = find_file( p );

    if ( pair.first )
    {
        pair.first->erase( pair.second );
    }

    return pair.second->second;
}


std::pair<FolderMap*, FolderMap::iterator> Folder::find_folder( const Path& p )
{
    Folder* folder = this;
    FolderMap::iterator it;
    PathStack s = this->get_path_stack( p );

    while ( !s.empty() )
    {
        it = folder->m_folders.find( s.top() );

        if ( it == m_folders.end() )
        {
            return std::pair<FolderMap*, FolderMap::iterator>();
        }

        folder = it->second;
        s.pop();
    }

    return std::pair<FolderMap*, FolderMap::iterator>( &folder->m_folders, it );
}


std::pair<FileMap*, FileMap::iterator> Folder::find_file( const Path& p )
{
    Path parent = p;
    Folder* folder = this;

    if ( !parent.empty() )
    {
        FolderMap::iterator it;
        PathStack s = this->get_path_stack( parent );

        while ( !s.empty() )
        {
            it = folder->m_folders.find( s.top() );

            if ( it == m_folders.end() )
            {
                return std::pair<FileMap*, FileMap::iterator>();
            }

            folder = it->second;
            s.pop();
        }
    }

    Path filename = p.filename();
    FileMap::iterator it = folder->m_files.find( filename );

    if ( it == folder->m_files.end() )
    {
        return std::pair<FileMap*, FileMap::iterator>();
    }

    return std::pair<FileMap*, FileMap::iterator>( &folder->m_files, it );
}
