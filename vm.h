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

#include <array>

#ifdef QT_CORE_LIB
#include <QObject>
#include <QTextStream>
#else
#include <cstdint>
#include <string>
#endif

#if WIN32
#pragma warning(push)
#pragma warning(disable : 4820 4514 4626 4324) 
#endif

namespace armv4vm {

struct VmProperties {

    VmProperties() {
        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
    ~VmProperties() { }

    VmProperties(const VmProperties &other) {

        m_bin = other.m_bin;
        m_memsize = other.m_memsize;
        m_debug = other.m_debug;
    }

#ifdef QT_CORE_LIB
    QString m_bin;
#else
    std::string m_bin;
#endif
    uint32_t m_memsize;
    bool m_debug;

    void clear() {

        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
};

class alignas(32) VirtualMachine

#ifdef QT_CORE_LIB
    : public QObject
#endif
{

  public:

#ifdef QT_CORE_LIB
    explicit VirtualMachine(struct VmProperties *, QObject *parent = nullptr);
#else
    explicit VirtualMachine(struct VmProperties *);
#endif
    ~VirtualMachine();

    enum Interrupt : int {

        Resume    = 1,
        Stop      = 2,
        Suspend   = 3,
        Undefined = 4,
    };

    uint8_t *       init();
    void            test();
    void            loadTest();
    uint64_t        load();
    const std::array<uint32_t, 16> & getRegisters() const;
    Interrupt       run(const uint32_t nbMaxIteration = 0);
    uint32_t        getCPSR() const;

#ifdef QT_CORE_LIB
    friend QTextStream &operator<<(QTextStream &, const VirtualMachine &);
    friend class TestVm;
#endif

    enum Error {

        E_NO_ERROR,
        E_LOAD_FAILED,
        E_UNDEFINED,
    };

#ifdef QT_CORE_LIB
  signals:
    void started();
    void finished();
    void suspended();
    void resumed();
    void crashed();
#endif

  private:
    struct VmProperties *m_vmProperties;
    uint8_t *            m_ram;
    enum Error           m_error;

    inline uint32_t fetch();
    inline void     decode(const uint32_t);
    inline void     evaluate();

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

    //uint32_t m_registers[16];
    std::array<uint32_t, 16> m_registers;

    uint32_t m_cpsr;
    uint32_t m_spsr;

    uint32_t *const m_sp = &m_registers[13];
    uint32_t *const m_lr = &m_registers[14];
    uint32_t *const m_pc = &m_registers[15];

    uint32_t m_workingInstruction;
    bool     m_running;
    //jmp_buf  m_runInterruptLongJump;

    static const uint32_t NEGATE_FLAG     = 0x80000000;
    static const uint32_t ZERO_FLAG       = 0x40000000;
    static const uint32_t CARRIED_FLAG    = 0x20000000;
    static const uint32_t OVERFLOWED_FLAG = 0x10000000;

    static const uint32_t IRQ_FLAG   = 0x00000080;
    static const uint32_t FIQ_FLAG   = 0x00000040;
    static const uint32_t THUMB_FLAG = 0x00000020;

    static const uint32_t MODE_FLAG = 0x0000001F;
};

class VmException : public std::exception {
  public:
    VmException(VirtualMachine::Interrupt interrupt = VirtualMachine::Interrupt::Resume) : m_interrupt(interrupt) {}
    VirtualMachine::Interrupt m_interrupt;
};

} // namespace armv4vm

#if WIN32
#pragma warning(pop)
#endif