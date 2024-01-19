message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(armv4vm)

set(CONANDEPS_LEGACY  armv4vm::armv4vm )