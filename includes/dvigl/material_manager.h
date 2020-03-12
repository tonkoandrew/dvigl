#pragma once
#include <dvigl/core.h>

class MaterialMgr : public Singleton<MaterialMgr> {
public:
  bool init();
  void release();
};