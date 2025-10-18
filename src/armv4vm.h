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


#ifdef QT_CORE_LIB
#include <QObject>
#include <QTextStream>
#endif

#if WIN32
#pragma warning(push)
#pragma warning(disable : 4820 4514 4626 4324) 
#endif

#include "memoryhandler.h"

#include <array>
#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <any>

namespace armv4vm {


// --- interface RAM minimaliste ---
struct InterfaceMemory {
    virtual ~InterfaceMemory() = default;
    // virtual uint8_t& at(std::size_t i) = 0;
    // virtual const uint8_t* data() const = 0;
    // virtual std::size_t size() const = 0;
    virtual operator uint8_t *() = 0;
};

// --- Adapter pour uint8_t* (non propriétaire) ---
class RawRamAdapter : public InterfaceMemory {
  public:
    RawRamAdapter(uint8_t* p, std::size_t n) : ptr(p), len(n) {}
    // uint8_t& at(std::size_t i) override { return ptr[i]; }
    // const uint8_t* data() const override { return ptr; }
    // std::size_t size() const override { return len; }
    operator uint8_t *() override { return ptr; }

  private:
    uint8_t* ptr;
    std::size_t len;
};

// --- Adapter pour MemoryProtected (propriétaire) ---
class ProtectedRamAdapter : public InterfaceMemory {
  public:
    ProtectedRamAdapter(MemoryProtected &m) : mp(&m) {}
    // uint8_t& at(std::size_t i) override { return mp->m_mem.at(i); }
    // const uint8_t* data() const override { return mp->m_mem.data(); }
    // std::size_t size() const override { return mp->m_mem.size(); }
     operator uint8_t *() override { return *mp; }
  private:
    MemoryProtected* mp;
};

class MemoryProtected;

struct VmProperties {

    struct MemModel {

        enum Type {
            DIRECT,
            PROTECTED,
        };

        MemModel(Type type = DIRECT) : m_type(type) { m_range.clear(); }

        Type                        m_type;
        std::vector<armv4vm::Range> m_range;
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

//class alignas(32) VirtualMachineBase;

class CoprocessorBase;

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

    virtual uint8_t* getRam() = 0;

  protected:

    std::array<uint32_t, 16> m_registers;
    //std::unique_ptr<InterfaceMemory> m_ramView;
    std::unique_ptr<CoprocessorBase> m_coprocessor;

  public:
    friend CoprocessorBase;
};


template <typename T>
class VirtualMachine : public VirtualMachineBase

#ifdef BUILD_WITH_QT
 , public QObject
#endif
{

  public:
#ifdef BUILD_WITH_QT
    explicit VirtualMachine(struct VmProperties *, QObject *parent = nullptr);
#else
    explicit VirtualMachine(struct VmProperties * = nullptr);
#endif
    ~VirtualMachine();


    uint8_t *       init() override;
    uint64_t        load() override;
    Interrupt       run(const uint32_t nbMaxIteration = 0) override;

    uint32_t        getCPSR() const;

#ifdef BUILD_WITH_QT
    friend QTextStream &operator<<(QTextStream &, const VirtualMachine &);
#endif
    friend class TestVm;

    /*public slots:*/


public:
    enum Error {

        E_NONE,
        E_LOAD_FAILED,
        E_UNDEFINED,
    };

#ifdef BUILD_WITH_QT    
  signals:
    void started();
    void finished();
    void suspended();
    void resumed();
    void crashed();
#endif

  protected:
    T m_ram;

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

    // todo use reference instead of ptr
    uint32_t *const m_sp = &m_registers[13];
    uint32_t *const m_lr = &m_registers[14];
    uint32_t *const m_pc = &m_registers[15];

    uint32_t m_workingInstruction;
    bool     m_running;


  public:
    uint8_t* getRam() { return m_ram; }

};

using VirtualMachineUnprotected = VirtualMachine<uint8_t *>;
using VirtualMachineProtected   = VirtualMachine<MemoryProtected>;


class CoprocessorBase {
  public:
    //CoprocessorBase(VirtualMachineUnprotected* vm) : m_vm(vm) {}
    CoprocessorBase(VirtualMachineBase* vm) : m_vm(vm) { m_ram = vm->getRam(); }

    virtual void coprocessorDataTransfers(const uint32_t m_workingInstruction);
    virtual void coprocessorDataOperations(const uint32_t m_workingInstruction);
    virtual void coprocessorRegisterTransfers(const uint32_t m_workingInstruction);

    using Factory = std::function<std::unique_ptr<CoprocessorBase>(VirtualMachineBase* vm)>;

    static void registerType(const std::string& name, Factory factory);
    static std::unique_ptr<CoprocessorBase> create(const std::string& name, VirtualMachineBase *vm);

  private:
    static std::unordered_map<std::string, Factory>& registry();

  protected:
    //std::variant<VirtualMachineUnprotected*, VirtualMachineProtected*> m_vm;
    VirtualMachineBase * m_vm;
    uint8_t* m_ram;
};

class VmException : public std::exception {
  public:
    using Interrupt = VirtualMachineBase::Interrupt;

    VmException(Interrupt interrupt = Interrupt::Resume)
        : m_interrupt(interrupt) {}
    Interrupt m_interrupt;
};


template <typename T> inline T cast(uint32_t instruction) { return *reinterpret_cast<T *>(&instruction); }


} // namespace armv4vm

#if WIN32
#pragma warning(pop)
#endif
