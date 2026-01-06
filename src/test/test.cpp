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
#include "../armv4vm.hpp"

namespace armv4vm {

struct VmProperties vmProperties;

       //#define MEMSIZE 128
#define MEMSIZE 300
#define UARTPOS 0x01000000;
                                  //#define UARTPOS 0x101f1000


class TestVm : public QObject {
    Q_OBJECT

public:
    TestVm() {
        vmProperties.m_memsize = 512;
        vmProperties.m_coproModel = "vfpv2";
    }

private slots:

    void testMOV() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe3a0002d; // mov r0, #45

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 45);
        QVERIFY(vm.m_cpsr == 0);
    }

    void testADD() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0900001; // add r0, r0, r1

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0x1;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testADD2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0900001; // add r0, r0, r1

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0x4;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 3);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testSUBS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0510002; // subs r0, r1, r2

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0x4;
        vm.m_registers[2] = 0x8;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xfffffffc);
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testSUBS2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0510002; // subs    r0, r1, r2

        vm.m_registers[0] = 0xFFFFFFFF;
        vm.m_registers[1] = 0xFFFFFFFF; // -1
        vm.m_registers[2] = 0x1;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xfffffffe); // -2
        QVERIFY(vm.m_cpsr == 0xa0000000);
    }

    void testSUBS3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0510002; // subs    r0, r1, r2
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0xFFFFFFFF; // -1
        vm.m_registers[2] = 0xFFFFFFFF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000); // -2
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testLSLS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00211;
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0xFFFFFFFF; // -1
        vm.m_registers[2] = 0xFFFFFFFF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000); // -2
        QVERIFY(vm.m_cpsr == 0x40000000);
    }

    void testLSLS2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00001;
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); //-2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testLSRS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00231; // lsrs r0, r1, r2
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1
        vm.m_registers[2] = 0x100;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testASRS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1
        vm.m_registers[2] = 0x10;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xffff8012); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testASRS2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x8012FFFF; // -1
        vm.m_registers[2] = 0x10;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xffff8012); // -2
        QVERIFY(vm.m_cpsr == 0xa0000000);
    }

    void testASRS3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x0012FFFF; // -1
        vm.m_registers[2] = 0x10;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000012); // -2
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testASRS4() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x8012FFFF; // -1
        vm.m_registers[2] = 0x00FF;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xFFFFFFFF); // -2
        QVERIFY(vm.m_cpsr == 0xa0000000);
    }

    void testASRS5() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x1012FFFF; // -1
        vm.m_registers[2] = 0x00FF;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0); // -2
        QVERIFY(vm.m_cpsr == 0x40000000);
    }

    void testASRS6() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, #0
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1
        vm.m_registers[2] = 0;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testASRS7() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00001; // asrs r0, r1, #0
        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x801200EF; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x801200EF); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testRORS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00271; // rors r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x00000010; // -1
        vm.m_registers[2] = 0x000000FF;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000020); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testRORS2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00271; // rors r0, r1, r2

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x00000010; // -1
        vm.m_registers[2] = 0x00000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000010); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testRORS3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00001; // rors r0, r1, #0

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x00000010; // -1
        vm.m_registers[2] = 0x00000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000010); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testRRXS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00061; // rrxs r0, r1

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x00000010; // -1
        vm.m_cpsr         = 0x20000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x80000008); // -2
        QVERIFY(vm.m_cpsr == 0x80000000);
    }

    void testRRXS2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00061; // rrxs r0, r1

        vm.m_registers[0] = 0xABABABAB;
        vm.m_registers[1] = 0x00000010; // -1
        vm.m_cpsr         = 0xD0000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000008); // -2
        QVERIFY(vm.m_cpsr == 0x10000000);
    }

    void testRORS4() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00170; // rors r0, r0, r1

        vm.m_registers[0] = 0x00000008;
        vm.m_registers[1] = 0x00000010; // -1

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00080000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testRORS5() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00260; // rors r0, #4
        vm.m_registers[0] = 0x00080000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00008000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testMOVS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe1b00221; // movs r0, r1, lsr #4

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00010000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00001000); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe3800d7d; // ORR r0, r0, #8000
        vm.m_registers[0] = 0x00000001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00001f41); // -2
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe3800002; // ORR r0, r0, #2 ; 0x2
        vm.m_registers[0] = 0xF000000F;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xF000000F);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe3800002; // ORR r0, r0, #2 ; 0x2
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[0] = 0x800F0001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x800F0003);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testORR4() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe3810602; // ORR r0, r1, #2097152
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x800f0001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x802f0001);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5910004; // ldr  r0, [r1, #4]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x0000000c;
        vm.m_ram.writePointer<uint32_t>(0xc + 0x4, 0x12345678);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x12345678);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTR() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5810004; // str     r0, [r1, #4]

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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe92d0007; // push     {r0, r1, r2}
        vm.m_registers[0]  = 0x00000001;
        vm.m_registers[1]  = 0x00000022;
        vm.m_registers[2]  = 0x00000333;
        vm.m_registers[13] = 0x50;
        vm.run(1);

        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 12) == 0x00000001);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 8) == 0x00000022);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 4) == 0x00000333);
        QVERIFY(vm.m_registers[13] == 0x44);

        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testPUSHPOP() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe92d001f; // push {r0, r1, r2, r3, r4}
        vm.m_ram.writePointer<uint32_t>(4) = 0xe8bd03e0; // pop {r5, r6, r7, r8, r9}

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

        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 20) == 0x00000001);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 16) == 0x00000022);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 12) == 0x00000333);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 8) == 0x00004444);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x50 - 4) == 0x00055555);
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0810002; // add r0, r1, r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000444;
        vm.m_registers[2] = 0x00001000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00001444);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testADDS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0910002; // adds r0, r1, r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x7FFFFFFF;
        vm.m_registers[2] = 0x00000001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x80000000);
        QVERIFY(vm.m_cpsr == 0x90000000);
    }

    void testADD4() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0910002; // add r0, r1, r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x40000000;
        vm.m_registers[2] = 0x40000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x80000000);
        QVERIFY(vm.m_cpsr == 0x90000000);
    }

    void testADD5() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0810002; // add r0, r1, r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x40000000;
        vm.m_registers[2] = 0x40000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x80000000);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testADD6() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0910002; // add r0, r1, r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x7FFFFFFF;
        vm.m_registers[2] = 0x00000001;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x80000000);
        QVERIFY(vm.m_cpsr == 0x90000000);
    }

    void testLDRB() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5f10002;     // ldrb r0, [r1, #2]!
        vm.m_ram.writePointer<uint32_t>(4, 0xe5e1000c); // strb r0, [r1, #12]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000022);
        QVERIFY(vm.m_registers[1] == 0x00000102);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x0000010e);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x00220000);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDRB2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5f10001;     // ldrb r0, [r1, #1]!
        vm.m_ram.writePointer<uint32_t>(4, 0xe5e1000b); // strb r0, [r1, #11]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000033);
        QVERIFY(vm.m_registers[1] == 0x00000101);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x0000010c);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x00000033);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDRB3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5d10000; // ldrb r0, [r1]
        vm.m_ram.writePointer<uint32_t>(4, 0xe5d20000);
        vm.m_ram.writePointer<uint32_t>(8, 0xe5d30000);
        vm.m_ram.writePointer<uint32_t>(12, 0xe5d40000);

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_registers[2] = 0x00000101;
        vm.m_registers[3] = 0x00000102;
        vm.m_registers[4] = 0x00000103;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5f10003;     // ldrb r0, [r1, #3]!
        vm.m_ram.writePointer<uint32_t>(4, 0xe5e1000d); // strb r0, [r1, #13]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000011);
        QVERIFY(vm.m_registers[1] == 0x00000103);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x00000110);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x110) == 0x00000011);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDRB5() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe4110004;     // ldr r0, [r1], #-4
        vm.m_ram.writePointer<uint32_t>(4, 0xe4010018); // str r0, [r1], #-24

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x000000fc);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x000000e4);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0xfc) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5910000; // ldr r0, [r1]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x11223344);
    }

    void testMUL1() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0020190; // mul r2,r0,r1
        vm.m_registers[0] = 0xffffff0f;
        vm.m_registers[1] = 0x00000002;

        vm.run(1);
        QVERIFY(vm.m_registers[2] == 0xfffffe1e);
    }

    void testMLA() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0203291; // mla r0,r1,r2,r3
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0203291; // mla r0,r1,r2,r3
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5b10000;     // ldr r0, [r1, #0]!
        vm.m_ram.writePointer<uint32_t>(4, 0xe5a1000c); // str r0, [r1, #12]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x0000010c);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR4() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5310004;     // ldr r0, [r1, #-4]!
        vm.m_ram.writePointer<uint32_t>(4, 0xe5210018); // str r0, [r1, #-24]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000104;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x000000e8);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0xe8) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR5() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe4910004;     // ldr r0, [r1], #4
        vm.m_ram.writePointer<uint32_t>(4, 0xe4010018); // str r0, [r1], #-24

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000104);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x000000ec);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR6() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe4910000;     // ldr r0, [r1], #0
        vm.m_ram.writePointer<uint32_t>(4, 0xe4010001); // str r0, [r1], #-24! -> str r0, [r1], #-1

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(vm.m_cpsr == 0x00000000);

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x000000ff);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR7() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe491f000; // ldr r15, [r1], #0

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(vm.m_registers[15] == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR8() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe6910002; // ldr r0, [r1], r2

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_registers[2] = 0x00000004;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000104);
        QVERIFY(vm.m_registers[2] == 0x00000004);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR9() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe7b10002; // ldr r0, [r1, r2]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_registers[2] = 0x00000004;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x55667788);
        QVERIFY(vm.m_registers[1] == 0x00000104);
        QVERIFY(vm.m_registers[2] == 0x00000004);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR10() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe7f10002; // ldrb r0, [r1, r2]!

        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000100;
        vm.m_registers[2] = 0x00000004;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000088);
        QVERIFY(vm.m_registers[1] == 0x00000104);
        QVERIFY(vm.m_registers[2] == 0x00000004);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR11() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe7e10002; // strb r0, [r1, r2]!

        vm.m_registers[0] = 0x99aabbcc;
        vm.m_registers[1] = 0x00000100;
        vm.m_registers[2] = 0x00000004;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbcc);
        QVERIFY(vm.m_registers[1] == 0x00000104);
        QVERIFY(vm.m_registers[2] == 0x00000004);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x556677cc);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR12() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe58f0000; // str r0, [r15]

        vm.m_registers[0] = 0x99aabbcc;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x8) == 0x99aabbcc);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR13() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5cf0000; // strb r0, [r15]
        vm.m_ram.writePointer<uint32_t>(4, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(8, 0x55667788);
        vm.m_registers[0] = 0x99aabbcc;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x8) == 0x556677cc);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR14() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5c10000; // strb r0, [r1]
        vm.m_ram.writePointer<uint32_t>(4, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(8, 0x55667788);
        vm.m_registers[0] = 0x99aabbee;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0xaabbccdd);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x99aabbee);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x4) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x8) == 0x55667788);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xaabbccee);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDR15() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe7f10002; // ldrb r0, [r1]

        vm.m_registers[0] = 0xdeadbeaf;
        vm.m_registers[1] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000044);
        QVERIFY(vm.m_registers[1] == 0x00000100);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testLDMFD() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8bd000f; // LDMFD r13!, {r0-r3}
        vm.m_registers[0]  = 0x00000000;
        vm.m_registers[1]  = 0x00000000;
        vm.m_registers[2]  = 0x00000000;
        vm.m_registers[3]  = 0x00000000;
        vm.m_registers[13] = 0x00000100;

        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);
        vm.m_ram.writePointer<uint32_t>(0x108, 0x99aabbcc);
        vm.m_ram.writePointer<uint32_t>(0x10c, 0xddee00ff);

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testLDMFA() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe83d000f; // LDMFA r13!, {r0-r3}

        vm.m_registers[0]  = 0x00000000;
        vm.m_registers[1]  = 0x00000000;
        vm.m_registers[2]  = 0x00000000;
        vm.m_registers[3]  = 0x00000000;
        vm.m_registers[13] = 0x0000010c;

        vm.m_ram.writePointer<uint32_t>(0x100, 0x11223344);
        vm.m_ram.writePointer<uint32_t>(0x104, 0x55667788);
        vm.m_ram.writePointer<uint32_t>(0x108, 0x99aabbcc);
        vm.m_ram.writePointer<uint32_t>(0x10c, 0xddee00ff);

        vm.run(1);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x000000fc);
    }

    void testSTMFA() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe92d000f; // STMFA r13!, {r0-r3}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0xddee00ff;
        vm.m_registers[13] = 0x00000110;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x00000100);
    }

    void testSTMED() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe82d000f; // STMED r13!, {r0-r3}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0xddee00ff;
        vm.m_registers[13] = 0x00000100;

        vm.m_ram.writePointer<uint32_t>(0xf4) = 0xABCDEF09;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0xf4) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0xf8) == 0x55667788);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0xfc) == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x000000f0);
    }

    void testSTMEA() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8ad000f; // STMEA r13!, {r0-r3}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0xddee00ff;
        vm.m_registers[13] = 0x00000100;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTMFA2() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe9ad000f; // STMFA r13!, {r0-r3}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0xddee00ff;
        vm.m_registers[13] = 0x00000100;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x11223344);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x55667788);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x99aabbcc);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x110) == 0xddee00ff);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTMFA3() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8a30038; // STM r3!, {r3-r5}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0x00000100;
        vm.m_registers[4]  = 0x00004444;
        vm.m_registers[5]  = 0x00055555;
        vm.m_registers[13] = 0x00000110;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x00000100);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTMFA4() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8a30030; // STM r3!, {r4-r5}

        vm.m_registers[0]  = 0x11223344;
        vm.m_registers[1]  = 0x55667788;
        vm.m_registers[2]  = 0x99aabbcc;
        vm.m_registers[3]  = 0x00000100;
        vm.m_registers[4]  = 0x00004444;
        vm.m_registers[5]  = 0x00055555;
        vm.m_registers[13] = 0x00000110;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x00004444);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x00055555);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x00000000);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x55667788);
        QVERIFY(vm.m_registers[2] == 0x99aabbcc);
        QVERIFY(vm.m_registers[3] == 0x00000108);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[13] == 0x00000110);
    }

    void testSTR2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe5a10004; // str r0, [r1, #4]!
        vm.m_registers[0] = 0x11223344;
        vm.m_registers[1] = 0x00000080;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x84) == 0x11223344);
        QVERIFY(vm.m_registers[0] == 0x11223344);
        QVERIFY(vm.m_registers[1] == 0x00000084);
    }

    void testSTM1() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8a30034; // stm	r3!,{r2,r4,r5}

        vm.m_registers[2] = 0x00000022;
        vm.m_registers[3] = 0x00000100;
        vm.m_registers[4] = 0x00004444;
        vm.m_registers[5] = 0x00055555;
        vm.m_registers[6] = 0x00666666;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x00000022);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000022);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[6] == 0x00666666);
    }

    void testSTM2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe8a30038; // stm	r3!,{r3,r4,r5}

        vm.m_registers[2] = 0x00000022;
        vm.m_registers[3] = 0x00000100;
        vm.m_registers[4] = 0x00004444;
        vm.m_registers[5] = 0x00055555;
        vm.m_registers[6] = 0x00666666;

        vm.run(1);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0x00000100);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000022);
        QVERIFY(vm.m_registers[3] == 0x0000010c);
        QVERIFY(vm.m_registers[4] == 0x00004444);
        QVERIFY(vm.m_registers[5] == 0x00055555);
        QVERIFY(vm.m_registers[6] == 0x00666666);
    }

    void testCONDPM() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0902001;     // adds r2, r0, r1
        vm.m_ram.writePointer<uint32_t>(4, 0x50823001); // addpl r3, r2, r1
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0) = 0xe0902001;     // adds r2, r0, r1
        vm.m_ram.writePointer<uint32_t>(4, 0x70823001); // addvc r3, r2, r1
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1540002);  // cmp   r4,r2
        vm.m_ram.writePointer<uint32_t>(4, 0x33a0600f);  // movcc r6,#15
        vm.m_ram.writePointer<uint32_t>(8, 0xe1540002);  // cmp   r4,r2
        vm.m_ram.writePointer<uint32_t>(12, 0x33a0600f); // movcc r6,#15
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

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1d030f2);  // ldrsh   r3, [r0, #2]
        vm.m_ram.writePointer<uint32_t>(4, 0xe1d140fe);  // ldrsh   r4, [r1, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000000;
        vm.m_registers[4] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        vm.m_ram.writePointer<uint32_t>(12, 0x9a8b7c6d);

        vm.run(2);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0xffffe1d0);
        QVERIFY(vm.m_registers[4] == 0xffff9a8b);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testHALF2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1d330fe);  // ldrsh   r3, [r3, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_registers[3] = 0x00000048;
        vm.m_registers[4] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        vm.m_ram.writePointer<uint32_t>(0x54, 0x1a2b3c4d);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_registers[3] == 0x00001a2b);
        QVERIFY(vm.m_registers[4] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testHALF3() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1d120de);  // ldrsb   r2, [r1, #14]
        vm.m_registers[0] = 0x00000000;
        vm.m_registers[1] = 0x00000000;
        vm.m_registers[2] = 0x00000000;
        vm.m_cpsr         = 0x00000000;
        vm.m_ram.writePointer<uint32_t>(12, 0x9a8b7c6d);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0x00000000);
        QVERIFY(vm.m_registers[2] == 0xffffff8b);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1c330be);  // strh   r3, [r3, #14]
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
        QVERIFY(vm.m_ram.readPointer<uint32_t>(32) == 0x00000012);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1c330be);  // strh   r3, [r3, #14]
        vm.m_registers[3] = 0x00000014;
        vm.m_cpsr         = 0x00000000;
        vm.m_ram.writePointer<uint32_t>(0x20, 0xa1b2c3d4);

        vm.run(1);

        QVERIFY(vm.m_registers[3] == 0x00000014);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x20) == 0x0014c3d4);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testSTRH3() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1c320be);  // strh r2, [r3, #14]
        vm.m_registers[2] = 0x00000001;
        vm.m_registers[3] = 0x00000048;
        vm.m_ram.writePointer<uint32_t>(0x54, 0xf3d7e5b9);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000001);
        QVERIFY(vm.m_registers[3] == 0x00000048);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x54) == 0x0001e5b9);
    }

    void testSTRH4() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1c320be);  // strh r2, [r3, #14]
        vm.m_registers[2] = 0x00000001;
        vm.m_registers[3] = 0x00000048;
        vm.m_ram.writePointer<uint32_t>(0x54, 0x00000009);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000001);
        QVERIFY(vm.m_registers[3] == 0x00000048);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x54) == 0x00010009);
    }

    void testSTRH5() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0c320b2);  // strh r2, [r3], #2
        vm.m_registers[2] = 0xaabbccdd;
        vm.m_registers[3] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0xaaaaaaaa);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0xaabbccdd);
        QVERIFY(vm.m_registers[3] == 0x00000102);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xaaaaccdd);
    }

    void testSTRH6() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0c320b3);  // strh r2, [r3], #3
        vm.m_registers[2] = 0xaabbccdd;
        vm.m_registers[3] = 0x00000102;
        vm.m_ram.writePointer<uint32_t>(0x100, 0xaaaaaaaa);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0xaabbccdd);
        QVERIFY(vm.m_registers[3] == 0x00000105);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xccddaaaa);
    }

    void testLDRH1() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0d320b2);  // ldrh r2, [r3], #2
        vm.m_registers[2] = 0x11223344;
        vm.m_registers[3] = 0x00000100;
        vm.m_ram.writePointer<uint32_t>(0x100, 0xaabbccdd);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x0000ccdd);
        QVERIFY(vm.m_registers[3] == 0x00000102);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xaabbccdd);
    }

    void testLDRH2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0d320b3);  // ldrh r2, [r3], #3
        vm.m_registers[2] = 0x11223344;
        vm.m_registers[3] = 0x00000102;
        vm.m_ram.writePointer<uint32_t>(0x100, 0xaabbccdd);

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x0000aabb);
        QVERIFY(vm.m_registers[3] == 0x00000105);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x100) == 0xaabbccdd);
    }

    void testSTRB() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe5c10000);  // strb r0, [r1]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        vm.m_ram.writePointer<uint32_t>(0x10, 0xeeeeeeee);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0xeeeeeed4);
    }

    void testSTRB2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe5c10002);  // strb r0, [r1, #2]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        vm.m_ram.writePointer<uint32_t>(0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0x11d41111);
    }

    void testSTRB3() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe5c10003);  // strb r0, [r1, #3]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        vm.m_ram.writePointer<uint32_t>(0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0xd4111111);
    }

    void testSTRB4() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe5c10001);  // strb r0, [r1, #1]
        vm.m_registers[0] = 0xa1b2c3d4;
        vm.m_registers[1] = 0x00000010;
        vm.m_ram.writePointer<uint32_t>(0x10, 0x11111111);

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0xa1b2c3d4);
        QVERIFY(vm.m_registers[1] == 0x00000010);
        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0x1111d411);
    }

    void testADCS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0b11553);  // adcs    r1, r1, r3, asr r5
        vm.m_registers[1] = 0x00148fbe;
        vm.m_registers[3] = 0xffe80000;
        vm.m_registers[5] = 0x00000000;

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0xfffc8fbe);
        QVERIFY(vm.m_registers[3] == 0xffe80000);
        QVERIFY(vm.m_registers[5] == 0x00000000);

    }

    void testANDS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe2100003);  // ands r0, r0, #3
        vm.m_registers[0] = 0x00055820;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testTEQ() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1340005);  // teq r4, r5
        vm.m_registers[4] = 0x7fe91f7c;
        vm.m_registers[5] = 0x7ff00000;
        vm.m_cpsr = 0x20000000;

        vm.run(1);

        QVERIFY(vm.m_registers[4] == 0x7fe91f7c);
        QVERIFY(vm.m_registers[5] == 0x7ff00000);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testTEST() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe3110102);  // tst     r1, #-2147483648        ; 0x80000000
        vm.m_registers[1] = 0x3ff48fbe;
        vm.m_cpsr = 0x80000000;

        vm.run(1);

        QVERIFY(vm.m_registers[1] == 0x3ff48fbe);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testRSC() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe2e33000);  // rsc r3, r3, #0
        vm.m_registers[3] = 0x00180000;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[3] == 0xffe80000);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testUMULL() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe08ec290);  // umull   ip, lr, r0, r2
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

    void testSMULL() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0c23290); // smull   r3, r2, r0, r2
        vm.m_registers[0] = 0x17;
        vm.m_registers[2] = 0xd;
        vm.m_registers[3] = 0x7;
        vm.m_cpsr         = 0x20000000;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x17);
        QVERIFY(vm.m_registers[2] == 0x0);
        QVERIFY(vm.m_registers[3] == 0x12b);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testSMULL2() {

        VirtualMachineUnprotected vm(&vmProperties);

        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe0c23190); // smull   r3, r2, r0, r1
        vm.m_registers[0] = -324;       // fffffebc
        vm.m_registers[1] = 45674233;   // 2b8eef9
        vm.m_registers[2] = 0x7213;
        vm.m_registers[3] = 0x72345;
        vm.m_cpsr         = 0x20000000;

        vm.run(1);

        QVERIFY(static_cast<int32_t>(vm.m_registers[0]) == -324);
        QVERIFY(vm.m_registers[1] == 45674233);
        QVERIFY(vm.m_registers[2] == 0xfffffffc);
        QVERIFY(vm.m_registers[3] == 0x8df18cdc);
        QVERIFY(vm.m_cpsr == 0x20000000);
    }

    void testRSBS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe2722000);  // rsbs r2,r2,#0
        vm.m_registers[2] = 0xe2f3f618;
        vm.m_cpsr = 0xa0000000;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x1d0c09e8);
        QVERIFY(vm.m_cpsr == 0x00000000);
    }

    void testTEST2() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe3110601);  // tst     r1, #1048576    ; 0x100000
        vm.m_registers[1] = 0x00000000;
        vm.m_cpsr = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x00000000);
        QVERIFY(vm.m_cpsr == 0x40000000);
    }

    void testR15() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe320f000); // nop
        vm.m_ram.writePointer<uint32_t>(4, 0xe780f001); // STR    R15, [R0, R1]
        vm.m_registers[0] = 0x0000000a;
        vm.m_registers[1] = 0x00000002;
        vm.m_cpsr         = 0x60000000;

        vm.run(2);

        QVERIFY(vm.m_ram.readPointer<uint32_t>(12) == 0x00000010);
    }

    void testSWP_1() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1020091); // SWP R0, R1, [R2]
        vm.m_ram.writePointer<uint32_t>(0x10, 0xABCDEF01);
        vm.m_registers[0] = 0x0000000a;
        vm.m_registers[1] = 0x11112222;
        vm.m_registers[2] = 0x00000010;
        vm.m_cpsr         = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0x11112222);
        QVERIFY(vm.m_registers[0] == 0xABCDEF01);
        QVERIFY(vm.m_registers[1] == 0x11112222);
        QVERIFY(vm.m_registers[2] == 0x00000010);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testSWPB_1() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xe1420091); // SWPB R0, R1, [R2]
        vm.m_ram.writePointer<uint32_t>(0x10, 0xABCDEF33);
        vm.m_registers[0] = 0x11223344;
        vm.m_registers[1] = 0x11112222;
        vm.m_registers[2] = 0x00000010;
        vm.m_cpsr         = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_ram.readPointer<uint32_t>(0x10) == 0xABCDEF22);
        QVERIFY(vm.m_registers[0] == 0x00000033);
        QVERIFY(vm.m_registers[1] == 0x11112222);
        QVERIFY(vm.m_registers[2] == 0x00000010);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testProgramHello() {

        std::string binPath(getBinPath());
        std::string data;

        vmProperties.m_memsize = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/hello.bin";
        std::byte *mem     = nullptr;
        std::byte *uart    = nullptr;
        bool           running = true;

        VirtualMachineUnprotected vm(&vmProperties);
        mem = vm.init();
        if(vm.load()) {

            uart = mem + UARTPOS;

            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Resume:
                    break;

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
                    data += (char)*uart;

                default:
                    break;
                }
            }
        }
        QVERIFY(data == "hello world\n");
    }

    void testProgramPrimeN() {

        std::string binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/src/test_compile/primen.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;

        VirtualMachineUnprotected vm(&vmProperties);
        mem = vm.init();
        uart = mem + UARTPOS;

        if (vm.load()) {

            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
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

        std::string binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/src/test_compile/float.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;

        VirtualMachineUnprotected vm(&vmProperties);
        //registerCoprocessor<MemoryRaw, Vfpv2>("vfpv2");

        mem = vm.init();
        uart = mem + UARTPOS;

        if (vm.load()) {

            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Resume:
                    break;

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
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

        std::string binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/src/test_compile/printf.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;
        QString data;

        VirtualMachineUnprotected vm(&vmProperties);
        mem = vm.init();
        uart = mem + UARTPOS;

        if(vm.load()) {


            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Resume:
                    break;

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
                    data += (char)*uart;
                    std::cout << (char)*uart;
                    break;

                default:
                    break;
                }
            }
        }

        QVERIFY(data == "[printf] 2 c hello 41.123000\n[cout] 2 c hello 41.123\n");
    }

    void testProgramModulo() {

        std::string binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/src/test_compile/modulo.bin";
        bool     running       = true;

        VirtualMachineUnprotected vm(&vmProperties);

        vm.init();

        if (vm.load()) {

            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Resume:
                    break;

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
                    break;

                default:
                    break;
                }
            }
        }

        QVERIFY(vm.m_registers[0] == 0);
    }

    void testProgramBench() {

        std::string binPath(getBinPath());
        vmProperties.m_memsize = 1024 * 1024 * MEMSIZE;
        vmProperties.m_bin     = binPath + "/src/test_compile/bench.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;
        QString data;

        VirtualMachineUnprotected vm(&vmProperties);
        mem = vm.init();
        uart = mem + UARTPOS;

        if(vm.load()) {

            while (running) {

                switch (vm.run()) {

                case VirtualMachineUnprotected::Interrupt::Resume:
                    break;

                case VirtualMachineUnprotected::Interrupt::Stop:
                    running = false;
                    break;

                case VirtualMachineUnprotected::Interrupt::Suspend:
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

    void testFMSR() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee002a90); // FMSR S1, R2
        vm.m_registers[2] = 0x40490fd0; // 3.14159f
        vm.m_cpsr         = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[1] == std::bit_cast<float>(3.14159f));
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testFMRS() {
        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee102a90); // FMRS R2, S1
        vm.m_ram.writePointer<uint32_t>(0x10, 0xABCDEF01);
        vm.m_coprocessor.m_sRegisters[1] = std::bit_cast<float>(0xaabbccdd);
        vm.m_cpsr         = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0xaabbccdd);
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void atestFMRS() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee123a10); // FMRS	R3, S4
        vm.m_ram.writePointer<uint32_t>(0x10, 0xABCDEF01);
        vm.m_registers[3] = 0xaabbccdd;
        vm.m_coprocessor.m_sRegisters[4] = 3213.4393f;
        vm.m_cpsr         = 0x60000000;

        vm.run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[4] == 3213.4393f);
        QVERIFY(vm.m_registers[3] == std::bit_cast<uint32_t>(3213.4393f));
        QVERIFY(vm.m_cpsr == 0x60000000);
    }

    void testFMDLR() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee041b10); // FMDLR D4, R1
        vm.m_registers[1] = 0xaabbccdd;
        vm.m_coprocessor.m_sRegisters[8] = 0.0f;
        vm.m_coprocessor.m_sRegisters[9] = 0.0f;

        vm.run(1);

        QVERIFY(vm.m_coprocessor.m_sRegisters[8] == std::bit_cast<float>(0xaabbccdd));
        QVERIFY(vm.m_coprocessor.m_sRegisters[9] == 0.0f);
        QVERIFY(vm.m_coprocessor.toSingle<uint32_t>(8) == 0xaabbccdd);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(4) == 0x00000000aabbccdd);
    }

    void testFMRDL() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee161b10); // FMRDL R1, D6
        vm.m_coprocessor.setDoubleRegister<uint64_t>(6, 0x11223344aabbccdd);
        vm.m_registers[1] = 0x99887766;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0xaabbccdd);
        QVERIFY(vm.m_registers[2] == 0x00000000);
    }

    void testFMDHR() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee292b10); // FMDHR D9, R2
        vm.m_coprocessor.setDoubleRegister<uint64_t>(9, 0x55667788FFEEDDCC);
        vm.m_registers[2] = 0x66993322;

        vm.run(1);

        QVERIFY(vm.m_registers[2] == 0x66993322);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(8) == 0x0000000000000000);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(9) == 0x66993322FFEEDDCC);
        QVERIFY(vm.m_coprocessor.toDouble<uint64_t>(10) == 0x0000000000000000);
    }

    void testFMRDH() {

        VirtualMachineUnprotected vm(&vmProperties);
        vm.init();

        vm.m_ram.writePointer<uint32_t>(0, 0xee351b10); // FMRDH R1, D5
        vm.m_coprocessor.setDoubleRegister<uint64_t>(5, 0xdeabcd24841cf651);
        vm.m_registers[1] = 0x99887766;

        vm.run(1);

        QVERIFY(vm.m_registers[0] == 0x00000000);
        QVERIFY(vm.m_registers[1] == 0xdeabcd24);
        QVERIFY(vm.m_registers[2] == 0x00000000);
    }
};
} // namespace armv4vm
QTEST_MAIN(armv4vm::TestVm)
#include "test.moc"
