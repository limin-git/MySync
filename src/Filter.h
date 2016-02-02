#pragma once
#include "CommonDefines.h"


class Filter
{
public:

    Filter() {}

    Filter( const std::vector<std::string>& inc_files, const std::vector<std::string>& exc_files, const std::vector<std::string>& inc_folders, const std::vector<std::string>& exc_folders )
        : m_include_files( inc_files ),
          m_exclude_files( exc_files ),
          m_include_folders( inc_folders ),
          m_exclude_folders( exc_folders )
    {
    }

    bool is_valid( const Path& p ) const;
    bool is_file_valid( const Path& p ) const;
    bool is_folder_valid( const Path& p ) const;

public:

    std::vector<std::string> m_include_files;
    std::vector<std::string> m_exclude_files;
    std::vector<std::string> m_include_folders;
    std::vector<std::string> m_exclude_folders;

public:

    static Filter default_filter;
};
