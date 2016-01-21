#pragma once


typedef boost::filesystem::path Path;
typedef std::pair<long, std::time_t> Key;
typedef std::set<Path> PathSet;
typedef std::map<Key, PathSet> KeyPathMap;
typedef std::map<Path, Key> PathKeyMap;

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
    PathSet files;
    KeyPathMap key_path_map;
    PathKeyMap path_key_map;
    PathInfoSetMap folder_map;

    void clear()
    {
        folders.clear();
        files.clear();
        key_path_map.clear();
        path_key_map.clear();
        folder_map.clear();
    }
};

typedef std::vector<FolderInfo> FolderInfoList;
