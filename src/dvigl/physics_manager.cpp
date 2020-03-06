#include <dvigl/physics_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>

PhysicsMgr gPhysicsMgr;

bool PhysicsMgr::init()
{

    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));



    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
    collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -50, 0));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);


rbInfo.m_restitution     = 1.0f;
rbInfo.m_friction        = 30.0f;
rbInfo.m_rollingFriction = 3.0f;
rbInfo.m_mass            = 0.0f;

    btRigidBody* body = new btRigidBody(rbInfo);

        // body->setDamping(0, 0);
body->setRestitution(1.0);
    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);













    {

    btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        collisionShapes.push_back(colShape);

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(1000.f);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
            colShape->calculateLocalInertia(mass, localInertia);

        startTransform.setOrigin(btVector3(0, 30, 0));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

        rbInfo.m_restitution   = 1.0f;
        rbInfo.m_friction        = 1.0f;
        rbInfo.m_rollingFriction = 1.0f;

        btRigidBody* body = new btRigidBody(rbInfo);
        // body->setDamping(0, 0);
body->setRestitution(1.0);

        dynamicsWorld->addRigidBody(body);
    }


    return true;
}

void PhysicsMgr::update(float time_delta)
{
    dynamicsWorld->stepSimulation(time_delta * 0.01f, 1);

    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];

    btRigidBody* body = btRigidBody::upcast(obj);

    btTransform trans;

    if (body && body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(trans);
    }
    else
    {
        trans = obj->getWorldTransform();
    }

ModelNode* m = ModelMgr::ptr()->get_model("yoda");
glm::vec3 v = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
m ->set_pose(v);
    // LOG("world pos object = %.8f, %.8f, %.8f\n", float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
}

void PhysicsMgr::release()
{
    //delete dynamics world
    delete dynamicsWorld;

    //delete solver
    delete solver;

    //delete broadphase
    delete overlappingPairCache;

    //delete dispatcher
    delete dispatcher;

    delete collisionConfiguration;

}
