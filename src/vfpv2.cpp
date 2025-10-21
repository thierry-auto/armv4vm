#include "vfpv2.h"

// VFPv2 ====
// An optional extension to the ARM instruction set in the ARMv5TE, ARMv5TEJ and ARMv6 architectures. VFPv2 has 16 64-bit FPU registers.
// ARMv5TE = ARM7EJ, ARM9E, ARM10E

namespace armv4vm {

template <typename VM>
void Vfpv2<VM>::coprocessorDataTransfers(const uint32_t m_workingInstruction) {

    // clang-format off
    struct CoprocessorDataOperations {

        uint32_t immediate           : 8;
        uint32_t                     : 4;
        uint32_t vd                  : 4;
        uint32_t rn                  : 4;
        uint32_t                     : 1;
        uint32_t                     : 1;
        uint32_t d                   : 1;
        uint32_t u                   : 1;
        uint32_t                     : 4;
        uint32_t condition           : 4;

    } instruction;
    // clang-format on

    instruction = cast<CoprocessorDataOperations>(m_workingInstruction);

    if(instruction.rn == 15) {

    }
    else {

        uint32_t address = instruction.u ? instruction.rn + instruction.immediate : instruction.rn - instruction.immediate;
        //m_sRegisters[(instruction.vd << 1) | instruction.d] = readPointer<uint32_t>(*m_mem + address);
    }


           // On leve une exception
    qt_assert(__FUNCTION__, __FILE__, __LINE__);
}

template <typename VM>
void Vfpv2<VM>::coprocessorDataOperations(const uint32_t m_workingInstruction) {

    qt_assert(__FUNCTION__, __FILE__, __LINE__); // remplacer par std::assert ou autre
}

template <typename VM>
void Vfpv2<VM>::coprocessorRegisterTransfers(const uint32_t m_workingInstruction) {

    qt_assert(__FUNCTION__, __FILE__, __LINE__); // remplacer par std::assert ou autre
}

template <typename VM>
void Vfpv2<VM>::decodeAndExecute(const uint32_t &wokingInstruction) {

    const uint32_t DATA_PROCESSING      = 0x00000000;
    const uint32_t MASK_DATA_PROCESSING = 0x0C000000;

    if ((wokingInstruction & MASK_DATA_PROCESSING) == DATA_PROCESSING) {

        //vadd(wokingInstruction);
    } else {
        qt_assert(__FUNCTION__, __FILE__, __LINE__); // remplacer par std::assert ou autre
    }
}

template <typename VM>
void Vfpv2<VM>::vadd(const uint32_t &wokingInstruction) {

    // clang-format off
    struct Multiply {

        uint32_t rm        : 4;
        uint32_t           : 4;
        uint32_t rs        : 4;
        uint32_t rn        : 4;
        uint32_t rd        : 4;
        uint32_t s         : 1;
        uint32_t a         : 1;
        uint32_t           : 6;
        uint32_t condition : 4;

    } instruction;
    // clang-format on

           // 4.12
    instruction = cast<Multiply>(wokingInstruction);
}

} // namespace armv4vm
