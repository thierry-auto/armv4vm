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
#include "properties.hpp"       // IWYU pragma: export
#include "memoryhandler.hpp"    // IWYU pragma: export
#include "nullcopro.hpp"        // IWYU pragma: export
#include "alu.hpp"              // IWYU pragma: export
#include "vm.hpp"               // IWYU pragma: export


using NullCoproUnsafe = armv4vm::NullCopro<armv4vm::MemoryRaw>;
using NullCoproSafe = armv4vm::NullCopro<armv4vm::MemoryProtected>;
using VirtualMachineUnsafe = armv4vm::Alu<armv4vm::MemoryRaw, NullCoproUnsafe>;
using VirtualMachineSafe   = armv4vm::Alu<armv4vm::MemoryProtected, NullCoproSafe>;

#ifdef MY_LIBRARY_STATIC_LIB

// Ne genere pas les constructions suivantes quand ce header est appelé.
// Elles seront construites une seule fois explicitement dans armv4vm.cpp

extern template class armv4vm::NullCopro<armv4vm::MemoryRaw>;
extern template class armv4vm::NullCopro<armv4vm::MemoryProtected>;

extern template class armv4vm::Alu<armv4vm::MemoryRaw, NullCoproUnsafe>;
extern template class armv4vm::Alu<armv4vm::MemoryProtected, NullCoproSafe>;

#endif
