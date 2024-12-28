cmake_minimum_required(VERSION 3.0.0)
project( main_detect )

set (CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
find_package( PkgConfig REQUIRED )
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

include_directories( 
	${OpenCV_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/include
	${LIBCAMERA_INCLUDE_DIRS}
)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable( main_detect ${SRC_FILES})
target_link_libraries( main_detect -llccv ${OpenCV_LIBS} )
