#pragma once

#include <QObject>
#include <QTest>

#include "armv4vm.hpp"


namespace armv4vm {

template<typename T>
class TestVfpInstruction  {

  private:

    std::unique_ptr<T> m_mem;
    std::unique_ptr<NullCopro<T>> m_vfp;
    std::unique_ptr<Alu<T, NullCopro<T>>> m_alu;
    VmProperties m_vmProperties;


  public:
    TestVfpInstruction() {

        m_vmProperties.m_memoryHandlerProperties.m_layout.push_back({0, 512, AccessPermission::READ_WRITE});
        m_vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 1_kb;

        m_mem = std::make_unique<T>(m_vmProperties.m_memoryHandlerProperties);
        m_vfp = std::make_unique<NullCopro<T>>(m_vmProperties.m_coproProperties);
        m_alu = std::make_unique<Alu<T, NullCopro<T>>>(m_vmProperties.m_aluProperties);

        m_mem->reset();
        m_vfp->attach(m_mem.get());
        m_vfp->attach(m_alu.get());
        m_alu->attach(m_mem.get());
        m_alu->attach(m_vfp.get());
    }

  public:

};

} // namespace armv4vm
