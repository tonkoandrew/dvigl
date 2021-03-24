#pragma once
#include <dvigl/core.h>


class LODGroup;

class LODGroupMgr : public Singleton<LODGroupMgr>
{
public:
    bool init();
    LODGroup* create_lod_group(std::string name, int levels_count);
    void release();
    std::unordered_map<std::string, LODGroup*> lod_groups;

private:
};