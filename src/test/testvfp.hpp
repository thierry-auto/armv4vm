#pragma once

#include <QObject>
#include <QTest>

#include "armv4vm.hpp"


namespace armv4vm {

template<typename T>
class TestVfpInstruction  {

  private:

    std::unique_ptr<T> m_mem;
    std::unique_ptr<Vfpv2<T>> m_vfp;
    std::unique_ptr<Alu<T, Vfpv2<T>>> m_alu;
    VmProperties m_vmProperties;


  public:
    TestVfpInstruction() {

        m_vmProperties.m_memoryHandlerProperties.m_layout.push_back({0, 512, AccessPermission::READ_WRITE});
        m_vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 1_kb;

        m_mem = std::make_unique<T>(m_vmProperties.m_memoryHandlerProperties);
        m_vfp = std::make_unique<Vfpv2<T>>(m_vmProperties.m_coproProperties);
        m_alu = std::make_unique<Alu<T, Vfpv2<T>>>(m_vmProperties.m_aluProperties);

        m_mem->reset();
        m_vfp->attach(m_mem.get());
        m_vfp->attach(m_alu.get());
        m_alu->attach(m_mem.get());
    }

  public slots:

    void testFMSR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee002a90); // FMSR S1, R2
        m_alu->m_registers[2] = 0x40490fd0; // 3.14159f
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_vfp->m_sRegisters[1] == std::bit_cast<float>(3.14159f));
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testFMRS() {
        //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee102a90); // FMRS R2, S1
        m_mem->template writePointer<uint32_t>(0x10, 0xABCDEF01);
        m_vfp->m_sRegisters[1] = std::bit_cast<float>(0xaabbccdd);
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0xaabbccdd);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void atestFMRS() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee123a10); // FMRS	R3, S4
        m_mem->template writePointer<uint32_t>(0x10, 0xABCDEF01);
        m_alu->m_registers[3] = 0xaabbccdd;
        m_vfp->m_sRegisters[4] = 3213.4393f;
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_vfp->m_sRegisters[4] == 3213.4393f);
        QVERIFY(m_alu->m_registers[3] == std::bit_cast<uint32_t>(3213.4393f));
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testFMDLR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee041b10); // FMDLR D4, R1
        m_alu->m_registers[1] = 0xaabbccdd;
        m_vfp->m_sRegisters[8] = 0.0f;
        m_vfp->m_sRegisters[9] = 0.0f;

        m_alu->run(1);

        QVERIFY(m_vfp->m_sRegisters[8] == std::bit_cast<float>(0xaabbccdd));
        QVERIFY(m_vfp->m_sRegisters[9] == 0.0f);
        QVERIFY(m_vfp->template toSingle<uint32_t>(8) == 0xaabbccdd);
        QVERIFY(m_vfp->template toDouble<uint64_t>(4) == 0x00000000aabbccdd);
    }

    void testFMRDL() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee161b10); // FMRDL R1, D6
        m_vfp->template setDoubleRegister<uint64_t>(6, 0x11223344aabbccdd);
        m_alu->m_registers[1] = 0x99887766;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0xaabbccdd);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
    }

    void testFMDHR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee292b10); // FMDHR D9, R2
        m_vfp->template setDoubleRegister<uint64_t>(9, 0x55667788FFEEDDCC);
        m_alu->m_registers[2] = 0x66993322;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x66993322);
        QVERIFY(m_vfp->template toDouble<uint64_t>(8) == 0x0000000000000000);
        QVERIFY(m_vfp->template toDouble<uint64_t>(9) == 0x66993322FFEEDDCC);
        QVERIFY(m_vfp->template toDouble<uint64_t>(10) == 0x0000000000000000);
    }

    void testFMRDH() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xee351b10); // FMRDH R1, D5
        m_vfp->template setDoubleRegister<uint64_t>(5, 0xdeabcd24841cf651);
        m_alu->m_registers[1] = 0x99887766;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0xdeabcd24);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
    }

    void testFMSRR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xec425a3d); // FMSRR {s27,s28}, r5, r2
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[5] = 0x12345678;

        m_alu->run(1);

        QVERIFY(m_vfp->template toSingle<uint32_t>(27) == 0x12345678);
        QVERIFY(m_vfp->template toSingle<uint32_t>(28) == 0xaabbccdd);
    }

    void testFMRRS() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xec525a3d); // FMRRS r5, r2, {s27,s28}
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[5] = 0x12345678;
        m_vfp->template setSingleRegister<uint32_t>(27, 0xDF34AC82);
        m_vfp->template setSingleRegister<uint32_t>(28, 0x11BBBB37);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x11BBBB37);
        QVERIFY(m_alu->m_registers[5] == 0xDF34AC82);
    }

    void testFMDRR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xec494b13); // FMDRR d3, r4, r9
        m_alu->m_registers[4] = 0x7EB1A776;
        m_alu->m_registers[9] = 0xACB4B4AC;
        m_vfp->template setDoubleRegister<uint64_t>(9, 0x55667788FFEEDDCC);

        m_alu->run(1);

        QVERIFY(m_vfp->template toDouble<uint64_t>(3) == 0xacb4b4ac7eb1a776);
    }

    void testFMRRD() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xec512b16); // FMRRD  r2, r1, d6
        m_alu->m_registers[1] = 0x7EB1A776;
        m_alu->m_registers[2] = 0xACB4B4AC;
        m_vfp->template setDoubleRegister<uint64_t>(6, 0x55667788FFEEDDCC);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0xFFEEDDCC);
    }

    void testFSTMS() {

        m_alu->reset();

        m_mem->template writePointer<uint32_t>(0, 0xec831a03); // VSTM R3, {S2-S4} aka FSTMS
        m_alu->m_registers[3] = 0x00000016;
        m_vfp->template setSingleRegister<uint32_t>(1, 0xCCDDEEFF);
        m_vfp->template setSingleRegister<uint32_t>(2, 0x11223344);
        m_vfp->template setSingleRegister<uint32_t>(3, 0x55667788);
        m_vfp->template setSingleRegister<uint32_t>(4, 0x9900AABB);

        m_alu->run(1);

        QVERIFY(m_mem->template readPointer<uint32_t>(0x16) == 0x11223344);
    }

};

} // namespace armv4vm
