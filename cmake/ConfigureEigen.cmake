message(STATUS ">>> finding Eigen 3.3.7")

mark_as_advanced(EIGEN_INCLUDE_DIR)

find_package(Eigen REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS})

if(EIGEN_FOUND)
    message(STATUS "EIGEN_INCLUDE_DIR: ${EIGEN_INCLUDE_DIR}")
    include_directories(${EIGEN_INCLUDE_DIR})
else()
    message(ERROR " eigen not found!")
endif() # EIGEN_FOUND