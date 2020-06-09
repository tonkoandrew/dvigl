cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)
# set(CMAKE_C_COMPILER /usr/bin/clang-10)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++-10)
project (dvigl)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


if(WIN32)
  set(CMAKE_INCLUDE_PATH "C:\\Program Files\\mingw-w64\\x86_64-8.1.0-posix-seh-rt_v6-rev0\\mingw64\\x86_64-w64-mingw32")
  set(CMAKE_LIBRARY_PATH "C:\\Program Files\\mingw-w64\\x86_64-8.1.0-posix-seh-rt_v6-rev0\\mingw64\\x86_64-w64-mingw32")
  set(CMAKE_PREFIX_PATH "C:\\Program Files\\mingw-w64\\x86_64-8.1.0-posix-seh-rt_v6-rev0\\mingw64\\x86_64-w64-mingw32")
endif(WIN32)

find_package(Threads REQUIRED)

find_package(ZLIB REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/;${CMAKE_MODULE_PATH}")

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# if(WIN32)
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# endif(WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

if(APPLE)
  list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -pedantic")
else()
  list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -s -pedantic")
  # list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -g -pedantic")
endif(APPLE)

# find the required packages
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

find_package(SDL2 REQUIRED)
message(STATUS "SDL 2 included at ${SDL2_INCLUDE_DIRS}")

find_package(Freetype REQUIRED)
message(STATUS "Freetype at ${FREETYPE_LIBRARIES}")

if(APPLE)
  include (FindPkgConfig)
  if (NOT PKG_CONFIG_FOUND)
      message (FATAL_ERROR "pkg-config not found")
  endif()

  # check for libpng
  pkg_check_modules (LIBPNG libpng16 REQUIRED)
  if (NOT LIBPNG_FOUND)
      message(FATAL_ERROR "You don't seem to have libpng16 development libraries installed")
  else()
    set(PNG_LIBRARIES "${LIBPNG_LIBRARIES}")
  endif()
else()
  find_package(PNG REQUIRED)
  message(STATUS "PNG at ${PNG_LIBRARIES}")
endif(APPLE)

find_package(SDL2_image REQUIRED)
message(STATUS "SDL2_image included at ${SDL2_IMAGE_INCLUDE_DIRS}")

find_package(SDL2_mixer REQUIRED)
message(STATUS "SDL2_mixer included at ${SDL2_MIXER_INCLUDE_DIRS}")

find_package(SDL2_ttf REQUIRED)
message(STATUS "SDL2_ttf included at ${SDL2_TTF_INCLUDE_DIRS}")

find_package(SDL2_net REQUIRED)
message(STATUS "SDL2_net included at ${SDL2_NET_INCLUDE_DIRS}")

find_package(Bullet REQUIRED)
message(STATUS "Found Bullet in ${BULLET_INCLUDE_DIRS}")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

find_package(IrrXML REQUIRED )
message(STATUS "Found IrrXML in ${IRRXML_INCLUDE_DIR}")



find_package(OGG REQUIRED )
message(STATUS "Found OGG in ${OGG_LIBRARY}")

find_package(Vorbis REQUIRED)
message(STATUS "Found VORBIS_LIBRARIES in ${VORBIS_LIBRARIES}")

find_package(THEORA REQUIRED )
message(STATUS "Found THEORA in ${THEORA_LIBRARIES}")


find_package(Opus REQUIRED )
message(STATUS "Found Opus in ${OPUS_LIBRARIES}")


find_package(VPX REQUIRED )
message(STATUS "Found VPX in ${VPX_LIBRARIES}")

# yaml-cpp-0.6.3
find_package(yaml-cpp REQUIRED)
message(STATUS "YAML_CPP_INCLUDE_DIR at ${YAML_CPP_INCLUDE_DIR}")

find_package(JPEG REQUIRED)
message(STATUS "JPEG at ${JPEG_LIBRARIES}")

if(WIN32)
	find_package(HARFBUZZ REQUIRED)
else()
	find_package(HarfBuzz REQUIRED)
endif(WIN32)
message(STATUS "HARFBUZZ at ${HARFBUZZ_LIBRARIES}")


if(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
else()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
endif(WIN32)

if (UNIX AND NOT APPLE)
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif(UNIX AND NOT APPLE)

if(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  set(LIBS ${OPUS_LIBRARIES} ${VPX_LIBRARIES} ${THEORA_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARY} yaml-cpp.a ${ASSIMP_LIBRARY} ${BULLET_LIBRARIES} ${SDL2_LIBRARIES} ${IRRXML_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} graphite2.a bz2 rpcrt4 ${PNG_LIBRARIES} ${SDL2_NET_LIBRARIES} pthread ${JPEG_LIBRARIES} opengl32.dll 
    m user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8 dxerr8 mingw32 ws2_32 wsock32 shlwapi lzma.a zstd.a mpg123.a FLAC.a tiff.a webp.a Iphlpapi vorbisfile.a vorbis.a ogg.a opusfile.a)
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -mwindows")
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  set(LIBS Qt5::Widgets ${OPUS_LIBRARIES} ${VPX_LIBRARIES} ${THEORA_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARY} ${YAML_CPP_LIBRARIES} ${ASSIMP_LIBRARY} ${BULLET_LIBRARIES} ${SDL2_LIBRARIES} ${IRRXML_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} sndio ${SDL2_TTF_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${SDL2_NET_LIBRARIES} X11 Xext Xrandr Xinerama Xi Xxf86vm Xcursor GL dl asound pthread FLAC ${JPEG_LIBRARIES} )
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  set(LIBS ${OPUS_LIBRARIES} ${VPX_LIBRARIES} ${THEORA_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARY} ${YAML_CPP_LIBRARIES} ${ASSIMP_LIBRARY} ${BULLET_LIBRARIES} ${SDL2_LIBRARIES} ${IRRXML_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${SDL2_NET_LIBRARIES} dl pthread FLAC ${JPEG_LIBRARIES} bz2 iconv modplug TIFF webp.a vorbisfile.a)
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -L/usr/local/lib -lSDL2 -lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal")

  # INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  # FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  # FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  # FIND_LIBRARY(IOKit_LIBRARY IOKit)
  # FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  # MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  # SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  # SET(APPLE_LIBS ${APPLE_LIBS} ${ASSIMP_LIBRARY})
  # set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

# first create relevant static libraries requried for other projects
add_library(GLAD STATIC "src/glad.c")

set(LIBS ${LIBS} GLAD)


add_library(THEORAPLAY STATIC "src/theoraplay.c")

target_link_libraries(THEORAPLAY ${VORBIS_LIBRARIES} ${OGG_LIBRARY} ${THEORA_LIBRARIES})
set(LIBS ${LIBS} THEORAPLAY)


file(GLOB SOURCE
    "src/webmplay/*.cpp"
)

add_library(WEBMPLAY STATIC ${SOURCE})
target_link_libraries(WEBMPLAY ${VORBIS_LIBRARIES} ${OGG_LIBRARY} ${OPUS_LIBRARIES} ${VPX_LIBRARIES} pthread)
set(LIBS ${LIBS} WEBMPLAY)


set(NAME "dvigl")

file(GLOB SOURCE
    "src/${NAME}/*.h"
    "src/${NAME}/*.cpp"
)

###
### Build dvigl engine as a library
###
add_library(${NAME} STATIC ${SOURCE})
# add_library(${NAME} SHARED ${SOURCE})

target_link_libraries(${NAME} ${LIBS})

set(LIBS ${NAME} ${LIBS})



###
### Build game
###
set(NAME "game")

file(GLOB SOURCE
    "src/${NAME}/*.h"
    "src/${NAME}/*.cpp"
)

add_executable(${NAME} ${SOURCE})
target_link_libraries(${NAME} ${LIBS})

if(WIN32)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
elseif(UNIX AND NOT APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
elseif(APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif(WIN32)



if(UNIX AND NOT APPLE)
  ###
  ### Build editor
  ###
  set(NAME "editor")

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  file(GLOB SOURCE
      "src/${NAME}/*.h"
      "src/${NAME}/*.cpp"
  )

  qt5_wrap_cpp(SOURCE src/${NAME}/mainwindow.h)
  qt5_wrap_ui(SOURCE src/${NAME}/mainwindow.ui)

  add_executable(${NAME} ${SOURCE})
  # set_target_properties(${NAME} PROPERTIES AUTOMOC TRUE)
  target_link_libraries(${NAME} ${LIBS})

  if(WIN32)
      set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
  elseif(UNIX AND NOT APPLE)
      set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

include_directories(
    ${CMAKE_SOURCE_DIR}/includes
    ${BULLET_INCLUDE_DIRS}
)