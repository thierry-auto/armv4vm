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

#include "armv4vm_p.hpp"        // IWYU pragma: export
#include "vmproperties.hpp"     // IWYU pragma: export
#include "memoryhandler.hpp"    // IWYU pragma: export
#include "vfpv2.hpp"            // IWYU pragma: export
#include "alu.hpp"              // IWYU pragma: export
#include "vm.hpp"               // IWYU pragma: export

//extern template class armv4vm::Alu<armv4vm::MemoryRaw, armv4vm::Vfpv2>;
//extern template class armv4vm::Alu<armv4vm::MemoryProtected, armv4vm::Vfpv2>;

//using VirtualMachineUnprotected = armv4vm::Alu<armv4vm::MemoryRaw>;
//using VirtualMachineProtected   = armv4vm::Alu<armv4vm::MemoryProtected, armv4vm::Vfpv2>;

