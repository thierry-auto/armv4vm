# Load the debug and release variables
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB DATA_FILES "${_DIR}/armv4vm-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${armv4vm_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${armv4vm_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET armv4vm::armv4vm)
    add_library(armv4vm::armv4vm INTERFACE IMPORTED)
    message(${armv4vm_MESSAGE_MODE} "Conan: Target declared 'armv4vm::armv4vm'")
endif()
# Load the debug and release library finders
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/armv4vm-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()