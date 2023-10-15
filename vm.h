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

#pragma once

#include "memoryhandler.h"

#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace armv4vm {

class MemoryProtected;

struct VmProperties {

    VmProperties() {
        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
    ~VmProperties() {}

    VmProperties(const VmProperties &other) {

        m_bin = other.m_bin;
        m_memsize = other.m_memsize;
        m_debug = other.m_debug;
    }

#ifdef UNABLE_QT
    QString m_bin;
#else
    std::string m_bin;
#endif

    int  m_memsize;
    bool m_debug;

    void clear() {

        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
};

template <typename T>
class VirtualMachine

#ifdef UNABLE_QT
    : public QObject
#endif
{

  public:
#ifdef UNABLE_QT
    explicit VirtualMachine(struct VmProperties *, QObject *parent = nullptr);
#else
    explicit VirtualMachine(struct VmProperties *);
#endif
    ~VirtualMachine();

    enum class Interrupt : int32_t {

        Resume     = 1,
        Stop       = 2,
        Suspend    = 3,
        LockPop    = 4,
        UnlockPop  = 5,
        LockPush   = 6,
        UnlockPush = 7,
        Fatal      = 8,
        Undefined  = 4,
    };

    T               init();
    void            test();
    void            loadTest();
    uint64_t        load();
    const uint32_t *getRegisters() const;

    uint32_t        getCPSR() const;

#ifdef UNABLE_QT
    friend QTextStream &operator<<(QTextStream &, const VirtualMachine &);
#endif
    friend class TestVm;

    /*public slots:*/
    Interrupt run(const uint32_t nbMaxIteration = 0);

public:
    enum Error {

        E_LOAD_FAILED,
        E_UNDEFINED
    };

#ifdef UNABLE_QT
  signals:
    void started();
    void finished();
    void suspended();
    void resumed();
    void crashed();
#endif

  private:
    struct VmProperties *m_vmProperties;
    T                    m_ram;
    enum Error           m_error;

    inline uint32_t fetch();
    inline void     decode(const uint32_t);
    inline void     evaluate();

    void execute();
    void print();

    void dataProcessingEval();
    void multiplyEval();
    void multiplyLongEval();
    void singleDataTranferEval();
    void branchAndExchangeEval();
    void branchEval();
    void blockDataTransferEval();
    void halfwordDataTransferRegisterOffEval();
    void halfwordDataTransferImmediateOffEval();
    void softwareInterruptEval();

    inline uint32_t rotate(const uint32_t operand2, uint32_t &carry) const;
    inline uint32_t shift(const uint32_t operand2, uint32_t &carry) const;

    bool testCondition(const uint32_t instruction) const;

    void setN() { m_cpsr |= 0x80000000; }
    void unsetN() { m_cpsr &= 0x7FFFFFFF; }
    void setZ() { m_cpsr |= 0x40000000; }
    void unsetZ() { m_cpsr &= 0xBFFFFFFF; }
    void setC() { m_cpsr |= 0x20000000; }
    void unsetC() { m_cpsr &= 0xDFFFFFFF; }
    void setV() { m_cpsr |= 0x10000000; }
    void unsetV() { m_cpsr &= 0xEFFFFFFF; }

    struct Undefined {

        uint32_t : 28;
        uint32_t cond : 4;
    };

    enum FormatSummary {

        data_processing,
        multiply,
        multiply_long,
        single_data_swap,
        branch_and_exchange,
        halfword_data_transfer_register_off,
        halfword_data_transfer_immediate_off,
        single_data_transfer,
        undefined,
        block_data_transfer,
        branch,
        coprocessor_data_transfer,
        coprocessor_data_operation,
        coprocessor_register_transfer,
        software_interrupt,
    };

    FormatSummary m_instructionSetFormat;

    uint32_t m_registers[16];
    uint32_t m_cpsr;
    uint32_t m_spsr;

    uint32_t *const m_sp = &m_registers[13];
    uint32_t *const m_lr = &m_registers[14];
    uint32_t *const m_pc = &m_registers[15];

    uint32_t m_workingInstruction;
    bool     m_running;
    jmp_buf  m_runInterruptLongJump;
#if 0
    static const uint32_t NEGATE_FLAG     = 0x80000000;
    static const uint32_t ZERO_FLAG       = 0x40000000;
    static const uint32_t CARRIED_FLAG    = 0x20000000;
    static const uint32_t OVERFLOWED_FLAG = 0x10000000;

    static const uint32_t IRQ_FLAG   = 0x00000080;
    static const uint32_t FIQ_FLAG   = 0x00000040;
    static const uint32_t THUMB_FLAG = 0x00000020;

    static const uint32_t MODE_FLAG = 0x0000001F;
#endif
};

// explicit template
template class VirtualMachine<uint8_t *>;
// template class VirtualMachine<MemoryProtected>;

using VirtualMachineUnprotected = VirtualMachine<uint8_t *>;
// using VirtualMachineProtected   = VirtualMachine<MemoryProtected>;

} // namespace armv4vm
