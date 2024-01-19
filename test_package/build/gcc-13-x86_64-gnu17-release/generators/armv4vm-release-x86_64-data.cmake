########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(armv4vm_COMPONENT_NAMES "")
set(armv4vm_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(armv4vm_PACKAGE_FOLDER_RELEASE "/home/th/.conan2/p/b/armv42ec704b8f253d/p")
set(armv4vm_BUILD_MODULES_PATHS_RELEASE )


set(armv4vm_INCLUDE_DIRS_RELEASE "${armv4vm_PACKAGE_FOLDER_RELEASE}/include")
set(armv4vm_RES_DIRS_RELEASE )
set(armv4vm_DEFINITIONS_RELEASE )
set(armv4vm_SHARED_LINK_FLAGS_RELEASE )
set(armv4vm_EXE_LINK_FLAGS_RELEASE )
set(armv4vm_OBJECTS_RELEASE )
set(armv4vm_COMPILE_DEFINITIONS_RELEASE )
set(armv4vm_COMPILE_OPTIONS_C_RELEASE )
set(armv4vm_COMPILE_OPTIONS_CXX_RELEASE )
set(armv4vm_LIB_DIRS_RELEASE "${armv4vm_PACKAGE_FOLDER_RELEASE}/lib")
set(armv4vm_BIN_DIRS_RELEASE )
set(armv4vm_LIBRARY_TYPE_RELEASE STATIC)
set(armv4vm_IS_HOST_WINDOWS_RELEASE 0)
set(armv4vm_LIBS_RELEASE armv4vm)
set(armv4vm_SYSTEM_LIBS_RELEASE )
set(armv4vm_FRAMEWORK_DIRS_RELEASE )
set(armv4vm_FRAMEWORKS_RELEASE )
set(armv4vm_BUILD_DIRS_RELEASE )
set(armv4vm_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(armv4vm_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${armv4vm_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${armv4vm_COMPILE_OPTIONS_C_RELEASE}>")
set(armv4vm_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${armv4vm_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${armv4vm_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${armv4vm_EXE_LINK_FLAGS_RELEASE}>")


set(armv4vm_COMPONENTS_RELEASE )