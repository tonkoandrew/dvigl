cmake_minimum_required (VERSION 3.5)
cmake_policy(VERSION 3.5)

project (dvigl)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -s")

# find the required packages
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

find_package(SDL2 REQUIRED)
message(STATUS "SDL 2 included at ${SDL2_INCLUDE_DIRS}")

find_package(Freetype REQUIRED)
message(STATUS "Freetype at ${FREETYPE_LIBRARIES}")

find_package(PNG REQUIRED)
message(STATUS "PNG at ${PNG_LIBRARIES}")

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

# yaml-cpp-0.6.3
find_package(yaml-cpp REQUIRED)
message(STATUS "YAML_CPP_INCLUDE_DIR at ${YAML_CPP_INCLUDE_DIR}")

if(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
else()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
endif(WIN32)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(WIN32)
  set(LIBS opengl32 assimp)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  set(LIBS Qt5::Widgets ${YAML_CPP_LIBRARIES} ${ASSIMP_LIBRARY} ${BULLET_LIBRARIES} ${SDL2_LIBRARIES} ${IRRXML_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} sndio ${SDL2_TTF_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${SDL2_NET_LIBRARIES} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread)
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${ASSIMP_LIBRARY})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

# first create relevant static libraries requried for other projects
add_library(GLAD STATIC "src/glad.c")

set(LIBS ${LIBS} GLAD)

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
### Build editor
###
set(NAME "editor")

set(CMAKE_INCLUDE_CURRENT_DIR ON)


file(GLOB SOURCE
    "src/${NAME}/*.h"
    "src/${NAME}/*.cpp"
)

qt5_wrap_cpp(SOURCE includes/${NAME}/controller.h includes/${NAME}/worker.h includes/${NAME}/mainwindow.h)
qt5_wrap_ui(SOURCE src/${NAME}/mainwindow.ui)

add_executable(editor ${SOURCE})
# set_target_properties(${NAME} PROPERTIES AUTOMOC TRUE)
target_link_libraries(${NAME} ${LIBS})

if(WIN32)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
elseif(UNIX AND NOT APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
elseif(APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif(WIN32)

###
### Build game
###
set(NAME "game")

file(GLOB SOURCE
    "src/${NAME}/*.h"
    "src/${NAME}/*.cpp"
)

add_executable(game ${SOURCE})
target_link_libraries(${NAME} ${LIBS})

if(WIN32)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
elseif(UNIX AND NOT APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
elseif(APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif(WIN32)

include_directories(
    ${CMAKE_SOURCE_DIR}/includes
    ${BULLET_INCLUDE_DIRS}
)