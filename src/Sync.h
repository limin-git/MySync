#pragma once
#include "CommonDefines.h"


class Sync
{
public:

    void sync_files( const Path& src, KeyPathMap& src_key_path_map, PathKeyMap& src_path_key_map, PathSet& src_folders, 
                     const Path& dst, KeyPathMap& dst_key_path_map, PathKeyMap& dst_path_key_map, PathSet& dst_folders );

public:

    void my_rename( const Path& p, const Path& bp, KeyPathMap& key_path_map, PathKeyMap& path_key_map, PathSet& path_set );
};
