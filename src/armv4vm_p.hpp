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

#include <cstdint>
#include <iostream>
#include <cassert>

#define armv4vm_assert(__FUNCTION__, __FILE__, __LINE__)                                \
{                                                                                       \
        std::cerr << __FUNCTION__ << " " << __FILE__ << " " << __LINE__ << std::endl;   \
        assert(0);                                                                      \
}

namespace armv4vm {

template<typename T>
inline constexpr T BITS(T x, unsigned int l, unsigned int h) {
    return (x >> l) & ((T{1} << ((h - l) + 1)) - 1);
}

template <typename T>
static inline T cast(uint32_t instruction) {
    return *reinterpret_cast<T *>(&instruction);
}

enum class Interrupt : int32_t {

    Resume     = 1,
    Stop       = 2,
    Suspend    = 3,
    LockPop    = 4,
    UnlockPop  = 5,
    LockPush   = 6,
    UnlockPush = 7,
    Fatal      = 8,
    Undefined  = 9,
};

} // namespace armv4vm
