message(STATUS ">>> finding OpenCV 4.1.1")
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV_LIBRARY_DIRS: ${OpenCV_LIBRARY_DIRS}")
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_libraries(${OpenCV_LIBRARY_DIRS})
    list(APPEND LIBRARIES ${OpenCV_LIBS})
else()
    message(ERROR " OpenCV not found!")
endif() # OpenCV_FOUND
