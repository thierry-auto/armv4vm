# 29/08/2025

cmake_minimum_required(VERSION 4.0.2)
project(armv4vm VERSION 1.0.4)

SET(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test)

    qt_standard_project_setup()

endif()

# FIND_PACKAGE(Qt6 COMPONENTS Core Test)

if(Qt6Core_FOUND)
	SET(CMAKE_AUTOMOC ON)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(-Wall -fPIC)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif()

SET(CMAKE_CXX_STANDARD 26)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)


SET(ARMV4VM_HEADER_FILES src/armv4vm.hpp src/armv4vm_p.hpp src/alu.hpp src/alu.tpp src/memoryhandler.hpp #[[src/vfpv2.hpp]] src/coprocessor.hpp src/assertcopro.hpp)
SET(ARMV4VM_SOURCE_FILES #[[src/vfpv2.cpp]] src/armv4vm.cpp src/assertcopro.cpp)
SET(ARMV4VM_TEST_SOURCE_FILES src/test/test.cpp src/test/testvfpv2.cpp src/test2.cpp)
SET(ARMV4VM_TEST2_SOURCE_FILES src/tu_memoryprotect.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DDEBUG -O0 -g3 -pg --coverage)
    add_link_options(-O0 -g3 -pg --coverage)
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif()

set(BINPATH_STRING ${CMAKE_CURRENT_SOURCE_DIR})
configure_file( config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)


if(Qt6Core_FOUND)
    SET(CMAKE_AUTOMOC ON)
    INCLUDE_DIRECTORIES(SYSTEM ${QT_USE_FILE})
    ADD_DEFINITIONS(-D${Qt6Core_COMPILE_DEFINITIONS})
    ADD_LIBRARY(armv4vm STATIC ${ARMV4VM_SOURCE_FILES} ${ARMV4VM_HEADER_FILES})
    TARGET_LINK_LIBRARIES(armv4vm Qt6::Core)
else()
    ADD_LIBRARY(armv4vm STATIC ${ARMV4VM_SOURCE_FILES} ${ARMV4VM_HEADER_FILES})
endif()

if(Qt6Core_FOUND)


    add_executable(tu ${ARMV4VM_TEST_SOURCE_FILES})
    add_test(tu ${ARMV4VM_TEST_SOURCE_FILES})
    add_executable(tu2 ${ARMV4VM_TEST2_SOURCE_FILES})
    add_test(tu2 ${ARMV4VM_TEST2_SOURCE_FILES})
    target_link_libraries(tu Qt6::Core Qt6::Test armv4vm)
    target_link_libraries(tu2 Qt6::Core Qt6::Test armv4vm)

    #target_link_libraries(test_qt PRIVATE Qt::Core )
endif()

set_target_properties(armv4vm PROPERTIES PUBLIC_HEADER "source/armv4vm.h")
set_target_properties(armv4vm PROPERTIES DEBUG_POSTFIX "d")

#install(TARGETS armv4vm DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

install(TARGETS armv4vm
    ARCHIVE DESTINATION lib       # pour .a
    LIBRARY DESTINATION lib       # pour .so (si librairie partagée)
    RUNTIME DESTINATION bin       # pour Windows .dll si partagé
)

install(FILES include/myheader.h
    DESTINATION include
)
