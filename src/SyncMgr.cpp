#include "StdAfx.h"
#include "SyncMgr.h"


SyncMgr::SyncMgr( ParameterPtr param )
    : m_param( param ),
      m_filter( param->m_include_files, param->m_exclude_files, param->m_include_folders, param->m_exclude_folders )
{

    m_src.base = param->m_src;

    BOOST_FOREACH( const std::string& p, param->m_dsts )
    {
        FolderInfo info;
        info.base = p;
        m_dests.push_back( info );
    }
}


void SyncMgr::sync()
{
    if ( !exists( m_src.base ) )
    {
        std::cout << "INVALID PATH: " << m_src.base.string() << std::endl;
        return;
    }

    {
        m_scan.scan( m_src.base, m_src.folder_map, m_filter );

        BOOST_FOREACH( FolderInfo& m_dst, m_dests )
        {
            if ( !exists( m_dst.base ) )
            {
                std::cout << "INVALID PATH: " << m_dst.base.string() << std::endl;
                continue;
            }

            m_scan.scan( m_dst.base, m_dst.folder_map, m_filter );
            m_sync.sync_folders( m_src.base, m_src.folder_map,
                                 m_dst.base, m_dst.folder_map );
        }
    }

    {
        m_scan.scan( m_src.base, m_src.key_path_map, m_src.path_key_map, m_src.folders, m_filter );

        BOOST_FOREACH( FolderInfo& m_dst, m_dests )
        {
            if ( exists( m_dst.base ) )
            {
                m_scan.scan( m_dst.base, m_dst.key_path_map, m_dst.path_key_map, m_dst.folders );
                m_sync.sync_files( m_src.base, m_src.key_path_map, m_src.path_key_map, m_src.folders,
                                   m_dst.base, m_dst.key_path_map, m_dst.path_key_map, m_dst.folders );
            }
        }
    }
}
