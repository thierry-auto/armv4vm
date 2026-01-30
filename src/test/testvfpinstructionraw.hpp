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

    void testFMSR() { m_test.testFMSR(); }
    void testFMRS() { m_test.testFMRS(); }
    void atestFMRS() {  m_test.atestFMRS();  }
    void testFMDLR() { m_test.testFMDLR(); }
    void testFMRDL() {  m_test.testFMRDL(); }
    void testFMDHR() { m_test.testFMDHR(); }

    void testFMRDH() { m_test.testFMRDH(); }
    void testFMSRR() { m_test.testFMSRR(); }
    void testFMRRS() {  m_test.testFMRRS();  }
    void testFMDRR() { m_test.testFMDRR(); }
    void testFMRRD() {  m_test.testFMRRD(); }
    void testFSTMSU() { m_test.testFSTMSU(); }
    void testFSTMDU() { m_test.testFSTMDU(); }
};

} // namespace armv4vm

