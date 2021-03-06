#pragma once

// Platform detection:

#if defined(__ANDROID__) || defined(ANDROID)
#define __PLATFORM_ANDROID__
#else
#if defined(_WIN32)
#define __PLATFORM_WINDOWS__
#else
#if defined(__APPLE__)
#define __PLATFORM_APPLE__
#else
#define __PLATFORM_LINUX__
#endif
#endif
#endif

#include <yaml-cpp/yaml.h>

// include SDL and libs:
#include <glad/glad.h>

#if defined(__PLATFORM_ANDROID__)
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_net.h"
#include "SDL_ttf.h"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef GLuint GLhandleARB;
#endif

#if defined(__PLATFORM_LINUX__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// #include <GL/glx.h>
// #include <GL/glxext.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#endif

#if defined(__PLATFORM_APPLE__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#endif

#if defined(__PLATFORM_WINDOWS__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl.h"
#include "imgui.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> //glm::pi<float>()
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> // glm::to_string

#include <assimp/cexport.h> // C exporter interface
#include <assimp/cimport.h> // C importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

#include <entt/entt.hpp>

// ---- include STL ----

#include <dvigl/singleton.h>

// #include <algorithm>
// #include <set>
// #include <stack>
// #include <queue>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <assert.h>

// LOGGING

#if defined(__PLATFORM_ANDROID__)
#include <android/log.h>
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "corelog", __VA_ARGS__)
#else
#include <cstdio>
#define LOG(...)                                                                                                       \
    {                                                                                                                  \
        printf(__VA_ARGS__);                                                                                           \
        std::cout << std::flush;                                                                                       \
    }
#endif

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

#ifdef WIN32
#define SNPRINTF _snprintf_s
#define VSNPRINTF vsnprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#else
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define SAFE_DELETE(p)                                                                                                 \
    if (p)                                                                                                             \
    {                                                                                                                  \
        delete p;                                                                                                      \
        p = NULL;                                                                                                      \
    }

#define GLCheckError() (glGetError() == GL_NO_ERROR)

// GLSL attributes location
#define attr_pos_loc 0
#define attr_normal_loc 1
#define attr_texcoord_loc 2
#define attr_tangent_loc 3
#define attr_bitangent_loc 4
#define attr_bone_id_loc 5
#define attr_bone_weight_loc 6

#define INVALID_UNIFORM_LOCATION 0xffffffff

template <typename Ty, size_t Num> char (&CountOfRequireArrayArgumentT(const Ty (&)[Num]))[Num];

#define COUNTOF(_x) sizeof(CountOfRequireArrayArgumentT(_x))

extern "C" int dvigl_init();
extern "C" int dvigl_main();
extern "C" Uint32 dvigl_get_window_id();
