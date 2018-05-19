cmake_minimum_required(VERSION 2.8)
project(StdUtils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE sources "*.cpp")

# Do not build FileSystemMonitor as it is only functional on Windows and Linux (Mac does not have inotify)
file(GLOB_RECURSE to_remove "FileSystemMonitor.cpp")
list(REMOVE_ITEM sources ${to_remove})

add_library(StdUtils ${sources})

#if(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "-fpermissive")
#endif()

#if(NOT DEFINED CMAKE_INSTALL_PREFIX)
#	set(CMAKE_INSTALL_PREFIX "/usr/local/StdUtils")
#endif()

#install(DIRECTORY "StdUtils"
#	DESTINATION "include")
#install(TARGETS "StdUtils"
#	ARCHIVE DESTINATION "lib")
