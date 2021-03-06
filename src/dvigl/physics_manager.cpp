// #include <dvigl/lod_group.h>
// #include <dvigl/lod_group_manager.h>
// #include <dvigl/model_manager.h>
// #include <dvigl/model_node.h>
// #include <dvigl/physics_manager.h>
// #include <dvigl/skinned_model_node.h>

// PhysicsMgr gPhysicsMgr;

// bool PhysicsMgr::init()
// {

//     /// collision configuration contains default setup for memory, collision
//     /// setup. Advanced users can create their own configuration.
//     collisionConfiguration = new btDefaultCollisionConfiguration();
//     /// use the default collision dispatcher. For parallel processing you can use
//     /// a diffent dispatcher (see Extras/BulletMultiThreaded)
//     dispatcher = new btCollisionDispatcher(collisionConfiguration);
//     /// btDbvtBroadphase is a good general purpose broadphase. You can also try
//     /// out btAxis3Sweep.
//     overlappingPairCache = new btDbvtBroadphase();
//     /// the default constraint solver. For parallel processing you can use a
//     /// different solver (see Extras/BulletMultiThreaded)
//     solver = new btSequentialImpulseConstraintSolver;
//     dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
//     dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

//     btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(250.), btScalar(50.), btScalar(50.)));
//     collisionShapes.push_back(groundShape);

//     btTransform groundTransform;
//     groundTransform.setIdentity();
//     groundTransform.setOrigin(btVector3(0, -25, 0));

//     btScalar mass(0.);

//     // rigidbody is dynamic if and only if mass is non zero, otherwise static
//     bool isDynamic = (mass != 0.f);

//     btVector3 localInertia(0, 0, 0);
//     if (isDynamic)
//         groundShape->calculateLocalInertia(mass, localInertia);

//     // using motionstate is optional, it provides interpolation capabilities, and
//     // only synchronizes 'active' objects
//     btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
//     btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);

//     rbInfo.m_restitution = 1.0f;
//     rbInfo.m_friction = 0.01f;
//     rbInfo.m_linearDamping = 0.01f;
//     rbInfo.m_rollingFriction = 0.01f;
//     // rbInfo.m_mass            = 0.0f;

//     btRigidBody* body = new btRigidBody(rbInfo);

//     // body->setDamping(0, 0);
//     body->setRestitution(1.0);
//     // add the body to the dynamics world
//     dynamicsWorld->addRigidBody(body);

//     {

//         btCollisionShape* colShape = new btSphereShape(btScalar(1.));
//         collisionShapes.push_back(colShape);

//         /// Create Dynamic Objects
//         btTransform startTransform;
//         startTransform.setIdentity();

//         btScalar mass(1.f);

//         // rigidbody is dynamic if and only if mass is non zero, otherwise static
//         bool isDynamic = (mass != 0.f);

//         btVector3 localInertia(0, 0, 0);
//         if (isDynamic)
//             colShape->calculateLocalInertia(mass, localInertia);

//         startTransform.setOrigin(btVector3(-50, 100, 0));

//         // using motionstate is recommended, it provides interpolation capabilities,
//         // and only synchronizes 'active' objects
//         btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//         btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

//         rbInfo.m_restitution = 1.0f;
//         rbInfo.m_friction = 0.1f;
//         rbInfo.m_linearDamping = 0.2f;
//         rbInfo.m_rollingFriction = 0.01f;

//         // rbInfo.m_restitution   = 1.0f;
//         // rbInfo.m_friction        = 1.0f;
//         // rbInfo.m_rollingFriction = 1.0f;

//         btRigidBody* body = new btRigidBody(rbInfo);
//         // body->setDamping(0, 0);
//         body->setRestitution(1.0);

//         dynamicsWorld->addRigidBody(body);
//     }

//     return true;
// }

// void PhysicsMgr::update(float time_delta)
// {
//     // LOG("%f\n", time_delta * 0.5f);
//     float mm = 0.005f;
//     dynamicsWorld->stepSimulation((time_delta * mm), 100, 1. / 100);

//     btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];

//     btRigidBody* body = btRigidBody::upcast(obj);

//     btTransform trans;

//     if (body && body->getMotionState())
//     {
//         body->getMotionState()->getWorldTransform(trans);
//     }
//     else
//     {
//         trans = obj->getWorldTransform();
//     }

//     glm::vec3 v
//         = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));

//     // auto renderable = LODGroupMgr::ptr()->lod_groups["suzan"];
//     // renderable->set_position(v);

//     ModelNode* sphere = ModelMgr::ptr()->get_model("sphere");
//     sphere->set_position(v);

//     // LOG("world pos object = %.8f, %.8f, %.8f\n",
//     // float(trans.getOrigin().getX()), float(trans.getOrigin().getY()),
//     // float(trans.getOrigin().getZ()));
// }

// void PhysicsMgr::release()
// {
//     // delete dynamics world
//     delete dynamicsWorld;

//     // delete solver
//     delete solver;

//     // delete broadphase
//     delete overlappingPairCache;

//     // delete dispatcher
//     delete dispatcher;

//     delete collisionConfiguration;
// }
