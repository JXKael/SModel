find_path(
    GLM_INCLUDE_DIR
    NAMES glm/glm.hpp
    PATHS ${CMAKE_SOURCE_DIR}/external/glm/glm0.9.9.6/
)

set(GLM_FOUND "NO")
if(GLM_INCLUDE_DIR)
    set(GLM_FOUND "YES")
endif()
