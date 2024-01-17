cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(armv4vm CONFIG REQUIRED)



add_executable(example src/example.cpp)
target_link_libraries(example armv4vm::armv4vm)
