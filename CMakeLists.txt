cmake_minimum_required(VERSION 2.8) 
PROJECT (testProject)
#find_package(OpenCV REQUIRED )

file( GLOB NAME_SRC "src/*.cpp" )

file( GLOB NAME_HEADERS "include/*.h" )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ) #OpenCV_INCLUDE_DIRS})

link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( testProject ${NAME_SRC} ${NAME_HEADERS} )
