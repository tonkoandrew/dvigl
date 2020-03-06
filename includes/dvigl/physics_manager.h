#pragma once
#include <dvigl/core.h>
#include "btBulletDynamicsCommon.h"


class PhysicsMgr : public Singleton<PhysicsMgr>
{
  public:
    bool init();
    void update(float time_delta);
    void release();
  private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btSequentialImpulseConstraintSolver* solver;
    btBroadphaseInterface* overlappingPairCache;
    btCollisionDispatcher* dispatcher;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
};
