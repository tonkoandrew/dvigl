#pragma once
#include <dvigl/core.h>

class ParticleSystemMgr : public Singleton<ParticleSystemMgr>
{
  public:
    bool init();
    void update(float time_delta);
    void release();
};