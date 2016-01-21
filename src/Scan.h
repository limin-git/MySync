#pragma once
#include "Filter.h"


class Scan
{
public:

    void scan( const Path& p, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& folders, const Filter& filter = Filter() );
    void scan( const Path& base, PathInfoSetMap& folder_map, const Filter& filter = Filter() );
};
