#pragma once
#include "CommonDefines.h"
#include "Filter.h"


class File;


class Folder
{
public:

    void initialize( const Path& dir );
    void assign( const Path& dir );
    void scan( const Path& base, PathSet& folders, PathSet& files, PathSet& invalid_folders, PathSet& invalid_files, const Filter& filter = Filter::default_filter );
    void get_key( const Path& base, PathKeyMap& path_key_map );


    Folder* remove_folder( const Path& p );
    File* remove_file( const Path& p );
    void copy_folder( const Path& p, Folder* folder );
    void copy_file( const Path& p, File* file );
    void rename_file( const Path& from, const Path& to );
    void rename_folder( const Path& from, const Path& to );

public:

    Folder* find_folder_parent( const Path& p );
    Folder* find_file_parent( const Path& p );
    Folder* create_directory( const Path& p );
    PathStack get_path_stack( Path p );

public:

    static Folder* create_folder( const Path& dir );

public:

    Path m_name;
    FolderMap m_folders;
    FileMap m_files;
};

typedef boost::shared_ptr<Folder> FolderPtr;
