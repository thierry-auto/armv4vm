#pragma once

#include <memory>
#include <optional>
#include <fstream>

#include "armv4vm_p.hpp"
#include "vmproperties.hpp"
#include "coprocessor.hpp"
#include "alu.hpp"

namespace armv4vm {

class TestVm;

template <typename MemoryHandler, typename CoproHandler>
class Vm {
  private:
    using PrivateAlu = Alu<MemoryHandler, CoproHandler>;
    using PrivateVfpv2 = Vfpv2<MemoryHandler>;

  public:
    friend TestVm;

    enum Error {

        E_NONE,
        E_LOAD_FAILED,
        E_UNDEFINED,
    };

    void init(struct VmProperties &vmProperties) {

        m_vmProperties = vmProperties;
        m_mem = std::make_unique<MemoryHandler>();
        m_alu = std::make_unique<PrivateAlu>(&vmProperties);
        m_vfp = std::make_unique<PrivateVfpv2>(&vmProperties);

        m_alu->attach(m_mem.get(), m_vfp.get());
        m_vfp->attach(m_mem.get(), m_alu.get());
        m_alu->reset();
    }

    void reset() {

        m_mem = std::make_unique<MemoryHandler>();
        m_alu = std::make_unique<PrivateAlu>(&m_vmProperties);
        m_vfp = std::make_unique<PrivateVfpv2>(&m_vmProperties);

        m_alu->attach(m_mem.get(), m_vfp.get());
        m_vfp->attach(m_mem.get(), m_alu.get());
        m_alu->reset();
    }

    uint64_t load() {

        std::fstream program;
        std::streampos programSize = 0;

        program.open(m_vmProperties.m_bin, std::ios::in | std::ios::binary | std::ios::ate);
        if (program.is_open()) {

            programSize = program.tellg();
            program.seekg(0, std::ios::beg);

                   //program.read((char *)(uint8_t *)m_mem, programSize);
            program.read((char*) m_mem->getAdressZero(), programSize);
            program.close();
        } else {

            m_error     = E_LOAD_FAILED;
            programSize = -1;
        }

        return programSize > 0;
    }

    Interrupt run(const uint32_t nbMaxIteration = 0) {

        return m_alu->run(nbMaxIteration);
    }

  private:

    struct VmProperties m_vmProperties;
    enum Error           m_error;
    std::unique_ptr<MemoryHandler> m_mem;
    std::unique_ptr<PrivateAlu> m_alu;
    std::unique_ptr<PrivateVfpv2> m_vfp;
};

using Vfpv2Unprotected = Vfpv2<MemoryRaw>;
using Vfpv2Protected = Vfpv2<MemoryProtected>;

using VmUnprotected = Vm<MemoryRaw, Vfpv2Unprotected>;
using VmProtected = Vm<MemoryProtected, Vfpv2Protected>;

#ifdef MY_LIBRARY_STATIC
// Ne genere pas les constructions suivantes quand ce header est appelé.
// Elles seront construites une seule fois explicitement dans vm.cpp
extern template class Vfpv2<MemoryRaw>;
extern template class Vfpv2<MemoryProtected>;
extern template class Vm<MemoryRaw, Vfpv2Unprotected>;
extern template class Vm<MemoryProtected, Vfpv2Protected>;
#endif

} // namespace armv4vm
