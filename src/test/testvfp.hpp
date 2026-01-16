
#include <QObject>
#include <QTest>
#include <cstddef>

#include "armv4vm.hpp"


namespace armv4vm {

class TestVfp : public QObject {
    Q_OBJECT
  private:
    using ProtectedVfp = Vfpv2<MemoryProtected>;
    using ProtectedAlu = Alu<MemoryProtected, ProtectedVfp>;

    std::unique_ptr<MemoryProtected> m_mem;
    std::unique_ptr<ProtectedAlu> m_alu;
    VmProperties m_vmProperties;


  public:
    TestVfp() {

        m_vmProperties.m_memoryHandlerProperties.m_layout.push_back({0, 512, AccessPermission::READ_WRITE});

        m_mem = std::make_unique<MemoryProtected>(m_vmProperties.m_memoryHandlerProperties);
        m_alu = std::make_unique<ProtectedAlu>(m_vmProperties.m_aluProperties);

        m_mem->reset();
        m_mem->addAccessRangeImpl({0, 512, AccessPermission::READ_WRITE});

        m_alu->attach(m_mem.get());
    }

  private slots:
#if 0
    void testFMSR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee002a90); // FMSR S1, R2
        m_alu->m_registers[2] = 0x40490fd0; // 3.14159f
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[1] == std::bit_cast<float>(3.14159f));
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testFMRS() {
        //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee102a90); // FMRS R2, S1
        m_alu->m_mem->writePointer<uint32_t>(0x10, 0xABCDEF01);
        vm.m_coprocessor.m_sRegisters[1] = std::bit_cast<float>(0xaabbccdd);
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0xaabbccdd);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void atestFMRS() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee123a10); // FMRS	R3, S4
        m_alu->m_mem->writePointer<uint32_t>(0x10, 0xABCDEF01);
        m_alu->m_registers[3] = 0xaabbccdd;
        vm.m_coprocessor.m_sRegisters[4] = 3213.4393f;
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[4] == 3213.4393f);
        QVERIFY(m_alu->m_registers[3] == std::bit_cast<uint32_t>(3213.4393f));
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testFMDLR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee041b10); // FMDLR D4, R1
        m_alu->m_registers[1] = 0xaabbccdd;
        vm.m_coprocessor.m_sRegisters[8] = 0.0f;
        vm.m_coprocessor.m_sRegisters[9] = 0.0f;

        m_alu->run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[8] == std::bit_cast<float>(0xaabbccdd));
        QVERIFY(vm.m_coprocessor.m_sRegisters[9] == 0.0f);
        QVERIFY(vm.m_coprocessor.toSingle<uint32_t>(8) == 0xaabbccdd);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(4) == 0x00000000aabbccdd);
    }

    void testFMRDL() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee161b10); // FMRDL R1, D6
        vm.m_coprocessor.setDoubleRegister<uint64_t>(6, 0x11223344aabbccdd);
        m_alu->m_registers[1] = 0x99887766;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0xaabbccdd);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
    }

    void testFMDHR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee292b10); // FMDHR D9, R2
        vm.m_coprocessor.setDoubleRegister<uint64_t>(9, 0x55667788FFEEDDCC);
        m_alu->m_registers[2] = 0x66993322;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x66993322);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(8) == 0x0000000000000000);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(9) == 0x66993322FFEEDDCC);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(10) == 0x0000000000000000);
    }

    void testFMRDH() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xee351b10); // FMRDH R1, D5
        vm.m_coprocessor.setDoubleRegister<uint64_t>(5, 0xdeabcd24841cf651);
        m_alu->m_registers[1] = 0x99887766;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0xdeabcd24);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
    }

    void testFMSRR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xec425a3d); // FMSRR {s27,s28}, r5, r2
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[5] = 0x12345678;

        m_alu->run(1);

        QVERIFY(vm.m_coprocessor.toSingle<uint32_t>(27) == 0x12345678);
        QVERIFY(vm.m_coprocessor.toSingle<uint32_t>(28) == 0xaabbccdd);
    }

    void testFMRRS() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xec525a3d); // FMRRS r5, r2, {s27,s28}
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[5] = 0x12345678;
        vm.m_coprocessor.setSingleRegister<uint32_t>(27, 0xDF34AC82);
        vm.m_coprocessor.setSingleRegister<uint32_t>(28, 0x11BBBB37);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x11BBBB37);
        QVERIFY(m_alu->m_registers[5] == 0xDF34AC82);
    }

    void testFMDRR() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xec494b13); // FMDRR d3, r4, r9
        m_alu->m_registers[4] = 0x7EB1A776;
        m_alu->m_registers[9] = 0xACB4B4AC;
        vm.m_coprocessor.setDoubleRegister<uint64_t>(9, 0x55667788FFEEDDCC);

        m_alu->run(1);

        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(3) == 0xacb4b4ac7eb1a776);
    }

    void testFMRRD() {

          //VirtualMachineUnprotected vm(&vmProperties);
        m_alu->reset();

        m_mem->writePointer<uint32_t>(0, 0xec512b16); // FMRRD  r2, r1, d6
        m_alu->m_registers[1] = 0x7EB1A776;
        m_alu->m_registers[2] = 0xACB4B4AC;
        m_vfp->setDoubleRegister<uint64_t>(6, 0x55667788FFEEDDCC);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0xFFEEDDCC);
    }

    void testFSTMS() {

        m_alu->reset();

        m_alu->m_mem->writePointer<uint32_t>(0, 0xec831a03); // VSTM R3, {S2-S4} aka FSTMS
        m_alu->m_registers[3] = 0x00000016;
        vm.m_coprocessor.setSingleRegister<uint32_t>(1, 0xCCDDEEFF);
        vm.m_coprocessor.setSingleRegister<uint32_t>(2, 0x11223344);
        vm.m_coprocessor.setSingleRegister<uint32_t>(3, 0x55667788);
        vm.m_coprocessor.setSingleRegister<uint32_t>(4, 0x9900AABB);

        m_alu->run(1);

        QVERIFY(m_alu->m_mem->readPointer<uint32_t>(0x16) == 0x11223344);
    }
#endif
};

} // namespace armv4vm
