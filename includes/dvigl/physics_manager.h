#pragma once
#include "btBulletDynamicsCommon.h"
#include <dvigl/core.h>

class PhysicsMgr : public Singleton<PhysicsMgr> {
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
