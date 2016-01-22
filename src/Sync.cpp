#include "StdAfx.h"
#include "Sync.h"


boost::system::error_code ec;


bool Sync::sync_remove_files( const Path& src, const PathSet& src_files,
                              const Path& dst, const PathSet& dst_files )
{
    bool changed = false;
    PathSet dst_isolated_files;
    std::set_difference( dst_files.begin(), dst_files.end(), src_files.begin(), src_files.end(), std::inserter(dst_isolated_files, dst_isolated_files.begin()) );

    BOOST_FOREACH( const Path& p, dst_isolated_files )
    {
        std::cout << "REMOVE: " << p << "\n";
        permissions( p, boost::filesystem::all_all, ec );
        remove( p, ec );
        changed = true;
    }

    return changed;
}


bool Sync::sync_remove_folders( const Path& src, const PathSet& src_folders,
                                const Path& dst, const PathSet& dst_folders )
{
    bool changed = false;
    PathSet dst_isolated_folders;
    std::set_difference( dst_folders.begin(), dst_folders.end(), src_folders.begin(), src_folders.end(), std::inserter(dst_isolated_folders, dst_isolated_folders.begin()) );
    this->remove_sub_folders( dst_isolated_folders );

    BOOST_FOREACH( const Path& p, dst_isolated_folders )
    {
        Path dir = dst / p;
        std::cout << "REMOVE FOLDER: " << dir << "\n";
        permissions( dir, boost::filesystem::all_all, ec );
        remove_all( dir, ec );
        changed = true;
    }

    return changed;
}


bool Sync::sync_copy_remote_files( const Path& src, const KeyPathMap& src_key_path_map,
                                   const Path& dst, const KeyPathMap& dst_key_path_map )
{
    bool changed = false;

    BOOST_FOREACH( const KeyPathMap::value_type& v, src_key_path_map )
    {
        const Key& key = v.first;
        const PathSet& src_file_set = v.second;
        KeyPathMap::const_iterator it = dst_key_path_map.find( key );

        if ( it == dst_key_path_map.end() )
        {
            BOOST_FOREACH( const Path& p, src_file_set )
            {
                const Path from = src / p;
                const Path to = dst / p;

                if ( exists( to ) )
                {
                    permissions( to, boost::filesystem::all_all, ec );
                    remove( to, ec );
                }

                std::cout << "COPY: " << from << " -- " << to << "\n";
                copy_file( from, to, ec );

                if ( ec )
                {
                    const Path to_parent = to.parent_path();
                    std::cout << "CREATE DIRECTORIES: " << to_parent << "\n";
                    create_directories( to_parent, ec );
                    copy_file( from, to, ec );
                }
            }

            changed = true;
        }
    }

    return changed;
}


bool Sync::sync_copy_remote_folders( const Path& src, const PathSet& src_folders,
                                     const Path& dst, const PathSet& dst_folders )
{
    bool changed = false;
    PathSet src_isolated_folders;
    std::set_difference( src_folders.begin(), src_folders.end(), dst_folders.begin(), dst_folders.end(), std::inserter(src_isolated_folders, src_isolated_folders.begin()) );
    this->remove_sub_folders( src_isolated_folders );

    BOOST_FOREACH( const Path& p, src_isolated_folders )
    {
        const Path from = src / p;
        const Path to = dst / p;

        std::cout << "COPY FOLDER: " << from << " -- " << to << "\n";
        copy_directory( from, to, ec );

        if ( ec )
        {
            const Path to_parent = to.parent_path();
            std::cout << "CREATE DIRECTORIES: " << to_parent << "\n";
            create_directories( to_parent, ec );
            copy_directory( from, to, ec );
        }

        changed = true;
    }

    return changed;
}


bool Sync::sync_move_local_files( const Path& src, const KeyPathMap& src_key_path_map,
                                  const Path& dst, const KeyPathMap& dst_key_path_map )
{
    bool changed = false;

    BOOST_FOREACH( const KeyPathMap::value_type& v, src_key_path_map )
    {
        const Key& key = v.first;
        KeyPathMap::const_iterator it = dst_key_path_map.find( key );

        if ( it != dst_key_path_map.end() ) // LOCAL MOVE / LOCAL COPY
        {
            const PathSet& src_files = v.second;
            const PathSet& dst_files = it->second;
            PathSet src_isolated_files;
            PathSet dst_isolated_files;

            std::set_difference( src_files.begin(), src_files.end(), dst_files.begin(), dst_files.end(), std::inserter(src_isolated_files, src_isolated_files.begin()) );
            std::set_difference( dst_files.begin(), dst_files.end(), src_files.begin(), src_files.end(), std::inserter(dst_isolated_files, dst_isolated_files.begin()) );

            BOOST_FOREACH( Path& src_file, src_isolated_files )
            {
                if ( !dst_isolated_files.empty() ) // LOCAL MOVE
                {
                    const Path& dst_file = *dst_isolated_files.begin();
                    const Path from = dst / dst_file;
                    const Path to = dst / src_file;

                    std::cout << "MOVE: " << from << " -- " << to << "\n";
                    rename( from, to, ec );

                    if ( ec )
                    {
                        const Path to_parent = to.parent_path();
                        std::cout << "CREATE DIRECTORIES: " << to_parent << "\n";
                        create_directories( to_parent, ec );
                        rename( from, to, ec );
                    }

                    dst_isolated_files.erase( dst_isolated_files.begin() );
                }
                else // LOCAL COPY
                {
                    const Path& dst_file = *dst_files.begin();
                    const Path from = dst / dst_file;
                    const Path to = dst / src_file;

                    std::cout << "COPY: " << from << " -- " << to << "\n";
                    copy_file( from, to, ec );
                }

                changed = true;
            }
        }
    }

    return changed;
}


bool Sync::sync_move_local_folders( const Path& src, PathInfoSetMap& src_folder_info_map,
                                    const Path& dst, PathInfoSetMap& dst_folder_info_map )
{
    bool changed = false;
    typedef std::pair<Path, Path> PathPair;
    std::set<PathPair> rename_set;

    BOOST_FOREACH( PathInfoSetMap::value_type& sv , src_folder_info_map )
    {
        BOOST_FOREACH( PathInfoSetMap::value_type& dv , dst_folder_info_map )
        {
            if ( sv.second == dv.second && sv.first != dv.first )
            {
                PathInfoSetMap::iterator it = dst_folder_info_map.find( sv.first );

                if ( it != dst_folder_info_map.end() )
                {
                    if ( sv.second != it->second )
                    {
                        std::cout << "TODO: destination folder already exist. " << dst / it->first << "\n";
                    }

                    continue;
                }

                rename_set.insert( PathPair( dv.first, sv.first ) );
            }
        }
    }

    std::set<PathPair> rename_sub_set;

    BOOST_FOREACH( const PathPair& pair, rename_set )
    {
        PathPair p = pair;

        while ( p.first.has_parent_path() && p.second.has_parent_path() )
        {
            p = PathPair( p.first.parent_path(), p.second.parent_path() );

            if ( rename_set.find( p ) != rename_set.end() )
            {
                rename_sub_set.insert( pair );
            }
        }
    }

    std::set<PathPair> new_rename_set;
    std::set_difference( rename_set.begin(), rename_set.end(), rename_sub_set.begin(), rename_sub_set.end(), std::inserter( new_rename_set, new_rename_set.begin() ) );

    BOOST_FOREACH( const PathPair& pair, new_rename_set )
    {
        Path from = dst / pair.first;
        Path to = dst / pair.second;

        if ( exists( to ) )
        {
            std::cout << "TODO: destination folder already exist. " << to << "\n";
            continue;
        }

        std::cout << "MOVE FOLDER: " << from << " -- " << to << "\n";
        rename( from, to, ec );

        if ( ec )
        {
            Path to_parent = to.parent_path();
            std::cout << "CREATE FOLDER: " << to_parent << "\n";
            create_directories( to_parent, ec );
            rename( from, to, ec );
        }

        changed = true;
    }

    return changed;
}


void Sync::remove_sub_folders( PathSet& folders )
{
    PathSet sub_folders;

    BOOST_FOREACH( const Path& folder, folders )
    {
        Path p = folder;

        while ( p.has_parent_path() )
        {
            p = p.parent_path();

            if ( folders.find( p ) != folders.end() )
            {
                sub_folders.insert( folder );
                break;
            }
        }
    }

    PathSet result;
    std::set_difference( folders.begin(), folders.end(), sub_folders.begin(), sub_folders.end(), std::inserter(result, result.begin()) );
    folders.swap( result );
}
