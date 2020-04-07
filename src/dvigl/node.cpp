#include <dvigl/camera_node.h>
#include <dvigl/node.h>

Node::Node()
{
    forward = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    left = glm::cross(up, forward);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 Node::get_position() { return position; }
glm::mat4 Node::get_model_matrix()
{
    glm::mat4 matrix;
    matrix[0] = glm::vec4(left, 0.0f);
    matrix[1] = glm::vec4(up, 0.0f);
    matrix[2] = glm::vec4(forward, 0.0f);
    matrix[3] = glm::vec4(position, 1.0f);
    return matrix;
}

glm::vec3 rotated_vector(glm::vec3 V, glm::mat3 rotMatr)
{
    glm::vec3 result;
    result.x = rotMatr[0][0] * V.x + rotMatr[1][0] * V.y + rotMatr[2][0] * V.z;
    result.y = rotMatr[0][1] * V.x + rotMatr[1][1] * V.y + rotMatr[2][1] * V.z;
    result.z = rotMatr[0][2] * V.x + rotMatr[1][2] * V.y + rotMatr[2][2] * V.z;
    return result;
}

glm::mat3 get_rotation_matrix(glm::vec3 V, float fAngle)
{
    glm::mat3 result;
    float vecLength, sinSave, cosSave, oneMinusCos;
    float xx, yy, zz, xy, yz, zx, xs, ys, zs;
    float x, y, z;

    x = V.x;
    y = V.y;
    z = V.z;
    if (x == 0.0f && y == 0.0f && z == 0.0f)
    {
        return result;
    }
    vecLength = glm::length(V);

    x /= vecLength;
    y /= vecLength;
    z /= vecLength;

    sinSave = (float)sin(fAngle);
    cosSave = (float)cos(fAngle);
    oneMinusCos = 1.0f - cosSave;

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinSave;
    ys = y * sinSave;
    zs = z * sinSave;

    result[0][0] = (oneMinusCos * xx) + cosSave;
    result[1][0] = (oneMinusCos * xy) - zs;
    result[2][0] = (oneMinusCos * zx) + ys;

    result[0][1] = (oneMinusCos * xy) + zs;
    result[1][1] = (oneMinusCos * yy) + cosSave;
    result[2][1] = (oneMinusCos * yz) - xs;

    result[0][2] = (oneMinusCos * zx) - ys;
    result[1][2] = (oneMinusCos * yz) + xs;
    result[2][2] = (oneMinusCos * zz) + cosSave;

    return result;
}

void Node::rotate_around_vector(glm::vec3 V, float angle)
{
    glm::mat3 rotation_matrix = get_rotation_matrix(V, angle);
    up = rotated_vector(up, rotation_matrix);
    forward = rotated_vector(forward, rotation_matrix);
    left = glm::cross(up, forward);
}

void Node::roll(float angle) { rotate_around_vector(forward, angle); }
void Node::pitch(float angle) { rotate_around_vector(left, angle); }
void Node::yaw(float angle) { rotate_around_vector(up, angle); }
void Node::move_forward(float step) { position += forward * step; }
void Node::move_back(float step) { position += forward * -step; }
void Node::move_up(float step) { position += up * step; }
void Node::move_down(float step) { position += up * -step; }
void Node::move_left(float step) { position += left * step; }
void Node::move_right(float step) { position += left * -step; }
void Node::set_position(glm::vec3 V) { position = V; }

void Node::set_rotation(glm::vec3 V)
{
    rotate_around_vector(glm::vec3(1.0, 0.0, 0.0), V.x);
    rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), V.y);
    rotate_around_vector(glm::vec3(0.0, 0.0, 1.0), V.z);
}

void Node::release() {}
