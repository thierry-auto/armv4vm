#pragma once

#include "armv4vm.hpp"
#include "testalu.hpp"

namespace armv4vm {

class TestAluProgramProtected : public QObject {
    Q_OBJECT
  private:

    TestAluProgram<MemoryProtected> m_test;

  public:
    TestAluProgramProtected() { }
    virtual ~TestAluProgramProtected() = default;

  public slots:

    void testProgramHello() { m_test.testProgramHello(); }
    void testProgramPrimeN() { m_test.testProgramPrimeN(); }
    void testProgramFloat() {  m_test.testProgramFloat();  }
    void testProgramPrintf() { m_test.testProgramPrintf(); }
    void testProgramModulo() {  m_test.testProgramModulo(); }
    void testProgramBench() { m_test.testProgramBench(); }
};


} // namespace armv4vm
