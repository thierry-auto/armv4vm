
#include <QObject>
#include <QtTest>
#include <iostream>
#include <vector>

#include "vm.h"

namespace armv4vm {

using Test1Vm = VirtualMachine<MemoryProtected>;

struct VmProperties vmProperties;

class TestMem : public QObject {
    Q_OBJECT

  public:
    TestMem() { vmProperties.m_memsize = 512; }

  private slots:

    void testInstance() {
        struct VmProperties vmProperties;
        Test1Vm             vm1(&vmProperties);
    }

    void testReadWrite8() {

        uint8_t            mem[64] = {0};
        std::vector<Range> ranges;
        MemoryProtected    pro;
        uint8_t            v1 = 0x11;
        uint8_t            v2 = 0;
        uint8_t            v3 = 0;

        bool exceptionRaised = false;
        ranges.push_back({0, 64});
        pro.init(mem, 64, ranges);

        try {
            writePointer<uint8_t>(mem + 10) = v1;
            writePointer<uint8_t>(pro + 20) = v1;
            QVERIFY(mem[20] == 0x11);
            QVERIFY(mem[21] == 0x0);
            QVERIFY(mem[22] == 0x0);
            QVERIFY(mem[23] == 0x0);

            writePointer<uint8_t>(mem + 24) = v1;
            writePointer<uint8_t>(pro + 28) = v1;
            QVERIFY(mem[24] == 0x11);
            QVERIFY(pro[28] == 0x11);

            v2 = readPointer<uint8_t>(mem + 24);
            v3 = readPointer<uint8_t>(pro + 28);
            QVERIFY(v2 == v3);

            writePointer<uint8_t>(&mem[32]) = 12;
            writePointer<uint8_t>(pro + 33) = 12;

            v1      = mem[32];
            v2      = pro[33];
            QVERIFY(v1 == v2);

        } catch (std::exception &e) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testReadWrite16() {

        uint8_t            mem[64] = {0};
        std::vector<Range> ranges;
        MemoryProtected    pro;
        const uint16_t     v1 = 0x1122;
        uint16_t           v2 = 0;
        uint16_t           v3 = 0;

        bool exceptionRaised = false;
        ranges.push_back({0, 64});
        pro.init(mem, 64, ranges);

        try {
            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            writePointer<uint8_t>(pro + 20) = static_cast<uint8_t>(v1);

            QVERIFY(mem[20] == 0x22);
            QVERIFY(mem[21] == 0x0);
            QVERIFY(mem[22] == 0x0);
            QVERIFY(mem[23] == 0x0);

            writePointer<uint16_t>(mem + 24) = v1;
            writePointer<uint16_t>(pro + 28) = v1;

            QVERIFY(mem[24] == 0x22);
            QVERIFY(mem[25] == 0x11);
            QVERIFY(pro[28] == 0x22);
            QVERIFY(pro[29] == 0x11);

            writePointer<uint16_t>(mem + 24) = v1;
            writePointer<uint16_t>(pro + 28) = v1;

            v2 = readPointer<uint8_t>(mem + 24);
            v3 = readPointer<uint8_t>(pro + 28);
            QVERIFY(v2 == v3);

            v2 = readPointer<uint16_t>(mem + 24);
            v3 = readPointer<uint16_t>(pro + 28);
            QVERIFY(v2 == v3);

            writePointer<uint8_t>(mem + 32) = 0xEB;
            writePointer<uint8_t>(pro + 33) = 0xEB;

            v2 = mem[32];
            v3 = pro[33];
            QVERIFY(v2 == v3);

        } catch (std::exception &e) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testOutOfRange8() {

        uint8_t            mem[64] = {0};
        std::vector<Range> ranges;
        MemoryProtected    pro;
        const uint8_t      v1 = 0x11;

        bool exceptionRaised = false;
        ranges.push_back({0, 32});
        pro.init(mem, 64, ranges);

        try {
            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            writePointer<uint8_t>(pro + 20) = readPointer<uint8_t>(mem + 10);

            QVERIFY(mem[20] == v1);

            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            writePointer<uint8_t>(pro + 42) = readPointer<uint8_t>(mem + 10);

        } catch (std::exception &e) {
            exceptionRaised = true;
        }

        QVERIFY(pro[42] == 0);
        QVERIFY(exceptionRaised == true);
    }

    void testOutOfRange16() {

        uint8_t            mem[64] = {0};
        std::vector<Range> ranges;
        MemoryProtected    pro;
        const uint16_t     v1 = 0x1122;

        bool exceptionRaised = false;
        ranges.push_back({0, 32});
        pro.init(mem, 64, ranges);

        try {
            writePointer<uint16_t>(mem + 10) = static_cast<uint16_t>(v1);
            writePointer<uint16_t>(pro + 20) = readPointer<uint16_t>(mem + 10);

            QVERIFY(mem[20] == 0x22);
            QVERIFY(mem[21] == 0x11);

            writePointer<uint16_t>(mem + 10) = static_cast<uint16_t>(v1);
            writePointer<uint16_t>(pro + 31) = readPointer<uint16_t>(mem + 10);

        } catch (std::exception &e) {
            exceptionRaised = true;
        }

        QVERIFY(pro[31] == 0);
        QVERIFY(pro[32] == 0);
        QVERIFY(exceptionRaised == true);
    }

    //    void testPlusValue16() {

    //        uint8_t            mem[64] = {0};
    //        std::vector<Range> ranges;
    //        MemoryProtected    pro;
    //        uint16_t           v1              = 0x1122;
    //        uint16_t           v2              = 0;
    //        uint16_t           v3              = 0;
    //        bool               exceptionRaised = false;
    //        ranges.push_back({0, 64});
    //        pro.init(mem, 64, ranges);

    //        try {
    //            mem[10] = v1;
    //            pro[20] = v1;

    //            QVERIFY(mem[20] == 0x22);
    //            QVERIFY(mem[21] == 0x0);

    //            *reinterpret_cast<uint16_t *>(&mem[24]) = v1;
    //            *reinterpret_cast<uint16_t *>(&pro[28]) = v1;

    //            QVERIFY(mem[24] == 0x22);
    //            QVERIFY(mem[25] == 0x11);

    //            QVERIFY(pro[28] == 0x22);
    //            QVERIFY(pro[29] == 0x11);

    //            v2 = *toPointerU16(&mem[24]);
    //            v3 = *toPointerU16(&pro[28]);

    //            QVERIFY(v2 == v3);

    //        } catch (std::exception &e) {
    //            exceptionRaised = true;
    //        }

    //        QVERIFY(exceptionRaised == false);
    //    }

    //    void testPlusValue32() {

    //        uint8_t            mem[64] = {0};
    //        std::vector<Range> ranges;
    //        MemoryProtected    pro;
    //        uint32_t           v1              = 0x11223344;
    //        uint32_t           v2              = 0;
    //        uint32_t           v3              = 0;
    //        bool               exceptionRaised = false;
    //        ranges.push_back({0, 64});
    //        pro.init(mem, 64, ranges);

    //        try {
    //            mem[10] = v1;
    //            pro[20] = v1;

    //            QVERIFY(mem[20] == 0x44);
    //            QVERIFY(mem[21] == 0x0);
    //            QVERIFY(mem[22] == 0x0);
    //            QVERIFY(mem[23] == 0x0);

    //            *toPointerU32(&mem[24]) = v1;
    //            *toPointerU32(&pro[28]) = v1;

    //            QVERIFY(mem[24] == 0x44);
    //            QVERIFY(mem[25] == 0x33);
    //            QVERIFY(mem[26] == 0x22);
    //            QVERIFY(mem[27] == 0x11);

    //            QVERIFY(pro[28] == 0x44);
    //            QVERIFY(pro[29] == 0x33);
    //            QVERIFY(pro[30] == 0x22);
    //            QVERIFY(pro[31] == 0x11);

    //            v2 = *reinterpret_cast<uint32_t *>(&mem[24]);
    //            v3 = *reinterpret_cast<const uint32_t *>(&pro[28]);

    //            QVERIFY(v2 == v3);

    //        } catch (std::exception &e) {
    //            exceptionRaised = true;
    //        }

    //        QVERIFY(exceptionRaised == false);
    //    }

    //    void testErrorBound() {

    //        uint8_t            mem[64] = {0};
    //        std::vector<Range> ranges;
    //        MemoryProtected    pro;
    //        uint8_t            v1              = 0x11;
    //        bool               exceptionRaised = false;

    //        ranges.push_back({32, 32});
    //        pro.init(mem, 64, ranges);

    //        try {
    //            *reinterpret_cast<uint32_t *>(&pro[38]) = v1;
    //        } catch (std::exception &e) {
    //            exceptionRaised = true;
    //        }
    //        QVERIFY(mem[30] == 0);
    //        QVERIFY(mem[31] == 0);
    //        QVERIFY(mem[32] == 0);
    //        QVERIFY(mem[33] == 0);
    //        QVERIFY(exceptionRaised == true);
    //    }
};

} // namespace armv4vm
QTEST_MAIN(armv4vm::TestMem)
#include "tu_memoryprotect.moc"
