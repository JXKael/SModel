message(STATUS ">>> finding OpenGL 4.6")
find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
    message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
    message(STATUS "OpenGL_DEFINITIONS: ${OpenGL_DEFINITIONS}")
    include_directories(${OPENGL_INCLUDE_DIR})
    link_libraries(${OPENGL_LIBRARIES})
    add_definitions(${OpenGL_DEFINITIONS})
else()
    message(ERROR " OpenGL not found!")
endif() # OPENGL_FOUND
