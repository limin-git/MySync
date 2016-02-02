#pragma once


typedef boost::filesystem::path Path;
typedef std::pair<long, std::time_t> Key;
typedef std::set<Path> PathSet;
typedef std::stack<Path> PathStack;
typedef std::map<Key, PathSet> KeyPathMap;
typedef std::map<Path, Key> PathKeyMap;

class File;
typedef std::map<Path, File*> FileMap;
class Folder;
typedef std::map<Path, Folder*> FolderMap;


struct PathInfo
{
    PathInfo()
        : size( 0 ),
        last_write_time( 0 )
    {
    }

    Path name;
    long size;
    std::time_t last_write_time;

    bool operator < ( const PathInfo& rhs ) const
    {
        return
            ( name < rhs.name ) ||
            ( name == rhs.name && size < rhs.size ) ||
            ( name == rhs.name && size == rhs.size && last_write_time < rhs.last_write_time )
            ;
    }

    bool operator == ( const PathInfo& rhs ) const
    {
        return name == rhs.name && size == rhs.size && last_write_time == rhs.last_write_time;
    }
};

typedef std::set<PathInfo> PathInfoSet;
typedef std::map<Path, PathInfoSet> PathInfoSetMap;


struct FolderInfo
{
    Path base;
    PathSet folders;
    PathSet folders_invalid;
    PathSet folders_has_invalid;
    PathSet folders_has_no_invalid;
    PathSet files;
    PathSet files_invalid;
    PathSet files_has_invalid;
    PathSet files_has_no_invalid;
    KeyPathMap key_path_map;
    PathKeyMap path_key_map;
    PathInfoSetMap folder_map;

    void clear()
    {
        folders.clear();
        folders_invalid.clear();
        folders_has_invalid.clear();
        folders_has_no_invalid.clear();
        files.clear();
        files_invalid.clear();
        files_has_invalid.clear();
        files_has_no_invalid.clear();
        key_path_map.clear();
        path_key_map.clear();
        folder_map.clear();
    }
};

typedef std::vector<FolderInfo> FolderInfoList;
