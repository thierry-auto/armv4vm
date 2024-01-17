#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define ARMV4VM_EXPORT __declspec(dllexport)
#else
  #define ARMV4VM_EXPORT
#endif

ARMV4VM_EXPORT void armv4vm();
ARMV4VM_EXPORT void armv4vm_print_vector(const std::vector<std::string> &strings);
