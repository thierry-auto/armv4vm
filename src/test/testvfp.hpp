
#include <QObject>
#include <QTest>
#include <cstddef>

#include "armv4vm.hpp"


namespace armv4vm {

class TestVfp : public QObject {
    Q_OBJECT
  private:
    VmProperties m_vmProperties;

  public:
    TestVfp() {  }

  private slots:

    void testInstance() {

    }

    void testReadWrite8() {
    }
};

} // namespace armv4vm
