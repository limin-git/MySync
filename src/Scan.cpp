#include "StdAfx.h"
#include "Scan.h"


void Scan::scan( const Path& base, PathSet& folders, PathSet& files, const Filter& filter )
{
    std::cout << "SCAN: " << base.string() << " ... ";

    folders.clear();
    files.clear();

    std::stack<Path> stack;
    stack.push( base );

    while ( !stack.empty() )
    {
        Path dir = stack.top();
        stack.pop();

        boost::filesystem::directory_iterator it( dir );
        boost::filesystem::directory_iterator end;

        for ( ; it != end; ++it )
        {
            const Path& p = it->path();

            if ( is_directory( p ) && ! is_symlink( p ) )
            {
                if ( filter.is_folder_valid( p ) )
                {
                    folders.insert( boost::filesystem::relative( p, base ) );
                    stack.push( p );
                }
            }
            else
            {
                if ( filter.is_file_valid( p ) )
                {
                    files.insert( boost::filesystem::relative( p, base ) );
                }
            }
        }
    }

    std::cout << folders.size() << " folders, " << files.size() << " files." << std::endl;
}


void Scan::get_folder_info( PathInfoSetMap& path_info_set_map, const Path& base, const PathSet& folders, const PathKeyMap& cache )
{
    path_info_set_map.clear();

    BOOST_FOREACH( const Path& dir, folders )
    {
        PathInfoSet& path_set = path_info_set_map[dir];
        boost::filesystem::directory_iterator it( base / dir );
        boost::filesystem::directory_iterator end;

        for ( ; it != end; ++it )
        {
            const Path& p = it->path();
            Path& rp = relative( p, base );
            PathInfo info;
            info.name = p.filename();

            if ( ! is_directory( p ) )
            {
                PathKeyMap::const_iterator find_it = cache.find( rp );

                if ( find_it != cache.end() )
                {
                    const Key& key = find_it->second;
                    info.size = key.first;
                    info.last_write_time = key.second;
                    path_set.insert( info );
                }
            }
            else
            {
                if ( folders.find( rp ) != folders.end() )
                {
                    path_set.insert( info );
                }
            }
        }
    }
}


void Scan::get_key( PathKeyMap& path_key_map, const Path& base, const PathSet& files )
{
    path_key_map.clear();

    BOOST_FOREACH( const Path& file, files )
    {
        struct stat st;
        ::stat( ( base / file ).string().c_str(), &st );
        path_key_map.insert( PathKeyMap::value_type( file, Key( st.st_size, st.st_mtime ) ) );
    }
}


void Scan::convert_to_key_path_map( const PathKeyMap& path_key_map, KeyPathMap& key_path_map )
{
    key_path_map.clear();

    BOOST_FOREACH( const PathKeyMap::value_type& v, path_key_map )
    {
        key_path_map[v.second].insert( v.first );
    }
}
