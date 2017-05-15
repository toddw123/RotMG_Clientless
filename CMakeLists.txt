cmake_minimum_required(VERSION 3.5)
project(Clientless)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Clientless ${SOURCES})
TARGET_LINK_LIBRARIES(Clientless curl ssl crypto pthread)