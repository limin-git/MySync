#pragma once
#include "Parameter.h"
#include "FolderInfo.h"
#include "Filter.h"
#include "Scan.h"
#include "Sync.h"


class SyncMgr
{
public:

    SyncMgr( ParameterPtr param );

    void sync();

public:

    ParameterPtr m_param;
    FolderInfo m_src;
    FolderInfoList m_dests;
    Filter m_filter;
    Scan m_scan;
    Sync m_sync;
};
