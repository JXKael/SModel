find_path(
    EIGEN_INCLUDE_DIR
    NAMES Eigen/Eigen
    PATHS ${CMAKE_SOURCE_DIR}/external/eigen/eigen3.3.7/
)

set(EIGEN_FOUND "NO")
if(EIGEN_INCLUDE_DIR)
    set(EIGEN_FOUND "YES")
endif()
