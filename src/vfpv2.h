#pragma once

#include <cstdint>
#include <array>

#include "armv4vm.h"

namespace armv4vm {

template<typename T>
class Vfpv2 : public CoprocessorBase<T> {
  public:
    Vfpv2(VirtualMachineBase* vm) : CoprocessorBase<T>(vm) {}
    //Vfpv2(VirtualMachineProtected* vm) : CoprocessorBase(vm) {}

    void coprocessorDataTransfers(const uint32_t m_workingInstruction) override;
    void coprocessorDataOperations(const uint32_t m_workingInstruction) override;
    void coprocessorRegisterTransfers(const uint32_t m_workingInstruction) override;

  private:
    std::array<float, 32> m_sRegisters;

    void decodeAndExecute(const uint32_t &wokingInstruction);
    void vadd(const uint32_t &wokingInstruction);
};

// Registre automatique à la construction du programme

const bool registered = []{

    CoprocessorBase::Factory f1 = [](VirtualMachineBase* vm) { return std::make_unique<Vfpv2>(vm); };
    CoprocessorBase::registerType("vfpv2", f1);

    return true;
}();



} // namespace armv4vm

#if 0
0xee777a87
    0xeea21103


    ee777a87

                                                                VD   VN   VM
    1110 11100 1 11 0111 0111 101 0 1 0 0 0 0111   vadd.f32     s15, s15, s14
    1110 11100 1 11 0111 0111 101 0 0 0 1 0 0111   vadd.f32	s15, s14, s15
               D    VN   VD       S N   M   VM

    1110 1110 1010 0010 0001 0001 000 0 0011   CDP p1,10,c1,c2,c3
                    OPCODE         CP01       C3
#endif
