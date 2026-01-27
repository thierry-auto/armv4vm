#pragma once

#include "armv4vm.hpp"
#include "testalu.hpp"

namespace armv4vm {

class TestAluInstructionRaw : public QObject {
    Q_OBJECT
  private:

    TestAluInstruction<MemoryRaw> m_test;

  public:
    TestAluInstructionRaw() {

    }
    virtual ~TestAluInstructionRaw() = default;

  private slots:

    void testMOV() { m_test.testMOV(); }
    void testADD() { m_test.testADD(); }
    void testADD2() { m_test.testADD2(); }
    void testSUBS() { m_test.testSUBS(); }
    void testSUBS2() { m_test.testSUBS2(); }
    void testSUBS3() { m_test.testSUBS3(); }
    void testLSLS() { m_test.testLSLS(); }
    void testLSLS2() { m_test.testLSLS2(); }
    void testLSRS() { m_test.testLSRS(); }
    void testASRS() { m_test.testASRS(); }
    void testASRS2() { m_test.testASRS2(); }
    void testASRS3() { m_test.testASRS3(); }
    void testASRS4() { m_test.testASRS4(); }
    void testASRS5() { m_test.testASRS5(); }
    void testASRS6() { m_test.testASRS6(); }
    void testASRS7() { m_test.testASRS7(); }
    void testRORS() { m_test.testRORS(); }
    void testRORS2() { m_test.testRORS2(); }
    void testRORS3() { m_test.testRORS3(); }
    void testRRXS() { m_test.testRRXS(); }
    void testRRXS2() { m_test.testRRXS2(); }
    void testRORS4() { m_test.testRORS4(); }
    void testRORS5() { m_test.testRORS5(); }
    void testMOVS() { m_test.testMOVS(); }
    void testORR() { m_test.testORR(); }
    void testORR2() { m_test.testORR2(); }
    void testORR3() { m_test.testORR3(); }
    void testORR4() { m_test.testORR4(); }
    void testLDR() { m_test.testLDR(); }
    void testSTR() { m_test.testSTR(); }
    void testPUSH() { m_test.testPUSH(); }
    void testPUSHPOP() { m_test.testPUSHPOP(); }
    void testADD3() { m_test.testADD3(); }
    void testADDS() { m_test.testADDS(); }
    void testADD4() { m_test.testADD4(); }
    void testADD5() { m_test.testADD5(); }
    void testADD6() { m_test.testADD6(); }
    void testLDRB() { m_test.testLDRB(); }
    void testLDRB2() { m_test.testLDRB2(); }
    void testLDRB3() { m_test.testLDRB3(); }
    void testLDRB4() { m_test.testLDRB4(); }
    void testLDRB5() { m_test.testLDRB5(); }
    void testLDR2() { m_test.testLDR2(); }
    void testMUL1() { m_test.testMUL1(); }
    void testMLA() { m_test.testMLA(); }
    void testMLA2() { m_test.testMLA2(); }
    void testMLA3() { m_test.testMLA3(); }
    void testMLA4() { m_test.testMLA4(); }
    void testMLA5() { m_test.testMLA5(); }
    void testLDR3() { m_test.testLDR3(); }
    void testLDR4() { m_test.testLDR4(); }
    void testLDR5() { m_test.testLDR5(); }
    void testLDR6() { m_test.testLDR6(); }
    void testLDR7() { m_test.testLDR7(); }
    void testLDR8() { m_test.testLDR8(); }
    void testLDR9() { m_test.testLDR9(); }
    void testLDR10() { m_test.testLDR10(); }
    void testLDR11() { m_test.testLDR11(); }
    void testLDR12() { m_test.testLDR12(); }
    void testLDR13() { m_test.testLDR13(); }
    void testLDR14() { m_test.testLDR14(); }
    void testLDR15() { m_test.testLDR15(); }
    void testLDMFD() { m_test.testLDMFD(); }
    void testLDMFA() { m_test.testLDMFA(); }
    void testSTMFA() { m_test.testSTMFA(); }
    void testSTMED() { m_test.testSTMED(); }
    void testSTMEA() { m_test.testSTMEA(); }
    void testSTMFA2() { m_test.testSTMFA2(); }
    void testSTMFA3() { m_test.testSTMFA3(); }
    void testSTMFA4() { m_test.testSTMFA4(); }
    void testSTR2() { m_test.testSTR2(); }
    void testSTM1() { m_test.testSTM1(); }
    void testSTM2() { m_test.testSTM2(); }
    void testCONDPM() { m_test.testCONDPM(); }
    void testCONDVC() { m_test.testCONDVC(); }
    void testCONDCC() { m_test.testCONDCC(); }
    void testHALF() { m_test.testHALF(); }
    void testHALF2() { m_test.testHALF2(); }
    void testHALF3() { m_test.testHALF3(); }
    void testSTRH() { m_test.testSTRH(); }
    void testSTRH2() { m_test.testSTRH2(); }
    void testSTRH3() { m_test.testSTRH3(); }
    void testSTRH4() { m_test.testSTRH4(); }
    void testSTRH5() { m_test.testSTRH5(); }
    void testSTRH6() { m_test.testSTRH6(); }
    void testLDRH1() { m_test.testLDRH1(); }
    void testLDRH2() { m_test.testLDRH2(); }
    void testSTRB() { m_test.testSTRB(); }
    void testSTRB2() { m_test.testSTRB2(); }
    void testSTRB3() { m_test.testSTRB3(); }
    void testSTRB4() { m_test.testSTRB4(); }
    void testADCS() { m_test.testADCS(); }
    void testANDS() { m_test.testANDS(); }
    void testTEQ() { m_test.testTEQ(); }
    void testTEST() { m_test.testTEST(); }
    void testRSC() { m_test.testRSC(); }
    void testUMULL() { m_test.testUMULL(); }
    void testSMULL() { m_test.testSMULL(); }
    void testSMULL2() { m_test.testSMULL2(); }
    void testRSBS() { m_test.testRSBS(); }
    void testTEST2() { m_test.testTEST2(); }
    void testR15() { m_test.testR15(); }
    void testSWP_1() { m_test.testSWP_1(); }
    void testSWPB_1() { m_test.testSWPB_1(); }
};

} // namespace armv4vm

