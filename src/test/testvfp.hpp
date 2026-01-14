
#include <QObject>
#include <QTest>
#include <cstddef>

#include "armv4vm.hpp"

namespace armv4vm {

//using Test1Vm = Alu<MemoryProtected>;

// template <typename T> inline T  readPointer(std::byte *mem) { return *reinterpret_cast<T *>(mem); }
// template <typename T> inline T &writePointer(std::byte *mem) { return *reinterpret_cast<T *>(mem); }


//struct VmProperties vmProperties;

class TestVfp : public QObject {
    Q_OBJECT
  private:
    VmProperties m_vmProperties;

  public:
    TestVfp() { m_vmProperties.m_memsize = 512; }

  private slots:

    void testInstance() {
        //struct VmProperties vmProperties;
        //Test1Vm             vm1(&vmProperties);
    }

    void testReadWrite8() {
    }
};
//#include "testvfp.moc"
} // namespace armv4vm
