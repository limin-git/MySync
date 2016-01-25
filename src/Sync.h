#pragma once
#include "CommonDefines.h"
#include "Filter.h"


class Sync
{
public:

    bool local_move_folders( const Path& src, PathInfoSetMap& src_folder_info_map,
                             const Path& dst, PathInfoSetMap& dst_folder_info_map );

    bool local_move_files( const Path& src, const KeyPathMap& src_key_path_map,
                           const Path& dst, const KeyPathMap& dst_key_path_map );

    bool remote_copy_folders( const Path& src, const PathSet& src_folders,
                              const Path& dst, const PathSet& dst_folders );

    bool remote_copy_files( const Path& src, const KeyPathMap& src_key_path_map,
                            const Path& dst, const KeyPathMap& dst_key_path_map );

    bool remove_folders( const Path& src, const PathSet& src_folders,
                         const Path& dst, const PathSet& dst_folders );

    bool remove_files( const Path& src, const PathSet& src_files,
                       const Path& dst, const PathSet& dst_files );

public:

    void erase_sub_folders( PathSet& folders );
};
