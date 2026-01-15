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
#include "alu.hpp"

#include <cstdint>
#include <array>
#include <cmath>

namespace armv4vm {



// On verra plus tard que finalement, CRTP ne sera peut-être pas nécessaire because tout est header.
// et par conséquent, on peut faire peut-être du polymorphisme classique..le compilo saura le dégager en -O2
template<typename Derived>
class CoprocessorBase {
  public:
    CoprocessorBase(struct VmProperties * vmProperties = nullptr) : m_vmProperties(*vmProperties) {};
    //void init(AluBase * alu) { m_alu = alu; }

    void coprocessorDataTransfers(const uint32_t workingInstruction) {
        static_cast<Derived *>(this)->coprocessorDataTransfersImpl(workingInstruction);
    }

    void coprocessorDataOperations(const uint32_t workingInstruction) {
        static_cast<Derived *>(this)->coprocessorDataOperationsImpl(workingInstruction);
    }

    void coprocessorRegisterTransfers(const uint32_t workingInstruction) {
        static_cast<Derived *>(this)->coprocessorRegisterTransfersImpl(workingInstruction);
    }

    void coprocessorDataTransfersImpl(const uint32_t workingInstruction);
    void coprocessorDataOperationsImpl(const uint32_t workingInstruction);
    void coprocessorRegisterTransfersImpl(const uint32_t workingInstruction);

  protected:
    struct VmProperties  m_vmProperties;
};

template<typename Derived>
inline void CoprocessorBase<Derived>::coprocessorDataTransfersImpl(const uint32_t workingInstruction) {

    // clang-format off
    struct CoprocessorDataTransfers {

        uint32_t immediateOffset                : 8;
        uint32_t coproNumber                    : 4;
        uint32_t coproSourceDestinationRegister : 4;
        uint32_t baseRegister                   : 4;
        uint32_t loadStoreBit                   : 1;
        uint32_t writeBackBit                   : 1;
        uint32_t transferLength                 : 1;
        uint32_t upDownBit                      : 1;
        uint32_t prePostIndexingBit             : 1;
        uint32_t                                : 3;
        uint32_t condition                      : 4;

    } instruction;
    // clang-format on

    instruction = cast<CoprocessorDataTransfers>(workingInstruction);

           // On leve une exception
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}

template<typename Derived>
inline void CoprocessorBase<Derived>::coprocessorDataOperationsImpl(const uint32_t workingInstruction) {

    // clang-format off
    struct CoprocessorDataOperations {

        uint32_t operandRegisterM    : 4;
        uint32_t                     : 1;
        uint32_t information         : 3;
        uint32_t number              : 4;
        uint32_t destinationRegister : 4;
        uint32_t operandRegisterN    : 4;
        uint32_t operationCode       : 4;
        uint32_t                     : 4;
        uint32_t condition           : 4;

    } instruction;
    // clang-format on

    instruction = cast<CoprocessorDataOperations>(workingInstruction);

           // On leve une exception
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}

template<typename Derived>
inline void CoprocessorBase<Derived>::coprocessorRegisterTransfersImpl(const uint32_t workingInstruction) {

    // clang-format off
    struct CoprocessorRegisterTransfers {


        uint32_t coproOperandRegister           : 4;
        uint32_t                                : 1;
        uint32_t coproInformation               : 3;
        uint32_t coproNumber                    : 4;
        uint32_t armSourceDestinationRegister   : 4;
        uint32_t coproSourceDestinationRegister : 4;
        uint32_t loadStoreBit                   : 1;
        uint32_t coproOperationMode             : 3;
        uint32_t                                : 4;
        uint32_t condition                      : 4;

    } instruction;
    // clang-format on

    instruction = cast<CoprocessorRegisterTransfers>(workingInstruction);

           // On leve une exception
    armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
}


} // namespace armv4vm
