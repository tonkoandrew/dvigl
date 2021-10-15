#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/maths/soa_transform.h"

class SkinnedMesh;
class Shader;

class PlaybackController
{
public:
    // Constructor.
    PlaybackController();
    void set_time_ratio(float _time);
    float time_ratio() const;
    float previous_time_ratio() const;
    void set_playback_speed(float _speed) { playback_speed_ = _speed; }
    float playback_speed() const { return playback_speed_; }
    void set_loop(bool _loop) { loop_ = _loop; }
    bool loop() const { return loop_; }
    void Update(const ozz::animation::Animation& _animation, float _dt);
    void Reset();

private:
    float time_ratio_;
    float previous_time_ratio_;
    float playback_speed_;
    bool play_;
    bool loop_;
};

class ScratchBuffer
{
public:
    ScratchBuffer();
    ~ScratchBuffer();

    // Resizes the buffer to the new size and return the memory address.
    void* Resize(size_t _size);

private:
    void* buffer_;
    size_t size_;
};

namespace dvigl
{
struct Mesh;
}

class SkinnedModelNode : public Node
{
public:
    SkinnedModelNode(char* content, int content_size, std::string format, float scale);
    void update(float time_delta);
    void draw(Shader* s);
    void release();
    float get_bounding_sphere_radius();
    float m_startTime = 0.0f;

    ScratchBuffer m_scratch_buffer;
    PlaybackController m_controller;

    ozz::animation::Skeleton m_skeleton;
    ozz::animation::Animation m_animation;
    ozz::animation::SamplingCache m_cache;
    ozz::vector<dvigl::Mesh> m_meshes;
    ozz::vector<ozz::math::SoaTransform> m_locals;
    ozz::vector<ozz::math::Float4x4> m_models;
    ozz::vector<ozz::math::Float4x4> m_skinning_matrices;

    GLuint dynamic_array_bo_ = 0;
    GLuint dynamic_index_bo_ = 0;

private:
    float bounding_radius = 0.0f;
    std::vector<SkinnedMesh*> meshes;
};
