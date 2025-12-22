
#include <QObject>
#include <QtTest>

#include "armv4vm.hpp"

namespace armv4vm {

//using Test1Vm = Alu<MemoryProtected>;

template <typename T> inline T  readPointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T &writePointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }


struct VmProperties vmProperties;

class TestMem : public QObject {
    Q_OBJECT

  public:
    TestMem() { vmProperties.m_memsize = 512; }

  private slots:

    void testInstance() {
        //struct VmProperties vmProperties;
        //Test1Vm             vm1(&vmProperties);
    }

    void testReadWrite8() {

        uint8_t           *mem = nullptr;
        MemoryProtected    pro;
        uint8_t            v1 = 0x11;
        uint8_t            v2 = 0;
        uint8_t            v3 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        mem = pro.allocate(512);

        try {
            writePointer<uint8_t>(mem + 10) = v1;
            pro.writePointer8(20) = v1;
            QVERIFY(mem[20] == 0x11);
            QVERIFY(mem[21] == 0x0);
            QVERIFY(mem[22] == 0x0);
            QVERIFY(mem[23] == 0x0);

            writePointer<uint8_t>(mem + 24) = v1;
            pro.writePointer8(28) = v1;
            QVERIFY(mem[24] == 0x11);
            QVERIFY(pro[28] == 0x11);

            v2 = readPointer<uint8_t>(mem + 24);
            v3 = pro.readPointer8(28);
            QVERIFY(v2 == v3);

            writePointer<uint8_t>(&mem[32]) = 12;
            pro.writePointer8(33) = 12;

            v1      = mem[32];
            v2      = pro[33];
            QVERIFY(v1 == v2);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testReadWrite16() {

        uint8_t           *mem = nullptr;
        MemoryProtected    pro;
        const uint16_t     v1 = 0x1122;
        uint16_t           v2 = 0;
        uint16_t           v3 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        mem = pro.allocate(512);

        try {
            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            pro.writePointer8(20) = static_cast<uint8_t>(v1);

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
            pro.writePointer16(28) = v1;

            v2 = readPointer<uint8_t>(mem + 24);
            v3 = pro.readPointer8(28);
            QVERIFY(v2 == v3);

            v2 = readPointer<uint16_t>(mem + 24);
            v3 = pro.readPointer16(28);
            QVERIFY(v2 == v3);

            writePointer<uint8_t>(mem + 32) = 0xEB;
            pro.writePointer8(33) = 0xEB;

            v2 = mem[32];
            v3 = pro[33];
            QVERIFY(v2 == v3);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testOutOfRange8() {

        uint8_t           *mem;

        MemoryProtected    pro;
        const uint8_t      v1 = 0x11;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 32, AccessPermission::READ_WRITE});
        mem = pro.allocate(64);

        try {
            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            pro.writePointer8(20) = readPointer<uint8_t>(mem + 10);

            QVERIFY(mem[20] == v1);

            writePointer<uint8_t>(mem + 10) = static_cast<uint8_t>(v1);
            pro.writePointer8(42) = readPointer<uint8_t>(mem + 10);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(pro[42] == 0);
        QVERIFY(exceptionRaised == true);
    }

    void testOutOfRange16() {

        uint8_t           *mem;

        MemoryProtected    pro;
        const uint16_t     v1 = 0x1122;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 32, AccessPermission::READ_WRITE});
        mem = pro.allocate(64);

        try {
            writePointer<uint16_t>(mem + 10) = static_cast<uint16_t>(v1);
            pro.writePointer16(20) = readPointer<uint16_t>(mem + 10);

            QVERIFY(mem[20] == 0x22);
            QVERIFY(mem[21] == 0x11);

            writePointer<uint16_t>(mem + 10) = static_cast<uint16_t>(v1);
            pro.writePointer16(31) = readPointer<uint16_t>(mem + 10);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(pro[31] == 0);
        QVERIFY(pro[32] == 0);
        QVERIFY(exceptionRaised == true);
    }

    void testOutOfRange32() {

        uint8_t           *mem;
        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;
        uint32_t           v2 = 0;
        uint32_t           v3 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({32, 32, AccessPermission::READ_WRITE});
        mem = pro.allocate(64);

        try {
            writePointer<uint32_t>(mem + 1)  = v1;
            pro.writePointer32(44) = readPointer<uint32_t>(mem + 1);

            QVERIFY(mem[44] == 0x44);
            QVERIFY(mem[45] == 0x33);
            QVERIFY(mem[46] == 0x22);
            QVERIFY(mem[47] == 0x11);

            v2 = readPointer<uint16_t>(mem + 1);
            v3 = pro.readPointer16(44);

            QVERIFY(v2 == v3);

            pro.writePointer32(30) = pro.readPointer32(44);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(pro[30] == 0);
        QVERIFY(pro[31] == 0);
        QVERIFY(pro[32] == 0);
        QVERIFY(pro[33] == 0);
        QVERIFY(exceptionRaised == true);
    }

    void testOutOfRange32_2() {

        uint8_t           *mem;

        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;
        const uint32_t     v2 = 0x55667788;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({4, 4, AccessPermission::READ_WRITE});
        mem = pro.allocate(8);

        try {
            pro.writePointer32(4) = v1;

            QVERIFY(mem[4] == 0x44);
            QVERIFY(mem[5] == 0x33);
            QVERIFY(mem[6] == 0x22);
            QVERIFY(mem[7] == 0x11);

            pro.writePointer32(2) = v2;

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(mem[2] == 0x0);
        QVERIFY(mem[3] == 0x0);
        QVERIFY(mem[4] == 0x44);
        QVERIFY(mem[5] == 0x33);
        QVERIFY(exceptionRaised == true);

        exceptionRaised = false;

        try {
            pro.writePointer32(5) = v1;

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(mem[5] == 0x33);
        QVERIFY(mem[6] == 0x22);
        QVERIFY(mem[7] == 0x11);
        QVERIFY(exceptionRaised == true);
    }

    void testMinus() {

        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({64, 64, AccessPermission::READ_WRITE});
        pro.allocate(256);

        try {
            pro.writePointer32(-3) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testMinus2() {


        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({64, 64, AccessPermission::READ_WRITE});
        pro.allocate(128);

        try {
            pro.writePointer32(-4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testMinus3() {

        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        pro.allocate(128);

        try {
            pro.writePointer32(4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testMinus4() {


        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        pro.allocate(128);

        try {
            pro.writePointer32(-4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testPlus4() {


        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        pro.allocate(128);

        try {
            pro.writePointer32(4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }


    void testReadSucced() {

        MemoryProtected    pro;
        uint32_t     v1 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({32, 64, AccessPermission::READ_WRITE});
        uint8_t *mem = pro.allocate(128);

        *reinterpret_cast<uint32_t*>(mem+32)=0x12345678;

        try {
            v1 = pro.readPointer32(32);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == false);
        QVERIFY(v1 == 0x12345678);
    }

    void testReadSucced2() {


        MemoryProtected    pro;
        uint32_t     v1 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({32, 64, AccessPermission::READ_WRITE});
        uint8_t *mem = pro.allocate(128);

        *reinterpret_cast<uint32_t*>(mem+32)=0x12345678;

        try {
            v1 = pro.readPointer32(32);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == false);
        QVERIFY(v1 == 0x12345678);
    }

    void testReadFailed() {


        MemoryProtected    pro;
        uint32_t     v1 = 0;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        pro.allocate(128);

        try {
            v1 = pro.readPointer32(-1);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == true);
    }

    void testMinusPlus() {

        uint8_t           *mem;

        MemoryProtected    pro;
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({64, 64, AccessPermission::READ_WRITE});
        mem = pro.allocate(128);

        try {
            pro.writePointer32(109) = v1;

            QVERIFY(mem[109] == 0x44);
            QVERIFY(mem[110] == 0x33);
            QVERIFY(mem[111] == 0x22);
            QVERIFY(mem[112] == 0x11);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }
    //    void testPlusValue16() {

    //        uint8_t            mem[64] = {0};
    //
    //        MemoryProtected    pro;
    //        uint16_t           v1              = 0x1122;
    //        uint16_t           v2              = 0;
    //        uint16_t           v3              = 0;
    //        bool               exceptionRaised = false;
    //        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
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

    //        } catch (std::exception &) {
    //            exceptionRaised = true;
    //        }

    //        QVERIFY(exceptionRaised == false);
    //    }

    //    void testPlusValue32() {

    //        uint8_t            mem[64] = {0};
    //
    //        MemoryProtected    pro;
    //        uint32_t           v1              = 0x11223344;
    //        uint32_t           v2              = 0;
    //        uint32_t           v3              = 0;
    //        bool               exceptionRaised = false;
    //        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
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

    //        } catch (std::exception &) {
    //            exceptionRaised = true;
    //        }

    //        QVERIFY(exceptionRaised == false);
    //    }

    //    void testErrorBound() {

    //        uint8_t            mem[64] = {0};
    //
    //        MemoryProtected    pro;
    //        uint8_t            v1              = 0x11;
    //        bool               exceptionRaised = false;

    //        ranges.push_back({32, 32});
    //        pro.init(mem, 64, ranges);

    //        try {
    //            *reinterpret_cast<uint32_t *>(&pro[38]) = v1;
    //        } catch (std::exception &) {
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
