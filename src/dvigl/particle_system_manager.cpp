#include <dvigl/particle_system_manager.h>

ParticleSystemMgr gParticleSystemMgr;

bool ParticleSystemMgr::init() { return true; }

void ParticleSystemMgr::update(float time_delta) {
  // LOG("ParticleSystemMgr::update: %f\n", time_delta);
}

void ParticleSystemMgr::release() {
  // release animation objects
}
