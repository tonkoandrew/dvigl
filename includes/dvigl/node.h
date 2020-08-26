#pragma once
#include <dvigl/core.h>

class CameraNode;

class Node
{
public:
    Node();
    glm::vec3 get_position();
    glm::mat4 get_model_matrix();

    glm::mat4 prev_model_matrix;

    void rotate_around_vector(glm::vec3 V, float angle);
    void roll(float angle);
    void pitch(float angle);
    void yaw(float angle);
    void move_forward(float step);
    void move_back(float step);
    void move_up(float step);
    void move_down(float step);
    void move_left(float step);
    void move_right(float step);
    void set_position(glm::vec3 V);
    void set_rotation(glm::vec3 V);
    void release();
    friend CameraNode;
    glm::vec3 forward;
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 left;
};
