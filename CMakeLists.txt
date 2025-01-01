	cmake_minimum_required(VERSION 3.11.4)
project(armv4vm VERSION 1.0.1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

IF(WIN32)
	set (CMAKE_PREFIX_PATH "C:\\Qt\\6.8.1\\msvc2022_64\\")
	set(Qt6_DIR "C:\\Qt\\6.8.1\\msvc2022_64\\lib\\cmake\\")
ENDIF(WIN32)

FIND_PACKAGE(Qt6 COMPONENTS Core Test)

IF(Qt6Core_FOUND)
SET(CMAKE_AUTOMOC ON)
ENDIF(Qt6Core_FOUND)

IF(UNIX)
    add_compile_options(-Wall -fPIC)
ELSEIF(WIN32)
ENDIF()

SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

INCLUDE_DIRECTORIES(include)

SET(ARMV4VM_HEADER_FILES include/armv4vm.h include/memoryhandler.h)
SET(ARMV4VM_SOURCE_FILES src/armv4vm.cpp src/test2.cpp)
SET(ARMV4VM_TEST_SOURCE_FILES test/test.cpp)
SET(ARMV4VM_TEST2_SOURCE_FILES src/tu_memoryprotect.cpp)

IF(UNIX)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DDEBUG -O0 -g3 -pg --coverage)
    add_link_options(-O0 -g3 -pg --coverage)
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(UNIX)

set(BINPATH_STRING ${CMAKE_CURRENT_SOURCE_DIR})
configure_file( config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)


IF(Qt6Core_FOUND)
    SET(CMAKE_AUTOMOC ON)
    INCLUDE_DIRECTORIES(SYSTEM ${QT_USE_FILE})
    ADD_DEFINITIONS(-D${Qt6Core_COMPILE_DEFINITIONS})
    ADD_LIBRARY(armv4vm STATIC ${ARMV4VM_SOURCE_FILES} ${ARMV4VM_HEADER_FILES})
    TARGET_LINK_LIBRARIES(armv4vm Qt6::Core)
ELSE(Qt6Core_FOUND)
    ADD_LIBRARY(armv4vm STATIC ${ARMV4VM_SOURCE_FILES} ${ARMV4VM_HEADER_FILES})
ENDIF(Qt6Core_FOUND)

IF(Qt6Core_FOUND)
    add_executable(tu ${ARMV4VM_TEST_SOURCE_FILES})
    add_test(tu ${ARMV4VM_TEST_SOURCE_FILES})
    add_executable(tu2 ${ARMV4VM_TEST2_SOURCE_FILES})
    add_test(tu2 ${ARMV4VM_TEST2_SOURCE_FILES})
    target_link_libraries(tu Qt6::Core Qt6::Test armv4vm)
    target_link_libraries(tu2 Qt6::Core Qt6::Test armv4vm)
ENDIF(Qt6Core_FOUND)

set_target_properties(armv4vm PROPERTIES PUBLIC_HEADER "include/armv4vm.h")
#install(TARGETS armv4vm DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
install(TARGETS armv4vm)

