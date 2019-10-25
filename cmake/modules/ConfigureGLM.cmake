message(STATUS ">>> finding GLM 0.9.9.6")
find_package(GLM REQUIRED)

mark_as_advanced(GLM_INCLUDE_DIR)

if(GLM_FOUND)
    message(STATUS "GLM_INCLUDE_DIR: ${GLM_INCLUDE_DIR}")
    include_directories(${GLM_INCLUDE_DIR})
else()
    message(ERROR " glm not found!")
endif() # GLM_FOUND
