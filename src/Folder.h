#pragma once
#include "CommonDefines.h"
#include "Filter.h"


class File;


class Folder
{
public:

    void initialize( const Path& dir );
    void scan( const Path& base, PathSet& folders, PathSet& files, PathSet& invalid_folders, PathSet& invalid_files, const Filter& filter = Filter::default_filter );

public:

    Folder* remove_folder( const Path& p );
    File* remove_file( const Path& p );
    PathStack get_path_stack( Path p );
    // TODO: change to find_folder_map
    std::pair<FolderMap*, FolderMap::iterator> find_folder( const Path& p );
    std::pair<FileMap*, FileMap::iterator> find_file( const Path& p );

public:

    static Folder* create_folder( const Path& dir );

public:

    Path m_name;
    FolderMap m_folders;
    FileMap m_files;
};

typedef boost::shared_ptr<Folder> FolderPtr;
