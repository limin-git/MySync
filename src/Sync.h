#pragma once
#include "CommonDefines.h"
#include "Filter.h"


class Sync
{
public:

    bool sync_move_local_folders( const Path& src, PathInfoSetMap& src_folder_info_map,
                                  const Path& dst, PathInfoSetMap& dst_folder_info_map );

    bool sync_move_local_files( const Path& src, const KeyPathMap& src_key_path_map,
                                const Path& dst, const KeyPathMap& dst_key_path_map );

    bool sync_copy_remote_folders( const Path& src, const PathSet& src_folders,
                                   const Path& dst, const PathSet& dst_folders );

    bool sync_copy_remote_files( const Path& src, const PathSet& src_files,
                                 const Path& dst, const PathSet& dst_files );

    bool sync_remove_folders( const Path& src, const PathSet& src_folders,
                              const Path& dst, const PathSet& dst_folders );

    bool sync_remove_files( const Path& src, const PathSet& src_files,
                            const Path& dst, const PathSet& dst_files );

public:

    void remove_sub_folders( PathSet& folders );
};
