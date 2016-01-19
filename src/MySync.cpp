#include "StdAfx.h"
#include "MySync.h"


void MySync::sync()
{
    if ( !exists( m_src ) || !exists( m_dst ) )
    {
        return;
    }

    init_file_map( m_src, m_src_map, m_src_path_map, m_src_folder_set );
    init_file_map( m_dst, m_dst_map, m_dst_path_map, m_dst_folder_set );

    FileSet src_isolated_folders;
    FileSet dst_isolated_folders;
    std::set_difference( m_src_folder_set.begin(), m_src_folder_set.end(), m_dst_folder_set.begin(), m_dst_folder_set.end(), std::inserter(src_isolated_folders, src_isolated_folders.begin()) );
    std::set_difference( m_dst_folder_set.begin(), m_dst_folder_set.end(), m_src_folder_set.begin(), m_src_folder_set.end(), std::inserter(dst_isolated_folders, dst_isolated_folders.begin()) );

    for ( FileSet::iterator it = src_isolated_folders.begin(); it != src_isolated_folders.end(); ++it )
    {
        std::cout << "CREATE FOLDER: " << (m_dst / *it).string() << std::endl;
        create_directories( m_dst / *it );
    }

    for ( FileMap::iterator it = m_src_map.begin(); it != m_src_map.end(); ++it )
    {
        process_one_key( it->first );
    }

    for ( FilePath2KeyMap::iterator it = m_dst_path_map.begin(); it != m_dst_path_map.end(); ++it )
    {
        if ( m_src_path_map.find( it->first ) == m_src_path_map.end() )
        {
            std::cout << "REMOVE: " << (m_dst / it->first).string() << std::endl;
            remove( m_dst / it->first );
        }
    }

    for ( FileSet::iterator it = dst_isolated_folders.begin(); it != dst_isolated_folders.end(); ++it )
    {
        std::cout << "REMOVE FOLDER: " << (m_dst / *it).string() << std::endl;
        boost::system::error_code ec;
        remove( m_dst / *it, ec );
    }
}


void MySync::process_one_key( const FileKey& file_key )
{
    FileSet& file_set = m_src_map[file_key];
    FileMap::iterator find_it = m_dst_map.find( file_key );
    
    if ( find_it == m_dst_map.end() ) // REMOTE COPY
    {
        for ( FileSet::iterator it = file_set.begin(); it != file_set.end(); ++it )
        {
            boost::filesystem::path& p = *it;

            if ( m_dst_path_map.find( p ) != m_dst_path_map.end() ) // RENAME
            {
                this->file_rename( p );
            }

            std::cout << "COPY: " << (m_src / p).string() << " --> " << (m_dst / p).string() << std::endl;
            copy_file( m_src / p, m_dst / p );
            m_dst_map[file_key].insert( p );
            m_dst_path_map[p] = file_key;
        }
    }
    else // MOVE, LOCAL COPY
    {
        FileSet& dst_file_set = find_it->second;
        FileSet src_isolated;
        FileSet dst_isolated;

        std::set_difference( file_set.begin(), file_set.end(), dst_file_set.begin(), dst_file_set.end(), std::inserter(src_isolated, src_isolated.begin()) );
        std::set_difference( dst_file_set.begin(), dst_file_set.end(), file_set.begin(), file_set.end(), std::inserter(dst_isolated, dst_isolated.begin()) );

        for ( FileSet::iterator it = src_isolated.begin(); it != src_isolated.end(); ++it )
        {
            boost::filesystem::path& p = *it;

            if ( dst_isolated.empty() ) // LOCAL COPY
            {
                std::cout << "COPY: " << (m_dst / *dst_file_set.begin()).string() << " --> " << (m_dst / p).string() << std::endl;
                copy_file( m_dst / *dst_file_set.begin(),  m_dst / p );
                m_dst_map[file_key].insert( p );
                m_dst_path_map[p] = file_key;
            }
            else // MOVE
            {
                boost::filesystem::path& op = *dst_isolated.begin();

                std::cout << "RENAME: " << (m_dst / op).string() << " --> " << (m_dst / p).string() << std::endl;
                rename( m_dst / op, m_dst / p );

                m_dst_map[file_key].erase( op );
                m_dst_map[file_key].insert( p );
                m_dst_path_map.erase( op );
                m_dst_path_map[p] = file_key;

                dst_isolated.erase( dst_isolated.begin() );
            }
        }
    }
}


void MySync::init_file_map( const boost::filesystem::path& dir_path, FileMap& file_map, FilePath2KeyMap& file_path_map, FileSet& folder_set )
{
    boost::filesystem::path p( dir_path );
    boost::filesystem::recursive_directory_iterator it( p );
    boost::filesystem::recursive_directory_iterator end;

    for ( ; it != end; ++it )
    {
        boost::filesystem::path rp = boost::filesystem::relative( it->path(), dir_path );

        if ( ! boost::filesystem::is_directory( it->path() ) )
        {
            boost::uintmax_t size = boost::filesystem::file_size( it->path() );
            std::time_t last_write_time = boost::filesystem::last_write_time( it->path() );
            FileKey key( size, last_write_time );
            file_map[key].insert( rp );
            file_path_map[rp] = key;
            //std::cout << "<" << size << ", " << last_write_time << "> " << boost::filesystem::relative( it->path(), dir_path ).string() << std::endl;
        }
        else
        {
            folder_set.insert( rp );
        }
    }
}


bool MySync::file_binary_compare( const std::string& lhs, const std::string& rhs )
{
    std::vector<char> lhs_data;
    std::vector<char> rhs_data;

    read_file( lhs, lhs_data );
    read_file( rhs, rhs_data );

    return lhs_data == rhs_data;
}


void MySync::read_file( const std::string& filename, std::vector<char>& data )
{
    std::ifstream stream( filename.c_str(), std::ios::binary );
    stream.unsetf( std::ios::skipws );
    stream.seekg( 0, std::ios::end );
    size_t sz = stream.tellg();
    stream.seekg( 0 );
    data.resize( sz + 1 );
    stream.read( &data.front(), static_cast<std::streamsize>(sz) );
    data[sz] = 0;
}


boost::filesystem::path MySync::file_rename( const boost::filesystem::path& p )
{
    boost::filesystem::path np;
    boost::filesystem::path parent = p.parent_path();
    std::string name = p.stem().string();
    std::string extension = p.extension().string();
    FileKey& key = m_dst_path_map[p];

    for ( size_t i = 1; ; ++i )
    {
        std::stringstream strm;
        strm << name << "(" << i << ")" << extension;
        np = parent / strm.str();
        FilePath2KeyMap::iterator it = m_dst_path_map.find( np );

        if ( it == m_dst_path_map.end() )  // RENAME
        {
            std::cout << "RENAME: " << (m_dst / p).string() << " --> " << (m_dst / np).string() << std::endl;
            rename( m_dst / p, m_dst / np );

            m_dst_path_map[np] = key;
            m_dst_path_map.erase( p );
            m_dst_map[key].insert( np );
            m_dst_map[key].erase( p );
            break;
        }
    }

    return np;
}
