//    Copyright (c) 2020-26, thierry vic
//
//    This file is part of armv4vm.
//
//    armv4vm is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    armv4vm is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with armv4vm.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "armv4vm_p.hpp"
#include "vmproperties.hpp"
#include "memoryhandler.hpp"
#include "vfpv2.hpp"
#include "alu.hpp"

// todo : traiter les overflow
constexpr std::uint64_t operator""_mb(const unsigned long long value) {
    return value * 1024ULL * 1024ULL;
}

constexpr std::uint64_t operator""_gb(const unsigned long long value) {
    return value * 1024ULL * 1024ULL * 1024ULL;
}

enum class AccessPermission {
    NONE    = 0b0000,
    READ    = 0b0001,
    WRITE   = 0b0010,
    READ_WRITE = READ | WRITE,
};

class AccessRange {
  public:
    uint32_t         start;
    size_t           size;
    AccessPermission permission;
};

//extern template class armv4vm::Alu<armv4vm::MemoryRaw, armv4vm::Vfpv2>;
//extern template class armv4vm::Alu<armv4vm::MemoryProtected, armv4vm::Vfpv2>;

//using VirtualMachineUnprotected = armv4vm::Alu<armv4vm::MemoryRaw>;
//using VirtualMachineProtected   = armv4vm::Alu<armv4vm::MemoryProtected, armv4vm::Vfpv2>;

