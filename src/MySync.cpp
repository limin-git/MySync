#include "StdAfx.h"
#include "MySync.h"


boost::system::error_code ec;


void MySync::sync()
{
    if ( !exists( m_src ) || !exists( m_dst ) )
    {
        return;
    }

    scan_folder( m_src, m_src_map, m_src_path_map, m_src_folder_set );
    scan_folder( m_dst, m_dst_map, m_dst_path_map, m_dst_folder_set );

    FileSet src_isolated_folders;
    FileSet dst_isolated_folders;
    std::set_difference( m_src_folder_set.begin(), m_src_folder_set.end(), m_dst_folder_set.begin(), m_dst_folder_set.end(), std::inserter(src_isolated_folders, src_isolated_folders.begin()) );
    std::set_difference( m_dst_folder_set.begin(), m_dst_folder_set.end(), m_src_folder_set.begin(), m_src_folder_set.end(), std::inserter(dst_isolated_folders, dst_isolated_folders.begin()) );

    BOOST_FOREACH( boost::filesystem::path& p, src_isolated_folders ) // CREATE FOLDER
    {
        std::cout << "CREATE FOLDER: " << (m_dst / p).string() << std::endl;
        create_directories( m_dst / p, ec );
    }

    BOOST_FOREACH( FileMap::value_type& v, m_src_map ) // SYNC
    {
        this->process_one_key( v.first );
    }

    BOOST_FOREACH( FilePath2KeyMap::value_type& v, m_dst_path_map ) // REMOVE FILES
    {
        if ( m_src_path_map.find( v.first ) == m_src_path_map.end() )
        {
            std::cout << "REMOVE: " << (m_dst / v.first).string() << std::endl;
            remove( m_dst / v.first, ec );
        }
    }

    BOOST_FOREACH( boost::filesystem::path& p, dst_isolated_folders ) // REMOVE FOLDERS
    {
        std::cout << "REMOVE FOLDER: " << (m_dst / p).string() << std::endl;
        remove_all( m_dst / p, ec );
    }
}


void MySync::process_one_key( const FileKey& file_key )
{
    FileSet& src_file_set = m_src_map[file_key];
    FileMap::iterator find_it = m_dst_map.find( file_key );
    
    if ( find_it == m_dst_map.end() ) // REMOTE COPY
    {
        BOOST_FOREACH( boost::filesystem::path& p, src_file_set )
        {
            if ( m_dst_path_map.find( p ) != m_dst_path_map.end() ) // RENAME
            {
                this->my_rename( p );
            }

            std::cout << "COPY: " << (m_src / p).string() << " --> " << (m_dst / p).string() << std::endl;
            copy_file( m_src / p, m_dst / p, ec );
            m_dst_map[file_key].insert( p );
            m_dst_path_map[p] = file_key;
        }
    }
    else // MOVE, LOCAL COPY
    {
        FileSet& dst_file_set = find_it->second;
        FileSet src_isolated;
        FileSet dst_isolated;

        std::set_difference( src_file_set.begin(), src_file_set.end(), dst_file_set.begin(), dst_file_set.end(), std::inserter(src_isolated, src_isolated.begin()) );
        std::set_difference( dst_file_set.begin(), dst_file_set.end(), src_file_set.begin(), src_file_set.end(), std::inserter(dst_isolated, dst_isolated.begin()) );

        BOOST_FOREACH( boost::filesystem::path& p, src_isolated )
        {
            if ( dst_isolated.empty() ) // LOCAL COPY
            {
                boost::filesystem::path& dst_file = *dst_file_set.begin();

                std::cout << "COPY: " << (m_dst / dst_file).string() << " --> " << (m_dst / p).string() << std::endl;
                copy_file( m_dst / dst_file,  m_dst / p, ec );
                m_dst_map[file_key].insert( p );
                m_dst_path_map[p] = file_key;
            }
            else // MOVE
            {
                boost::filesystem::path& dst_file = *dst_isolated.begin();

                std::cout << "RENAME: " << (m_dst / dst_file).string() << " --> " << (m_dst / p).string() << std::endl;
                rename( m_dst / dst_file, m_dst / p, ec );

                m_dst_map[file_key].insert( p );
                m_dst_map[file_key].erase( dst_file );
                m_dst_path_map[p] = file_key;
                m_dst_path_map.erase( dst_file );

                dst_isolated.erase( dst_isolated.begin() );
            }
        }
    }
}


void MySync::scan_folder( const boost::filesystem::path& dir_path, FileMap& file_map, FilePath2KeyMap& file_path_map, FileSet& folder_set )
{
    file_map.clear();
    file_path_map.clear();
    folder_set.clear();

    boost::filesystem::path p( dir_path );
    boost::filesystem::recursive_directory_iterator it( p );
    boost::filesystem::recursive_directory_iterator end;

    std::cout << "SCAN: " << dir_path << " ... ";

    for ( ; it != end; ++it )
    {
        boost::filesystem::path relative_path = boost::filesystem::relative( it->path(), dir_path );

        if ( boost::filesystem::is_directory( it->path() ) )
        {
            folder_set.insert( relative_path );
            continue;
        }

        boost::uintmax_t size = boost::filesystem::file_size( it->path() );
        std::time_t last_write_time = boost::filesystem::last_write_time( it->path() );
        FileKey key( size, last_write_time );
        file_map[key].insert( relative_path );
        file_path_map[relative_path] = key;
        //std::cout << "<" << size << ", " << last_write_time << "> " << boost::filesystem::relative( it->path(), dir_path ).string() << std::endl;
    }

    std::cout << folder_set.size() << " folders, " << file_map.size() << " files." << std::endl;
}


void MySync::my_rename( const boost::filesystem::path& p )
{
    boost::filesystem::path parent = p.parent_path();
    std::string name = p.stem().string();
    std::string extension = p.extension().string();
    FileKey& key = m_dst_path_map[p];

    for ( size_t i = 1; ; ++i )
    {
        std::stringstream strm;
        strm << name << "(" << i << ")" << extension;
        boost::filesystem::path np = parent / strm.str();
        FilePath2KeyMap::iterator it = m_dst_path_map.find( np );

        if ( it == m_dst_path_map.end() )  // RENAME
        {
            std::cout << "RENAME: " << (m_dst / p).string() << " --> " << (m_dst / np).string() << std::endl;
            rename( m_dst / p, m_dst / np, ec );

            m_dst_path_map[np] = key;
            m_dst_path_map.erase( p );
            m_dst_map[key].insert( np );
            m_dst_map[key].erase( p );
        }
    }
}
