#include "StdAfx.h"
#include "Scan.h"


void Scan::scan( const Path& p, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& folders, const Filter& filter )
{
    std::cout << "SCAN: " << p << " ... ";

    key_path_map.clear();
    path_key_map.clear();
    folders.clear();

    boost::filesystem::recursive_directory_iterator it( p );
    boost::filesystem::recursive_directory_iterator end;

    for ( ; it != end; ++it )
    {
        Path rp = boost::filesystem::relative( it->path(), p );

        if ( is_directory( it->path() ) )
        {
            if ( ! filter.is_folder_valid( rp ) )
            {
                continue;
            }

            folders.insert( rp );
            continue;
        }

        if ( ! filter.is_file_valid( rp ) )
        {
            continue;
        }

        struct stat st;
        ::stat( it->path().string().c_str(), &st );
        Key key( st.st_size, st.st_mtime );
        key_path_map[key].insert( rp );
        path_key_map[rp] = key;
        //std::cout << "<" << size << ", " << last_write_time << "> " << boost::filesystem::relative( it->path(), dir_path ).string() << std::endl;
    }

    std::cout << folders.size() << " folders, " << key_path_map.size() << " files." << std::endl;
}
