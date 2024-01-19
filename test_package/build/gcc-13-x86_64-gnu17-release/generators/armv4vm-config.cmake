########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(armv4vm_FIND_QUIETLY)
    set(armv4vm_MESSAGE_MODE VERBOSE)
else()
    set(armv4vm_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/armv4vmTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${armv4vm_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(armv4vm_VERSION_STRING "1.0")
set(armv4vm_INCLUDE_DIRS ${armv4vm_INCLUDE_DIRS_RELEASE} )
set(armv4vm_INCLUDE_DIR ${armv4vm_INCLUDE_DIRS_RELEASE} )
set(armv4vm_LIBRARIES ${armv4vm_LIBRARIES_RELEASE} )
set(armv4vm_DEFINITIONS ${armv4vm_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${armv4vm_BUILD_MODULES_PATHS_RELEASE} )
    message(${armv4vm_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


