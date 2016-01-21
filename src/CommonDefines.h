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

    Path p;
    long size;
    std::time_t last_write_time;

    bool operator < ( const PathInfo& rhs ) const 
    {
        return
            ( p < rhs.p ) ||
            ( p == rhs.p && size < rhs.size ) ||
            ( p == rhs.p && size == rhs.size && last_write_time < rhs.last_write_time )
            ;
    }

    bool operator == ( const PathInfo& rhs ) const
    {
        return p == rhs.p && size == rhs.size && last_write_time == rhs.last_write_time;
    }
};

typedef std::set<PathInfo> PathInfoSet;
typedef std::map<Path, PathInfoSet> PathInfoSetMap;


struct FolderInfo 
{
    Path base;
    KeyPathMap key_path_map;
    PathKeyMap path_key_map;
    PathSet folders;
    PathInfoSetMap folder_map;
};

typedef std::vector<FolderInfo> FolderInfoList;
