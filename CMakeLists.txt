cmake_minimum_required(VERSION 3.0...3.28)

project(NoodleScript)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(NoodleScript ${SRCS})
