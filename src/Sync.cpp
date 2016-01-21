#include "StdAfx.h"
#include "Sync.h"


boost::system::error_code ec;


void Sync::sync_files( const Path& src, KeyPathMap& src_key_path_map, PathKeyMap& src_path_key_map, PathSet& src_folders,
                       const Path& dst, KeyPathMap& dst_key_path_map, PathKeyMap& dst_path_key_map, PathSet& dst_folders )
{
    PathSet src_isolated_folders;
    PathSet dst_isolated_folders;
    std::set_difference( src_folders.begin(), src_folders.end(), dst_folders.begin(), dst_folders.end(), std::inserter(src_isolated_folders, src_isolated_folders.begin()) );
    std::set_difference( dst_folders.begin(), dst_folders.end(), src_folders.begin(), src_folders.end(), std::inserter(dst_isolated_folders, dst_isolated_folders.begin()) );

    BOOST_FOREACH( Path& p, src_isolated_folders ) // CREATE FOLDER
    {
        std::cout << "CREATE FOLDER: " << (dst / p).string() << "\n";
        create_directories( dst / p, ec );
    }

    BOOST_FOREACH( const KeyPathMap::value_type& v, src_key_path_map ) // SYNC
    {
        const Key& file_key = v.first;
        const PathSet& src_file_set = src_key_path_map[file_key];
        KeyPathMap::iterator find_it = dst_key_path_map.find( file_key );

        if ( find_it == dst_key_path_map.end() ) // REMOTE COPY
        {
            BOOST_FOREACH( const Path& p, src_file_set )
            {
                if ( dst_path_key_map.find( p ) != dst_path_key_map.end() ) // RENAME
                {
                    this->my_rename( p, dst, dst_key_path_map, dst_path_key_map, dst_folders );
                }

                std::cout << "COPY: " << (src / p).string() << " --> " << (dst / p).string() << "\n";
                copy_file( src / p, dst / p, ec );
                dst_key_path_map[file_key].insert( p );
                dst_path_key_map[p] = file_key;
            }
        }
        else // MOVE, LOCAL COPY
        {
            PathSet& dst_file_set = find_it->second;
            PathSet src_isolated;
            PathSet dst_isolated;

            std::set_difference( src_file_set.begin(), src_file_set.end(), dst_file_set.begin(), dst_file_set.end(), std::inserter(src_isolated, src_isolated.begin()) );
            std::set_difference( dst_file_set.begin(), dst_file_set.end(), src_file_set.begin(), src_file_set.end(), std::inserter(dst_isolated, dst_isolated.begin()) );

            BOOST_FOREACH( Path& p, src_isolated )
            {
                if ( dst_isolated.empty() ) // LOCAL COPY
                {
                    Path& dst_file = *dst_file_set.begin();

                    std::cout << "COPY: " << (dst / dst_file).string() << " --> " << (dst / p).string() << "\n";
                    copy_file( dst / dst_file,  dst / p, ec );
                    dst_key_path_map[file_key].insert( p );
                    dst_path_key_map[p] = file_key;
                }
                else // MOVE
                {
                    Path& dst_file = *dst_isolated.begin();

                    std::cout << "RENAME: " << (dst / dst_file).string() << " --> " << (dst / p).string() << "\n";
                    rename( dst / dst_file, dst / p, ec );

                    dst_key_path_map[file_key].insert( p );
                    dst_key_path_map[file_key].erase( dst_file );
                    dst_path_key_map[p] = file_key;
                    dst_path_key_map.erase( dst_file );

                    dst_isolated.erase( dst_isolated.begin() );
                }
            }
        }
    }

    BOOST_FOREACH( PathKeyMap::value_type& v, dst_path_key_map ) // REMOVE FILES
    {
        if ( src_path_key_map.find( v.first ) == src_path_key_map.end() )
        {
            std::cout << "REMOVE: " << (dst / v.first).string() << "\n";
            permissions( dst / v.first, boost::filesystem::all_all, ec );
            remove( dst / v.first, ec );
        }
    }

    BOOST_FOREACH( Path& p, dst_isolated_folders ) // REMOVE FOLDERS
    {
        std::cout << "REMOVE FOLDER: " << (dst / p).string() << "\n";
        permissions( dst / p, boost::filesystem::all_all, ec );
        remove_all( dst / p, ec );
    }
}


void Sync::sync_folders( const Path& src, PathInfoSetMap& src_folder_map,
                         const Path& dst, PathInfoSetMap& dst_folder_map )
{
    typedef std::pair<Path, Path> PathPair;
    std::set<PathPair> rename_set;

    BOOST_FOREACH( PathInfoSetMap::value_type& sv , src_folder_map )
    {
        BOOST_FOREACH( PathInfoSetMap::value_type& dv , dst_folder_map )
        {
            if ( sv.second == dv.second && sv.first != dv.first )
            {
                PathInfoSetMap::iterator it = dst_folder_map.find( sv.first );

                if ( it != dst_folder_map.end() )
                {
                    if ( sv.second != it->second )
                    {
                        std::cout << "TODO: destination folder already exist. " << (dst / it->first).string() << "\n";
                    }

                    continue;
                }

                rename_set.insert( std::make_pair( dv.first, sv.first ) );
            }
        }
    }

    std::set<PathPair> child_set;

    BOOST_FOREACH( const PathPair& pp, rename_set )
    {
        if ( rename_set.find( std::make_pair( pp.first.parent_path(), pp.second.parent_path() ) ) != rename_set.end() )
        {
            child_set.insert( pp );
        }
    }

    std::set<PathPair> new_rename_set;
    std::set_difference( rename_set.begin(), rename_set.end(), child_set.begin(), child_set.end(), std::inserter( new_rename_set, new_rename_set.begin() ) );

    BOOST_FOREACH( const PathPair& pp, new_rename_set )
    {
        if ( exists( dst / pp.second ) )
        {
            // TODO
            std::cout << "TODO: destination folder already exist. " << pp.second.string() << "\n";
            continue;
        }

        Path parent = ( dst / pp.second ).parent_path();
        if ( ! exists( parent ) )
        {
            std::cout << "CREATE FOLDER: " << (parent).string() << "\n";
            create_directories( parent, ec );
        }

        std::cout << "RENAME FOLDER: " << (dst / pp.first).string() << " --> " << (dst / pp.second).string() << "\n";
        rename( dst / pp.first, dst / pp.second, ec );
    }
}


void Sync::my_rename( const Path& p, const Path& bp, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& path_set )
{
    Path parent = p.parent_path();
    std::string name = p.stem().string();
    std::string extension = p.extension().string();
    Key& key = path_key_map[p];

    for ( size_t i = 1; ; ++i )
    {
        std::stringstream strm;
        strm << name << "(" << i << ")" << extension;
        Path np = parent / strm.str();
        PathKeyMap::iterator it = path_key_map.find( np );

        if ( it == path_key_map.end() )  // RENAME
        {
            std::cout << "RENAME: " << (bp / p).string() << " --> " << (bp / np).string() << "\n";
            rename( bp / p, bp / np, ec );

            path_key_map[np] = key;
            path_key_map.erase( p );
            key_path_map[key].insert( np );
            key_path_map[key].erase( p );
            path_set.insert( np );
            path_set.erase( p );
            break;
        }
    }
}