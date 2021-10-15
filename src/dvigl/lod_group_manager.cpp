#include <dvigl/lod_group.h>
#include <dvigl/lod_group_manager.h>

LODGroupMgr gLODGroupMgr;

bool LODGroupMgr::init() { return true; }

LODGroup* LODGroupMgr::create_lod_group(std::string name, int levels_count)
{
    lod_groups[name] = new LODGroup(levels_count);
    return lod_groups[name];
}

void LODGroupMgr::release() { }
