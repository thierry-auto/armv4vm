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

#include "coprocessor.hpp"

#include <cstdint>

namespace armv4vm {

class TestMem;
template<typename T>
class TestVfpInstruction;
class TestVfp;

template <typename MemoryHandler>
class NullCopro final : public CoprocessorBase<NullCopro<MemoryHandler>> {

  public:
    friend class TestMem;
    friend TestAluInstruction<MemoryHandler>;
    friend TestVfpInstruction<MemoryHandler>;

  public:
    friend TestVfpInstruction<MemoryHandler>;
    friend TestVfp;

    NullCopro(struct CoproProperties & properties) : CoprocessorBase<NullCopro<MemoryHandler>>(properties) {}

    void coprocessorDataTransfersImpl(const uint32_t workingInstruction);
    void coprocessorDataOperationsImpl(const uint32_t workingInstruction);
    void coprocessorRegisterTransfersImpl(const uint32_t workingInstruction);

    void attach(MemoryHandler *mem) { m_mem = mem; }
    void attach(Alu<MemoryHandler, NullCopro> *alu) { m_alu = alu; }

  private:
    MemoryHandler *m_mem;
    Alu<MemoryHandler, NullCopro>  *m_alu;
    friend class TestVm;
};

template <typename MemoryHandler>
inline void NullCopro<MemoryHandler>::coprocessorDataTransfersImpl([[maybe_unused]]const uint32_t workingInstruction) {
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}

template <typename MemoryHandler>
inline void NullCopro<MemoryHandler>::coprocessorDataOperationsImpl([[maybe_unused]]const uint32_t workingInstruction) {
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}

template <typename MemoryHandler>
inline void NullCopro<MemoryHandler>::coprocessorRegisterTransfersImpl([[maybe_unused]]const uint32_t workingInstruction) {
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}

} // namespace armv4vm
