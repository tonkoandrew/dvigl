#pragma once
#include <dvigl/core.h>

enum FrustumSide
{
    RIGHT = 0, // The RIGHT side of the frustum
    LEFT = 1, // The LEFT  side of the frustum
    BOTTOM = 2, // The BOTTOM side of the frustum
    TOP = 3, // The TOP side of the frustum
    BACK = 4, // The BACK side of the frustum
    FRONT = 5 // The FRONT side of the frustum
};

enum PlaneData
{
    A = 0, // The X value of the plane's normal
    B = 1, // The Y value of the plane's normal
    C = 2, // The Z value of the plane's normal
    D = 3 // The distance the plane is from the origin
};

class Frustum
{
public:
    Frustum();

    void update(glm::mat4 view_proj_m);
    bool point_test(glm::vec3 point);
    bool sphere_test(glm::vec3 center, float radius);
    bool cube_test(glm::vec3 center, float size);

    void release();

    float m_Frustum[6][4];
};
