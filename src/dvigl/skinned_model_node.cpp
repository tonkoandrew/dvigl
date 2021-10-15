#include <dvigl/shader.h>
#include <dvigl/skinned_mesh.h>
#include <dvigl/skinned_model_node.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

// =============================== TODO: remove it after integration ================================================
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/archive_traits.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/box.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/math_ex.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/simd_math_archive.h"
#include "ozz/base/maths/simd_quaternion.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/platform.h"
#include "ozz/geometry/runtime/skinning_job.h"

namespace dvigl
{

// Defines a mesh with skinning information (joint indices and weights).
// The mesh is subdivided into parts that group vertices according to their
// number of influencing joints. Triangle indices are shared across mesh parts.
struct Mesh
{
    // Number of triangle indices for the mesh.
    int triangle_index_count() const { return static_cast<int>(triangle_indices.size()); }

    // Number of vertices for all mesh parts.
    int vertex_count() const
    {
        int vertex_count = 0;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            vertex_count += parts[i].vertex_count();
        }
        return vertex_count;
    }

    // Maximum number of joints influences for all mesh parts.
    int max_influences_count() const
    {
        int max_influences_count = 0;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            const int influences_count = parts[i].influences_count();
            max_influences_count = influences_count > max_influences_count ? influences_count : max_influences_count;
        }
        return max_influences_count;
    }

    // Test if the mesh has skinning informations.
    bool skinned() const
    {
        for (size_t i = 0; i < parts.size(); ++i)
        {
            if (parts[i].influences_count() != 0)
            {
                return true;
            }
        }
        return false;
    }

    // Returns the number of joints used to skin the mesh.
    int num_joints() const { return static_cast<int>(inverse_bind_poses.size()); }

    // Returns the highest joint number used in the skeleton.
    int highest_joint_index() const
    {
        // Takes advantage that joint_remaps is sorted.
        return joint_remaps.size() != 0 ? static_cast<int>(joint_remaps.back()) : 0;
    }

    // Defines a portion of the mesh. A mesh is subdivided in sets of vertices
    // with the same number of joint influences.
    struct Part
    {
        int vertex_count() const { return static_cast<int>(positions.size()) / 3; }

        int influences_count() const
        {
            const int _vertex_count = vertex_count();
            if (_vertex_count == 0)
            {
                return 0;
            }
            return static_cast<int>(joint_indices.size()) / _vertex_count;
        }

        typedef ozz::vector<float> Positions;
        Positions positions;
        enum
        {
            kPositionsCpnts = 3
        }; // x, y, z components

        typedef ozz::vector<float> Normals;
        Normals normals;
        enum
        {
            kNormalsCpnts = 3
        }; // x, y, z components

        typedef ozz::vector<float> Tangents;
        Tangents tangents;
        enum
        {
            kTangentsCpnts = 4
        }; // x, y, z, right or left handed.

        typedef ozz::vector<float> UVs;
        UVs uvs; // u, v components
        enum
        {
            kUVsCpnts = 2
        };

        typedef ozz::vector<uint8_t> Colors;
        Colors colors;
        enum
        {
            kColorsCpnts = 4
        }; // r, g, b, a components

        typedef ozz::vector<uint16_t> JointIndices;
        JointIndices joint_indices; // Stride equals influences_count

        typedef ozz::vector<float> JointWeights;
        JointWeights joint_weights; // Stride equals influences_count - 1
    };

    typedef ozz::vector<Part> Parts;
    Parts parts;

    // Triangles indices. Indices are shared across all parts.
    typedef ozz::vector<uint16_t> TriangleIndices;
    TriangleIndices triangle_indices;

    // Joints remapping indices. As a skin might be influenced by a part of the
    // skeleton only, joint indices and inverse bind pose matrices are reordered
    // to contain only used ones. Note that this array is sorted.
    typedef ozz::vector<uint16_t> JointRemaps;
    JointRemaps joint_remaps;

    // Inverse bind-pose matrices. These are only available for skinned meshes.
    typedef ozz::vector<ozz::math::Float4x4> InversBindPoses;
    InversBindPoses inverse_bind_poses;
};
} // namespace dvigl

namespace ozz
{
namespace io
{

    OZZ_IO_TYPE_TAG("ozz-sample-Mesh-Part", dvigl::Mesh::Part)
    OZZ_IO_TYPE_VERSION(1, dvigl::Mesh::Part)

    template <> struct Extern<dvigl::Mesh::Part>
    {
        static void Save(OArchive& _archive, const dvigl::Mesh::Part* _parts, size_t _count);
        static void Load(IArchive& _archive, dvigl::Mesh::Part* _parts, size_t _count, uint32_t _version);
    };

    OZZ_IO_TYPE_TAG("ozz-sample-Mesh", dvigl::Mesh)
    OZZ_IO_TYPE_VERSION(1, dvigl::Mesh)

    template <> struct Extern<dvigl::Mesh>
    {
        static void Save(OArchive& _archive, const dvigl::Mesh* _meshes, size_t _count);
        static void Load(IArchive& _archive, dvigl::Mesh* _meshes, size_t _count, uint32_t _version);
    };
} // namespace io
} // namespace dvigl

void ozz::io::Extern<dvigl::Mesh>::Load(IArchive& _archive, dvigl::Mesh* _meshes, size_t _count, uint32_t _version)
{
    // (void)_version;
    for (size_t i = 0; i < _count; ++i)
    {
        dvigl::Mesh& mesh = _meshes[i];
        _archive >> mesh.parts;
        _archive >> mesh.triangle_indices;
        _archive >> mesh.joint_remaps;
        _archive >> mesh.inverse_bind_poses;
    }
}

void ozz::io::Extern<dvigl::Mesh::Part>::Load(
    IArchive& _archive, dvigl::Mesh::Part* _parts, size_t _count, uint32_t _version)
{
    (void)_version;
    for (size_t i = 0; i < _count; ++i)
    {
        dvigl::Mesh::Part& part = _parts[i];
        _archive >> part.positions;
        _archive >> part.normals;
        _archive >> part.tangents;
        _archive >> part.uvs;
        _archive >> part.colors;
        _archive >> part.joint_indices;
        _archive >> part.joint_weights;
    }
}

std::string skeleton = "../res/models/skeleton.ozz";
std::string animation = "../res/models/animation.ozz";
std::string mesh = "../res/models/mesh.ozz";

const uint8_t kDefaultColorsArray[][4]
    = { { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 },
          { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 } };

const float kDefaultUVsArray[][2] = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f },
    { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } };

ScratchBuffer::ScratchBuffer()
    : buffer_(nullptr)
    , size_(0)
{
}
ScratchBuffer::~ScratchBuffer() { ozz::memory::default_allocator()->Deallocate(buffer_); }
void* ScratchBuffer::Resize(size_t _size)
{
    if (_size > size_)
    {
        size_ = _size;
        ozz::memory::default_allocator()->Deallocate(buffer_);
        buffer_ = ozz::memory::default_allocator()->Allocate(_size, 16);
    }
    return buffer_;
}

PlaybackController::PlaybackController()
    : time_ratio_(0.f)
    , previous_time_ratio_(0.f)
    , playback_speed_(1.f)
    , play_(true)
    , loop_(true)
{
}

void PlaybackController::Update(const ozz::animation::Animation& _animation, float _dt)
{
    float new_time = time_ratio_;

    if (play_)
    {
        new_time = time_ratio_ + _dt * playback_speed_ / _animation.duration();
    }

    // Must be called even if time doesn't change, in order to update previous
    // frame time ratio. Uses set_time_ratio function in order to update
    // previous_time_ an wrap time value in the unit interval (depending on loop
    // mode).
    set_time_ratio(new_time);
}

void PlaybackController::set_time_ratio(float _ratio)
{
    previous_time_ratio_ = time_ratio_;
    if (loop_)
    {
        // Wraps in the unit interval [0:1], even for negative values (the reason
        // for using floorf).
        time_ratio_ = _ratio - floorf(_ratio);
    }
    else
    {
        // Clamps in the unit interval [0:1].
        time_ratio_ = ozz::math::Clamp(0.f, _ratio, 1.f);
    }
}

// Gets animation current time.
float PlaybackController::time_ratio() const { return time_ratio_; }

// Gets animation time of last update.
float PlaybackController::previous_time_ratio() const { return previous_time_ratio_; }

void PlaybackController::Reset()
{
    previous_time_ratio_ = time_ratio_ = 0.f;
    playback_speed_ = 1.f;
    play_ = true;
}

bool LoadSkeleton(const char* _filename, ozz::animation::Skeleton* _skeleton)
{
    assert(_filename && _skeleton);
    ozz::log::Out() << "Loading skeleton archive " << _filename << "." << std::endl;
    ozz::io::File file(_filename, "rb");
    if (!file.opened())
    {
        ozz::log::Err() << "Failed to open skeleton file " << _filename << "." << std::endl;
        return false;
    }
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Skeleton>())
    {
        ozz::log::Err() << "Failed to load skeleton instance from file " << _filename << "." << std::endl;
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> *_skeleton;

    return true;
}

bool LoadAnimation(const char* _filename, ozz::animation::Animation* _animation)
{
    assert(_filename && _animation);
    ozz::log::Out() << "Loading animation archive: " << _filename << "." << std::endl;
    ozz::io::File file(_filename, "rb");
    if (!file.opened())
    {
        ozz::log::Err() << "Failed to open animation file " << _filename << "." << std::endl;
        return false;
    }
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Animation>())
    {
        ozz::log::Err() << "Failed to load animation instance from file " << _filename << "." << std::endl;
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> *_animation;

    return true;
}

bool LoadMeshes(const char* _filename, ozz::vector<dvigl::Mesh>* _meshes)
{
    assert(_filename && _meshes);
    ozz::log::Out() << "Loading meshes archive: " << _filename << "." << std::endl;
    ozz::io::File file(_filename, "rb");
    if (!file.opened())
    {
        ozz::log::Err() << "Failed to open mesh file " << _filename << "." << std::endl;
        return false;
    }
    ozz::io::IArchive archive(&file);

    while (archive.TestTag<dvigl::Mesh>())
    {
        _meshes->resize(_meshes->size() + 1);
        archive >> _meshes->back();
    }

    return true;
}

// ==============================================================

SkinnedModelNode::SkinnedModelNode(char* content, int content_size, std::string format, float scale)
{
    const struct aiScene* scene;

    int flags = aiProcess_SplitLargeMeshes | aiProcess_LimitBoneWeights | aiProcess_SplitByBoneCount
        | aiProcess_GenSmoothNormals | aiProcess_SortByPType | aiProcess_CalcTangentSpace
        | aiProcess_RemoveRedundantMaterials | aiProcess_TransformUVCoords | aiProcess_ValidateDataStructure
        | aiProcess_ImproveCacheLocality | aiProcess_FindInvalidData | aiProcess_FindDegenerates
        | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipUVs
        | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale | 0;

    aiPropertyStore* store = aiCreatePropertyStore();
    aiSetImportPropertyFloat(store, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);

    int import_components = aiComponent_CAMERAS | aiComponent_LIGHTS | aiComponent_MATERIALS | aiComponent_COLORS;
    aiSetImportPropertyInteger(store, AI_CONFIG_PP_RVC_FLAGS, import_components);

    scene = aiImportFileFromMemoryWithProperties(content, content_size, flags, format.c_str(), store);
    aiReleasePropertyStore(store);

    if (!scene)
    {
        LOG("ERROR LOADING MODEL\n");
        return;
    }

    meshes.resize(1);
    meshes[0] = new SkinnedMesh(scene);

    bounding_radius = meshes[0]->bounding_radius;

    // ==============
    glGenBuffers(1, &dynamic_array_bo_);
    glGenBuffers(1, &dynamic_index_bo_);

    // Reading skeleton.
    if (!LoadSkeleton(skeleton.c_str(), &m_skeleton))
    {
        // return false;
        return;
    }

    // Reading animation.
    if (!LoadAnimation(animation.c_str(), &m_animation))
    {
        // return false;
        return;
    }

    // Skeleton and animation needs to match.
    if (m_skeleton.num_joints() != m_animation.num_tracks())
    {
        ozz::log::Err() << "The provided animation doesn't match skeleton "
                           "(joint count mismatch)."
                        << std::endl;
        // return false;
        return;
    }

    // Allocates runtime buffers.
    const int num_soa_joints = m_skeleton.num_soa_joints();
    m_locals.resize(num_soa_joints);
    const int num_joints = m_skeleton.num_joints();
    m_models.resize(num_joints);

    // Allocates a cache that matches animation requirements.
    m_cache.Resize(num_joints);

    // Reading skinned meshes.
    if (!LoadMeshes(mesh.c_str(), &m_meshes))
    {
        // return false;
        return;
    }

    // Computes the number of skinning matrices required to skin all meshes.
    // A mesh is skinned by only a subset of joints, so the number of skinning
    // matrices might be less that the number of skeleton joints.
    // Mesh::joint_remaps is used to know how to order skinning matrices. So the
    // number of matrices required is the size of joint_remaps.
    size_t num_skinning_matrices = 0;
    for (const dvigl::Mesh& mesh : m_meshes)
    {
        num_skinning_matrices = ozz::math::Max(num_skinning_matrices, mesh.joint_remaps.size());
    }

    // Allocates skinning matrices.
    m_skinning_matrices.resize(num_skinning_matrices);

    // Check the skeleton matches with the mesh, especially that the mesh
    // doesn't expect more joints than the skeleton has.
    for (const dvigl::Mesh& mesh : m_meshes)
    {
        if (num_joints < mesh.highest_joint_index())
        {
            ozz::log::Err() << "The provided mesh doesn't match skeleton "
                               "(joint count mismatch)."
                            << std::endl;
            // return false;
            return;
        }
    }

    // return true;
}

void SkinnedModelNode::update(float time_delta)
{
    m_startTime += time_delta / 1000.0f;

    // =========
    // Updates current animation time.
    m_controller.Update(m_animation, time_delta);

    // Samples optimized animation at t = animation_time_.
    ozz::animation::SamplingJob sampling_job;
    sampling_job.animation = &m_animation;
    sampling_job.cache = &m_cache;

    sampling_job.ratio = m_controller.time_ratio();

    sampling_job.output = make_span(m_locals);
    if (!sampling_job.Run())
    {
        // return false;
        return;
    }

    // Converts from local space to model space matrices.
    ozz::animation::LocalToModelJob ltm_job;
    ltm_job.skeleton = &m_skeleton;
    ltm_job.input = make_span(m_locals);
    ltm_job.output = make_span(m_models);
    if (!ltm_job.Run())
    {
        // return false;
        return;
    }

    // return true;
}

void SkinnedModelNode::draw(Shader* s)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw(m_startTime, s);
    }
    //  ===========
    bool success = true;
    // Builds skinning matrices, based on the output of the animation stage.
    // The mesh might not use (aka be skinned by) all skeleton joints. We use
    // the joint remapping table (available from the mesh object) to reorder
    // model-space matrices and build skinning ones.
    // LOG("m_meshes size = %d\n", (int)m_meshes.size());
    for (const dvigl::Mesh& mesh : m_meshes)
    {
        for (size_t i = 0; i < mesh.joint_remaps.size(); ++i)
        {
            m_skinning_matrices[i] = m_models[mesh.joint_remaps[i]] * mesh.inverse_bind_poses[i];
        }

        // ==================== AAA
        {
            const dvigl::Mesh& _mesh = mesh;
            const ozz::span<ozz::math::Float4x4> _skinning_matrices = make_span(m_skinning_matrices);
            const ozz::math::Float4x4& _transform = ozz::math::Float4x4::identity();

            const int vertex_count = _mesh.vertex_count();

            const GLsizei positions_offset = 0;
            const GLsizei normals_offset = sizeof(float) * 3;
            const GLsizei tangents_offset = sizeof(float) * 6;
            const GLsizei positions_stride = sizeof(float) * 9;
            const GLsizei normals_stride = positions_stride;
            const GLsizei tangents_stride = positions_stride;
            const GLsizei skinned_data_size = vertex_count * positions_stride;

            const GLsizei colors_offset = skinned_data_size;
            const GLsizei colors_stride = sizeof(uint8_t) * 4;
            const GLsizei colors_size = vertex_count * colors_stride;
            const GLsizei uvs_offset = colors_offset + colors_size;
            const GLsizei uvs_stride = sizeof(float) * 2;
            const GLsizei uvs_size = vertex_count * uvs_stride;
            const GLsizei fixed_data_size = colors_size + uvs_size;

            const GLsizei vbo_size = skinned_data_size + fixed_data_size;
            void* vbo_map = m_scratch_buffer.Resize(vbo_size);

            size_t processed_vertex_count = 0;

            for (size_t i = 0; i < _mesh.parts.size(); ++i)
            {
                const dvigl::Mesh::Part& part = _mesh.parts[i];

                const size_t part_vertex_count = part.positions.size() / 3;
                if (part_vertex_count == 0)
                {
                    continue;
                }

                ozz::geometry::SkinningJob skinning_job;
                skinning_job.vertex_count = static_cast<int>(part_vertex_count);
                const int part_influences_count = part.influences_count();
                skinning_job.influences_count = part_influences_count;
                skinning_job.joint_matrices = _skinning_matrices;
                skinning_job.joint_indices = make_span(part.joint_indices);
                skinning_job.joint_indices_stride = sizeof(uint16_t) * part_influences_count;
                if (part_influences_count > 1)
                {
                    skinning_job.joint_weights = make_span(part.joint_weights);
                    skinning_job.joint_weights_stride = sizeof(float) * (part_influences_count - 1);
                }
                skinning_job.in_positions = make_span(part.positions);
                skinning_job.in_positions_stride = sizeof(float) * dvigl::Mesh::Part::kPositionsCpnts;
                float* out_positions_begin = reinterpret_cast<float*>(
                    ozz::PointerStride(vbo_map, positions_offset + processed_vertex_count * positions_stride));
                float* out_positions_end
                    = ozz::PointerStride(out_positions_begin, part_vertex_count * positions_stride);
                skinning_job.out_positions = { out_positions_begin, out_positions_end };
                skinning_job.out_positions_stride = positions_stride;

                float* out_normal_begin = reinterpret_cast<float*>(
                    ozz::PointerStride(vbo_map, normals_offset + processed_vertex_count * normals_stride));
                float* out_normal_end = ozz::PointerStride(out_normal_begin, part_vertex_count * normals_stride);
                if (part.normals.size() / dvigl::Mesh::Part::kNormalsCpnts == part_vertex_count)
                {
                    skinning_job.in_normals = make_span(part.normals);
                    skinning_job.in_normals_stride = sizeof(float) * dvigl::Mesh::Part::kNormalsCpnts;
                    skinning_job.out_normals = { out_normal_begin, out_normal_end };
                    skinning_job.out_normals_stride = normals_stride;
                }
                else
                {
                    for (float* normal = out_normal_begin; normal < out_normal_end;
                         normal = ozz::PointerStride(normal, normals_stride))
                    {
                        normal[0] = 0.f;
                        normal[1] = 1.f;
                        normal[2] = 0.f;
                    }
                }

                float* out_tangent_begin = reinterpret_cast<float*>(
                    ozz::PointerStride(vbo_map, tangents_offset + processed_vertex_count * tangents_stride));
                float* out_tangent_end = ozz::PointerStride(out_tangent_begin, part_vertex_count * tangents_stride);

                if (part.tangents.size() / dvigl::Mesh::Part::kTangentsCpnts == part_vertex_count)
                {
                    skinning_job.in_tangents = make_span(part.tangents);
                    skinning_job.in_tangents_stride = sizeof(float) * dvigl::Mesh::Part::kTangentsCpnts;
                    skinning_job.out_tangents = { out_tangent_begin, out_tangent_end };
                    skinning_job.out_tangents_stride = tangents_stride;
                }
                else
                {
                    for (float* tangent = out_tangent_begin; tangent < out_tangent_end;
                         tangent = ozz::PointerStride(tangent, tangents_stride))
                    {
                        tangent[0] = 1.f;
                        tangent[1] = 0.f;
                        tangent[2] = 0.f;
                    }
                }

                // Execute the job, which should succeed unless a parameter is invalid.
                if (!skinning_job.Run())
                {
                    LOG("skinning_job.Run failed\n");
                    // return false;
                    return;
                }

                static_assert(sizeof(kDefaultColorsArray[0]) == colors_stride, "Stride mismatch");

                for (size_t j = 0; j < part_vertex_count; j += OZZ_ARRAY_SIZE(kDefaultColorsArray))
                {
                    const size_t this_loop_count
                        = ozz::math::Min(OZZ_ARRAY_SIZE(kDefaultColorsArray), part_vertex_count - j);
                    memcpy(ozz::PointerStride(vbo_map, colors_offset + (processed_vertex_count + j) * colors_stride),
                        kDefaultColorsArray, colors_stride * this_loop_count);
                }

                if (part_vertex_count == part.uvs.size() / dvigl::Mesh::Part::kUVsCpnts)
                {
                    // Optimal path used when the right number of uvs is provided.
                    memcpy(ozz::PointerStride(vbo_map, uvs_offset + processed_vertex_count * uvs_stride),
                        array_begin(part.uvs), part_vertex_count * uvs_stride);
                }
                else
                {
                    // Un-optimal path used when the right number of uvs is not provided.
                    assert(sizeof(kDefaultUVsArray[0]) == uvs_stride);
                    for (size_t j = 0; j < part_vertex_count; j += OZZ_ARRAY_SIZE(kDefaultUVsArray))
                    {
                        const size_t this_loop_count
                            = ozz::math::Min(OZZ_ARRAY_SIZE(kDefaultUVsArray), part_vertex_count - j);
                        memcpy(ozz::PointerStride(vbo_map, uvs_offset + (processed_vertex_count + j) * uvs_stride),
                            kDefaultUVsArray, uvs_stride * this_loop_count);
                    }
                }

                // Some more vertices were processed.
                processed_vertex_count += part_vertex_count;
            }

            // LOG("dynamic_array_bo_ = %d\n", (int)dynamic_array_bo_);
            // LOG("vbo_size = %d\n", (int)vbo_size);
            // LOG("vbo_map = %d\n", (int)vbo_map);
            glBindBuffer(GL_ARRAY_BUFFER, dynamic_array_bo_);
            glBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vbo_size, vbo_map);

            // Shader* shader = nullptr;
            //   ambient_textured_shader->Bind(_transform, camera()->view_proj(),
            //                                 positions_stride, positions_offset,
            //                                 normals_stride, normals_offset, colors_stride,
            //                                 colors_offset, uvs_stride, uvs_offset);
            //   shader = ambient_textured_shader.get();
            //   // Binds default texture
            //   glBindTexture(GL_TEXTURE_2D, checkered_texture_);

            // Maps the index dynamic buffer and update it.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamic_index_bo_);
            const dvigl::Mesh::TriangleIndices& indices = _mesh.triangle_indices;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(dvigl::Mesh::TriangleIndices::value_type),
                array_begin(indices), GL_STREAM_DRAW);

            // Draws the mesh.
            static_assert(sizeof(dvigl::Mesh::TriangleIndices::value_type) == 2, "Expects 2 bytes indices.");
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, 0);
            // LOG("DRAW %d indices\n", (int) indices.size());
            // // Unbinds.
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            // shader->Unbind();

            // return true;
        }

        // ==================== AAA
    }
    // return success;
}

void SkinnedModelNode::release()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->release();
    }
}

float SkinnedModelNode::get_bounding_sphere_radius() { return bounding_radius; }
