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
                        info.p = it2->path().filename();

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
