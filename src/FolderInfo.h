#pragma once
#include "CommonDefines.h"


struct FolderInfo 
{
    Path base;
    KeyPathMap key_path_map;
    PathKeyMap path_key_map;
    PathSet folders;
};

typedef std::vector<FolderInfo> FolderInfoList;
