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
#include "alu.hpp"

#include <cstdint>
#include <array>
#include <cmath>

namespace armv4vm {

// class DoubleInFloatArray {
//   public:
//     DoubleInFloatArray(std::array<float, 32>* floatArray, size_t index) : m_floatArray(floatArray), m_index(index) {}

//            // Opérateur d'affectation pour écrire un double
//     DoubleInFloatArray& operator=(double value) {
//         std::memcpy(&m_floatArray[m_index], &value, sizeof(double));
//         return *this;
//     }

//            // Opérateur de conversion pour lire un double
//     operator double() const {
//         double value;
//         std::memcpy(&value, &m_floatArray[m_index], sizeof(double));
//         return value;
//     }

//   private:
//     std::array<float, 32> *m_floatArray;
//     size_t m_index;
// };

// On verra plus tard que finalement, CRTP ne sera peut-être pas nécessaire because tout est header.
// et par conséquent, on peut faire peut-être du polymorphisme classique..le compilo saura le dégager en -O2
template<typename Derived>
class CoprocessorBase {
  public:
    CoprocessorBase() {}
    void init(AluBase * alu) { m_alu = alu; }

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
    AluBase * m_alu;
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

// Obligé de faire mon propre trait. std::floating_point autorise long double.
template <typename T>
concept FloatingPoint = std::same_as<T, float> || std::same_as<T, double> /*|| std::same_as<T, DoubleInFloatArray>*/;

class Vfpv2 : public CoprocessorBase<Vfpv2> {
  public:
    Vfpv2() : m_fpscr(0), m_fpexc(0) {

        m_sRegisters.fill(0.0f);
    }

    void coprocessorDataTransfersImpl(const uint32_t workingInstruction);
    void coprocessorDataOperationsImpl(const uint32_t workingInstruction);
    void coprocessorRegisterTransfersImpl(const uint32_t workingInstruction);

  private:
    std::array<float, 32> m_sRegisters;

    // Demander un jour a un expert (pas une ia..) si ce ne serait pas
    // plus simple et sans risque de faire une union.. On s'apargnerait
    // ainsi tous ces cast bien relouds..
    // union { float registerFloat[32], double registersDouble[16] };

    uint32_t m_fpscr;
    uint32_t m_fpexc;
    const uint32_t m_fpsid = 0x54001000; // 54 comme T :p

    void decodeAndExecute(const uint32_t &wokingInstruction);
    void vadd(const uint32_t &wokingInstruction);

    inline const float& toSingle(const uint32_t index) const noexcept {
        return m_sRegisters[index];
    }

    inline const double toDouble(const uint32_t index) const noexcept {
        double result;
        memcpy(&result, &m_sRegisters[index * 2], sizeof(double));
        return result;
    }

    inline void setSingleRegister(const int index, const float value) noexcept {
        m_sRegisters[index] = value;
    }

    void setDoubleRegister(const int index, const double value) noexcept {
        memcpy(&m_sRegisters[index], &value, sizeof(double));
    }

    static uint32_t getExtensionOpcode(const uint32_t instruction) noexcept;
    void setNZCV(bool N, bool Z, bool C, bool V) noexcept;
    void updateCPSR() const;

    template <FloatingPoint T>
    void setFPSCRCompare(const T a, const T b, const bool raiseException);

    friend class TestVm;
};

inline void Vfpv2::coprocessorDataTransfersImpl(const uint32_t workingInstruction) {

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

inline uint32_t Vfpv2::getExtensionOpcode(const uint32_t instruction) noexcept {

    return (((instruction >> 16) & 0xF) << 1) | ((instruction >> 7) & 0x1);
}

inline void Vfpv2::setNZCV(bool N, bool Z, bool C, bool V) noexcept {

    m_fpscr &= ~(0xF << 28);
    m_fpscr |= (uint32_t(N) << 31) | (uint32_t(Z) << 30) | (uint32_t(C) << 29) | (uint32_t(V) << 28);
}

inline void Vfpv2::updateCPSR() const {
    m_alu->setCPSR((m_alu->getCPSR() & ~(0xF << 28)) | (m_fpscr & (0xF << 28)));
}

template <FloatingPoint T>
inline void Vfpv2::setFPSCRCompare(const T a, const T b, const bool raiseVFPV2Exception) {

    if (std::isnan(a) || std::isnan(b)) {

        setNZCV(0, 0, 0, 1);
        if(raiseVFPV2Exception) {

        }
    } else if (a < b) {

        setNZCV(1, 0, 0, 0);
    } else if (a == b) {

        setNZCV(0, 1, 1, 0);
    } else {

        setNZCV(0, 0, 1, 0);
    }

           // Update Alu CPSR from FPSCR
    updateCPSR();
}

inline void Vfpv2::coprocessorDataOperationsImpl(const uint32_t workingInstruction) {

    constexpr uint32_t MASK_SD     = 0x00000300; // bits 9:8
    constexpr uint32_t SD_DOUBLE   = 0x00000200;

           // Composition            0bpqrs
    constexpr auto OP_FMAC  = 0b0000;
    constexpr auto OP_FNMAC = 0b0001;
    constexpr auto OP_FMSC  = 0b0010;
    constexpr auto OP_FNMSC = 0b0011;
    constexpr auto OP_FMUL  = 0b0100;
    constexpr auto OP_FNMUL = 0b0101;
    constexpr auto OP_FADD  = 0b0110;
    constexpr auto OP_FSUB  = 0b0111;
    constexpr auto OP_FDIV  = 0b1000;
    constexpr auto OP_EXTE  = 0b1111;

           // Composition extension        0b-Fn-N
    constexpr auto OP_EXTE_FCPY   = 0b00000;
    constexpr auto OP_EXTE_FABS   = 0b00001;
    constexpr auto OP_EXTE_FNEG   = 0b00010;
    constexpr auto OP_EXTE_FSQRT  = 0b00011;
    constexpr auto OP_EXTE_FCMP   = 0b01000;
    constexpr auto OP_EXTE_FCMPE  = 0b01001;
    constexpr auto OP_EXTE_FCMPZ  = 0b01010;
    constexpr auto OP_EXTE_FCMPEZ = 0b01011;
    constexpr auto OP_EXTE_FCVTD  = 0b01111;
    constexpr auto OP_EXTE_FUITO  = 0b10000;
    constexpr auto OP_EXTE_FSITO  = 0b10001;
    constexpr auto OP_EXTE_FTOUI  = 0b11000;
    constexpr auto OP_EXTE_FTOUIZ = 0b11001;
    constexpr auto OP_EXTE_FTOSI  = 0b11010;
    constexpr auto OP_EXTE_FTOSIZ = 0b11011;

    bool isDouble = false;
    uint32_t Fd;
    uint32_t Fn;
    uint32_t Fm;

    if((workingInstruction & MASK_SD) == SD_DOUBLE) {

        isDouble = true;
        Fn = BITS(workingInstruction, 16, 19);
        Fd = BITS(workingInstruction, 12, 15);
        Fm = BITS(workingInstruction, 0, 3);
    }
    else {
        Fn = (BITS(workingInstruction, 16, 19) << 1) | BITS(workingInstruction, 7, 7);
        Fd = (BITS(workingInstruction, 12, 15) << 1) | BITS(workingInstruction, 22, 22);
        Fm = (BITS(workingInstruction, 0, 3)   << 1) | BITS(workingInstruction, 5, 5);
    }

    switch (BITS(workingInstruction, 24, 27)) {

    case OP_FADD:
        if (isDouble) setDoubleRegister(Fd, toDouble(Fn) + toDouble(Fm));
        else     setSingleRegister(Fd, toSingle(Fn) + toSingle(Fm));
        break;

    case OP_FSUB:
        if (isDouble) setDoubleRegister(Fd, toDouble(Fn) - toDouble(Fm));
        else     setSingleRegister(Fd, toSingle(Fn) - toSingle(Fm));
        break;

    case OP_FMUL:
        if (isDouble) setDoubleRegister(Fd, toDouble(Fn) * toDouble(Fm));
        else     setSingleRegister(Fd, toSingle(Fn) * toSingle(Fm));
        break;

    case OP_FNMUL:
        if (isDouble) setDoubleRegister(Fd, -(toDouble(Fn) * toDouble(Fm)));
        else     setSingleRegister(Fd, -(toSingle(Fn) * toSingle(Fm)));
        break;

    case OP_FDIV:
        if (isDouble) setDoubleRegister(Fd, toDouble(Fn) / toDouble(Fm));
        else     setSingleRegister(Fd, toSingle(Fn) / toSingle(Fm));
        break;

    case OP_FMAC:
        if (isDouble) setDoubleRegister(Fd, (toDouble(Fn) * toDouble(Fm)) + toDouble(Fd));
        else     setSingleRegister(Fd, (toSingle(Fn) * toSingle(Fm)) + toSingle(Fd));
        break;

    case OP_FNMAC:
        if (isDouble) setDoubleRegister(Fd, -(toDouble(Fn) * toDouble(Fm)) + toDouble(Fd));
        else     setSingleRegister(Fd, -(toSingle(Fn) * toSingle(Fm)) + toSingle(Fd));
        break;

    case OP_FMSC:
        if (isDouble) setDoubleRegister(Fd, toDouble(Fd) - (toDouble(Fn) * toDouble(Fm)));
        else     setSingleRegister(Fd, toSingle(Fd) - (toSingle(Fn) * toSingle(Fm)));
        break;

    case OP_FNMSC:
        if (isDouble) setDoubleRegister(Fd, -(toDouble(Fn) * toDouble(Fm)) - toDouble(Fd));
        else     setSingleRegister(Fd, -(toSingle(Fn) * toSingle(Fm)) - toSingle(Fd));
        break;

    case OP_EXTE:
        switch(getExtensionOpcode(workingInstruction)) {

        case OP_EXTE_FCPY:
            if (isDouble) setDoubleRegister(Fd, toDouble(Fm));
            else setSingleRegister(Fd, toSingle(Fm));
            break;

        case OP_EXTE_FNEG:
            if (isDouble) setDoubleRegister(Fd, -toDouble(Fm));
            else setSingleRegister(Fd, -toSingle(Fm));
            break;

        case OP_EXTE_FABS:
            if (isDouble) setDoubleRegister(Fd, std::fabs(toDouble(Fm)));
            else setSingleRegister(Fd, std::fabs(toSingle(Fm)));
            break;

        case OP_EXTE_FSQRT:
            if (isDouble) setDoubleRegister(Fd, std::sqrt(toDouble(Fm)));
            else setSingleRegister(Fd, std::sqrt(toSingle(Fm)));
            break;

        case OP_EXTE_FTOSIZ:
            if (isDouble) setSingleRegister(Fd, static_cast<int32_t>(toDouble(Fm)));
            else setSingleRegister(Fd, static_cast<int32_t>(toSingle(Fm)));
            break;

        case OP_EXTE_FTOUIZ:
            if (isDouble) setSingleRegister(Fd, static_cast<uint32_t>(toDouble(Fm)));
            else setSingleRegister(Fd, static_cast<uint32_t>(toSingle(Fm)));
            break;

        case OP_EXTE_FSITO:
            if (isDouble) setDoubleRegister(Fd, static_cast<int32_t>(BITS(workingInstruction, 0, 3)));
            else setSingleRegister(Fd, static_cast<int32_t>(BITS(workingInstruction, 0, 3)));
            break;

        case OP_EXTE_FUITO:
            if (isDouble) setDoubleRegister(Fd, static_cast<uint32_t>(BITS(workingInstruction, 0, 3)));
            else setSingleRegister(Fd, static_cast<uint32_t>(BITS(workingInstruction, 0, 3)));
            break;

        case OP_EXTE_FCMP:
            isDouble ? setFPSCRCompare<double>(toDouble(Fm), toDouble(Fd), false) : setFPSCRCompare<float>(toSingle(Fm), toSingle(Fd), false);
            break;

        case OP_EXTE_FCMPE:
            isDouble ? setFPSCRCompare(toDouble(Fm), toDouble(Fd), true) : setFPSCRCompare(toSingle(Fm), toSingle(Fd), true);
            break;

        case OP_EXTE_FCMPZ:
            isDouble ? setFPSCRCompare(toDouble(Fm), 0.0, false) : setFPSCRCompare(toSingle(Fm), 0.0f, false);
            break;

        case OP_EXTE_FCMPEZ:
            isDouble ? setFPSCRCompare(toDouble(Fm), 0.0, true) : setFPSCRCompare(toSingle(Fm), 0.0f, true);
            break;

        case OP_EXTE_FCVTD:
            setDoubleRegister(Fd, static_cast<double>(toSingle(Fm)));
            break;

        case OP_EXTE_FTOUI:
            isDouble ? setSingleRegister(Fd, static_cast<uint32_t>(toDouble(Fm))) : setSingleRegister(Fd, static_cast<uint32_t>(toSingle(Fm)));
            break;

        case OP_EXTE_FTOSI:
            isDouble ? setSingleRegister(Fd, static_cast<int32_t>(toDouble(Fm))) : setSingleRegister(Fd, static_cast<int32_t>(toSingle(Fm)));
            break;

        default:
            armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
            break;
        }
        break;

    default:
        armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
    }
}


inline void Vfpv2::coprocessorRegisterTransfersImpl(const uint32_t workingInstruction) {

    constexpr uint32_t OP_MASK  = 0x0FF00F00;
    constexpr uint32_t OP_FMSR  = 0x0E000a00;
    constexpr uint32_t OP_FMRS  = 0x0E100a00;
    constexpr uint32_t OP_FMDLR = 0x0E000700;
    constexpr uint32_t OP_FMRDL = 0x0E100700;
    constexpr uint32_t OP_FMDHR = 0x0E200700;
    constexpr uint32_t OP_FMRDH = 0x0E300700;
    constexpr uint32_t OP_FMXR  = 0x0EE00600;
    constexpr uint32_t OP_FMRX  = 0x0EF00600;

    constexpr uint32_t OP_FMSRR = 0x0C400A00;
    constexpr uint32_t OP_FMRRS = 0x0C500A00;
    constexpr uint32_t OP_FMDRR = 0x0C400B00;
    constexpr uint32_t OP_FMRRD = 0x0C500B00;

    constexpr auto FPSID  = 0b0000;
    constexpr auto FPSCR  = 0b0001;
    constexpr auto FPEXC  = 0b1000;

    const uint32_t Rd = BITS(workingInstruction, 12, 15);
    uint32_t Fn, Rn, Fm;

    auto decodeFnSingle = [](const uint32_t instruction) {
        return (BITS(instruction, 16, 19) << 1) | BITS(instruction, 7, 7);
    };

    auto decodeFnDouble = [](const uint32_t instruction) {
        return BITS(instruction, 16, 19);
    };

    auto decodeFmSingle = [](const uint32_t instruction) {
        return (BITS(instruction, 0, 3) << 1) | BITS(instruction, 5, 5);
    };

    auto decodeFmDouble = [](const uint32_t instruction) {
        return BITS(instruction, 0, 3);
    };

    switch (workingInstruction & OP_MASK) {

    case OP_FMSR:
        Fn = decodeFnSingle(workingInstruction);
        m_alu->getRegisters()[Rd] = toSingle(Fn);
        break;

    case OP_FMRS:
        Fn = decodeFnSingle(workingInstruction);
        m_alu->getRegisters()[Rd] = toSingle(Fn);
        break;

    case OP_FMDLR:
        Fn = decodeFnDouble(workingInstruction);
        m_sRegisters[Fn] = m_alu->getRegisters()[Rd];
        break;

    case OP_FMRDL:
        Fn = decodeFnDouble(workingInstruction);
        m_alu->getRegisters()[Rd] = m_sRegisters[Fn];
        break;

    case OP_FMDHR:
         Fn = decodeFnDouble(workingInstruction);
         m_sRegisters[Fn * 2 + 1] = m_alu->getRegisters()[Rd];
        break;

    case OP_FMRDH:
        Fn = decodeFnDouble(workingInstruction);
        m_alu->getRegisters()[Rd] = m_sRegisters[Fn * 2 + 1];
        break;

    case OP_FMXR:
        Fn = decodeFnDouble(workingInstruction);
        switch (Fn)
        {
        case FPSCR:
            m_fpscr = m_alu->getRegisters()[Rd];
            break;

        case FPEXC:
            m_fpexc = m_alu->getRegisters()[Rd];
            break;

        default:
        case FPSID:
            // read-only → ignore ou UNPREDICTABLE
            armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
            break;
        }
        break;

    case OP_FMRX:
        Fn = decodeFnDouble(workingInstruction);
        switch (Fn)
        {
        case FPSCR:
            m_alu->getRegisters()[Rd] = m_fpscr;
            break;

        case FPEXC:
            m_alu->getRegisters()[Rd] = m_fpexc;
            break;

        case FPSID:
            m_alu->getRegisters()[Rd] = m_fpsid;
            break;

        default:
            armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
        }
        break;

    case OP_FMSRR:
        Fm = decodeFmSingle(workingInstruction);
        Rn = BITS(workingInstruction, 16, 19);
        setSingleRegister(Fm, m_alu->getRegisters()[Rn]);
        setSingleRegister(Fm + 1, m_alu->getRegisters()[Rd]);
        break;

    case OP_FMRRS:
        Fm = decodeFmSingle(workingInstruction);
        Rn = BITS(workingInstruction, 16, 19);
        m_alu->getRegisters()[Rn] = toSingle(Fm);
        m_alu->getRegisters()[Rd] = toSingle(Fm + 1);
        break;

    case OP_FMDRR:
        Fm = decodeFmDouble(workingInstruction);
        Rn = BITS(workingInstruction, 16, 19);
        setSingleRegister(Fm * 2, m_alu->getRegisters()[Rd]);
        setSingleRegister(Fm * 2 + 1, m_alu->getRegisters()[Rn]);
        break;

    case OP_FMRRD:
        Fm = decodeFmSingle(workingInstruction);
        Rn = BITS(workingInstruction, 16, 19);
        m_alu->getRegisters()[Rd] = toSingle(Fm);
        m_alu->getRegisters()[Rn] = toSingle(Fm + 1);
        break;

    default:
        // On leve une exception
        armv4vm_assert(__FUNCTION__, __FILE__, __LINE__);
    }
}

} // namespace armv4vm
