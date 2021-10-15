#include <dvigl/frustum.h>

Frustum::Frustum() { }

void NormalizePlane(float frustum[6][4], int side)
{
    // Here we calculate the magnitude of the normal to the plane (point A B C)
    // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
    // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
    float magnitude = (float)sqrt(frustum[side][A] * frustum[side][A] + frustum[side][B] * frustum[side][B]
        + frustum[side][C] * frustum[side][C]);

    // Then we divide the plane's values by it's magnitude.
    // This makes it easier to work with.
    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude;
}

void Frustum::update(glm::mat4 view_proj_m)
{
    const float* clip = glm::value_ptr(view_proj_m);

    // Now we actually want to get the sides of the frustum.  To do this we take
    // the clipping planes we received above and extract the sides from them.

    // This will extract the RIGHT side of the frustum
    m_Frustum[RIGHT][A] = clip[3] - clip[0];
    m_Frustum[RIGHT][B] = clip[7] - clip[4];
    m_Frustum[RIGHT][C] = clip[11] - clip[8];
    m_Frustum[RIGHT][D] = clip[15] - clip[12];

    // Now that we have a normal (A,B,C) and a distance (D) to the plane,
    // we want to normalize that normal and distance.

    // Normalize the RIGHT side
    NormalizePlane(m_Frustum, RIGHT);

    // This will extract the LEFT side of the frustum
    m_Frustum[LEFT][A] = clip[3] + clip[0];
    m_Frustum[LEFT][B] = clip[7] + clip[4];
    m_Frustum[LEFT][C] = clip[11] + clip[8];
    m_Frustum[LEFT][D] = clip[15] + clip[12];

    // Normalize the LEFT side
    NormalizePlane(m_Frustum, LEFT);

    // This will extract the BOTTOM side of the frustum
    m_Frustum[BOTTOM][A] = clip[3] + clip[1];
    m_Frustum[BOTTOM][B] = clip[7] + clip[5];
    m_Frustum[BOTTOM][C] = clip[11] + clip[9];
    m_Frustum[BOTTOM][D] = clip[15] + clip[13];

    // Normalize the BOTTOM side
    NormalizePlane(m_Frustum, BOTTOM);

    // This will extract the TOP side of the frustum
    m_Frustum[TOP][A] = clip[3] - clip[1];
    m_Frustum[TOP][B] = clip[7] - clip[5];
    m_Frustum[TOP][C] = clip[11] - clip[9];
    m_Frustum[TOP][D] = clip[15] - clip[13];

    // Normalize the TOP side
    NormalizePlane(m_Frustum, TOP);

    // This will extract the BACK side of the frustum
    m_Frustum[BACK][A] = clip[3] - clip[2];
    m_Frustum[BACK][B] = clip[7] - clip[6];
    m_Frustum[BACK][C] = clip[11] - clip[10];
    m_Frustum[BACK][D] = clip[15] - clip[14];

    // Normalize the BACK side
    NormalizePlane(m_Frustum, BACK);

    // This will extract the FRONT side of the frustum
    m_Frustum[FRONT][A] = clip[3] + clip[2];
    m_Frustum[FRONT][B] = clip[7] + clip[6];
    m_Frustum[FRONT][C] = clip[11] + clip[10];
    m_Frustum[FRONT][D] = clip[15] + clip[14];

    // Normalize the FRONT side
    NormalizePlane(m_Frustum, FRONT);
}

bool Frustum::point_test(glm::vec3 point)
{
    for (int i = 0; i < 6; i++)
    {
        // Calculate the plane equation and check if the point is behind a side of the frustum
        if (m_Frustum[i][A] * point.x + m_Frustum[i][B] * point.y + m_Frustum[i][C] * point.z + m_Frustum[i][D] <= 0)
        {
            // The point was behind a side, so it ISN'T in the frustum
            return false;
        }
    }

    // The point was inside of the frustum (In front of ALL the sides of the frustum)
    return true;
}

bool Frustum::sphere_test(glm::vec3 center, float radius)
{

    // Go through all the sides of the frustum
    for (int i = 0; i < 6; i++)
    {
        // If the center of the sphere is farther away from the plane than the radius
        if (m_Frustum[i][A] * center.x + m_Frustum[i][B] * center.y + m_Frustum[i][C] * center.z + m_Frustum[i][D]
            <= -radius)
        {
            // The distance was greater than the radius so the sphere is outside of the frustum
            return false;
        }
    }

    // The sphere was inside of the frustum!
    return true;
}

bool Frustum::cube_test(glm::vec3 center, float size)
{

    for (int i = 0; i < 6; i++)
    {
        if (m_Frustum[i][A] * (center.x - size) + m_Frustum[i][B] * (center.y - size)
                + m_Frustum[i][C] * (center.z - size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x + size) + m_Frustum[i][B] * (center.y - size)
                + m_Frustum[i][C] * (center.z - size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x - size) + m_Frustum[i][B] * (center.y + size)
                + m_Frustum[i][C] * (center.z - size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x + size) + m_Frustum[i][B] * (center.y + size)
                + m_Frustum[i][C] * (center.z - size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x - size) + m_Frustum[i][B] * (center.y - size)
                + m_Frustum[i][C] * (center.z + size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x + size) + m_Frustum[i][B] * (center.y - size)
                + m_Frustum[i][C] * (center.z + size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x - size) + m_Frustum[i][B] * (center.y + size)
                + m_Frustum[i][C] * (center.z + size) + m_Frustum[i][D]
            > 0)
            continue;
        if (m_Frustum[i][A] * (center.x + size) + m_Frustum[i][B] * (center.y + size)
                + m_Frustum[i][C] * (center.z + size) + m_Frustum[i][D]
            > 0)
            continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
}

void Frustum::release() { }
