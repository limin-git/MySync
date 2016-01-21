#include "StdAfx.h"
#include "Scan.h"


void Scan::scan( const Path& base, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& folders, const Filter& filter )
{
    std::cout << "SCAN: " << base.string() << " ... ";

    key_path_map.clear();
    path_key_map.clear();
    folders.clear();

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

            if ( is_directory(p) && ! is_symlink(p) )
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
                    Path rp = boost::filesystem::relative( p, base );
                    struct stat st;
                    ::stat( p.string().c_str(), &st );
                    Key key( st.st_size, st.st_mtime );
                    key_path_map[key].insert( rp );
                    path_key_map[rp] = key;
                }
            }
        }
    }

    std::cout << folders.size() << " folders, " << key_path_map.size() << " files." << std::endl;
}


void Scan::scan( const Path& base, PathInfoSetMap& folder_map, const Filter& filter )
{
    std::cout << "SCAN(for moving): " << base.string() << " ... ";

    folder_map.clear();

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
                    Path rp = relative( p, base );
                    PathInfoSet& path_set = folder_map[rp];
                    boost::filesystem::directory_iterator it2( p );
                    boost::filesystem::directory_iterator end2;

                    for ( ; it2 != end2; ++it2 )
                    {
                        PathInfo info;
                        info.name = it2->path().filename();

                        if ( ! is_directory( it2->path() ) )
                        {
                            struct stat st;
                            ::stat( it2->path().string().c_str(), &st );
                            info.size = st.st_size;
                            info.last_write_time = st.st_mtime;
                        }

                        path_set.insert( info );
                    }

                    stack.push( p );
                }
            }
        }
    }

    std::cout << std::endl;
}


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


void Scan::get_info( PathInfoSetMap& path_info_set_map, const Path& base, const PathSet& folders, const PathKeyMap& cache )
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
            PathInfo info;
            info.name = p.filename();

            if ( ! is_directory( p ) )
            {
                Path& rp = relative( p, base );

                PathKeyMap::const_iterator find_it = cache.find( rp );

                if ( find_it != cache.end() )
                {
                    const Key& key = find_it->second;
                    info.size = key.first;
                    info.last_write_time = key.second;
                }
            }

            path_set.insert( info );
        }
    }
}


void Scan::get_info( PathKeyMap& path_key_map, const Path& base, const PathSet& files )
{
    path_key_map.clear();

    BOOST_FOREACH( const Path& file, files )
    {
        struct stat st;
        ::stat( ( base / file ).string().c_str(), &st );
        path_key_map.insert( PathKeyMap::value_type( file, Key( st.st_size, st.st_mtime ) ) );
    }
}


void Scan::convert( const PathKeyMap& path_key_map, KeyPathMap& key_path_map )
{
    key_path_map.clear();

    BOOST_FOREACH( const PathKeyMap::value_type& v, path_key_map )
    {
        key_path_map[v.second].insert( v.first );
    }
}
