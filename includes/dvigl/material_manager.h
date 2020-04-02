#pragma once
#include <dvigl/core.h>

class MaterialMgr : public Singleton<MaterialMgr> {
public:
    bool init();
    bool load_material(std::string name);
    void release();
};