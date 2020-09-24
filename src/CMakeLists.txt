# CMakeList.txt : CMake project for SourceStatistics, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project("SourceStatistics")

set (CMAKE_CXX_STANDARD 11)	# cmake 3.1以上版本 添加C++11支持，这里要用到C++11多线程

# 这段代码可以区分操作系统
MESSAGE("Identifying the OS...")
if(WIN32)
	MESSAGE("This is Windows.")
	message(STATUS "Setting MSVC flags")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest")
	add_definitions(-D WINDOWS)
elseif(APPLE)
	MESSAGE("This is MacOS.")	
elseif(UNIX)
	MESSAGE("This is Linux.")
	SET(CMAKE_BUILD_TYPE "Debug")  # Debug模式 选项: Release Debug MinSizeRel RelWithDebInf
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # release模式下 gdb相关选项
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  # debug模式下 gdb相关选项
	add_definitions(-D LINUX)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")

add_subdirectory("fileHelper")
add_subdirectory("thread")
add_subdirectory("test")
add_subdirectory("app")


# TODO: Add tests and install targets if needed.


