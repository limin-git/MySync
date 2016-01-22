#include "StdAfx.h"
#include "Filter.h"


bool Filter::is_valid( const Path& p ) const
{
    if ( is_directory( p ) && !is_symlink( p ) )
    {
        return is_folder_valid( p );
    }

    return is_file_valid( p );
}


bool Filter::is_file_valid( const Path& p ) const
{
    std::string extension = p.extension().string();

    if ( ! m_include_files.empty() )
    {
        if ( std::find( m_include_files.begin(), m_include_files.end(), extension ) == m_include_files.end() )
        {
            return false;
        }
    }

    if ( ! m_exclude_files.empty() )
    {
        if ( std::find( m_exclude_files.begin(), m_exclude_files.end(), extension ) != m_exclude_files.end() )
        {
            return false;
        }
    }

    return true;
}


bool Filter::is_folder_valid( const Path& p ) const
{
    std::string filename = p.filename().string();

    //if ( ! m_include_folders.empty() )
    //{
    //    if ( std::find( m_include_folders.begin(), m_include_folders.end(), filename ) == m_include_folders.end() )
    //    {
    //        return false;
    //    }
    //}

    if ( ! m_exclude_folders.empty() )
    {
        if ( std::find( m_exclude_folders.begin(), m_exclude_folders.end(), filename ) != m_exclude_folders.end() )
        {
            return false;
        }
    }

    return true;
}
