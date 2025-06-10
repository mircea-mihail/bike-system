cmake_minimum_required(VERSION 3.0.0)
project(main_detect)

set(CMAKE_CXX_STANDARD 17)

# Find necessary packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED) # Add threading support

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${CMAKE_SOURCE_DIR}/include
    #    ${LIBCAMERA_INCLUDE_DIRS}
    /usr/include/libcamera
)

# Source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Define the executable
add_executable(main_detect ${SRC_FILES})

# Link libraries
# in raspi:
# target_link_libraries( main_detect -llccv ${OpenCV_LIBS} Threads::Threads)

# on linux:
target_link_libraries(main_detect ${OpenCV_LIBS} Threads::Threads)
