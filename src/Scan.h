#pragma once
#include "Filter.h"


class Scan
{
public:

    void scan( const Path& p, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& folders, const Filter& filter = Filter() );
    void scan( const Path& base, PathInfoSetMap& folder_map, const Filter& filter = Filter() );

    void scan( const Path& base, PathSet& folders, PathSet& files, const Filter& filter = Filter() );

public:

    void get_info( const Path& base, const PathSet& files, PathKeyMap& path_key_map );
    void get_info( const Path& base, const PathSet& folders, PathKeyMap& cache, PathInfoSetMap& folder_map );
    void convert( const PathKeyMap& path_key_map, KeyPathMap& key_path_map );
};
