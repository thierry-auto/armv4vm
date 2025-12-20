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
#include "memoryhandler.hpp"
#include "coprocessor.hpp"
#include "alu.hpp"

// todo : traiter les overflow
constexpr std::uint64_t operator""_mb(const unsigned long long value) {
    return value * 1024ULL * 1024ULL;
}

constexpr std::uint64_t operator""_gb(const unsigned long long value) {
    return value * 1024ULL * 1024ULL * 1024ULL;
}

namespace armv4vm {

extern template class Alu<MemoryRaw, Vfpv2>;
extern template class Alu<MemoryProtected, Vfpv2>;

using VirtualMachineUnprotected = Alu<MemoryRaw, Vfpv2>;
using VirtualMachineProtected   = Alu<MemoryProtected, Vfpv2>;

} // namespace armv4vm
