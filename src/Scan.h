#pragma once
#include "Filter.h"


class Scan
{
public:

    void scan( const Path& p, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& folders, const Filter& filter = Filter() );
    void scan( const Path& base, PathInfoSetMap& folder_map, const Filter& filter = Filter() );

    void scan( const Path& base, PathSet& folders, PathSet& files, const Filter& filter = Filter() );

public:

    void get_info( PathKeyMap& path_key_map, const Path& base, const PathSet& files );
    void get_info( PathInfoSetMap& folder_map, const Path& base, const PathSet& folders, const PathKeyMap& cache );
    void convert( const PathKeyMap& path_key_map, KeyPathMap& key_path_map );
};
