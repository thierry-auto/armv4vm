//    Copyright (c) 2020, thierry vic
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


#include "armv4vm.h"
#include "memoryhandler.h"


long long debugHook = 0;

#ifdef UNABLE_QT
#include <QDataStream>
#include <QFile>
#else
#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#endif

#ifndef UNABLE_QT
#define qt_assert(__FUNCTION__, __FILE__, __LINE__)                                                                    \
{                                                                                                                  \
    std::cerr << __FUNCTION__ << " " << __FILE__ << " " << __LINE__ << std::endl;                                  \
    assert(0);                                                                                                     \
    }
#endif

namespace armv4vm {

static inline uint32_t getSigned24(const uint32_t i) { return ((i & 0x00800000) ? i | 0xFF000000 : i & 0x00FFFFFF); }
static inline uint32_t getSigned16(const uint32_t i) { return ((i & 0x00008000) ? i | 0xFFFF0000 : i & 0x0000FFFF); }
static inline uint32_t getSigned8(const uint32_t i) { return ((i & 0x00000080) ? i | 0xFFFFFF00 : i & 0x000000FF); }

template <typename T> inline T cast(uint32_t instruction) { return *reinterpret_cast<T *>(&instruction); }

#ifdef UNABLE_QT
VirtualMachine<T>::VirtualMachine(struct VmProperties *vmProperties, QObject *parent) : QObject(parent) {

    m_ram          = nullptr;
    m_vmProperties = vmProperties;
}
#else
template <typename T> VirtualMachine<T>::VirtualMachine(struct VmProperties *vmProperties) {

    m_vmProperties = vmProperties;
}

template <> VirtualMachine<uint8_t *>::VirtualMachine(struct VmProperties *vmProperties) {

    m_ram          = nullptr;
    m_vmProperties = *vmProperties;
}

template <> VirtualMachine<MemoryProtected>::VirtualMachine(struct VmProperties *vmProperties) {

    m_vmProperties = *vmProperties;
}

template <> VirtualMachine<uint8_t *>::~VirtualMachine() { m_ram = nullptr; }
template <> VirtualMachine<MemoryProtected>::~VirtualMachine() {}
#endif

template <typename T> uint8_t *VirtualMachine<T>::init() { return nullptr; }

template <> uint8_t *VirtualMachine<uint8_t *>::init() {

    m_ram = new uint8_t[m_vmProperties.m_memsize];
    memset(m_ram, 0x00, m_vmProperties.m_memsize);
    memset(m_registers, 0, sizeof(m_registers));

    m_cpsr = 0;
    m_spsr = 0;

    return m_ram;
}

template <> uint8_t *VirtualMachine<MemoryProtected>::init() {

    // m_ram = new uint8_t[m_vmProperties.m_memsize];
    //  std::vector<uint8_t> m_ram;

    // m_ram.resize(m_vmProperties.m_memsize);
    memset(m_registers, 0, sizeof(m_registers));

    m_cpsr = 0;
    m_spsr = 0;

    m_ram.init(m_vmProperties.m_memsize, m_vmProperties.m_memModel.m_range);

    return m_ram.getMem();
}

#ifdef UNABLE_QT
uint64_t VirtualMachine<T>::load() {

    QFile    program(m_vmProperties.m_bin);
    uint64_t programSize = 0;

    if (program.open(QIODevice::ReadOnly)) {

        QDataStream ds(&program);
        programSize = ds.readRawData(reinterpret_cast<char *>(m_ram), program.size());
    } else {

        m_error = E_LOAD_FAILED;
    }

    return programSize;
}
#else
template <typename T> uint64_t VirtualMachine<T>::load() {

    std::fstream program;
    uint64_t     programSize = 0;

    program.open(m_vmProperties.m_bin, std::ios::in | std::ios::binary | std::ios::ate);
    if (program.is_open()) {

        programSize = program.tellg();
        program.seekg(0, std::ios::beg);

        program.read((char *)(uint8_t *)m_ram, programSize);
        program.close();
    } else {
        m_error     = E_LOAD_FAILED;
        programSize = -1;
    }

    return programSize;
}
#endif

static uint32_t stage1 = 0;

template <typename T> typename VirtualMachine<T>::Interrupt VirtualMachine<T>::run(const uint32_t nbMaxIteration) {

    VirtualMachine<T>::Interrupt result        = VirtualMachine<T>::Interrupt::Undefined;
    int                       setJumpResult = 0;

    m_running = true;

    // A remplacer par un try..catch..
    memset(m_runInterruptLongJump, 0, sizeof(m_runInterruptLongJump));
    setJumpResult = setjmp(m_runInterruptLongJump);

    switch (setJumpResult) {

    case 0:
        if (nbMaxIteration != 0) {

            for (uint32_t i = 0; i < nbMaxIteration; i++) {

                stage1 = fetch();
                decode(stage1);
                evaluate();
            }
        } else {

#ifdef DEBUG
            debugHook = 6;
#endif

            while (true) {

                stage1 = fetch();
                decode(stage1);
                evaluate();
            }
        }
        break;

    case 1:
        result = VirtualMachine<T>::Interrupt::Resume;
        break;

    case 2:
        result = VirtualMachine<T>::Interrupt::Stop;
        break;

    case 3:
        result = VirtualMachine<T>::Interrupt::Suspend;
        break;

    case 4:
        result = VirtualMachine<T>::Interrupt::LockPop;
        break;

    case 5:
        result = VirtualMachine<T>::Interrupt::UnlockPop;
        break;

    case 6:
        result = VirtualMachine<T>::Interrupt::LockPush;
        break;

    case 7:
        result = VirtualMachine<T>::Interrupt::UnlockPush;
        break;

    case 8:
        result = VirtualMachine<T>::Interrupt::Fatal;
        break;

    default:
        result = VirtualMachine<T>::Interrupt::Undefined;
        break;
    }

    return result;
}

template <typename T> uint32_t VirtualMachine<T>::fetch() {

    uint32_t result = readPointer<uint32_t>(m_ram + (*m_pc));
    *m_pc += 4;

    return result;
}

template <typename T> void VirtualMachine<T>::decode(const uint32_t instruction) {

    static const uint32_t DATA_PROCESSING                      = 0x00000000;
    static const uint32_t MULTIPLY                             = 0x00000090;
    static const uint32_t MULTIPLY_LONG                        = 0x00800090;
    static const uint32_t SINGLE_DATA_SWAP                     = 0x01000090;
    static const uint32_t BRANCH_AND_EXCHANGE                  = 0x012FFF10;
    static const uint32_t HALFWORD_DATA_TRANSFER_REGISTER_OFF  = 0x00000090;
    static const uint32_t HALFWORD_DATA_TRANSFER_IMMEDIATE_OFF = 0x00400090;
    static const uint32_t SINGLE_DATA_TRANSFER                 = 0x04000000;
    static const uint32_t UNDEFINED                            = 0x06000010;
    static const uint32_t BLOCK_DATA_TRANSFER                  = 0x08000000;
    static const uint32_t BRANCH                               = 0x0A000000;
    static const uint32_t COPROCESSOR_DATA_TRANSFER            = 0x0C000000;
    static const uint32_t COPROCESSOR_DATA_OPERATION           = 0x0E000000;
    static const uint32_t COPROCESSOR_REGISTER_TRANSFER        = 0x0E000010;
    static const uint32_t SOFTWARE_INTERRUPT                   = 0x0F000000;

    static const uint32_t MASK_DATA_PROCESSING                      = 0x0C000000;
    static const uint32_t MASK_MULTIPLY                             = 0x0FC000F0;
    static const uint32_t MASK_MULTIPLY_LONG                        = 0x0F8000F0;
    static const uint32_t MASK_SINGLE_DATA_SWAP                     = 0x0FB00FF0;
    static const uint32_t MASK_BRANCH_AND_EXCHANGE                  = 0x0FFFFFF0;
    static const uint32_t MASK_HALFWORD_DATA_TRANSFER_REGISTER_OFF  = 0x0E400F90;
    static const uint32_t MASK_HALFWORD_DATA_TRANSFER_IMMEDIATE_OFF = 0x0E400090;
    static const uint32_t MASK_SINGLE_DATA_TRANSFER                 = 0x0C000000;
    static const uint32_t MASK_UNDEFINED                            = 0x0E000010;
    static const uint32_t MASK_BLOCK_DATA_TRANSFER                  = 0x0E000000;
    static const uint32_t MASK_BRANCH                               = 0x0E000000;
    static const uint32_t MASK_COPROCESSOR_DATA_TRANSFER            = 0x0E000000;
    static const uint32_t MASK_COPROCESSOR_DATA_OPERATION           = 0x0F000010;
    static const uint32_t MASK_COPROCESSOR_REGISTER_TRANSFER        = 0x0F000010;
    static const uint32_t MASK_SOFTWARE_INTERRUPT                   = 0x0F000000;

    m_workingInstruction = instruction;

    if ((instruction & MASK_BRANCH_AND_EXCHANGE) == BRANCH_AND_EXCHANGE) {

        m_instructionSetFormat = branch_and_exchange;
    } else if ((instruction & MASK_SINGLE_DATA_SWAP) == SINGLE_DATA_SWAP) {

        m_instructionSetFormat = single_data_swap;
#ifdef DEBUG
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
#endif
    } else if ((instruction & MASK_MULTIPLY) == MULTIPLY) {

        m_instructionSetFormat = multiply;
    } else if ((instruction & MASK_HALFWORD_DATA_TRANSFER_REGISTER_OFF) == HALFWORD_DATA_TRANSFER_REGISTER_OFF) {

        m_instructionSetFormat = halfword_data_transfer_register_off;
    } else if ((instruction & MASK_MULTIPLY_LONG) == MULTIPLY_LONG) {

        m_instructionSetFormat = multiply_long;
    } else if ((instruction & MASK_HALFWORD_DATA_TRANSFER_IMMEDIATE_OFF) == HALFWORD_DATA_TRANSFER_IMMEDIATE_OFF) {

        m_instructionSetFormat = halfword_data_transfer_immediate_off;
    } else if ((instruction & MASK_COPROCESSOR_DATA_OPERATION) == COPROCESSOR_DATA_OPERATION) {

        m_instructionSetFormat = coprocessor_data_operation;
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    } else if ((instruction & MASK_COPROCESSOR_REGISTER_TRANSFER) == COPROCESSOR_REGISTER_TRANSFER) {

        m_instructionSetFormat = coprocessor_register_transfer;
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    } else if ((instruction & MASK_SOFTWARE_INTERRUPT) == SOFTWARE_INTERRUPT) {

        m_instructionSetFormat = software_interrupt;
    } else if ((instruction & MASK_UNDEFINED) == UNDEFINED) {

        m_instructionSetFormat = undefined;
    } else if ((instruction & MASK_BLOCK_DATA_TRANSFER) == BLOCK_DATA_TRANSFER) {

        m_instructionSetFormat = block_data_transfer;
    } else if ((instruction & MASK_BRANCH) == BRANCH) {

        m_instructionSetFormat = branch;
    } else if ((instruction & MASK_COPROCESSOR_DATA_TRANSFER) == COPROCESSOR_DATA_TRANSFER) {

        m_instructionSetFormat = coprocessor_data_transfer;
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    } else if ((instruction & MASK_DATA_PROCESSING) == DATA_PROCESSING) {

        m_instructionSetFormat = data_processing;
    } else if ((instruction & MASK_SINGLE_DATA_TRANSFER) == SINGLE_DATA_TRANSFER) {

        m_instructionSetFormat = single_data_transfer;
    } else {
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    }
}

template <typename T> void VirtualMachine<T>::evaluate() {

    switch (m_instructionSetFormat) {

    case data_processing:
        dataProcessingEval();
        break;

    case multiply:
        multiplyEval();
        break;

    case multiply_long:
        multiplyLongEval();
        break;

    case single_data_transfer:
        singleDataTranferEval();
        break;

    case branch_and_exchange:
        branchAndExchangeEval();
        break;

    case halfword_data_transfer_register_off:
        halfwordDataTransferRegisterOffEval();
        break;

    case halfword_data_transfer_immediate_off:
        halfwordDataTransferImmediateOffEval();
        break;

    case branch:
        branchEval();
        break;

    case block_data_transfer:
        blockDataTransferEval();
        break;

    case software_interrupt:
        softwareInterruptEval();
        break;

    default:
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
        break;
    }
}

#define POS(i) ((~(i)) >> 31)
#define NEG(i) ((i) >> 31)

inline bool isOverflowAdd(const uint32_t op1, const uint32_t op2, const uint32_t result) {

    return ((NEG(op1) && NEG(op2) && POS(result)) || (POS(op1) && POS(op2) && NEG(result)));
}

inline bool isOverflowSub(const uint32_t op1, const uint32_t op2, const uint32_t result) {

    return ((NEG(op1) && POS(op2) && POS(result)) || (POS(op1) && NEG(op2) && NEG(result)));
}

inline bool isCarryFromALUAdd(const uint32_t op1, const uint32_t op2, const uint32_t result) {

    return ((NEG(op1) && NEG(op2)) || (NEG(op1) && POS(result)) || (NEG(op2) && POS(result)));
}

inline bool isCarryFromALUSub(const uint32_t op1, const uint32_t op2, const uint32_t result) {

    return ((NEG(op1) && POS(op2)) || (NEG(op1) && POS(result)) || (POS(op2) && POS(result)));
}

template <typename T> void VirtualMachine<T>::dataProcessingEval() {

    enum OpCode {

        AND = 0x0,
        EOR = 0x1,
        SUB = 0x2,
        RSB = 0x3,
        ADD = 0x4,
        ADC = 0x5,
        SBC = 0x6,
        RSC = 0x7,
        TST = 0x8,
        TEQ = 0x9,
        CMP = 0xA,
        CMN = 0xB,
        ORR = 0xC,
        MOV = 0xD,
        BIC = 0xE,
        MVN = 0xF
    };

    // clang-format off
    static struct DataProcessing {

        uint32_t operand2  : 12;
        uint32_t rd        :  4;
        uint32_t rn        :  4;
        uint32_t s         :  1;
        uint32_t opcode    :  4;
        uint32_t immediate :  1;
        uint32_t           :  2;
        uint32_t condition :  4;

    } instruction;
    // clang-format on

    // DataProcessing instruction;
    static uint32_t operand1         = 0;
    static uint32_t operand2         = 0;
    static uint32_t carryFromShifter = 0;
    static bool     carryFromALU     = 0;
    static uint32_t notWrittenResult = 0;
    bool            overflow         = false;

    if (false == testCondition(m_workingInstruction))
        return;

    instruction      = cast<DataProcessing>(m_workingInstruction);
    carryFromALU     = 0;
    carryFromShifter = 0;
    overflow         = false;

    // § 4.5.5
    operand1 = m_registers[instruction.rn] + (instruction.rn != 15 ? 0 : 4);
    operand2 = instruction.immediate ? rotate(instruction.operand2, carryFromShifter) : shift(instruction.operand2, carryFromShifter);

    switch (instruction.opcode) {

    case AND:
        m_registers[instruction.rd] = operand1 & operand2;
        break;

    case EOR:
        m_registers[instruction.rd] = operand1 ^ operand2;
        break;

    case SUB:
        m_registers[instruction.rd] = operand1 - operand2;
        carryFromALU = isCarryFromALUSub(operand1, operand2, m_registers[instruction.rd]);
        overflow     = isOverflowSub(operand1, operand2, m_registers[instruction.rd]);
        break;

    case RSB:
        m_registers[instruction.rd] = operand2 - operand1;
        carryFromALU                = isCarryFromALUSub(operand2, operand1, m_registers[instruction.rd]);
        overflow                    = isOverflowSub(operand2, operand1, m_registers[instruction.rd]);
        break;

    case ADD:
        m_registers[instruction.rd] = operand1 + operand2;
        carryFromALU                = isCarryFromALUAdd(operand1, operand2, m_registers[instruction.rd]);
        overflow                    = isOverflowAdd(operand1, operand2, m_registers[instruction.rd]);
        break;

    case ADC:
        m_registers[instruction.rd] = operand1 + operand2 + ((m_cpsr >> 29) & 0x1);
        carryFromALU                = isCarryFromALUAdd(operand1, operand2, m_registers[instruction.rd]);
        overflow                    = isOverflowAdd(operand1, operand2, m_registers[instruction.rd]);
        break;

    case SBC:
        m_registers[instruction.rd] = operand1 - operand2 + ((m_cpsr >> 29) & 0x1) - 1;
        carryFromALU                = isCarryFromALUSub(operand1, operand2, m_registers[instruction.rd]);
        overflow                    = isOverflowSub(operand1, operand2, m_registers[instruction.rd]);
        break;

    case RSC:
        m_registers[instruction.rd] = operand2 - operand1 + ((m_cpsr >> 29) & 0x1) - 1;
        carryFromALU                = isCarryFromALUSub(operand2, operand1, m_registers[instruction.rd]);
        overflow                    = isOverflowSub(operand2, operand1, m_registers[instruction.rd]);
        break;

    case TST:
        notWrittenResult = operand1 & operand2;
#ifdef DEBUG
        if(instruction.s == 0)
            qt_assert(__FUNCTION__, __FILE__, __LINE__);
#endif
        break;

    case TEQ:
        notWrittenResult = operand1 ^ operand2;
#ifdef DEBUG
        // if(instruction.s == 0)
        //   qt_assert(__FUNCTION__, __FILE__, __LINE__);
#endif
        break;

    case CMP:
        notWrittenResult = operand1 - operand2;
        carryFromALU     = isCarryFromALUSub(operand1, operand2, notWrittenResult);
        overflow         = isOverflowSub(operand1, operand2, notWrittenResult);
#ifdef DEBUG
        if(instruction.s == 0)
            qt_assert(__FUNCTION__, __FILE__, __LINE__);
#endif
        break;

    case CMN:
        notWrittenResult = operand1 + operand2;
        carryFromALU     = isCarryFromALUAdd(operand1, operand2, notWrittenResult);
        overflow         = isOverflowAdd(operand1, operand2, notWrittenResult);
#ifdef DEBUG
        if(instruction.s == 0)
            qt_assert(__FUNCTION__, __FILE__, __LINE__);
#endif
        break;
    case ORR:
        m_registers[instruction.rd] = operand1 | operand2;
        break;

    case MOV:
        m_registers[instruction.rd] = operand2;
        break;

    case BIC:
        m_registers[instruction.rd] = m_registers[instruction.rn] & (~operand2);
        break;

    case MVN:
        m_registers[instruction.rd] = ~operand2;
        break;

    default:
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
        break;
    }

    // § 4.5.1 - Mise à jour CPSR NZCV.....

    if (instruction.s) {

#ifdef DEBUG
        if (instruction.rd == 15) {

            m_cpsr = m_spsr;
            qt_assert(__FUNCTION__, __FILE__, __LINE__);
        }
#endif
        switch (instruction.opcode) {

        // LOGICAL
        case AND:
        case EOR:
        case ORR:
        case MOV:
        case BIC:
        case MVN:
            m_registers[instruction.rd] & 0x80000000 ? setN() : unsetN();
            m_registers[instruction.rd] ? unsetZ() : setZ();
            carryFromShifter ? setC() : unsetC();
            break;

        case TST:
        case TEQ:
            notWrittenResult & 0x80000000 ? setN() : unsetN();
            notWrittenResult ? unsetZ() : setZ();
            carryFromShifter ? setC() : unsetC();
            break;

            // ARITHMETIC
        case RSC:
        case SUB:
        case RSB:
        case ADD:
        case ADC:
        case SBC:
            m_registers[instruction.rd] & 0x80000000 ? setN() : unsetN();
            m_registers[instruction.rd] ? unsetZ() : setZ();
            carryFromALU ? setC() : unsetC();
            overflow ? setV() : unsetV();
            break;

        case CMP:
        case CMN:
            notWrittenResult & 0x80000000 ? setN() : unsetN();
            notWrittenResult ? unsetZ() : setZ();
            carryFromALU ? setC() : unsetC();
            overflow ? setV() : unsetV();
            break;

        default:
            qt_assert(__FUNCTION__, __FILE__, __LINE__);
            break;
        }
    }
}

template <typename T> void VirtualMachine<T>::multiplyEval() {

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

    if (false == testCondition(m_workingInstruction))
        return;

    // 4.12
    instruction = cast<Multiply>(m_workingInstruction);

    if (instruction.a) {

        // Multiply accumulate
        m_registers[instruction.rd] =
                m_registers[instruction.rm] * m_registers[instruction.rs] + m_registers[instruction.rn];
    } else {

        // Multiply
        m_registers[instruction.rd] = m_registers[instruction.rm] * m_registers[instruction.rs];
    }

    if (instruction.s && instruction.rd != 15) {

        (m_registers[instruction.rd] == 0) ? setZ() : unsetZ();
        (m_registers[instruction.rd] & 0x80000000) ? setN() : unsetN();
    }
}

inline int64_t  signedCastTo64(const uint32_t value) { return static_cast<int64_t>(static_cast<int32_t>(value)); }
inline uint64_t unsignedCastTo64(const uint32_t value) { return static_cast<uint64_t>(value); }


template <typename T> void VirtualMachine<T>::multiplyLongEval() {

    // clang-format off
    static struct MultiplyLong {

        uint32_t rm        : 4;
        uint32_t           : 4;
        uint32_t rs        : 4;
        uint32_t rdlo      : 4;
        uint32_t rdhi      : 4;
        uint32_t s         : 1;
        uint32_t a         : 1;
        uint32_t u         : 1;
        uint32_t           : 5;
        uint32_t condition : 4;

    } instruction;
    // clang-format on

    int64_t  signedResult   = 0;
    uint64_t unsignedResult = 0;

    if (false == testCondition(m_workingInstruction))
        return;

    // § 4.12
    instruction = cast<MultiplyLong>(m_workingInstruction);

    // https:   // cpulator.01xz.net/?sys=arm

    if (instruction.u) { // signed

        if (instruction.a) {

            // Multiply accumulate RdHi,RdLo := Rm * Rs + RdHi,RdLo
            signedResult = m_registers[instruction.rdhi];
            signedResult = (signedResult << 32) | m_registers[instruction.rdlo];
            signedResult += signedCastTo64(m_registers[instruction.rm]) * signedCastTo64(m_registers[instruction.rs]);
        }

        signedResult += signedCastTo64(m_registers[instruction.rm]) * signedCastTo64(m_registers[instruction.rs]);
        m_registers[instruction.rdhi] = signedResult >> 32;
        m_registers[instruction.rdlo] = signedResult & 0xFFFFFFFF;

    } else { // unsigned
        if (instruction.a) {

            // Multiply accumulate RdHi,RdLo := Rm * Rs + RdHi,RdLo
            unsignedResult = m_registers[instruction.rdhi];
            unsignedResult = (unsignedResult << 32) | m_registers[instruction.rdlo];
        }

        unsignedResult += unsignedCastTo64(m_registers[instruction.rm]) * unsignedCastTo64(m_registers[instruction.rs]);
        m_registers[instruction.rdhi] = unsignedResult >> 32;
        m_registers[instruction.rdlo] = unsignedResult & 0xFFFFFFFF;
    }

    if (instruction.s && instruction.rdhi != 15 && instruction.rdlo != 15) {

        ((m_registers[instruction.rdlo] | m_registers[instruction.rdlo]) == 0) ? setZ() : unsetZ();
        (m_registers[instruction.rdhi] & 0x80000000) ? setN() : unsetN();
    }
}

template <typename T> void VirtualMachine<T>::singleDataTranferEval() {

    // clang-format off
    static struct SingleDataTranfer {

        uint32_t offset    : 12;
        uint32_t rd        :  4;
        uint32_t rn        :  4;
        uint32_t load      :  1;
        uint32_t w         :  1;
        uint32_t b         :  1;
        uint32_t u         :  1;
        uint32_t p         :  1;
        uint32_t immediate :  1;
        uint32_t           :  2;
        uint32_t condition :  4;

    } instruction;
    // clang-format on

    static uint32_t offset = 0;
    static uint32_t carry  = 0;
    static uint32_t value  = 0;
    static uint32_t rd     = 0;
    static uint32_t rn     = 0;

    if (false == testCondition(m_workingInstruction))
        return;

    // § 4.14
    instruction = cast<SingleDataTranfer>(m_workingInstruction);

    carry  = 0;
    offset = instruction.immediate ? shift(instruction.offset & 0xFEF, carry) : instruction.offset;

    // § 4.9.4
    rd     = instruction.rd != 15 ? m_registers[instruction.rd] : m_registers[instruction.rd] + 8; // et pas + 12
    rn     = instruction.rn != 15 ? m_registers[instruction.rn] : m_registers[instruction.rn] + 4; // et pas + 8

    if (instruction.load) {

        if (instruction.b) {

            if (instruction.p) {

                if (instruction.u) {

                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + rn + offset) & 0x000000FF;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn + offset;
                    }
                } else {
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + rn - offset) & 0x000000FF;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn - offset;
                    }
                }
            } else {
                m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + rn) & 0x000000FF;

                if (instruction.u) {

                    m_registers[instruction.rn] = rn + offset;
                } else {

                    m_registers[instruction.rn] = rn - offset;
                }
            }

        } else {
            if (instruction.p) {

                if (instruction.u) {

                    value = readPointer<uint32_t>(m_ram + rn + offset);

                    m_registers[instruction.rd] = value;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn + offset;
                    }
                } else {

                    value = readPointer<uint32_t>(m_ram + rn - offset);

                    m_registers[instruction.rd] = value;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn - offset;
                    }
                }
            } else {

                value = readPointer<uint32_t>(m_ram + rn);

                m_registers[instruction.rd] = value;
                if (instruction.u) {

                    m_registers[instruction.rn] = rn + offset;
                } else {

                    m_registers[instruction.rn] = rn - offset;
                }
            }
        }
    } else { // store

        if (instruction.b) {

            value = 0x000000FF & rd;

            if (instruction.p) {

                if (instruction.u) {

                    writePointer<uint8_t>(m_ram + rn + offset) = static_cast<uint8_t>(value);
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn + offset;
                    }
                } else {

                    writePointer<uint8_t>(m_ram + rn - offset) = static_cast<uint8_t>(value);
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn - offset;
                    }
                }
            } else {

                writePointer<uint8_t>(m_ram + rn) = static_cast<uint8_t>(value);

                if (instruction.u) {

                    m_registers[instruction.rn] = rn + offset;
                } else {

                    m_registers[instruction.rn] = rn - offset;
                }
            }
        } else {

            if (instruction.p) {

                if (instruction.u) {

                    writePointer<uint32_t>(m_ram + rn + offset) = rd;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn + offset;
                    }
                } else {

                    writePointer<uint32_t>(m_ram + rn - offset) = rd;
                    if (instruction.w) {

                        m_registers[instruction.rn] = rn - offset;
                    }
                }
            } else {

                writePointer<uint32_t>(m_ram + rn) = rd;

                if (instruction.u) {

                    m_registers[instruction.rn] = rn + offset;
                } else {

                    m_registers[instruction.rn] = rn - offset;
                }
            }
        }
    }

    // § 4.9.1
    // In the case of post-indexed addressing, the write back bit is
    // redundant and is always set to zero, since the old base value can be
    // retained by setting the offset to zero.
}

template <typename T> void VirtualMachine<T>::branchAndExchangeEval() {

    // clang-format off
    static struct BranchAndExchange {

        uint32_t rn        :  4;
        uint32_t           : 24;
        uint32_t condition :  4;

    } instruction;
    // clang-format on

    if (false == testCondition(m_workingInstruction))
        return;

    // 4.14
    instruction.condition = m_workingInstruction >> 28;
    instruction.rn        = m_workingInstruction >> 0;

    *m_pc = m_registers[instruction.rn];
}

template <typename T> void VirtualMachine<T>::branchEval() {

    // clang-format off
    static struct Branch {

        uint32_t offset    : 24; // Signed ! §4.4
        uint32_t l         :  1;
        uint32_t           :  3;
        uint32_t condition :  4;

    } instruction;
    // clang-format on

    if (false == testCondition(m_workingInstruction))
        return;

    instruction = cast<Branch>(m_workingInstruction);

    if (instruction.l) {

        // § 4.4.1
        *m_lr = *m_pc;
    }

    // Signed + Signed = Signed, Unsigned + Signed = Unsigned..
    (*reinterpret_cast<uint32_t *>(m_pc)) += getSigned24((instruction.offset) << 2) + 4; // et pas + 8
}

template <typename T> void VirtualMachine<T>::blockDataTransferEval() {

    static struct BlockDatatransfer {

        // clang-format off
        uint32_t registerList : 16;
        uint32_t rn           :  4;
        uint32_t l            :  1;
        uint32_t w            :  1;
        uint32_t s            :  1;
        uint32_t u            :  1;
        uint32_t p            :  1;
        uint32_t              :  3;
        uint32_t condition    :  4;
        // clang-format on

    } instruction;

    static uint32_t offset = 0;
    static int      i      = 0;

    if (false == testCondition(m_workingInstruction))
        return;

    instruction = cast<BlockDatatransfer>(m_workingInstruction);

    offset = m_registers[instruction.rn];

    if (instruction.l) {

        // Load registers..LDM

        if (instruction.u) {

            if (instruction.p) {

                // pre-increment load, LDMED, LDMIB
                for (i = 0; i < 16; i++) {

                    if (instruction.registerList & (1 << i)) {

                        offset += 4;
                        m_registers[i] = readPointer<uint32_t>(m_ram + offset);
                    }
                }
            } else {

                // post-increment load, LDMFD, LDMIA
                for (i = 0; i < 16; i++) {

                    if (instruction.registerList & (1 << i)) {

                        m_registers[i] = readPointer<uint32_t>(m_ram + offset);
                        offset += 4;
                    }
                }
            }
        } else {

            if (instruction.p) {

                // pre-decrement load, LDMEA, LDMBD
                for (i = 15; i >= 0; i--) {

                    if (instruction.registerList & (1 << i)) {

                        offset -= 4;
                        m_registers[i] = readPointer<uint32_t>(m_ram + offset);
                    }
                }
            } else {

                // post-decrement load, LDMFA, LDMDA
                for (i = 15; i >= 0; i--) {

                    if (instruction.registerList & (1 << i)) {

                        m_registers[i] = readPointer<uint32_t>(m_ram + offset);
                        offset -= 4;
                    }
                }
            }
        }

        // § 4.11.6
        if (instruction.w) {

            m_registers[instruction.rn] = offset;
        }
    } else {

        // Store registers..STM

        if (instruction.u) {

            if (instruction.p) {

                // pre-increment store, STMFA, STMIB
                for (i = 0; i < 15; i++) {

                    if (instruction.registerList & (1 << i)) {

                        offset += 4;
                        //* reinterpret_cast<uint32_t*>(m_ram + offset) =
                        // m_registers[i];
                        writePointer<uint32_t>(m_ram + offset) = m_registers[i];
                    }
                }

                // Cas particulier du registre 15 (PC)
                if (instruction.registerList & 0x8000) {

                    offset += 4;
                    writePointer<uint32_t>(m_ram + offset) = m_registers[15] + 4; // et pas + 12
                }
            } else {

                // post-increment store, STMEA, STMIA
                for (i = 0; i < 15; i++) {

                    if (instruction.registerList & (1 << i)) {

                        writePointer<uint32_t>(m_ram + offset) = m_registers[i];
                        offset += 4;
                    }
                }

                // Registre 15
                if (instruction.registerList & 0x8000) {

                    writePointer<uint32_t>(m_ram + offset) = m_registers[15] + 4;
                    offset += 4;
                }
            }
        } else {

            if (instruction.p) { // predecrement

                // pre-decrement store, STMFD, STMDB
                // Registre 15
                if (instruction.registerList & 0x8000) {

                    offset -= 4;
                    // readPointer<uint32_t>(m_ram + offset) = m_registers[15] + 4;
                    writePointer<uint32_t>(m_ram + offset) = m_registers[15] + 4;
                }

                // Registre 14, 13, 12, ...
                for (i = 14; i >= 0; i--) {

                    if (instruction.registerList & (1 << i)) {

                        offset -= 4;
                        writePointer<uint32_t>(m_ram + offset) = m_registers[i];
                    }
                }
            } else {
                // STMDA

                // Registre 15
                if (instruction.registerList & 0x8000) {

                    writePointer<uint32_t>(m_ram + offset) = m_registers[15] + 4;
                    offset -= 4;
                }

                // Registres 14, 13, ...
                for (i = 14; i >= 0; i--) {

                    if (instruction.registerList & (1 << i)) {

                        m_registers[i]                         = readPointer<uint32_t>(m_ram + offset);
                        writePointer<uint32_t>(m_ram + offset) = m_registers[i];
                        offset -= 4;
                    }
                }
            }
        }

        // 4.11.6
        if (instruction.w) {

            m_registers[instruction.rn] = offset;
        }
    }

    // Pas de gestion du bit S pour le moment. Il ne sert qu'en mode privilege.
    // § 4.11.5
    if (instruction.s) {

        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    }
}

template <typename T> void VirtualMachine<T>::halfwordDataTransferRegisterOffEval() {

    // clang-format off
    struct HalfWordDataTransferRegisterOffset {

        uint32_t rm        : 4;
        uint32_t           : 1;
        uint32_t h         : 1;
        uint32_t s         : 1;
        uint32_t           : 5;
        uint32_t rd        : 4;
        uint32_t rn        : 4;
        uint32_t l         : 1;
        uint32_t w         : 1;
        uint32_t           : 1;
        uint32_t u         : 1;
        uint32_t p         : 1;
        uint32_t           : 3;
        uint32_t condition : 4;

    } instruction;
    // clang-format on

    uint32_t offset;

    // qt_assert(__FUNCTION__, __FILE__, __LINE__);

    if (false == testCondition(m_workingInstruction))
        return;

    instruction = cast<HalfWordDataTransferRegisterOffset>(m_workingInstruction);

    offset = m_registers[instruction.rn];

    if (instruction.rn == 15) {

        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    }

    if (instruction.l) {

        if (instruction.p) {

            if (instruction.u)
                offset = offset + m_registers[instruction.rm];
            else
                offset = offset - m_registers[instruction.rm];

            if (instruction.s) {

                if (instruction.h)
                    m_registers[instruction.rd] = getSigned16(readPointer<uint32_t>(m_ram + offset));
                else
                    m_registers[instruction.rd] = getSigned8(readPointer<uint32_t>(m_ram + offset));
            } else {
                if (instruction.h)
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x0000FFFF;
                else
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x000000FF;
            }

            if (instruction.w) {

                m_registers[instruction.rn] = offset;
            }
        } else {

            if (instruction.s) {

                if (instruction.h)
                    m_registers[instruction.rd] = getSigned16(readPointer<uint32_t>(m_ram + offset));
                else
                    m_registers[instruction.rd] = getSigned8(readPointer<uint32_t>(m_ram + offset));
            } else {
                if (instruction.h)
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x0000FFFF;
                else
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x000000FF;
            }

            if (instruction.u)
                offset = offset + m_registers[instruction.rm];
            else
                offset = offset - m_registers[instruction.rm];

            if((offset % 4) == 0) {

                offset += 2;
            }

            m_registers[instruction.rn] = offset;
        }
    } else {

        uint32_t rd = instruction.rd != 15 ? m_registers[instruction.rd] : m_registers[instruction.rd] + 4;

        if (instruction.p) {

            if (instruction.u)
                offset = offset + m_registers[instruction.rm];
            else
                offset = offset - m_registers[instruction.rm];

            if((offset % 4) == 0) {

                offset += 2;
            }

            writePointer<uint32_t>(m_ram + offset) = (rd & 0x0000FFFF) | (rd << 16);

            if (instruction.w) {

                m_registers[instruction.rn] = offset;
            }
        } else {

            writePointer<uint32_t>(m_ram + offset) = (rd & 0x0000FFFF) | (rd << 16);

            if (instruction.u)
                offset = offset + m_registers[instruction.rm];
            else
                offset = offset - m_registers[instruction.rm];

            if((offset % 4) == 0) {

                offset += 2;
            }

            m_registers[instruction.rn] = offset;
        }
    }
}

template <typename T> void VirtualMachine<T>::halfwordDataTransferImmediateOffEval() {

    // clang-format off
    struct HalfWordDataTransferImmediateOffset {

        uint32_t offset1   : 4;
        uint32_t           : 1;
        uint32_t h         : 1;
        uint32_t s         : 1;
        uint32_t           : 1;
        uint32_t offset2   : 4;
        uint32_t rd        : 4;
        uint32_t rn        : 4;
        uint32_t l         : 1;
        uint32_t w         : 1;
        uint32_t           : 1;
        uint32_t u         : 1;
        uint32_t p         : 1;
        uint32_t           : 3;
        uint32_t condition : 4;

    } instruction;
    // clang-format on

    static uint32_t offset;

    if (false == testCondition(m_workingInstruction))
        return;

    instruction = cast<HalfWordDataTransferImmediateOffset>(m_workingInstruction);

    if (instruction.rn == 15) {

        qt_assert(__FUNCTION__, __FILE__, __LINE__);
    }

    offset = m_registers[instruction.rn];

    if (instruction.l) {

        if (instruction.p) {

            if (instruction.u)
                offset = offset + ((instruction.offset2 << 4) | instruction.offset1);
            else
                offset = offset - ((instruction.offset2 << 4) | instruction.offset1);

            if (instruction.s) {

                if (instruction.h)
                    m_registers[instruction.rd] = getSigned16(readPointer<uint32_t>(m_ram + offset));
                else
                    m_registers[instruction.rd] = getSigned8(readPointer<uint32_t>(m_ram + offset));
            } else {
                if (instruction.h)
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x0000FFFF;
                else
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x000000FF;
            }

            if (instruction.w) {

                m_registers[instruction.rn] = offset;
            }
        } else {

            if (instruction.s) {

                if (instruction.h)
                    m_registers[instruction.rd] = getSigned16(readPointer<uint32_t>(m_ram + offset));
                else
                    m_registers[instruction.rd] = getSigned8(readPointer<uint32_t>(m_ram + offset));
            } else {
                if (instruction.h)
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x0000FFFF;
                else
                    m_registers[instruction.rd] = readPointer<uint32_t>(m_ram + offset) & 0x000000FF;
            }

            if (instruction.u)
                offset = offset + ((instruction.offset2 << 4) | instruction.offset1);
            else
                offset = offset - ((instruction.offset2 << 4) | instruction.offset1);

            m_registers[instruction.rn] = offset;
        }
    } else {

        uint32_t rd = instruction.rd != 15 ? m_registers[instruction.rd] : m_registers[instruction.rd] + 4;

        if (instruction.p) {

            if (instruction.u)
                offset = offset + ((instruction.offset2 << 4) | instruction.offset1);
            else
                offset = offset - ((instruction.offset2 << 4) | instruction.offset1);

            if((offset % 4) == 0)
                writePointer<uint32_t>(m_ram + offset) =
                    (readPointer<uint32_t>(m_ram + offset) & 0xFFFF0000) | (rd & 0x0000FFFF);
            else
                writePointer<uint32_t>(m_ram + offset - 2) =
                    (readPointer<uint32_t>(m_ram + offset - 2) & 0x0000FFFF) | (rd << 16);

            if (instruction.w) {

                if((offset % 4) == 0) {

                    offset -= 2;
                }
                m_registers[instruction.rn] = offset;
            }
        } else {

            if((offset % 4) == 0)
                writePointer<uint32_t>(m_ram + offset) =
                    (readPointer<uint32_t>(m_ram + offset) & 0xFFFF0000) | (rd & 0x0000FFFF);
            else
                writePointer<uint32_t>(m_ram + offset - 2) =
                    (readPointer<uint32_t>(m_ram + offset) & 0x0000FFFF) | (rd << 16);

            if (instruction.u)
                offset = offset + ((instruction.offset2 << 4) | instruction.offset1);
            else
                offset = offset - ((instruction.offset2 << 4) | instruction.offset1);

            if((offset % 4) == 0) {

                offset -= 2;
            }

            m_registers[instruction.rn] = offset;
        }
    }
}

template <typename T> void VirtualMachine<T>::softwareInterruptEval() {

    // clang-format off
    struct SoftwareInterrupt {

        uint32_t comment   : 24;
        uint32_t           :  4;
        uint32_t condition :  4;

    } instruction;
    // clang-format on

    if (false == testCondition(m_workingInstruction))
        return;

    instruction = cast<SoftwareInterrupt>(m_workingInstruction);
    longjmp(m_runInterruptLongJump, instruction.comment); // A remplacer par un thrown
}

template <typename T> bool VirtualMachine<T>::testCondition(const uint32_t instruction) const {

    // N Z C V . . . . . .
    enum ConditionCode {

        EQ = 0x0,
        NE = 0x1,
        CS = 0x2,
        CC = 0x3,
        MI = 0x4,
        PL = 0x5,
        VS = 0x6,
        VC = 0x7,
        HI = 0x8,
        LS = 0x9,
        GE = 0xA,
        LT = 0xB,
        GT = 0xC,
        LE = 0xD,
        AL = 0xE
    };

    switch (instruction >> 28) {

    case AL:
        return true;

    case EQ: // Z==1
        return ((m_cpsr >> 28) & 0x4) == 0x4;

    case NE: // Z==0
        return ((m_cpsr >> 28) & 0x4) == 0x0;

    case CS: // C==1
        return ((m_cpsr >> 28) & 0x2) == 0x2;

    case CC: // C==0
        return ((m_cpsr >> 28) & 0x2) == 0x0;

    case MI: // N==1
        return ((m_cpsr >> 28) & 0x8) == 0x8;

    case PL: // N==0
        return ((m_cpsr >> 28) & 0x8) == 0x0;

    case VS: // V==1
        return ((m_cpsr >> 28) & 0x1) == 0x1;

    case VC: // V==0
        return ((m_cpsr >> 28) & 0x1) == 0x0;

    case HI:                                  // (Z==0) && (C==1)
        return ((m_cpsr >> 28) & 0x6) == 0x2; // relu

    case LS: // (Z==1) || (C==0)
        return (((m_cpsr >> 28) & 0x4) == 0x4) || (((m_cpsr >> 28) & 0x2) == 0x0);

    case GE: // N==V
        return ((m_cpsr >> 31) & 0x1) == ((m_cpsr >> 28) & 0x1);

    case LT: // N!=V
        return ((m_cpsr >> 31) & 0x1) != ((m_cpsr >> 28) & 0x1);

    case GT: // (Z==0) && (N==V)
        return (((m_cpsr >> 28) & 0x4) == 0x0) && (((m_cpsr >> 31) & 0x1) == ((m_cpsr >> 28) & 0x1));

    case LE: // (Z==1) || (N!=V)
        return (((m_cpsr >> 28) & 0x4) == 0x4) || (((m_cpsr >> 31) & 0x1) != ((m_cpsr >> 28) & 0x1));

    default: // What the hell is going on ?
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
        return false;
    }
}

template <typename T> uint32_t VirtualMachine<T>::rotate(const uint32_t operand2, uint32_t &carry) const {

    // § 4.5.3
    // On shift de 7 et pas de 8 pour multiplier par 2 la valeur de rotation.

    uint32_t result = (operand2 & 0xFF) << (32 - ((operand2 & 0xF00) >> 7)) | (operand2 & 0xFF) >> ((operand2 & 0xF00) >> 7);
    carry = operand2 & 0xF00 ? result & 0x80000000 : m_cpsr & 0x20000000;

    return result;
}

template <typename T> uint32_t VirtualMachine<T>::shift(const uint32_t operand2, uint32_t &carry) const {

    uint32_t              shiftResult     = 0;
    uint32_t              shiftValue      = 0;
    uint32_t              value           = 0;
    static const uint32_t BOTTOM_BYTE     = 0x000000FF;
    static const uint16_t SHIFT_TYPE_MASK = 0x060;

    enum ShiftType {

        LOGICAL_LEFT     = 0x00,
        LOGICAL_RIGHT    = 0x20,
        ARITHMETIC_RIGHT = 0x40,
        ROTATE_RIGHT     = 0x60

    };

    // § 4.5.2
    if (operand2 & 0x010) {

        // Shift by register
        // On increment ensuite cette valeur de 8 si le registre est le PC (15).
        value = ((operand2 & 0xF) != 15) ? m_registers[operand2 & 0xF] : m_registers[operand2 & 0xF] + 8; // et pas + 12
        shiftValue = m_registers[operand2 >> 8] & BOTTOM_BYTE;
    } else {

        // Shift by value
        // On increment ensuite cette valeur de 4 si le registre est le PC.
        value = ((operand2 & 0xF) != 15) ? m_registers[operand2 & 0xF] : m_registers[operand2 & 0xF] + 4; // et pas + 8
        shiftValue = (operand2) >> 7;                                                                     // relu
    }

    switch (operand2 & SHIFT_TYPE_MASK) {

    case LOGICAL_LEFT:
        if (shiftValue == 32) {

            shiftResult = 0;
            carry       = value & 1;
        } else if (shiftValue > 32) {

            shiftResult = 0;
            carry       = 0;
        } else if (shiftValue == 0) {

            shiftResult = value;
            carry       = m_cpsr & 0x20000000;
        } else {

            shiftResult = value << shiftValue;
            carry       = (value >> (32 - shiftValue)) & 0x1;
        }
        break;

    case LOGICAL_RIGHT:
        if (shiftValue == 32) {

            shiftResult = 0;
            carry       = value & 0x80000000;
        } else if (shiftValue > 32) {

            shiftResult = 0;
            carry       = 0;
        } else if (shiftValue == 0) {

            shiftResult = value;
            carry       = 0;
        } else {

            shiftResult = value >> shiftValue;
            carry       = (value >> (shiftValue - 1)) & 0x1;
        }
        break;

    case ARITHMETIC_RIGHT:
        if (shiftValue >= 32) {

            if (value & 0x80000000) {

                shiftResult = 0xFFFFFFFF;
                carry       = 1;
            } else {

                shiftResult = 0;
                carry       = 0;
            }
        } else if (shiftValue == 0) {

            shiftResult = value;
            carry       = 0;
        } else {

            if (value & 0x80000000) {

                shiftResult = (value >> shiftValue) | (0xFFFFFFFF << (32 - shiftValue));
                carry       = (value >> (shiftValue - 1)) & 0x1;
            } else {

                shiftResult = value >> shiftValue;
                carry       = (value >> (shiftValue - 1)) & 0x1;
            }
        }
        break;

    case ROTATE_RIGHT:
        if (shiftValue == 32 || (shiftValue == 0 && operand2 & 0x010)) {

            shiftResult = value;
            carry       = value & 0x80000000;
        } else if (shiftValue > 0) {

            shiftValue  = shiftValue & 0x1F;
            shiftResult = (value << (32 - shiftValue)) | (value >> shiftValue);
            carry       = (value >> (shiftValue - 1)) & 0x1;
        } else {

            // Rotate Right Extended
            shiftResult = ((m_cpsr & 0x20000000) << 2) | (value >> 1);
            carry       = value & 0x1;
        }
        break;

    default:
        qt_assert(__FUNCTION__, __FILE__, __LINE__);
        break;
    }

    return shiftResult;
}

template <typename T> const uint32_t *VirtualMachine<T>::getRegisters() const { return m_registers; }
template <typename T> uint32_t        VirtualMachine<T>::getCPSR() const { return m_cpsr; }

} // namespace armv4vm
