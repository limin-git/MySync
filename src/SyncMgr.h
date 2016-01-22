#pragma once
#include "CommonDefines.h"
#include "Parameter.h"
#include "Filter.h"
#include "Scan.h"
#include "Sync.h"


class SyncMgr
{
public:

    SyncMgr( ParameterPtr param );

    void sync();

public:

    FolderInfo m_src;
    FolderInfoList m_dst_list;
    Filter m_filter;
    Scan m_scan;
    Sync m_sync;
};
