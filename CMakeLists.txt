﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("serverdemo")

# 包含子项目。
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -march=native -O3 -lpthread -lprotobuf" )
LINK_LIBRARIES(-lpthread -lm)
LINK_LIBRARIES(-lmysqlclient -lm)
LINK_LIBRARIES(-lprotobuf -lm)
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -g -Wall")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)   
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/liba)   

add_subdirectory ("lib")
add_subdirectory ("protoFiles")
add_subdirectory ("gameServer")
add_subdirectory ("gateServer")
add_subdirectory ("accountServer")
add_subdirectory ("client")

