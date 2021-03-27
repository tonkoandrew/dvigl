set(IMGUI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_INCLUDE_DIR}/*.cpp ${IMGUI_INCLUDE_DIR}/backends/imgui_impl_sdl.cpp ${IMGUI_INCLUDE_DIR}/backends/imgui_impl_opengl3.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI_INCLUDE_DIR}/*.h)
                 
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_SOURCES})

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM=<glad/glad.h>)

include_directories(
    ${IMGUI_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    )
    
target_link_libraries(imgui
    )
    
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER 3rdparty)

set(IMGUI_LIBRARIES imgui)