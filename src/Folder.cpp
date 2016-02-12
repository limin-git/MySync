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


void Folder::assign( const Path& dir )
{
    m_name = dir.filename();
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


void Folder::get_key( const Path& base, PathKeyMap& path_key_map )
{
    //BOOST_FOREACH( FileMap::value_type& v, m_files )
    //{
    //    Path p = base / v.first;
    //}

    //BOOST_FOREACH( FolderMap::value_type& v, m_folders )
    //{
    //    const Path& folder_path = v.first;
    //}
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
    Folder* folder = NULL;
    Folder* parent = this->find_folder_parent( p );

    if ( parent )
    {
        FolderMap::iterator it = parent->m_folders.find( p.filename() );
        folder = it->second;
        parent->m_folders.erase( it );
    }

    return folder;
}


File* Folder::remove_file( const Path& p )
{
    File* file = NULL;
    Folder* parent = this->find_file_parent( p );

    if ( parent )
    {
        FileMap::iterator it = parent->m_files.find( p.filename() );
        file = it->second;
        parent->m_files.erase( it );
    }

    return file;
}


Folder* Folder::find_folder_parent( const Path& p )
{
    Folder* folder = this;
    PathStack s = this->get_path_stack( p );

    while ( !s.empty() )
    {
        FolderMap::iterator it = folder->m_folders.find( s.top() );

        if ( it == m_folders.end() )
        {
            return NULL;
        }

        folder = it->second;
        s.pop();
    }

    return folder;
}


Folder* Folder::find_file_parent( const Path& p )
{
    Folder* folder = this;
    Path parent = p;

    if ( !parent.empty() )
    {
        FolderMap::iterator it;
        PathStack s = this->get_path_stack( parent );

        while ( !s.empty() )
        {
            it = folder->m_folders.find( s.top() );

            if ( it == m_folders.end() )
            {
                return NULL;
            }

            folder = it->second;
            s.pop();
        }
    }

    if ( folder->m_files.find( p.filename() ) == folder->m_files.end() )
    {
        return NULL;
    }

    return folder;
}


Folder* Folder::create_directory( const Path& p )
{
    Folder* folder = this;
    PathStack s = this->get_path_stack( p );

    while ( !s.empty() )
    {
        FolderMap::iterator it = folder->m_folders.find( s.top() );

        if ( it == m_folders.end() )
        {
            Folder* new_folder = new Folder;
            new_folder->assign( s.top() );
            it = folder->m_folders.insert( FolderMap::value_type( s.top(), new_folder ) ).first;
        }

        folder = it->second;
        s.pop();
    }

    return folder;
}


void Folder::copy_folder( const Path& p, Folder* folder )
{
    Folder* parent = this->create_directory( p.parent_path() );
    parent->m_folders[p.filename()] = folder;
}


void Folder::copy_file( const Path& p, File* file )
{
    Folder* parent = this->create_directory( p.parent_path() );
    parent->m_files[p.filename()] = file;
}


void Folder::rename_file( const Path& from, const Path& to )
{
    File* file = this->remove_file( from );

    if ( file )
    {
        this->copy_file( to, file );
    }
}


void Folder::rename_folder( const Path& from, const Path& to )
{
    Folder* folder = this->remove_folder( from );

    if ( folder )
    {
        this->copy_folder( to, folder );
    }
}
