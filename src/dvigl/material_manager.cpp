#include <dvigl/material_manager.h>

MaterialMgr gMaterialMgr;

bool MaterialMgr::init() { return true; }
void MaterialMgr::release()
{
    // release materials
}
