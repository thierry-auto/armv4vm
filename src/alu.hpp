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

#if WIN32
#pragma warning(push)
#pragma warning(disable : 4820 4514 4626 4324) 
#endif

#include "armv4vm_p.hpp"
#include "memoryhandler.hpp"
#include "coprocessor.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>


namespace armv4vm {

class MemoryProtected;

struct VmProperties {

    struct MemModel {

        enum Type {
            DIRECT,
            PROTECTED,
        };

        MemModel(Type type = DIRECT) : m_type(type) { m_accessPermission.clear(); }

        Type                        m_type;
        std::vector<armv4vm::AccessPermission> m_accessPermission;
    };

    VmProperties() {
        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
        m_memModel = MemModel::DIRECT;
        m_coproModel = "assert";
    }
    ~VmProperties() { }

    VmProperties(const VmProperties &other) {

        m_bin = other.m_bin;
        m_memsize = other.m_memsize;
        m_debug = other.m_debug;
        m_memModel = other.m_memModel;
        m_coproModel = other.m_coproModel;
    }

    VmProperties operator=(const VmProperties &other) {

        m_bin      = other.m_bin;
        m_memsize  = other.m_memsize;
        m_debug    = other.m_debug;
        m_memModel = other.m_memModel;
        m_coproModel = other.m_coproModel;

        return *this;
    }

    std::string m_bin;
    uint32_t m_memsize;
    bool m_debug;
    MemModel m_memModel;
    std::string m_coproModel;

    void clear() {

        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
};

class alignas(32) VirtualMachineBase {
  public:
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

    virtual uint8_t  *init() = 0;
    virtual uint64_t  load() = 0;
    virtual Interrupt run(const uint32_t nbMaxIteration = 0) = 0;
    const std::array<uint32_t, 16> & getRegisters() const { return m_registers; }

  protected:
    std::array<uint32_t, 16> m_registers;
};

template <typename T, template <typename> class MemoryInterface>
concept MemDerived = std::derived_from<T, MemoryInterface<T>>;

template <typename T, template <typename> class CoprocessorInterface>
concept CoproDerived = std::derived_from<T, CoprocessorInterface<T>>;


template <typename MemoryHandler, typename CoproHandler>
class VirtualMachine : public VirtualMachineBase {

  public:
    VirtualMachine(struct VmProperties * vmProperties = nullptr) :
        m_sp(m_registers[13]),
        m_lr(m_registers[14]),
        m_pc(m_registers[15])
    {
        m_vmProperties = *vmProperties;

        m_error = E_NONE;
        m_instructionSetFormat = unknown;
        m_registers.fill(0);
        m_spsr = 0;
        m_sp = m_registers[13];
        m_lr = m_registers[14];
        m_pc = m_registers[15];

        m_coprocessor.init(this);
    }
    ~VirtualMachine();

    uint8_t *       init() override;
    uint64_t        load() override;
    Interrupt       run(const uint32_t nbMaxIteration = 0) override;
    uint32_t        getCPSR() const;

    friend class TestVm;

public:
    enum Error {

        E_NONE,
        E_LOAD_FAILED,
        E_UNDEFINED,
    };

  protected:
    MemoryHandler m_ram;
    CoproHandler m_coprocessor;

  private:
    struct VmProperties  m_vmProperties;

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
    void singleDataSwapEval();
    void halfwordDataTransferRegisterOffEval();
    void halfwordDataTransferImmediateOffEval();
    void softwareInterruptEval();
    void coprocessorDataOperations();
    void coprocessorDataTransfers();
    void coprocessorRegisterTransfers();

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
        unknown,
    };

    FormatSummary m_instructionSetFormat;
#if 0
    uint32_t m_registers[16];
#endif


    uint32_t m_cpsr;
    uint32_t m_spsr;
    uint32_t & m_sp;
    uint32_t & m_lr;
    uint32_t & m_pc;
    uint32_t m_workingInstruction;
    bool     m_running;
};

class VmException : public std::exception {
  public:
    using Interrupt = VirtualMachineBase::Interrupt;

    VmException(Interrupt interrupt = Interrupt::Resume)
        : m_interrupt(interrupt) {}
    Interrupt m_interrupt;
};

} // namespace armv4vm


#if WIN32
#pragma warning(pop)
#endif

#include "alu.tpp"
