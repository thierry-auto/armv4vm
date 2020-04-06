//    Copyright (c) 2020, thierry vic
//
//    This file is part of armv4vm.
//
//    armv4vm is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    armv4vm is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with armv4vm.  If not, see <http://www.gnu.org/licenses/>.

#include <QObject>
#include <QtTest>
#include <iostream>

#include "config.h"
#include "vm.h"

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

    void testMOV() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe3a0002d); // mov r0, #45

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 45);
        QVERIFY(vm.m_cpsr == 0);
    }

    void testADD() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0900001); // add r0, r0, r1

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0x1;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testADD2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0900001); // add r0, r0, r1

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0x4;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 3);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testSUBS() {

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

    void testSUBS2() {
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

    void testSUBS3() {
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

    void testLSLS() {
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

    void testLSLS2() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe1b00001);
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testLSRS() {
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

    void testASRS() {
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

    void testASRS2() {

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

    void testASRS3() {
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

    void testASRS4() {
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

    void testASRS5() {
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

    void testASRS6() {
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

    void testASRS7() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe1b00001); // asrs r0, r1, #0
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testRORS() {
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

    void testRORS2() {
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

    void testRORS3() {
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

    void testRRXS() {
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

    void testRRXS2() {
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

    void testRORS4() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe1b00170); // rors r0, r0, r1

        vm.m_registers[0] = 0x00000008;
        vm.m_registers[1] = 0x00000010; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00080000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testRORS5() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe1b00260); // rors r0, #4
        vm.m_registers[0] = 0x00080000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00008000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testMOVS() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe1b00221); // movs r0, r1, lsr #4

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00010000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00001000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe3800d7d); // ORR r0, r0, #8000
        vm.m_registers[0] = 0x00000001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00001f41); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR2() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe3800002); // ORR r0, r0, #2 ; 0x2
        vm.m_registers[0] = 0xF000000F;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xF000000F);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR3() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe3800002); // ORR r0, r0, #2 ; 0x2
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[0] = 0x800F0001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x800F0003);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR4() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe3810602); // ORR r0, r1, #2097152
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x800f0001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x802f0001);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR() {
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

    void testSTR() {

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

    void testPUSH() {
        // uint8_t ram[128] = {0};

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

    void testPUSHPOP() {
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

    void testADD3() {

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

    void testADDS() {

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

    void testADD4() {

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

    void testADD5() {

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

    void testADD6() {

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

    void testLDRB() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5f10002);     // ldrb r0, [r1, #2]!
        seti(vm.m_ram + 4, 0xe5e1000c); // strb r0, [r1, #12]!

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

    void testLDRB2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5f10001);     // ldrb r0, [r1, #1]!
        seti(vm.m_ram + 4, 0xe5e1000b); // strb r0, [r1, #11]!

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

    void testLDRB3() {
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

    void testLDRB4() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5f10003);     // ldrb r0, [r1, #3]!
        seti(vm.m_ram + 4, 0xe5e1000d); // strb r0, [r1, #13]!

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

    void testLDRB5() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe4110004);     // ldr r0, [r1], #-4
        seti(vm.m_ram + 4, 0xe4010018); // str r0, [r1], #-24

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

    void testLDR2() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5910000); // ldr r0, [r1]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        seti(vm.m_ram + 0x100, 0x11223344);

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x11223344);
    }

    void testMUL1() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0020190); // mul r2,r0,r1
        vm.m_registers[0] = 0xffffff0f;
        vm.m_registers[1] = 0x00000002;

        vm.run(1);
        QVERIFY(vm.m_registers[2] == 0xfffffe1e);
    }

    void testMLA() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0203291); // mla r0,r1,r2,r3
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000012;
        vm.m_registers[2] = 0x0000007f;
        vm.m_registers[3] = 0x00000024;

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x00000912);
        QVERIFY(vm.m_registers[1] == 0x00000012);
        QVERIFY(vm.m_registers[2] == 0x0000007f);
        QVERIFY(vm.m_registers[3] == 0x00000024);
    }

    void testMLA2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0203291); // mla r0,r1,r2,r3
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0xFFFFFFF6;
        vm.m_registers[2] = 0x00000014;
        vm.m_registers[3] = 0x00000003;

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0xffffff3b);
        QVERIFY(vm.m_registers[1] == 0xFFFFFFF6);
        QVERIFY(vm.m_registers[2] == 0x00000014);
        QVERIFY(vm.m_registers[3] == 0x00000003);
    }

    void testMLA3() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0303291); // mlas r0,r1,r2,r3
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x7FFFEFF6;
        vm.m_registers[2] = 0x70070014;
        vm.m_registers[3] = 0x00000003;

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x2fb8bf3b);
        QVERIFY(vm.m_registers[1] == 0x7FFFEFF6);
        QVERIFY(vm.m_registers[2] == 0x70070014);
        QVERIFY(vm.m_registers[3] == 0x00000003);

    }

    void testMLA4() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0303291); // mlas r0,r1,r2,r3
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x7FFFEFF6;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000000;

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x7FFFEFF6);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x40000000);
    }

    void testMLA5() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0303291); // mlas r0,r1,r2,r3
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0xfffffff6;
        vm.m_registers[2] = 0x00000002;
        vm.m_registers[3] = 0x00000000;

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0xffffffec);
        QVERIFY(vm.m_registers[1] == 0xfffffff6);
        QVERIFY(vm.m_registers[2] == 0x00000002);
        QVERIFY(vm.m_registers[3] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x80000000);
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testLDR3() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5b10000);     // ldr r0, [r1, #0]!
        seti(vm.m_ram + 4, 0xe5a1000c); // str r0, [r1, #12]!

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

    void testLDR4() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5310004);     // ldr r0, [r1, #-4]!
        seti(vm.m_ram + 4, 0xe5210018); // str r0, [r1, #-24]!

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

    void testLDR5() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe4910004);     // ldr r0, [r1], #4
        seti(vm.m_ram + 4, 0xe4010018); // str r0, [r1], #-24

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

    void testLDR6() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe4910000);     // ldr r0, [r1], #0
        seti(vm.m_ram + 4, 0xe4010001); // str r0, [r1], #-24! -> str r0, [r1], #-1

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

    void testLDR7() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe491f000); // ldr r15, [r1], #0

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

    void testLDR8() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe6910002); // ldr r0, [r1], r2

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

    void testLDR9() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe7b10002); // ldr r0, [r1, r2]!

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

    void testLDR10() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe7f10002); // ldrb r0, [r1, r2]!

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

    void testLDR11() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe7e10002); // strb r0, [r1, r2]!

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

    void testLDR12() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe58f0000); // str r0, [r15]

        vm.m_registers[0] = 0x99aabbcc;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbcc);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x99aabbcc);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR13() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5cf0000); // strb r0, [r15]
        seti(vm.m_ram + 4, 0x11223344);
        seti(vm.m_ram + 8, 0x55667788);
        vm.m_registers[0] = 0x99aabbcc;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbcc);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x556677cc);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR14() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe5c10000); // strb r0, [r1]
        seti(vm.m_ram + 4, 0x11223344);
        seti(vm.m_ram + 8, 0x55667788);
        vm.m_registers[0] = 0x99aabbee;
        vm.m_registers[1] = 0x00000100;
        seti(vm.m_ram + 0x100, 0xaabbccdd);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbee);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x4) == 0x11223344);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x8) == 0x55667788);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0xaabbccee);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR15() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe7f10002); // ldrb r0, [r1]

        vm.m_registers[0] = 0xdeadbeaf;
        vm.m_registers[1] = 0x00000100;
        seti(vm.m_ram + 0x100, 0x11223344);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000044);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDMFD() {
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

    void testLDMFA() {
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

    void testSTMFA() {
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

    void testSTMED() {
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

    void testSTMEA() {

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

    void testSTMFA2() {
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

    void testSTMFA3() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe8a30038); // STM r3!, {r3-r5}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0x00000100;
        vm.m_registers[4]  = 0x00004444;
        vm.m_registers[5]  = 0x00055555;
        vm.m_registers[13] = 0x00000110;

        vm.run(1);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x00000100);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x00004444);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x00055555);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTMFA4() {
        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe8a30030); // STM r3!, {r4-r5}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0x00000100;
        vm.m_registers[4]  = 0x00004444;
        vm.m_registers[5]  = 0x00055555;
        vm.m_registers[13] = 0x00000110;

        vm.run(1);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x00004444);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x00055555);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x00000000);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0x00000108);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTR2() {

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

    void testSTM1() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe8a30034); // stm	r3!,{r2,r4,r5}

        vm.m_registers[2] = 0x00000022;
        vm.m_registers[3] = 0x00000100;
        vm.m_registers[4] = 0x00004444;
        vm.m_registers[5] = 0x00055555;
        vm.m_registers[6] = 0x00666666;

        vm.run(1);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x00000022);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x00004444);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x00055555);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000022);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[6] == 0x00666666);
    }

    void testSTM2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe8a30038); // stm	r3!,{r3,r4,r5}

        vm.m_registers[2] = 0x00000022;
        vm.m_registers[3] = 0x00000100;
        vm.m_registers[4] = 0x00004444;
        vm.m_registers[5] = 0x00055555;
        vm.m_registers[6] = 0x00666666;

        vm.run(1);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x100) == 0x00000100);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x104) == 0x00004444);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x108) == 0x00055555);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000022);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[6] == 0x00666666);
    }

    void testCONDPM() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0902001);     // adds r2, r0, r1
        seti(vm.m_ram + 4, 0x50823001); // addpl r3, r2, r1
        vm.m_registers[0] = 0xFFFFFF0F;
        vm.m_registers[1] = 0x00000002;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0xDEADBEAF;
        vm.m_cpsr         = 0x00000000;

        vm.run(2);

        QVERIFY(vm.m_registers[0] == 0xFFFFFF0F);
        QVERIFY(vm.m_registers[1] == 0x00000002);
        QVERIFY(vm.m_registers[2] == 0xffffff11);
        QVERIFY(vm.m_registers[3] == 0xDEADBEAF);
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testCONDVC() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram, 0xe0902001);     // adds r2, r0, r1
        seti(vm.m_ram + 4, 0x70823001); // addvc r3, r2, r1
        vm.m_registers[0] = 0xFFFFFF0F;
        vm.m_registers[1] = 0x000F0000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0xDEADBEAF;
        vm.m_cpsr         = 0x00000000;

        vm.run(2);

        QVERIFY(vm.m_registers[0] == 0xFFFFFF0F);
        QVERIFY(vm.m_registers[1] == 0x000F0000);
        QVERIFY(vm.m_registers[2] == 0x000eff0f);
        QVERIFY(vm.m_registers[3] == 0x001dff0f);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testCONDCC() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1540002);  // cmp   r4,r2
        seti(vm.m_ram + 4, 0x33a0600f);  // movcc r6,#15
        seti(vm.m_ram + 8, 0xe1540002);  // cmp   r4,r2
        seti(vm.m_ram + 12, 0x33a0600f); // movcc r6,#15
        vm.m_registers[2] = 0x0000000A;
        vm.m_registers[4] = 0x0000000F;
        vm.m_registers[6] = 0x00004444;
        vm.m_cpsr         = 0x00000000;

        vm.run(2);

        QVERIFY(vm.m_registers[2] == 0x0000000A);
        QVERIFY(vm.m_registers[4] == 0x0000000F);
        QVERIFY(vm.m_registers[6] == 0x00004444);
        QVERIFY(vm.m_cpsr == 0x20000000);

        vm.m_registers[2] = 0x00000014;
        vm.m_registers[4] = 0x00000010;
        vm.m_registers[6] = 0x00004444;
        vm.m_cpsr         = 0x00000000;

        vm.run(2);

        QVERIFY(vm.m_registers[2] == 0x00000014);
        QVERIFY(vm.m_registers[4] == 0x00000010);
        QVERIFY(vm.m_registers[6] == 0x0000000F);
        QVERIFY(vm.m_cpsr == 0x80000000);

    }

    void testHALF() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1d030f2);  // ldrsh   r3, [r0, #2]
        seti(vm.m_ram + 4, 0xe1d140fe);  // ldrsh   r4, [r1, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000000;
        vm.m_registers[4] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        seti(vm.m_ram + 12, 0x9a8b7c6d);

        vm.run(2);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0xffffe1d0);
        QVERIFY(vm.m_registers[4] == 0xffff9a8b);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testHALF2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1d330fe);  // ldrsh   r3, [r3, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000048;
        vm.m_registers[4] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        seti(vm.m_ram + 0x54, 0x1a2b3c4d);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0x00001a2b);
        QVERIFY(vm.m_registers[4] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testHALF3() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1d120de);  // ldrsb   r2, [r1, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        seti(vm.m_ram + 12, 0x9a8b7c6d);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0xffffff8b);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1c330be);  // strh   r3, [r3, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000012;
        vm.m_cpsr         = 0x00000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0x00000012);
        QVERIFY(*(uint32_t *)(vm.m_ram + 32) == 0x00000012);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1c330be);  // strh   r3, [r3, #14]
        vm.m_registers[3] = 0x00000014;
        vm.m_cpsr         = 0x00000000;
        seti(vm.m_ram + 0x20, 0xa1b2c3d4);

        vm.run(1);

        QVERIFY(vm.m_registers[3] == 0x00000014);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x20) == 0x0014c3d4);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH3() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1c320be);  // strh r2, [r3, #14]
        vm.m_registers[2] = 0x00000001;
        vm.m_registers[3] = 0x00000048;
        seti(vm.m_ram + 0x54, 0xf3d7e5b9);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000001);
        QVERIFY(vm.m_registers[3] == 0x00000048);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x54) == 0x0001e5b9);
    }

    void testSTRH4() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1c320be);  // strh r2, [r3, #14]
        vm.m_registers[2] = 0x00000001;
        vm.m_registers[3] = 0x00000048;
        seti(vm.m_ram + 0x54, 0x00000009);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000001);
        QVERIFY(vm.m_registers[3] == 0x00000048);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x54) == 0x00010009);
    }

    void testSTRB() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe5c10000);  // strb r0, [r1]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        seti(vm.m_ram + 0x10, 0xeeeeeeee);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10) == 0xeeeeeed4);
    }

    void testSTRB2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe5c10002);  // strb r0, [r1, #2]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        seti(vm.m_ram + 0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10) == 0x11d41111);
    }

    void testSTRB3() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe5c10003);  // strb r0, [r1, #3]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        seti(vm.m_ram + 0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10) == 0xd4111111);
    }

    void testSTRB4() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe5c10001);  // strb r0, [r1, #1]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        seti(vm.m_ram + 0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(*(uint32_t *)(vm.m_ram + 0x10) == 0x1111d411);
    }

    void testADCS() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe0b11553);  // adcs    r1, r1, r3, asr r5
        vm.m_registers[1] = 0x00148fbe;
        vm.m_registers[3] = 0xffe80000;
        vm.m_registers[5] = 0x00000000;

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0xfffc8fbe);
        QVERIFY(vm.m_registers[3] == 0xffe80000);
        QVERIFY(vm.m_registers[5] == 0x00000000);

    }

    void testANDS() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe2100003);  // ands r0, r0, #3
        vm.m_registers[0] = 0x00055820;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testTEQ() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe1340005);  // teq r4, r5
        vm.m_registers[4] = 0x7fe91f7c;
        vm.m_registers[5] = 0x7ff00000;
        vm.m_cpsr = 0x20000000;

        vm.run(1);

        QVERIFY(vm.m_registers[4] == 0x7fe91f7c);
        QVERIFY(vm.m_registers[5] == 0x7ff00000);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testTEST() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe3110102);  // tst     r1, #-2147483648        ; 0x80000000
        vm.m_registers[1] = 0x3ff48fbe;
        vm.m_cpsr = 0x80000000;

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x3ff48fbe);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testRSC() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe2e33000);  // rsc r3, r3, #0
        vm.m_registers[3] = 0x00180000;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[3] == 0xffe80000);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testUMULL() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe08ec290);  // umull   ip, lr, r0, r2
        vm.m_registers[0] = 0x49ba5e38;
        vm.m_registers[2] = 0x636f4361;
        vm.m_registers[12] = 0x000007ff;
        vm.m_registers[14] = 0x0003a910;
        vm.m_cpsr = 0x20000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x49ba5e38);
        QVERIFY(vm.m_registers[2] == 0x636f4361);
        QVERIFY(vm.m_registers[12] == 0x388e5b38);
        QVERIFY(vm.m_registers[14] == 0x1ca31da6);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testRSBS() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe2722000);  // rsbs r2,r2,#0
        vm.m_registers[2] = 0xe2f3f618;
        vm.m_cpsr = 0xa0000000;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x1d0c09e8);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testTEST2() {

        VirtualMachine vm(&vmProperties, this);
        vm.init();

        seti(vm.m_ram + 0, 0xe3110601);  // tst     r1, #1048576    ; 0x100000
        vm.m_registers[1] = 0x00000000;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x40000000);
    }

    //#define MEMSIZE 128
#define MEMSIZE 300
#define UARTPOS 0x05000000
    //#define UARTPOS 0x101f1000

    void testProgramHello() {

        QString binPath(getBinPath());
        QString data;

        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/test_compile/hello.bin";
        unsigned char *mem     = nullptr;
        uint8_t *uart          = nullptr;
        bool           running = true;

        VirtualMachine vm(&vmProperties, this);
        mem = vm.init();
        vm.load();
        uart = mem + UARTPOS;

        while (running) {

            switch (vm.run()) {

            case VirtualMachine::Resume:
                break;

            case VirtualMachine::Stop:
                running = false;
                break;

            case VirtualMachine::Suspend:
                data += (char)*uart;

            default:
                break;
            }
        }

        QVERIFY(data == "hello world\n");
    }

    void testProgramPrimeN() {

        QString binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/test_compile/primen.bin";
        uint8_t *mem           = nullptr;
        uint8_t *uart          = nullptr;
        bool     running       = true;

        VirtualMachine vm(&vmProperties, this);
        mem = vm.init();
        if (vm.load()) {

            uart = mem + UARTPOS;

            while (running) {

                switch (vm.run()) {

                case VirtualMachine::Stop:
                    running = false;
                    break;

                case VirtualMachine::Suspend:
                    std::cout << (char)*uart;
                    break;

                default:
                    break;
                }
            }
        }

        QVERIFY(*(uint32_t *)(uart) == 2999);
    }

    void testProgramFloat() {

        QString binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/test_compile/float.bin";
        uint8_t *mem           = nullptr;
        uint8_t *uart          = nullptr;
        bool     running       = true;

        VirtualMachine vm(&vmProperties, this);
        mem = vm.init();
        if (vm.load()) {

            uart = mem + UARTPOS;

            while (running) {

                switch (vm.run()) {

                case VirtualMachine::Resume:
                    break;

                case VirtualMachine::Stop:
                    running = false;
                    break;

                case VirtualMachine::Suspend:
                    break;


                default:
                    break;
                }
            }
        }

        QVERIFY(*(uint32_t *)(uart) == 0x075bcd15);
        QVERIFY(*(uint32_t *)(uart + 4) == 0x449a522c);
        QVERIFY(*(uint64_t *)(uart + 8) == 0x40fe240ca03fe924);
        QVERIFY(*(uint32_t *)(uart + 16) == 0x075bae50);
        QVERIFY(*(uint8_t *)(uart + 20) == 0xDF);
        QVERIFY(*(uint64_t *)(uart + 21) == 0x2bdb9cf8d41aef);
    }

    void testProgramPrintf() {

        QString binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/test_compile/printf.bin";
        uint8_t *mem           = nullptr;
        uint8_t *uart          = nullptr;
        bool     running       = true;
        QString data;

        VirtualMachine vm(&vmProperties, this);
        mem = vm.init();
        vm.load();
        uart = mem + UARTPOS;

        while (running) {

            switch (vm.run()) {

            case VirtualMachine::Resume:
                break;

            case VirtualMachine::Stop:
                running = false;
                break;

            case VirtualMachine::Suspend:
                data += (char)*uart;
                std::cout << (char)*uart;
                break;

            default:
                break;
            }
        }

        QVERIFY(data == "[printf] 2 c hello 41.123000\n[cout] 2 c hello 41.123\n");
    }

    void testProgramBench() {

        QString binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/test_compile/bench.bin";
        uint8_t *mem           = nullptr;
        uint8_t *uart          = nullptr;
        bool     running       = true;
        QString data;

        VirtualMachine vm(&vmProperties, this);
        mem = vm.init();
        uart = mem + UARTPOS;

        if(vm.load()) {

            while (running) {

                switch (vm.run()) {

                case VirtualMachine::Resume:
                    break;

                case VirtualMachine::Stop:
                    running = false;
                    break;

                case VirtualMachine::Suspend:
                    data += (char)*uart;
                    std::cout << (char)*uart;
                    break;

                default:
                    break;
                }
            }
        }
        QVERIFY(data == "4 11337 64624 74501 98671 149983 166011 167964 230031 276464 290271 343718 353417 378247 433098 443959 447113 449806 456279");
    }
};
QTEST_MAIN(TestVm)
#include "test.moc"
