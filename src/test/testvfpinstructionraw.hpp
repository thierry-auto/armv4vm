#pragma once

#include "armv4vm.hpp"
#include "testvfp.hpp"

namespace armv4vm {

class TestVfpInstructionRaw : public QObject {
    Q_OBJECT
  private:

    TestVfpInstruction<MemoryRaw> m_test;

  public:
    TestVfpInstructionRaw() { }
    virtual ~TestVfpInstructionRaw() = default;

  private slots:

};

} // namespace armv4vm

