//    Copyright (c) 2020, thierry vic

//    This file is part of armv4vm.

//    armv4vm is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    armv4vm is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with armv4vm.  If not, see <http://www.gnu.org/licenses/>.


#include <QObject>
#include <QtTest>

#include "vm.h"
#include "config.h"

struct VmProperties vmProperties;

static void seti(uint8_t *ram, uint32_t ii) {

    // Identical endianness x86/arm, no arrangement
    *reinterpret_cast<uint32_t *>(ram) = ii;
}

class TestVm : public QObject {
    Q_OBJECT

  public:
    TestVm() { vmProperties.m_memsize = 512; }

  private slots:

    void testMOV();
    void testASRS();
    void testASRS2();
    void testASRS3();
    void testASRS4();
    void testASRS5();
    void testASRS6();
    void testASRS7();
    void testRORS();
    void testRORS2();
    void testRORS3();
    void testRORS4();
    void testRORS5();
    void testRRXS();
    void testRRXS2();
    void testORR();
    void testORR2();
    void testORR3();
    void testORR4();
    void testADD();
    void testADD2();
    void testSUBS();

    void testSUBS2();
    void testSUBS3();
    void testLSLS();
    void testLSLS2();
    void testLSRS();
    void testMOVS();
    void testADDS();
    void testADD3();
    void testADD4();
    void testADD5();
    void testADD6();
    void testLDRB();
    void testLDRB2();
    void testLDRB3();
    void testLDRB4();
    void testLDRB5();
    void testLDR3();
    void testLDR4();
    void testLDR5();
    void testLDR6();
    void testLDR7();
    void testLDR8();
    void testLDR9();
    void testLDR10();
    void testLDR11();
    void testLDR12();
    void testLDR13();
    void testLDR14();
    void testLDR15();

    void testLDR();
    void testPUSHPOP();
    void testLDMFD();
    void testLDMFA();
    void testSTMFA();
    void testSTMEA();
    void testSTMFA2();
    void testSTMED();
    void testSTR();
    void testPUSH();
    void testSTR2();
    void testLDR2();

    void testProgram1();
};

void TestVm::testMOV() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe3a0002d); // mov r0, #45

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 45);
    QVERIFY(vm.m_cpsr == 0);
}

void TestVm::testADD() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0900001); // add r0, r0, r1

    vm.m_registers[0] = 0xFFFFFFFF;
    vm.m_registers[1] = 0x1;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0);
    QVERIFY(vm.m_cpsr == 0x60000000);
}

void TestVm::testADD2() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0900001); // add r0, r0, r1

    vm.m_registers[0] = 0xFFFFFFFF;
    vm.m_registers[1] = 0x4;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 3);
    QVERIFY(vm.m_cpsr == 0x20000000);
}

void TestVm::testSUBS() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0510002); // subs r0, r1, r2

    vm.m_registers[0] = 0xFFFFFFFF;
    vm.m_registers[1] = 0x4;
    vm.m_registers[2] = 0x8;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xfffffffc);
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testSUBS2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0510002); // subs    r0, r1, r2

    vm.m_registers[0] = 0xFFFFFFFF;
    vm.m_registers[1] = 0xFFFFFFFF; // -1
    vm.m_registers[2] = 0x1;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xfffffffe); // -2
    QVERIFY(vm.m_cpsr == 0xa0000000);
}

void TestVm::testSUBS3() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0510002); // subs    r0, r1, r2
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0xFFFFFFFF; // -1
    vm.m_registers[2] = 0xFFFFFFFF; // -1

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000000); // -2
    QVERIFY(vm.m_cpsr == 0x60000000);
}

void TestVm::testLSLS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00211);
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0xFFFFFFFF; // -1
    vm.m_registers[2] = 0xFFFFFFFF; // -1

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000000); // -2
    QVERIFY(vm.m_cpsr == 0x40000000);
}

void TestVm::testLSLS2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00001);
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x801200EF; // -1

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testLSRS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00231); // lsrs r0, r1, r2
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x801200EF; // -1
    vm.m_registers[2] = 0x100;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testASRS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x801200EF; // -1
    vm.m_registers[2] = 0x10;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xffff8012); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testASRS2() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x8012FFFF; // -1
    vm.m_registers[2] = 0x10;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xffff8012); // -2
    QVERIFY(vm.m_cpsr == 0xa0000000);
}

void TestVm::testASRS3() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, r2
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x0012FFFF; // -1
    vm.m_registers[2] = 0x10;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000012); // -2
    QVERIFY(vm.m_cpsr == 0x20000000);
}

void TestVm::testASRS4() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x8012FFFF; // -1
    vm.m_registers[2] = 0x00FF;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xFFFFFFFF); // -2
    QVERIFY(vm.m_cpsr == 0xa0000000);
}

void TestVm::testASRS5() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x1012FFFF; // -1
    vm.m_registers[2] = 0x00FF;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0); // -2
    QVERIFY(vm.m_cpsr == 0x40000000);
}

void TestVm::testASRS6() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00251); // asrs r0, r1, #0
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x801200EF; // -1
    vm.m_registers[2] = 0;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testASRS7() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00001); // asrs r0, r1, #0
    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x801200EF; // -1

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testRORS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00271); // rors r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x00000010; // -1
    vm.m_registers[2] = 0x000000FF;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000020); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testRORS2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00271); // rors r0, r1, r2

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x00000010; // -1
    vm.m_registers[2] = 0x00000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000010); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testRORS3() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00001); // rors r0, r1, #0

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x00000010; // -1
    vm.m_registers[2] = 0x00000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000010); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testRRXS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00061); // rrxs r0, r1

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x00000010; // -1
    vm.m_cpsr         = 0x20000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x80000008); // -2
    QVERIFY(vm.m_cpsr == 0x80000000);
}

void TestVm::testRRXS2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00061); // rrxs r0, r1

    vm.m_registers[0] = 0xABABABAB;
    vm.m_registers[1] = 0x00000010; // -1
    vm.m_cpsr         = 0xD0000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000008); // -2
    QVERIFY(vm.m_cpsr == 0x10000000);
}

void TestVm::testRORS4() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00170); // rors r0, r0, r1

    vm.m_registers[0] = 0x00000008;
    vm.m_registers[1] = 0x00000010; // -1

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00080000); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testRORS5() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00260); // rors r0, #4
    vm.m_registers[0] = 0x00080000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00008000); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testMOVS() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe1b00221); // movs r0, r1, lsr #4

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00010000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00001000); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testORR() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe3800d7d); // ORR r0, r0, #8000
    vm.m_registers[0] = 0x00000001;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00001f41); // -2
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testORR2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe3800002); // ORR r0, r0, #2 ; 0x2
    vm.m_registers[0] = 0xF000000F;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0xF000000F);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testORR3() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe3800002); // ORR r0, r0, #2 ; 0x2
    vm.m_registers[0] = 0x00000000;
    vm.m_registers[0] = 0x800F0001;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x800F0003);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testORR4() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe3810602); // ORR r0, r1, #2097152
    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x800f0001;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x802f0001);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDR() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5910004); // ldr  r0, [r1, #4]
    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x0000000c;
    seti(vm.m_ram + 0xc + 0x4, 0x12345678);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x12345678);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testSTR() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5810004); // str     r0, [r1, #4]

    vm.m_registers[0] = 0xabcdef02;
    vm.m_registers[1] = 0x0000000c;
    vm.run(1);

    QVERIFY(vm.m_ram[0xc + 4] == 0x02);
    QVERIFY(vm.m_ram[0xc + 5] == 0xef);
    QVERIFY(vm.m_ram[0xc + 6] == 0xcd);
    QVERIFY(vm.m_ram[0xc + 7] == 0xab);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testPUSH() {
    //uint8_t ram[128] = {0};

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe92d0007); // push     {r0, r1, r2}
    vm.m_registers[0]  = 0x00000001;
    vm.m_registers[1]  = 0x00000022;
    vm.m_registers[2]  = 0x00000333;
    vm.m_registers[13] = 0x50;
    vm.run(1);

    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 12) == 0x00000001);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 8) == 0x00000022);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 4) == 0x00000333);
    QVERIFY(vm.m_registers[13] == 0x44);

    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testPUSHPOP() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe92d001f);     // push {r0, r1, r2, r3, r4}
    seti(vm.m_ram + 4, 0xe8bd03e0); // pop {r5, r6, r7, r8, r9}

    vm.m_registers[0]  = 0x00000001;
    vm.m_registers[1]  = 0x00000022;
    vm.m_registers[2]  = 0x00000333;
    vm.m_registers[3]  = 0x00004444;
    vm.m_registers[4]  = 0x00055555;
    vm.m_registers[5]  = 0x0;
    vm.m_registers[6]  = 0x0;
    vm.m_registers[7]  = 0x0;
    vm.m_registers[8]  = 0x0;
    vm.m_registers[9]  = 0x0;
    vm.m_registers[13] = 0x50;

    vm.run(1);

    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 20) == 0x00000001);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 16) == 0x00000022);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 12) == 0x00000333);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 8) == 0x00004444);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x50 - 4) == 0x00055555);
    QVERIFY(vm.m_registers[13] == 0x3c);

    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[5] == 0x00000001);
    QVERIFY(vm.m_registers[6] == 0x00000022);
    QVERIFY(vm.m_registers[7] == 0x00000333);
    QVERIFY(vm.m_registers[8] == 0x00004444);
    QVERIFY(vm.m_registers[9] == 0x00055555);
    QVERIFY(vm.m_registers[13] == 0x50);

    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testADD3() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0810002); // add r0, r1, r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000444;
    vm.m_registers[2] = 0x00001000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00001444);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testADDS() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0910002); // adds r0, r1, r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x7FFFFFFF;
    vm.m_registers[2] = 0x00000001;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x80000000);
    QVERIFY(vm.m_cpsr == 0x90000000);
}

void TestVm::testADD4() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0910002); // add r0, r1, r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x40000000;
    vm.m_registers[2] = 0x40000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x80000000);
    QVERIFY(vm.m_cpsr == 0x90000000);
}

void TestVm::testADD5() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0810002); // add r0, r1, r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x40000000;
    vm.m_registers[2] = 0x40000000;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x80000000);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testADD6() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe0910002); // add r0, r1, r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x7FFFFFFF;
    vm.m_registers[2] = 0x00000001;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x80000000);
    QVERIFY(vm.m_cpsr == 0x90000000);
}

void TestVm::testLDRB() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5f10002); // ldrb r0, [r1, #2]!
    seti(vm.m_ram + 4, 0xe5e1000c);  // strb r0, [r1, #12]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000022);
    QVERIFY(vm.m_registers[1] == 0x00000102);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x0000010e);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x00220000);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDRB2() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5f10001); // ldrb r0, [r1, #1]!
    seti(vm.m_ram + 4, 0xe5e1000b);  // strb r0, [r1, #11]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000033);
    QVERIFY(vm.m_registers[1] == 0x00000101);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x0000010c);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x00000033);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDRB3() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5d10000); // ldrb r0, [r1]
    seti(vm.m_ram + 4, 0xe5d20000);
    seti(vm.m_ram + 8, 0xe5d30000);
    seti(vm.m_ram + 12, 0xe5d40000);

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    vm.m_registers[2] = 0x00000101;
    vm.m_registers[3] = 0x00000102;
    vm.m_registers[4] = 0x00000103;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x00000044);
    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x00000033);
    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x00000022);
    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x00000011);
}

void TestVm::testLDRB4() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5f10003);      // ldrb r0, [r1, #3]!
    seti(vm.m_ram + 4, 0xe5e1000d);  // strb r0, [r1, #13]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000011);
    QVERIFY(vm.m_registers[1] == 0x00000103);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x00000110);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x110) == 0x00000011);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDRB5() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe4110004);      // ldr r0, [r1], #-4
    seti(vm.m_ram + 4, 0xe4010018);  // str r0, [r1], #-24

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x000000fc);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x000000e4);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0xfc) == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDR2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5910000); // ldr r0, [r1]
    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x11223344);
}

void TestVm::testLDR3() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5b10000);      // ldr r0, [r1, #0]!
    seti(vm.m_ram + 4, 0xe5a1000c);  // str r0, [r1, #12]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000100);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x0000010c);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR4() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5310004);      // ldr r0, [r1, #-4]!
    seti(vm.m_ram + 4, 0xe5210018);  // str r0, [r1, #-24]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000104;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000100);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x000000e8);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0xe8) == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR5() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe4910004);      // ldr r0, [r1], #4
    seti(vm.m_ram + 4, 0xe4010018);  // str r0, [r1], #-24

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000104);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x000000ec);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR6() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe4910000);      // ldr r0, [r1], #0
    seti(vm.m_ram + 4, 0xe4010001);  // str r0, [r1], #-24! -> str r0, [r1], #-1

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000100);
    QVERIFY(vm.m_cpsr == 0x00000000);

    vm.run(1);

    QVERIFY(vm.m_registers[1] == 0x000000ff);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x55667788);
    QVERIFY(vm.m_cpsr == 0x00000000);
}

void TestVm::testLDR7() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe491f000);      // ldr r15, [r1], #0

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000000);
    QVERIFY(vm.m_registers[1] == 0x00000100);
    QVERIFY(vm.m_registers[15] == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR8() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe6910002);      // ldr r0, [r1], r2

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    vm.m_registers[2] = 0x00000004;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000104);
    QVERIFY(vm.m_registers[2] == 0x00000004);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR9() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe7b10002);      // ldr r0, [r1, r2]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    vm.m_registers[2] = 0x00000004;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x55667788);
    QVERIFY(vm.m_registers[1] == 0x00000104);
    QVERIFY(vm.m_registers[2] == 0x00000004);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR10() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe7f10002);      // ldrb r0, [r1, r2]!

    vm.m_registers[0] = 0x00000000;
    vm.m_registers[1] = 0x00000100;
    vm.m_registers[2] = 0x00000004;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000088);
    QVERIFY(vm.m_registers[1] == 0x00000104);
    QVERIFY(vm.m_registers[2] == 0x00000004);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR11() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe7e10002);      // strb r0, [r1, r2]!

    vm.m_registers[0] = 0x99aabbcc;
    vm.m_registers[1] = 0x00000100;
    vm.m_registers[2] = 0x00000004;
    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x99aabbcc);
    QVERIFY(vm.m_registers[1] == 0x00000104);
    QVERIFY(vm.m_registers[2] == 0x00000004);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x556677cc);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR12() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe58f0000);      // str r0, [r15]

    vm.m_registers[0] = 0x99aabbcc;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x99aabbcc);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR13() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5cf0000);      // strb r0, [r15]
    seti(vm.m_ram+4, 0x11223344);
    seti(vm.m_ram+8, 0x55667788);
    vm.m_registers[0] = 0x99aabbcc;

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x556677cc);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR14() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5c10000);      // strb r0, [r1]
    seti(vm.m_ram+4, 0x11223344);
    seti(vm.m_ram+8, 0x55667788);
    vm.m_registers[0] = 0x99aabbee;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram+0x100, 0xaabbccdd);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x99aabbee);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x4) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x55667788);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0xaabbccee);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDR15() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe7f10002);      // ldrb r0, [r1]

    vm.m_registers[0] = 0xdeadbeaf;
    vm.m_registers[1] = 0x00000100;
    seti(vm.m_ram + 0x100, 0x11223344);

    vm.run(1);

    QVERIFY(vm.m_registers[0] == 0x00000044);
    QVERIFY(vm.m_registers[1] == 0x00000100);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x11223344);
    QVERIFY(vm.m_cpsr == 0x00000000);

}

void TestVm::testLDMFD() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe8bd000f); // LDMFD r13!, {r0-r3}
    vm.m_registers[0]  = 0x00000000;
    vm.m_registers[1]  = 0x00000000;
    vm.m_registers[2]  = 0x00000000;
    vm.m_registers[3]  = 0x00000000;
    vm.m_registers[13] = 0x00000100;

    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);
    seti(vm.m_ram + 0x108, 0x99aabbcc);
    seti(vm.m_ram + 0x10c, 0xddee00ff);

    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x55667788);
    QVERIFY(vm.m_registers[2] == 0x99aabbcc);
    QVERIFY(vm.m_registers[3] == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x00000110);
}

void TestVm::testLDMFA() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe83d000f); // LDMFA r13!, {r0-r3}

    vm.m_registers[0]  = 0x00000000;
    vm.m_registers[1]  = 0x00000000;
    vm.m_registers[2]  = 0x00000000;
    vm.m_registers[3]  = 0x00000000;
    vm.m_registers[13] = 0x0000010c;

    seti(vm.m_ram + 0x100, 0x11223344);
    seti(vm.m_ram + 0x104, 0x55667788);
    seti(vm.m_ram + 0x108, 0x99aabbcc);
    seti(vm.m_ram + 0x10c, 0xddee00ff);

    vm.run(1);
    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x55667788);
    QVERIFY(vm.m_registers[2] == 0x99aabbcc);
    QVERIFY(vm.m_registers[3] == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x000000fc);
}

void TestVm::testSTMFA() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe92d000f); // STMFA r13!, {r0-r3}

    vm.m_registers[0]  = 0x11223344;
    vm.m_registers[1]  = 0x55667788;
    vm.m_registers[2]  = 0x99aabbcc;
    vm.m_registers[3]  = 0xddee00ff;
    vm.m_registers[13] = 0x00000110;

    vm.run(1);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x55667788);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x00000100);
}

void TestVm::testSTMED() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe82d000f); // STMED r13!, {r0-r3}

    vm.m_registers[0]  = 0x11223344;
    vm.m_registers[1]  = 0x55667788;
    vm.m_registers[2]  = 0x99aabbcc;
    vm.m_registers[3]  = 0xddee00ff;
    vm.m_registers[13] = 0x00000100;

    vm.run(1);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0xf4) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0xf8) == 0x55667788);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0xfc) == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x000000f0);
}

void TestVm::testSTMEA() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe8ad000f); // STMEA r13!, {r0-r3}

    vm.m_registers[0]  = 0x11223344;
    vm.m_registers[1]  = 0x55667788;
    vm.m_registers[2]  = 0x99aabbcc;
    vm.m_registers[3]  = 0xddee00ff;
    vm.m_registers[13] = 0x00000100;

    vm.run(1);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x55667788);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x00000110);
}

void TestVm::testSTMFA2() {
    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe9ad000f); // STMFA r13!, {r0-r3}

    vm.m_registers[0]  = 0x11223344;
    vm.m_registers[1]  = 0x55667788;
    vm.m_registers[2]  = 0x99aabbcc;
    vm.m_registers[3]  = 0xddee00ff;
    vm.m_registers[13] = 0x00000100;

    vm.run(1);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x11223344);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x55667788);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x99aabbcc);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x110) == 0xddee00ff);
    QVERIFY(vm.m_registers[13] == 0x00000110);
}

void TestVm::testSTR2() {

    VirtualMachine vm(&vmProperties, this);
    vm.init();

    seti(vm.m_ram, 0xe5a10004); // str r0, [r1, #4]!
    vm.m_registers[0] = 0x11223344;
    vm.m_registers[1] = 0x00000080;

    vm.run(1);
    QVERIFY(*(uint32_t *)(vm.m_ram + 0x84) == 0x11223344);
    QVERIFY(vm.m_registers[0] == 0x11223344);
    QVERIFY(vm.m_registers[1] == 0x00000084);
}

void TestVm::testProgram1() {

    QString binPath(getBinPath());

    vmProperties.m_memsize = 1024*1024*24; // 24 Mio
    vmProperties.m_bin = binPath + "/test_compile/game.bin";
    unsigned char * mem = nullptr;

    VirtualMachine vm(&vmProperties, this);
    mem = vm.init();
    vm.load();

    while(true) {

        vm.run(1);
        if(*(mem + 0x00C00000) != 0) {
            qDebug() << (char)*(mem + 0x00C00000);
            *(mem + 0x00C00000) = 0;
        }
    }

}

QTEST_MAIN(TestVm)
#include "test.moc"
