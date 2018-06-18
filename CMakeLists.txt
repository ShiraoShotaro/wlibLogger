cmake_minimum_required(VERSION 3.0.0)

project(wlibLogger)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src/)
file(GLOB_RECURSE logger_cpp_files src/*.cpp)
file(GLOB_RECURSE logger_header_files src/*.hpp)
set(EXECUTABLE_NAME "wlibLoggerTest")
add_executable(${EXECUTABLE_NAME} ${logger_cpp_files} ${logger_header_files})

source_group("Logger" REGULAR_EXPRESSION "src/Logger.(cpp|hpp)")
source_group("Test" FILES src/main.cpp)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O0 -L${CMAKE_SOURCE_DIR}/lib/x86_64-linux-gnu")
#	target_link_libraries(${PROJECT_NAME} X11 pthread)
elseif(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
endif(UNIX)


