#pragma once
#include "Parameter.h"

typedef std::pair<boost::uintmax_t, std::time_t> FileKey;
typedef std::set<boost::filesystem::path> FileSet;
typedef std::map<FileKey, FileSet> FileMap;
typedef std::map<boost::filesystem::path, FileKey> FilePath2KeyMap;


class MySync
{
public:

    MySync( const boost::filesystem::path& src, const boost::filesystem::path& dst )
        : m_src( src ),
          m_dst( dst )
    {
    }

    void sync();

public:

    void process_one_key( const FileKey& key );
    void init_file_map( const boost::filesystem::path& path, FileMap& file_map, FilePath2KeyMap& file_path_map, FileSet& folder_set );
    bool file_binary_compare( const std::string& lhs, const std::string& rhs );
    void read_file( const std::string& filename, std::vector<char>& data );
    boost::filesystem::path file_rename( const boost::filesystem::path& p );

public:

    boost::filesystem::path m_src;
    boost::filesystem::path m_dst;
    FileMap m_src_map;
    FileMap m_dst_map;
    FilePath2KeyMap m_src_path_map;
    FilePath2KeyMap m_dst_path_map;
    FileSet m_src_folder_set;
    FileSet m_dst_folder_set;
};
