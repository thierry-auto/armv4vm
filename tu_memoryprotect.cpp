
#include <QObject>
#include <QtTest>
#include <iostream>
#include <vector>

#include "vm.h"

namespace armv4vm {

struct VmProperties vmProperties;

class TestMem : public QObject {
    Q_OBJECT

  public:
    TestMem() { vmProperties.m_memsize = 512; }

  private slots:

    void testNoBound() {

        uint8_t                             mem[200] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint8_t                             v1 = 112;
        bool                                exceptionRaised = false;

        spaces.push_back({40, 10});
        pro.init(mem, 200, spaces);

        try {
            pro[41] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[41] == v1);
        QVERIFY(exceptionRaised == false);
    }

    void testMaxBound() {

        uint8_t                             mem[200] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint8_t                             v1 = 124;
        bool                                exceptionRaised = false;
        spaces.push_back({40, 10});
        pro.init(mem, 200, spaces);

        try {
            pro[49] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[49] == v1);
        QVERIFY(exceptionRaised == false);
    }

    void testMaxBound2() {

        uint8_t                             mem[200] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint8_t                             v1 = 124;
        bool                                exceptionRaised = false;
        spaces.push_back({39, 10});
        pro.init(mem, 200, spaces);

        try {
            pro[50] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }

        QVERIFY(mem[50] != v1);
        QVERIFY(exceptionRaised == true);
    }

    void testNoBound32() {

        uint8_t                             mem[64] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint32_t                            v1 = 0x11223344;
        bool                                exceptionRaised = false;
        spaces.push_back({10, 20});
        pro.init(mem, 64, spaces);

        try {
            pro[11] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[11] == 0x44);
        QVERIFY(mem[12] == 0x33);
        QVERIFY(mem[13] == 0x22);
        QVERIFY(mem[14] == 0x11);
        QVERIFY(exceptionRaised == false);
    }

    void testBoundMin32() {

        uint8_t                             mem[64] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint32_t                            v1 = 0xAA44DD77;
        bool                                exceptionRaised = false;
        spaces.push_back({10, 20});
        pro.init(mem, 64, spaces);

        try {
            pro[10] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[10] == 0x77);
        QVERIFY(mem[11] == 0xDD);
        QVERIFY(mem[12] == 0x44);
        QVERIFY(mem[13] == 0xAA);
        QVERIFY(exceptionRaised == false);
    }

    void testBoundMax32() {

        uint8_t                             mem[64] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint32_t                            v1 = 0x12fead57;
        bool                                exceptionRaised = false;
        spaces.push_back({9, 20});
        pro.init(mem, 64, spaces);

        try {
            pro[26] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[26] == 0x57);
        QVERIFY(mem[27] == 0xAD);
        QVERIFY(mem[28] == 0xFE);
        QVERIFY(mem[29] == 0x12);
        QVERIFY(exceptionRaised == false);
    }

    void test2BoundMax32() {

        uint8_t                             mem[64] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint32_t                            v1              = 0x4c9fb3d1;
        bool                                exceptionRaised = false;
        spaces.push_back({9, 20});
        pro.init(mem, 64, spaces);

        try {
            pro[27] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[27] == 0x00);
        QVERIFY(mem[28] == 0x00);
        QVERIFY(mem[29] == 0x00);
        QVERIFY(mem[30] == 0x00);
        QVERIFY(exceptionRaised == true);
    }

    void test2BoundMax16() {

        uint8_t                             mem[64] = {0};
        std::vector<Space>                  spaces;
        MemoryProtected                     pro;
        uint16_t                            v1              = 0x4c9f;
        bool                                exceptionRaised = false;
        spaces.push_back({10, 20});
        pro.init(mem, 64, spaces);

        try {
            pro[27] = v1;
        } catch (std::exception &e) {
            exceptionRaised = true;
        }
        QVERIFY(mem[27] == 0x9F);
        QVERIFY(mem[28] == 0x4C);
        QVERIFY(exceptionRaised == false);
    }
};
} // namespace armv4vm
QTEST_MAIN(armv4vm::TestMem)
#include "tu_memoryprotect.moc"
