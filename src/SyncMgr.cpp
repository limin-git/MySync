#include "StdAfx.h"
#include "SyncMgr.h"


SyncMgr::SyncMgr( ParameterPtr param )
    : m_filter( param->m_include_files, param->m_exclude_files, param->m_include_folders, param->m_exclude_folders )
{
    m_src.base = param->m_src;

    BOOST_FOREACH( const std::string& p, param->m_dst_list )
    {
        FolderInfo info;
        info.base = p;
        m_dst_list.push_back( info );
    }
}


void SyncMgr::sync()
{
    if ( !exists( m_src.base ) )
    {
        std::cout << "INVALID PATH: " << m_src.base.string() << std::endl;
        return;
    }

    m_scan.scan( m_src.base, m_src.folders, m_src.files, m_filter ); // scan folders, files
    m_scan.get_key( m_src.path_key_map, m_src.base, m_src.files );   // get key(size, last-write-time) for each files

    BOOST_FOREACH( FolderInfo& m_dst, m_dst_list )
    {
        if ( !exists( m_dst.base ) )
        {
            std::cout << "INVALID PATH: " << m_dst.base.string() << std::endl;
            continue;
        }

        m_scan.scan( m_dst.base, m_dst.folders, m_dst.files );
        m_scan.get_key( m_dst.path_key_map, m_dst.base, m_dst.files );

        if ( m_src.folders == m_dst.folders &&
             m_src.files == m_dst.files &&
             m_src.path_key_map == m_dst.path_key_map )
        {
            std::cout << "EQUAL: " << m_src.base << " == " << m_dst.base << std::endl;
            continue;
        }

        if ( m_src.folder_map.empty() )
        {
            m_scan.get_folder_info( m_src.folder_map, m_src.base, m_src.folders, m_src.path_key_map );
        }

        m_scan.get_folder_info( m_dst.folder_map, m_dst.base, m_dst.folders, m_dst.path_key_map );

        bool changed = m_sync.sync_folders( m_src.base, m_src.folder_map,
                                            m_dst.base, m_dst.folder_map );

        if ( changed )
        {
            m_dst.clear();
            m_scan.scan( m_dst.base, m_dst.folders, m_dst.files );
            m_scan.get_key( m_dst.path_key_map, m_dst.base, m_dst.files );
            m_scan.get_folder_info( m_dst.folder_map, m_dst.base, m_dst.folders, m_dst.path_key_map );
        }

        if ( m_src.key_path_map.empty() )
        {
            m_scan.convert_to_key_path_map( m_src.path_key_map, m_src.key_path_map );
        }

        m_scan.convert_to_key_path_map( m_dst.path_key_map, m_dst.key_path_map );

        m_sync.sync_files( m_src.base, m_src.key_path_map, m_src.path_key_map, m_src.folders,
                           m_dst.base, m_dst.key_path_map, m_dst.path_key_map, m_dst.folders );
    }
}
