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

#include <memory>
#include <fstream>

#include "armv4vm_p.hpp"
#include "properties.hpp"
#include "vfpv2.hpp"
#include "alu.hpp"

namespace armv4vm {

class TestMem;
template <typename T>
class TestAluInstruction;
class TestVfp;


class VmException : public std::exception {
  public:
    VmException() = default;
    ~VmException() = default;
};


class Vm {
  protected:
    Vm() = default;

  public:

    virtual ~Vm() = default;

    //virtual void init(struct VmProperties &vmProperties) = 0;
    virtual std::byte* reset() = 0;
    virtual uint64_t load() = 0;
    virtual Interrupt run(const uint32_t nbMaxIteration = 0) = 0;
    static std::unique_ptr<Vm> build(const struct VmProperties &vmProperties);
};

template <typename MemoryHandler, typename CoproHandler>
class VmImplementation final : public Vm {
  private:
    using PrivateAlu = Alu<MemoryHandler, CoproHandler>;
    using PrivateVfpv2 = Vfpv2<MemoryHandler>;

    VmImplementation(const struct VmProperties &vmProperties) {

        m_vmProperties = vmProperties;
    }

  public:
    friend TestMem;
    friend TestAluInstruction<MemoryHandler>;
    friend TestVfp;
    friend class Vm;

    ~VmImplementation() = default;

    enum Error {

        E_NONE,
        E_LOAD_FAILED,
        E_UNDEFINED,
    };

    std::byte* reset() {

        m_mem = std::make_unique<MemoryHandler>(m_vmProperties.m_memoryHandlerProperties);
        m_alu = std::make_unique<PrivateAlu>(m_vmProperties.m_aluProperties);
        m_vfp = std::make_unique<PrivateVfpv2>(m_vmProperties.m_coproProperties);

        m_alu->attach(m_mem.get());
        m_alu->attach(m_vfp.get());
        m_vfp->attach(m_mem.get());
        m_vfp->attach(m_alu.get());
        return m_alu->reset();
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

    inline Interrupt run(const uint32_t nbMaxIteration = 0) {

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

using VmUnprotected = VmImplementation<MemoryRaw, Vfpv2Unprotected>;
using VmProtected = VmImplementation<MemoryProtected, Vfpv2Protected>;

#ifdef MY_LIBRARY_STATIC
// Ne genere pas les constructions suivantes quand ce header est appelé.
// Elles seront construites une seule fois explicitement dans vm.cpp
extern template class Vfpv2<MemoryRaw>;
extern template class Vfpv2<MemoryProtected>;
extern template class Vm<MemoryRaw, Vfpv2Unprotected>;
extern template class Vm<MemoryProtected, Vfpv2Protected>;
#endif

inline std::unique_ptr<Vm> Vm::build(const struct VmProperties &vmProperties) {

    std::unique_ptr<Vm> vm = nullptr;

    // Une mémoire ne peut pas avoir de taille et des layout en même temps.
    // C'est une incohérence de configuration. On ne sait donc pas quel type de mémoire il faut créer.
    if(vmProperties.m_memoryHandlerProperties.m_layout.size() && vmProperties.m_memoryHandlerProperties.m_memsize) {

        throw VmException();
    }

    // Quand des permissions sont renseignées,
    // une mémoire de type protegée est créée.
    if(vmProperties.m_memoryHandlerProperties.m_layout.empty()) {

        vm = std::unique_ptr<Vm>(new VmUnprotected(vmProperties));
    }
    else {
        vm = std::unique_ptr<Vm>(new VmProtected(vmProperties));
    }

    return vm;
}


} // namespace armv4vm
