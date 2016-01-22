#pragma once
#include "Filter.h"


class Scan
{
public:

    void scan( const Path& base, PathSet& folders, PathSet& files, PathSet& invalid_folders, PathSet& invalid_files, const Filter& filter = Filter() );

public:

    void get_key( PathKeyMap& path_key_map, const Path& base, const PathSet& files );
    void get_folder_info( PathInfoSetMap& folder_map, const Path& base, const PathSet& folders, const PathKeyMap& cache );
    void convert_to_key_path_map( const PathKeyMap& path_key_map, KeyPathMap& key_path_map );
    bool has_invalid( const Path& dir, const Filter& filter );
    void splite_folders_by_validity( const Path& base, const PathSet& folders, const Filter& filter, PathSet& has_invalid_folders, PathSet& not_has_invalid_folders );
};
