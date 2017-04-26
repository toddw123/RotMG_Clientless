cmake_minimum_required(VERSION 3.7)
project(Clientless)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Clientless ${SOURCES})
TARGET_LINK_LIBRARIES(Clientless curl ssl crypto pthread)