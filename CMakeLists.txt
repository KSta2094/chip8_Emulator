cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project (out)



FILE(GLOB SOURCE "src/*.c")
add_executable(out ${SOURCE})
