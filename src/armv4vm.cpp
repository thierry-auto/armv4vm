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

#include "armv4vm.hpp"

namespace armv4vm {

// instanciations des templates explicites

template class NullCopro<MemoryRaw>;
template class NullCopro<MemoryProtected>;

template class Alu<MemoryRaw, NullCoproUnsafe>;
template class Alu<MemoryProtected, NullCoproSafe>;

} // namespace armv4vm


