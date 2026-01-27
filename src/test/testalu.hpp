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

#pragma once

#include <QObject>
#include <QtTest>
#include <iostream>
#include <bit>

#include "config.h"
#include "armv4vm.hpp"

namespace armv4vm {

//struct VmProperties vmProperties;

       //#define MEMSIZE 128
#define MEMSIZE 300
#define UARTPOS 0x01000000;
                            //#define UARTPOS 0x101f1000

template<typename T>
class TestAluInstruction
{
  private:
    //using ProtectedVfp = Vfpv2<T>;
    //using ProtectedAlu = Alu<T, ProtectedVfp>;

    std::unique_ptr<T> m_mem;
    //std::unique_ptr<ProtectedAlu> m_alu;

    using Copro = Vfpv2<T>;

    std::unique_ptr<Alu<T, Copro>> m_alu;
    VmProperties m_vmProperties;


  public:
    TestAluInstruction()
    {
        m_vmProperties.m_memoryHandlerProperties.m_layout.push_back({0, 512, AccessPermission::READ_WRITE});
        m_vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 1_kb;

        m_mem = std::make_unique<T>(m_vmProperties.m_memoryHandlerProperties);
        m_alu = std::make_unique<Alu<T, Copro>>(m_vmProperties.m_aluProperties);

        m_mem->reset();
        m_alu->attach(m_mem.get());
    }

    void testMOV() {

        m_alu->reset();
        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe3a0002d; // mov r0, #45

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 45);
        QVERIFY(m_alu->m_cpsr == 0);
    }

    void testADD() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0900001; // add r0, r0, r1

        m_alu->m_registers[0] = 0xFFFFFFFF;
        m_alu->m_registers[1] = 0x1;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testADD2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0900001; // add r0, r0, r1

        m_alu->m_registers[0] = 0xFFFFFFFF;
        m_alu->m_registers[1] = 0x4;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 3);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testSUBS() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0510002; // subs r0, r1, r2

        m_alu->m_registers[0] = 0xFFFFFFFF;
        m_alu->m_registers[1] = 0x4;
        m_alu->m_registers[2] = 0x8;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xfffffffc);
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testSUBS2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0510002; // subs    r0, r1, r2

        m_alu->m_registers[0] = 0xFFFFFFFF;
        m_alu->m_registers[1] = 0xFFFFFFFF; // -1
        m_alu->m_registers[2] = 0x1;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xfffffffe); // -2
        QVERIFY(m_alu->m_cpsr == 0xa0000000);
    }

    void testSUBS3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0510002; // subs    r0, r1, r2
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0xFFFFFFFF; // -1
        m_alu->m_registers[2] = 0xFFFFFFFF; // -1

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000); // -2
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testLSLS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00211;
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0xFFFFFFFF; // -1
        m_alu->m_registers[2] = 0xFFFFFFFF; // -1

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000); // -2
        QVERIFY(m_alu->m_cpsr == 0x40000000);
    }

    void testLSLS2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00001;
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x801200EF; // -1

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x801200EF); //-2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testLSRS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00231; // lsrs r0, r1, r2
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x801200EF; // -1
        m_alu->m_registers[2] = 0x100;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x801200EF); // -2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testASRS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x801200EF; // -1
        m_alu->m_registers[2] = 0x10;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xffff8012); // -2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testASRS2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x8012FFFF; // -1
        m_alu->m_registers[2] = 0x10;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xffff8012); // -2
        QVERIFY(m_alu->m_cpsr == 0xa0000000);
    }

    void testASRS3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x0012FFFF; // -1
        m_alu->m_registers[2] = 0x10;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000012); // -2
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testASRS4() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x8012FFFF; // -1
        m_alu->m_registers[2] = 0x00FF;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xFFFFFFFF); // -2
        QVERIFY(m_alu->m_cpsr == 0xa0000000);
    }

    void testASRS5() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x1012FFFF; // -1
        m_alu->m_registers[2] = 0x00FF;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0); // -2
        QVERIFY(m_alu->m_cpsr == 0x40000000);
    }

    void testASRS6() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00251; // asrs r0, r1, #0
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x801200EF; // -1
        m_alu->m_registers[2] = 0;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x801200EF); // -2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testASRS7() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00001; // asrs r0, r1, #0
        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x801200EF; // -1

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x801200EF); // -2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testRORS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00271; // rors r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x00000010; // -1
        m_alu->m_registers[2] = 0x000000FF;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000020); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testRORS2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00271; // rors r0, r1, r2

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x00000010; // -1
        m_alu->m_registers[2] = 0x00000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000010); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testRORS3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00001; // rors r0, r1, #0

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x00000010; // -1
        m_alu->m_registers[2] = 0x00000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000010); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testRRXS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00061; // rrxs r0, r1

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x00000010; // -1
        m_alu->m_cpsr         = 0x20000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x80000008); // -2
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testRRXS2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00061; // rrxs r0, r1

        m_alu->m_registers[0] = 0xABABABAB;
        m_alu->m_registers[1] = 0x00000010; // -1
        m_alu->m_cpsr         = 0xD0000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000008); // -2
        QVERIFY(m_alu->m_cpsr == 0x10000000);
    }

    void testRORS4() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00170; // rors r0, r0, r1

        m_alu->m_registers[0] = 0x00000008;
        m_alu->m_registers[1] = 0x00000010; // -1

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00080000); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testRORS5() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00260; // rors r0, #4
        m_alu->m_registers[0] = 0x00080000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00008000); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testMOVS() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe1b00221; // movs r0, r1, lsr #4

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00010000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00001000); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testORR() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe3800d7d; // ORR r0, r0, #8000
        m_alu->m_registers[0] = 0x00000001;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00001f41); // -2
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testORR2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe3800002; // ORR r0, r0, #2 ; 0x2
        m_alu->m_registers[0] = 0xF000000F;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xF000000F);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testORR3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe3800002; // ORR r0, r0, #2 ; 0x2
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[0] = 0x800F0001;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x800F0003);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testORR4() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe3810602; // ORR r0, r1, #2097152
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x800f0001;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x802f0001);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5910004; // ldr  r0, [r1, #4]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x0000000c;
        m_alu->m_mem->template writePointer<uint32_t>(0xc + 0x4, 0x12345678);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x12345678);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testSTR() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5810004; // str     r0, [r1, #4]

        m_alu->m_registers[0] = 0xabcdef02;
        m_alu->m_registers[1] = 0x0000000c;
        m_alu->run(1);

        QVERIFY(m_alu->m_mem->template readPointer<std::byte>(0xc + 4) == 0x02);
        QVERIFY(m_alu->m_mem->template readPointer<std::byte>(0xc + 5) == 0xef);
        QVERIFY(m_alu->m_mem->template readPointer<std::byte>(0xc + 6) == 0xcd);
        QVERIFY(m_alu->m_mem->template readPointer<std::byte>(0xc + 7) == 0xab);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testPUSH() {
        // uint8_t ram[128] = {0};


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe92d0007; // push     {r0, r1, r2}
        m_alu->m_registers[0]  = 0x00000001;
        m_alu->m_registers[1]  = 0x00000022;
        m_alu->m_registers[2]  = 0x00000333;
        m_alu->m_registers[13] = 0x50;
        m_alu->run(1);

        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 12) == 0x00000001);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 8) == 0x00000022);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 4) == 0x00000333);
        QVERIFY(m_alu->m_registers[13] == 0x44);

        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testPUSHPOP() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe92d001f; // push {r0, r1, r2, r3, r4}
        m_alu->m_mem->template writePointer<uint32_t>(4) = 0xe8bd03e0; // pop {r5, r6, r7, r8, r9}

        m_alu->m_registers[0]  = 0x00000001;
        m_alu->m_registers[1]  = 0x00000022;
        m_alu->m_registers[2]  = 0x00000333;
        m_alu->m_registers[3]  = 0x00004444;
        m_alu->m_registers[4]  = 0x00055555;
        m_alu->m_registers[5]  = 0x0;
        m_alu->m_registers[6]  = 0x0;
        m_alu->m_registers[7]  = 0x0;
        m_alu->m_registers[8]  = 0x0;
        m_alu->m_registers[9]  = 0x0;
        m_alu->m_registers[13] = 0x50;

        m_alu->run(1);

        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 20) == 0x00000001);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 16) == 0x00000022);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 12) == 0x00000333);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 8) == 0x00004444);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x50 - 4) == 0x00055555);
        QVERIFY(m_alu->m_registers[13] == 0x3c);

        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[5] == 0x00000001);
        QVERIFY(m_alu->m_registers[6] == 0x00000022);
        QVERIFY(m_alu->m_registers[7] == 0x00000333);
        QVERIFY(m_alu->m_registers[8] == 0x00004444);
        QVERIFY(m_alu->m_registers[9] == 0x00055555);
        QVERIFY(m_alu->m_registers[13] == 0x50);

        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testADD3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0810002; // add r0, r1, r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000444;
        m_alu->m_registers[2] = 0x00001000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00001444);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testADDS() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0910002; // adds r0, r1, r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x7FFFFFFF;
        m_alu->m_registers[2] = 0x00000001;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x80000000);
        QVERIFY(m_alu->m_cpsr == 0x90000000);
    }

    void testADD4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0910002; // add r0, r1, r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x40000000;
        m_alu->m_registers[2] = 0x40000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x80000000);
        QVERIFY(m_alu->m_cpsr == 0x90000000);
    }

    void testADD5() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0810002; // add r0, r1, r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x40000000;
        m_alu->m_registers[2] = 0x40000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x80000000);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testADD6() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0910002; // add r0, r1, r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x7FFFFFFF;
        m_alu->m_registers[2] = 0x00000001;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x80000000);
        QVERIFY(m_alu->m_cpsr == 0x90000000);
    }

    void testLDRB() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5f10002;     // ldrb r0, [r1, #2]!
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5e1000c); // strb r0, [r1, #12]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000022);
        QVERIFY(m_alu->m_registers[1] == 0x00000102);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x0000010e);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x00220000);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDRB2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5f10001;     // ldrb r0, [r1, #1]!
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5e1000b); // strb r0, [r1, #11]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000033);
        QVERIFY(m_alu->m_registers[1] == 0x00000101);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x0000010c);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x00000033);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDRB3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5d10000; // ldrb r0, [r1]
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5d20000);
        m_alu->m_mem->template writePointer<uint32_t>(8, 0xe5d30000);
        m_alu->m_mem->template writePointer<uint32_t>(12, 0xe5d40000);

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_registers[2] = 0x00000101;
        m_alu->m_registers[3] = 0x00000102;
        m_alu->m_registers[4] = 0x00000103;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000044);
        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000033);
        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000022);
        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000011);
    }

    void testLDRB4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5f10003;     // ldrb r0, [r1, #3]!
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5e1000d); // strb r0, [r1, #13]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000011);
        QVERIFY(m_alu->m_registers[1] == 0x00000103);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x00000110);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x110) == 0x00000011);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDRB5() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe4110004;     // ldr r0, [r1], #-4
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe4010018); // str r0, [r1], #-24

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x000000fc);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x000000e4);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0xfc) == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5910000; // ldr r0, [r1]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
    }

    void testMUL1() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0020190; // mul r2,r0,r1
        m_alu->m_registers[0] = 0xffffff0f;
        m_alu->m_registers[1] = 0x00000002;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[2] == 0xfffffe1e);
    }

    void testMLA() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0203291; // mla r0,r1,r2,r3
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000012;
        m_alu->m_registers[2] = 0x0000007f;
        m_alu->m_registers[3] = 0x00000024;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000912);
        QVERIFY(m_alu->m_registers[1] == 0x00000012);
        QVERIFY(m_alu->m_registers[2] == 0x0000007f);
        QVERIFY(m_alu->m_registers[3] == 0x00000024);
    }

    void testMLA2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0203291; // mla r0,r1,r2,r3
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0xFFFFFFF6;
        m_alu->m_registers[2] = 0x00000014;
        m_alu->m_registers[3] = 0x00000003;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0xffffff3b);
        QVERIFY(m_alu->m_registers[1] == 0xFFFFFFF6);
        QVERIFY(m_alu->m_registers[2] == 0x00000014);
        QVERIFY(m_alu->m_registers[3] == 0x00000003);
    }

    void testMLA3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x7FFFEFF6;
        m_alu->m_registers[2] = 0x70070014;
        m_alu->m_registers[3] = 0x00000003;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x2fb8bf3b);
        QVERIFY(m_alu->m_registers[1] == 0x7FFFEFF6);
        QVERIFY(m_alu->m_registers[2] == 0x70070014);
        QVERIFY(m_alu->m_registers[3] == 0x00000003);

    }

    void testMLA4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x7FFFEFF6;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0x00000000;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x7FFFEFF6);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
        QVERIFY(m_alu->m_registers[3] == 0x00000000);
        QVERIFY(m_alu->m_cpsr == 0x40000000);
    }

    void testMLA5() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0303291; // mlas r0,r1,r2,r3
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0xfffffff6;
        m_alu->m_registers[2] = 0x00000002;
        m_alu->m_registers[3] = 0x00000000;

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0xffffffec);
        QVERIFY(m_alu->m_registers[1] == 0xfffffff6);
        QVERIFY(m_alu->m_registers[2] == 0x00000002);
        QVERIFY(m_alu->m_registers[3] == 0x00000000);
        QVERIFY(m_alu->m_cpsr == 0x80000000);
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testLDR3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5b10000;     // ldr r0, [r1, #0]!
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5a1000c); // str r0, [r1, #12]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000100);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x0000010c);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5310004;     // ldr r0, [r1, #-4]!
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe5210018); // str r0, [r1, #-24]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000104;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000100);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x000000e8);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0xe8) == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR5() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe4910004;     // ldr r0, [r1], #4
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe4010018); // str r0, [r1], #-24

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000104);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x000000ec);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR6() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe4910000;     // ldr r0, [r1], #0
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe4010001); // str r0, [r1], #-24! -> str r0, [r1], #-1

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000100);
        QVERIFY(m_alu->m_cpsr == 0x00000000);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x000000ff);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR7() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe491f000; // ldr r15, [r1], #0

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x00000100);
        QVERIFY(m_alu->m_registers[15] == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR8() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe6910002; // ldr r0, [r1], r2

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_registers[2] = 0x00000004;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000104);
        QVERIFY(m_alu->m_registers[2] == 0x00000004);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR9() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe7b10002; // ldr r0, [r1, r2]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_registers[2] = 0x00000004;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x55667788);
        QVERIFY(m_alu->m_registers[1] == 0x00000104);
        QVERIFY(m_alu->m_registers[2] == 0x00000004);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR10() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe7f10002; // ldrb r0, [r1, r2]!

        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_registers[2] = 0x00000004;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000088);
        QVERIFY(m_alu->m_registers[1] == 0x00000104);
        QVERIFY(m_alu->m_registers[2] == 0x00000004);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR11() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe7e10002; // strb r0, [r1, r2]!

        m_alu->m_registers[0] = 0x99aabbcc;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_registers[2] = 0x00000004;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x99aabbcc);
        QVERIFY(m_alu->m_registers[1] == 0x00000104);
        QVERIFY(m_alu->m_registers[2] == 0x00000004);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x556677cc);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR12() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe58f0000; // str r0, [r15]

        m_alu->m_registers[0] = 0x99aabbcc;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x8) == 0x99aabbcc);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR13() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5cf0000; // strb r0, [r15]
        m_alu->m_mem->template writePointer<uint32_t>(4, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(8, 0x55667788);
        m_alu->m_registers[0] = 0x99aabbcc;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x8) == 0x556677cc);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR14() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5c10000; // strb r0, [r1]
        m_alu->m_mem->template writePointer<uint32_t>(4, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(8, 0x55667788);
        m_alu->m_registers[0] = 0x99aabbee;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0xaabbccdd);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x99aabbee);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x4) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x8) == 0x55667788);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xaabbccee);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDR15() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe7f10002; // ldrb r0, [r1]

        m_alu->m_registers[0] = 0xdeadbeaf;
        m_alu->m_registers[1] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000044);
        QVERIFY(m_alu->m_registers[1] == 0x00000100);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testLDMFD() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8bd000f; // LDMFD r13!, {r0-r3}
        m_alu->m_registers[0]  = 0x00000000;
        m_alu->m_registers[1]  = 0x00000000;
        m_alu->m_registers[2]  = 0x00000000;
        m_alu->m_registers[3]  = 0x00000000;
        m_alu->m_registers[13] = 0x00000100;

        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);
        m_alu->m_mem->template writePointer<uint32_t>(0x108, 0x99aabbcc);
        m_alu->m_mem->template writePointer<uint32_t>(0x10c, 0xddee00ff);

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0x99aabbcc);
        QVERIFY(m_alu->m_registers[3] == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x00000110);
    }

    void testLDMFA() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe83d000f; // LDMFA r13!, {r0-r3}

        m_alu->m_registers[0]  = 0x00000000;
        m_alu->m_registers[1]  = 0x00000000;
        m_alu->m_registers[2]  = 0x00000000;
        m_alu->m_registers[3]  = 0x00000000;
        m_alu->m_registers[13] = 0x0000010c;

        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0x11223344);
        m_alu->m_mem->template writePointer<uint32_t>(0x104, 0x55667788);
        m_alu->m_mem->template writePointer<uint32_t>(0x108, 0x99aabbcc);
        m_alu->m_mem->template writePointer<uint32_t>(0x10c, 0xddee00ff);

        m_alu->run(1);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0x99aabbcc);
        QVERIFY(m_alu->m_registers[3] == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x000000fc);
    }

    void testSTMFA() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe92d000f; // STMFA r13!, {r0-r3}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0xddee00ff;
        m_alu->m_registers[13] = 0x00000110;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x00000100);
    }

    void testSTMED() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe82d000f; // STMED r13!, {r0-r3}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0xddee00ff;
        m_alu->m_registers[13] = 0x00000100;

        m_alu->m_mem->template writePointer<uint32_t>(0xf4) = 0xABCDEF09;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0xf4) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0xf8) == 0x55667788);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0xfc) == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x000000f0);
    }

    void testSTMEA() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8ad000f; // STMEA r13!, {r0-r3}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0xddee00ff;
        m_alu->m_registers[13] = 0x00000100;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x55667788);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x00000110);
    }

    void testSTMFA2() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe9ad000f; // STMFA r13!, {r0-r3}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0xddee00ff;
        m_alu->m_registers[13] = 0x00000100;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x11223344);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x55667788);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x99aabbcc);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x110) == 0xddee00ff);
        QVERIFY(m_alu->m_registers[13] == 0x00000110);
    }

    void testSTMFA3() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8a30038; // STM r3!, {r3-r5}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0x00000100;
        m_alu->m_registers[4]  = 0x00004444;
        m_alu->m_registers[5]  = 0x00055555;
        m_alu->m_registers[13] = 0x00000110;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x00000100);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0x99aabbcc);
        QVERIFY(m_alu->m_registers[3] == 0x0000010c);
        QVERIFY(m_alu->m_registers[4] == 0x00004444);
        QVERIFY(m_alu->m_registers[5] == 0x00055555);
        QVERIFY(m_alu->m_registers[13] == 0x00000110);
    }

    void testSTMFA4() {

        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8a30030; // STM r3!, {r4-r5}

        m_alu->m_registers[0]  = 0x11223344;
        m_alu->m_registers[1]  = 0x55667788;
        m_alu->m_registers[2]  = 0x99aabbcc;
        m_alu->m_registers[3]  = 0x00000100;
        m_alu->m_registers[4]  = 0x00004444;
        m_alu->m_registers[5]  = 0x00055555;
        m_alu->m_registers[13] = 0x00000110;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x00004444);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x00055555);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x00000000);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x55667788);
        QVERIFY(m_alu->m_registers[2] == 0x99aabbcc);
        QVERIFY(m_alu->m_registers[3] == 0x00000108);
        QVERIFY(m_alu->m_registers[4] == 0x00004444);
        QVERIFY(m_alu->m_registers[5] == 0x00055555);
        QVERIFY(m_alu->m_registers[13] == 0x00000110);
    }

    void testSTR2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe5a10004; // str r0, [r1, #4]!
        m_alu->m_registers[0] = 0x11223344;
        m_alu->m_registers[1] = 0x00000080;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x84) == 0x11223344);
        QVERIFY(m_alu->m_registers[0] == 0x11223344);
        QVERIFY(m_alu->m_registers[1] == 0x00000084);
    }

    void testSTM1() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8a30034; // stm	r3!,{r2,r4,r5}

        m_alu->m_registers[2] = 0x00000022;
        m_alu->m_registers[3] = 0x00000100;
        m_alu->m_registers[4] = 0x00004444;
        m_alu->m_registers[5] = 0x00055555;
        m_alu->m_registers[6] = 0x00666666;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x00000022);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0x00000022);
        QVERIFY(m_alu->m_registers[3] == 0x0000010c);
        QVERIFY(m_alu->m_registers[4] == 0x00004444);
        QVERIFY(m_alu->m_registers[5] == 0x00055555);
        QVERIFY(m_alu->m_registers[6] == 0x00666666);
    }

    void testSTM2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe8a30038; // stm	r3!,{r3,r4,r5}

        m_alu->m_registers[2] = 0x00000022;
        m_alu->m_registers[3] = 0x00000100;
        m_alu->m_registers[4] = 0x00004444;
        m_alu->m_registers[5] = 0x00055555;
        m_alu->m_registers[6] = 0x00666666;

        m_alu->run(1);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0x00000100);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x104) == 0x00004444);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x108) == 0x00055555);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10c) == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0x00000022);
        QVERIFY(m_alu->m_registers[3] == 0x0000010c);
        QVERIFY(m_alu->m_registers[4] == 0x00004444);
        QVERIFY(m_alu->m_registers[5] == 0x00055555);
        QVERIFY(m_alu->m_registers[6] == 0x00666666);
    }

    void testCONDPM() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0902001;     // adds r2, r0, r1
        m_alu->m_mem->template writePointer<uint32_t>(4, 0x50823001); // addpl r3, r2, r1
        m_alu->m_registers[0] = 0xFFFFFF0F;
        m_alu->m_registers[1] = 0x00000002;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0xDEADBEAF;
        m_alu->m_cpsr         = 0x00000000;

        m_alu->run(2);

        QVERIFY(m_alu->m_registers[0] == 0xFFFFFF0F);
        QVERIFY(m_alu->m_registers[1] == 0x00000002);
        QVERIFY(m_alu->m_registers[2] == 0xffffff11);
        QVERIFY(m_alu->m_registers[3] == 0xDEADBEAF);
        QVERIFY(m_alu->m_cpsr == 0x80000000);
    }

    void testCONDVC() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0) = 0xe0902001;     // adds r2, r0, r1
        m_alu->m_mem->template writePointer<uint32_t>(4, 0x70823001); // addvc r3, r2, r1
        m_alu->m_registers[0] = 0xFFFFFF0F;
        m_alu->m_registers[1] = 0x000F0000;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0xDEADBEAF;
        m_alu->m_cpsr         = 0x00000000;

        m_alu->run(2);

        QVERIFY(m_alu->m_registers[0] == 0xFFFFFF0F);
        QVERIFY(m_alu->m_registers[1] == 0x000F0000);
        QVERIFY(m_alu->m_registers[2] == 0x000eff0f);
        QVERIFY(m_alu->m_registers[3] == 0x001dff0f);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testCONDCC() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1540002);  // cmp   r4,r2
        m_alu->m_mem->template writePointer<uint32_t>(4, 0x33a0600f);  // movcc r6,#15
        m_alu->m_mem->template writePointer<uint32_t>(8, 0xe1540002);  // cmp   r4,r2
        m_alu->m_mem->template writePointer<uint32_t>(12, 0x33a0600f); // movcc r6,#15
        m_alu->m_registers[2] = 0x0000000A;
        m_alu->m_registers[4] = 0x0000000F;
        m_alu->m_registers[6] = 0x00004444;
        m_alu->m_cpsr         = 0x00000000;

        m_alu->run(2);

        QVERIFY(m_alu->m_registers[2] == 0x0000000A);
        QVERIFY(m_alu->m_registers[4] == 0x0000000F);
        QVERIFY(m_alu->m_registers[6] == 0x00004444);
        QVERIFY(m_alu->m_cpsr == 0x20000000);

        m_alu->m_registers[2] = 0x00000014;
        m_alu->m_registers[4] = 0x00000010;
        m_alu->m_registers[6] = 0x00004444;
        m_alu->m_cpsr         = 0x00000000;

        m_alu->run(2);

        QVERIFY(m_alu->m_registers[2] == 0x00000014);
        QVERIFY(m_alu->m_registers[4] == 0x00000010);
        QVERIFY(m_alu->m_registers[6] == 0x0000000F);
        QVERIFY(m_alu->m_cpsr == 0x80000000);

    }

    void testHALF() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1d030f2);  // ldrsh   r3, [r0, #2]
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe1d140fe);  // ldrsh   r4, [r1, #14]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000000;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0x00000000;
        m_alu->m_registers[4] = 0x00000000;
        m_alu->m_cpsr         = 0x00000000;
        m_alu->m_mem->template writePointer<uint32_t>(12, 0x9a8b7c6d);

        m_alu->run(2);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
        QVERIFY(m_alu->m_registers[3] == 0xffffe1d0);
        QVERIFY(m_alu->m_registers[4] == 0xffff9a8b);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testHALF2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1d330fe);  // ldrsh   r3, [r3, #14]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000000;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0x00000048;
        m_alu->m_registers[4] = 0x00000000;
        m_alu->m_cpsr         = 0x00000000;
        m_alu->m_mem->template writePointer<uint32_t>(0x54, 0x1a2b3c4d);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
        QVERIFY(m_alu->m_registers[3] == 0x00001a2b);
        QVERIFY(m_alu->m_registers[4] == 0x00000000);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testHALF3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1d120de);  // ldrsb   r2, [r1, #14]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000000;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_cpsr         = 0x00000000;
        m_alu->m_mem->template writePointer<uint32_t>(12, 0x9a8b7c6d);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0xffffff8b);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testSTRH() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1c330be);  // strh   r3, [r3, #14]
        m_alu->m_registers[0] = 0x00000000;
        m_alu->m_registers[1] = 0x00000000;
        m_alu->m_registers[2] = 0x00000000;
        m_alu->m_registers[3] = 0x00000012;
        m_alu->m_cpsr         = 0x00000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_registers[1] == 0x00000000);
        QVERIFY(m_alu->m_registers[2] == 0x00000000);
        QVERIFY(m_alu->m_registers[3] == 0x00000012);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(32) == 0x00000012);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testSTRH2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1c330be);  // strh   r3, [r3, #14]
        m_alu->m_registers[3] = 0x00000014;
        m_alu->m_cpsr         = 0x00000000;
        m_alu->m_mem->template writePointer<uint32_t>(0x20, 0xa1b2c3d4);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[3] == 0x00000014);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x20) == 0x0014c3d4);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testSTRH3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1c320be);  // strh r2, [r3, #14]
        m_alu->m_registers[2] = 0x00000001;
        m_alu->m_registers[3] = 0x00000048;
        m_alu->m_mem->template writePointer<uint32_t>(0x54, 0xf3d7e5b9);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x00000001);
        QVERIFY(m_alu->m_registers[3] == 0x00000048);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x54) == 0x0001e5b9);
    }

    void testSTRH4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1c320be);  // strh r2, [r3, #14]
        m_alu->m_registers[2] = 0x00000001;
        m_alu->m_registers[3] = 0x00000048;
        m_alu->m_mem->template writePointer<uint32_t>(0x54, 0x00000009);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x00000001);
        QVERIFY(m_alu->m_registers[3] == 0x00000048);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x54) == 0x00010009);
    }

    void testSTRH5() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0c320b2);  // strh r2, [r3], #2
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[3] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0xaaaaaaaa);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0xaabbccdd);
        QVERIFY(m_alu->m_registers[3] == 0x00000102);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xaaaaccdd);
    }

    void testSTRH6() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0c320b3);  // strh r2, [r3], #3
        m_alu->m_registers[2] = 0xaabbccdd;
        m_alu->m_registers[3] = 0x00000102;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0xaaaaaaaa);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0xaabbccdd);
        QVERIFY(m_alu->m_registers[3] == 0x00000105);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xccddaaaa);
    }

    void testLDRH1() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0d320b2);  // ldrh r2, [r3], #2
        m_alu->m_registers[2] = 0x11223344;
        m_alu->m_registers[3] = 0x00000100;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0xaabbccdd);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x0000ccdd);
        QVERIFY(m_alu->m_registers[3] == 0x00000102);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xaabbccdd);
    }

    void testLDRH2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0d320b3);  // ldrh r2, [r3], #3
        m_alu->m_registers[2] = 0x11223344;
        m_alu->m_registers[3] = 0x00000102;
        m_alu->m_mem->template writePointer<uint32_t>(0x100, 0xaabbccdd);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x0000aabb);
        QVERIFY(m_alu->m_registers[3] == 0x00000105);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x100) == 0xaabbccdd);
    }

    void testSTRB() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe5c10000);  // strb r0, [r1]
        m_alu->m_registers[0] = 0xa1b2c3d4;
        m_alu->m_registers[1] = 0x00000010;
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0xeeeeeeee);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xa1b2c3d4);
        QVERIFY(m_alu->m_registers[1] == 0x00000010);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0xeeeeeed4);
    }

    void testSTRB2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe5c10002);  // strb r0, [r1, #2]
        m_alu->m_registers[0] = 0xa1b2c3d4;
        m_alu->m_registers[1] = 0x00000010;
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0x11111111);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xa1b2c3d4);
        QVERIFY(m_alu->m_registers[1] == 0x00000010);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0x11d41111);
    }

    void testSTRB3() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe5c10003);  // strb r0, [r1, #3]
        m_alu->m_registers[0] = 0xa1b2c3d4;
        m_alu->m_registers[1] = 0x00000010;
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0x11111111);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xa1b2c3d4);
        QVERIFY(m_alu->m_registers[1] == 0x00000010);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0xd4111111);
    }

    void testSTRB4() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe5c10001);  // strb r0, [r1, #1]
        m_alu->m_registers[0] = 0xa1b2c3d4;
        m_alu->m_registers[1] = 0x00000010;
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0x11111111);

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0xa1b2c3d4);
        QVERIFY(m_alu->m_registers[1] == 0x00000010);
        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0x1111d411);
    }

    void testADCS() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0b11553);  // adcs    r1, r1, r3, asr r5
        m_alu->m_registers[1] = 0x00148fbe;
        m_alu->m_registers[3] = 0xffe80000;
        m_alu->m_registers[5] = 0x00000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0xfffc8fbe);
        QVERIFY(m_alu->m_registers[3] == 0xffe80000);
        QVERIFY(m_alu->m_registers[5] == 0x00000000);

    }

    void testANDS() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe2100003);  // ands r0, r0, #3
        m_alu->m_registers[0] = 0x00055820;
        m_alu->m_cpsr = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x00000000);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testTEQ() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1340005);  // teq r4, r5
        m_alu->m_registers[4] = 0x7fe91f7c;
        m_alu->m_registers[5] = 0x7ff00000;
        m_alu->m_cpsr = 0x20000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[4] == 0x7fe91f7c);
        QVERIFY(m_alu->m_registers[5] == 0x7ff00000);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testTEST() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe3110102);  // tst     r1, #-2147483648        ; 0x80000000
        m_alu->m_registers[1] = 0x3ff48fbe;
        m_alu->m_cpsr = 0x80000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[1] == 0x3ff48fbe);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testRSC() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe2e33000);  // rsc r3, r3, #0
        m_alu->m_registers[3] = 0x00180000;
        m_alu->m_cpsr = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[3] == 0xffe80000);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testUMULL() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe08ec290);  // umull   ip, lr, r0, r2
        m_alu->m_registers[0] = 0x49ba5e38;
        m_alu->m_registers[2] = 0x636f4361;
        m_alu->m_registers[12] = 0x000007ff;
        m_alu->m_registers[14] = 0x0003a910;
        m_alu->m_cpsr = 0x20000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x49ba5e38);
        QVERIFY(m_alu->m_registers[2] == 0x636f4361);
        QVERIFY(m_alu->m_registers[12] == 0x388e5b38);
        QVERIFY(m_alu->m_registers[14] == 0x1ca31da6);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testSMULL() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0c23290); // smull   r3, r2, r0, r2
        m_alu->m_registers[0] = 0x17;
        m_alu->m_registers[2] = 0xd;
        m_alu->m_registers[3] = 0x7;
        m_alu->m_cpsr         = 0x20000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[0] == 0x17);
        QVERIFY(m_alu->m_registers[2] == 0x0);
        QVERIFY(m_alu->m_registers[3] == 0x12b);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testSMULL2() {



        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe0c23190); // smull   r3, r2, r0, r1
        m_alu->m_registers[0] = -324;       // fffffebc
        m_alu->m_registers[1] = 45674233;   // 2b8eef9
        m_alu->m_registers[2] = 0x7213;
        m_alu->m_registers[3] = 0x72345;
        m_alu->m_cpsr         = 0x20000000;

        m_alu->run(1);

        QVERIFY(static_cast<int32_t>(m_alu->m_registers[0]) == -324);
        QVERIFY(m_alu->m_registers[1] == 45674233);
        QVERIFY(m_alu->m_registers[2] == 0xfffffffc);
        QVERIFY(m_alu->m_registers[3] == 0x8df18cdc);
        QVERIFY(m_alu->m_cpsr == 0x20000000);
    }

    void testRSBS() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe2722000);  // rsbs r2,r2,#0
        m_alu->m_registers[2] = 0xe2f3f618;
        m_alu->m_cpsr = 0xa0000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x1d0c09e8);
        QVERIFY(m_alu->m_cpsr == 0x00000000);
    }

    void testTEST2() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe3110601);  // tst     r1, #1048576    ; 0x100000
        m_alu->m_registers[1] = 0x00000000;
        m_alu->m_cpsr = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_registers[2] == 0x00000000);
        QVERIFY(m_alu->m_cpsr == 0x40000000);
    }

    void testR15() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe320f000); // nop
        m_alu->m_mem->template writePointer<uint32_t>(4, 0xe780f001); // STR    R15, [R0, R1]
        m_alu->m_registers[0] = 0x0000000a;
        m_alu->m_registers[1] = 0x00000002;
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(2);

        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(12) == 0x00000010);
    }

    void testSWP_1() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1020091); // SWP R0, R1, [R2]
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0xABCDEF01);
        m_alu->m_registers[0] = 0x0000000a;
        m_alu->m_registers[1] = 0x11112222;
        m_alu->m_registers[2] = 0x00000010;
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0x11112222);
        QVERIFY(m_alu->m_registers[0] == 0xABCDEF01);
        QVERIFY(m_alu->m_registers[1] == 0x11112222);
        QVERIFY(m_alu->m_registers[2] == 0x00000010);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }

    void testSWPB_1() {


        m_alu->reset();

        m_alu->m_mem->template writePointer<uint32_t>(0, 0xe1420091); // SWPB R0, R1, [R2]
        m_alu->m_mem->template writePointer<uint32_t>(0x10, 0xABCDEF33);
        m_alu->m_registers[0] = 0x11223344;
        m_alu->m_registers[1] = 0x11112222;
        m_alu->m_registers[2] = 0x00000010;
        m_alu->m_cpsr         = 0x60000000;

        m_alu->run(1);

        QVERIFY(m_alu->m_mem->template readPointer<uint32_t>(0x10) == 0xABCDEF22);
        QVERIFY(m_alu->m_registers[0] == 0x00000033);
        QVERIFY(m_alu->m_registers[1] == 0x11112222);
        QVERIFY(m_alu->m_registers[2] == 0x00000010);
        QVERIFY(m_alu->m_cpsr == 0x60000000);
    }
};

template<typename T>
class TestAluProgram {

  private:
    using ProtectedVfp = Vfpv2<T>;
    using ProtectedAlu = Alu<T, ProtectedVfp>;

    std::unique_ptr<T> m_mem;
    std::unique_ptr<ProtectedAlu> m_alu;

  public:
    TestAluProgram() {
    }
    virtual ~TestAluProgram() = default;

  public:

    void testProgramHello() {

        VmProperties vmProperties;
        std::string binPath(getBinPath());
        std::string data;

        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/hello.bin";

        std::byte *mem     = nullptr;
        std::byte *uart    = nullptr;
        bool           running = true;

        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        mem = vm->reset();
        if(vm->load()) {

            uart = mem + UARTPOS;

            while (running) {

                switch (vm->run()) {

                case Interrupt::Resume:
                    break;

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
                    data += (char)*uart;

                default:
                    break;
                }
            }
        }
        QVERIFY(data == "hello world\n");
    }

    void testProgramPrimeN() {

        VmProperties vmProperties;

        std::string binPath(getBinPath());
        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/primen.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;


        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        mem = vm->reset();
        uart = mem + UARTPOS;

        if (vm->load()) {

            while (running) {

                switch (vm->run()) {

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
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

        VmProperties vmProperties;
        std::string binPath(getBinPath());
        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/float.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;


        //registerCoprocessor<MemoryRaw, Vfpv2>("vfpv2");

        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        mem = vm->reset();
        uart = mem + UARTPOS;

        if (vm->load()) {

            while (running) {

                switch (m_alu->run()) {

                case Interrupt::Resume:
                    break;

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
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

        VmProperties vmProperties;
        std::string binPath(getBinPath());
        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/printf.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;
        QString data;

        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        mem = vm->reset();
        uart = mem + UARTPOS;

        if(vm->load()) {

            while (running) {

                switch (m_alu->run()) {

                case Interrupt::Resume:
                    break;

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
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

        VmProperties vmProperties;
        std::string binPath(getBinPath());
        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/modulo.bin";
        bool     running       = true;



        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        vm->reset();

        if (vm->load()) {

            while (running) {

                switch (vm->run()) {

                case Interrupt::Resume:
                    break;

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
                    break;

                default:
                    break;
                }
            }
        }

               //QVERIFY(m_alu->m_registers[0] == 0);
    }

    void testProgramBench() {

        VmProperties vmProperties;
        std::string binPath(getBinPath());
        vmProperties.m_memoryHandlerProperties.m_memorySizeBytes = 20_mb;
        vmProperties.m_bin     = binPath + "/src/test_compile/bench.bin";
        std::byte *mem           = nullptr;
        std::byte *uart          = nullptr;
        bool     running       = true;
        QString data;


        std::unique_ptr<Vm> vm = Vm::build(vmProperties);
        mem = vm->reset();
        uart = mem + UARTPOS;

        if(vm->load()) {

            while (running) {

                switch (vm->run()) {

                case Interrupt::Resume:
                    break;

                case Interrupt::Stop:
                    running = false;
                    break;

                case Interrupt::Suspend:
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




} // namespace armv4vm
