cmake_minimum_required(VERSION 2.8)
project( main_detect )
find_package( OpenCV REQUIRED )

include_directories( 
	${OpenCV_INCLUDE_DIRS} 
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable( main_detect ${SRC_FILES})
target_link_libraries( main_detect ${OpenCV_LIBS} )
