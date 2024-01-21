from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import *
import os
from conan import ConanFile
from conan.tools.files import copy
from os.path import join

class armv4vmRecipe(ConanFile):
    name = "armv4vm"
    version = "1.0"
    package_type = "library"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of armv4vm package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*",  "test/*", "config.h.in"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)        
        copy(self, "*.h", join(self.source_folder, "include"), join(self.package_folder, "include"))
        cmake.install()

    def package_info(self):
        # pour donner accès aux include depuis au projets parents
        self.cpp_info.libs = ["armv4vm"]        
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.set_property("cmake_file_name", "armv4vm")
        self.cpp_info.set_property("cmake_target_name", "armv4vm::armv4vm")
        #self.cpp_info.includedirs.append(os.path.join("include", "armv4vm"))
    

    
