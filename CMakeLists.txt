cmake_minimum_required(VERSION 4.0.2)
project(armv4vm VERSION 1.0.4)

SET(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test)
    qt_standard_project_setup()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test)
    qt_standard_project_setup()
endif()

# FIND_PACKAGE(Qt6 COMPONENTS Core Test)

if(Qt6Core_FOUND)
    SET(CMAKE_AUTOMOC ON)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(-Wall -fPIC -Wextra -Wstrict-aliasing -Wfloat-conversion#[[-Wextra -Wstrict-aliasing -fsanitize=undefined -Wfloat-conversion -Wconversion]])
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Wno-unused-but-set-variable)
        #add_compile_options("-Wunused-but-set-variable=0")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

endif()

SET(CMAKE_CXX_STANDARD 26)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Options pour choisir le type de bibliothèque
#option(MY_LIBRARY_HEADER_ONLY "Build as header-only library" ON)
#option(MY_LIBRARY_STATIC "Build static library" OFF)

# ===========================================================
# Files
# ===========================================================

SET(ARMV4VM_HEADER_FILES
    src/vm.hpp
    src/vmproperties.hpp
    src/armv4vm.hpp
    src/armv4vm_p.hpp
    src/alu.hpp
    src/memoryhandler.hpp
    src/vfpv2.hpp
    src/coprocessor.hpp
    #[[src/assertcopro.hpp]])

SET(ARMV4VM_SOURCE_FILES
    src/vm.cpp
    src/armv4vm.cpp)

# SET(ARMV4VM_TEST2_SOURCE_FILES
#     src/tu_memoryprotect.cpp)

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
endif()

# ===========================================================
# Main Target
# ===========================================================
if(MY_LIBRARY_HEADER_ONLY)
    ADD_DEFINITIONS(-DMY_LIBRARY_HEADER_ONLY)

    # Version header-only : pas de bibliothèque physique
    add_library(armv4vm INTERFACE ${ARMV4VM_HEADER_FILES})
    target_include_directories(armv4vm INTERFACE
        $<TARGET_PROPERTY:my_library,INTERFACE_INCLUDE_DIRECTORIES>
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_features(armv4vm INTERFACE cxx_std_23)

    # Pour les utilisateurs, on peut aussi créer une version "fake"
    # qui ne fait rien mais permet de garder la même interface
    add_library(armv4vm_header_only INTERFACE)
    target_include_directories(armv4vm_header_only INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_features(armv4vm_header_only INTERFACE cxx_std_23)
    message("11111111111111111111111")
elseif(MY_LIBRARY_STATIC)
    ADD_LIBRARY(armv4vm STATIC ${ARMV4VM_HEADER_FILES} ${ARMV4VM_SOURCE_FILES})
    TARGET_LINK_LIBRARIES(armv4vm Qt6::Core)
    set_target_properties(armv4vm PROPERTIES DEBUG_POSTFIX "d")
    message("222222222222222222222222")
endif()

# ===========================================================
# Alias
# ===========================================================
if(MY_LIBRARY_HEADER_ONLY)
    add_library(armv4vm::armv4vm ALIAS armv4vm_header_only)
else()
    add_library(armv4vm::armv4vm ALIAS armv4vm)
endif()

# ===========================================================
# Test
# ===========================================================
SET(ARMV4VM_TEST_SOURCE_FILES
    src/test/main.cpp
    src/test/testmem.hpp
    src/test/testalu.hpp
    src/test/testvfp.hpp)

if(Qt6Core_FOUND)
   add_executable(tu ${ARMV4VM_TEST_SOURCE_FILES} ${ARMV4VM_HEADER_FILES})
   add_test(NAME tu COMMAND tu)
   target_include_directories(tu PRIVATE
          #$<TARGET_PROPERTY:armv4vm,INTERFACE_INCLUDE_DIRECTORIES>
          ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

if(MY_LIBRARY_HEADER_ONLY)
    if(Qt6Core_FOUND)
        target_link_libraries(tu Qt6::Core Qt6::Test armv4vm::armv4vm)
    endif()
elseif(MY_LIBRARY_STATIC)
    if(Qt6Core_FOUND)
        target_link_libraries(tu Qt6::Core Qt6::Test armv4vm::armv4vm)
    endif()
endif()

#set_target_properties(armv4vm PROPERTIES PUBLIC_HEADER "source/armv4vm.h")


# ===========================================================
# Installation
# ===========================================================
#install(TARGETS armv4vm DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

if(MY_LIBRARY_HEADER_ONLY)
    install(FILES include/myheader.h
        DESTINATION include
    )
elseif(MY_LIBRARY_STATIC)
    install(FILES include/myheader.h
        DESTINATION include
    )

    install(TARGETS armv4vm
        ARCHIVE DESTINATION lib       # pour .a
        LIBRARY DESTINATION lib       # pour .so (si librairie partagée)
        RUNTIME DESTINATION bin       # pour Windows .dll si partagé
    )
endif()
