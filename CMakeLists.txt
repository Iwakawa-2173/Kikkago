cmake_minimum_required(VERSION 3.0)

project(kikka VERSION 1.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -o1 -g -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -o3")
set(CMAKE_CXX_FLAGS_RELEASELTO_INIT "-Wall -o3 -flto")


set(SOURCES kikka.cpp)
#set_target_properties(kikka PROPERTIES
#	WIN32_EXECUTABLE TRUE
#)

add_executable(kikka ${SOURCES})