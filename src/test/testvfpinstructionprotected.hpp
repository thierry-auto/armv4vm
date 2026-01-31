#include "armv4vm.hpp"
#include "testvfp.hpp"

namespace armv4vm {

class TestVfpInstructionProtected : public QObject {
    Q_OBJECT
  private:

    TestVfpInstruction<MemoryProtected> m_test;

  public:
    TestVfpInstructionProtected() { }
    virtual ~TestVfpInstructionProtected() = default;

  private slots:

};

} // namespace armv4vm

