
#include <QObject>
#include <QTest>
#include <cstddef>

#include "armv4vm.hpp"

namespace armv4vm {

// inline auto to_int = [](std::byte b) { return std::to_integer<int>(b); };

template<typename T>
    requires std::is_trivially_copyable_v<T>
T read(std::byte* mem, std::size_t offset)
{
    T value;
    std::memcpy(&value, mem + offset, sizeof(T));
    return value;
}


template<typename T>
    requires std::is_trivially_copyable_v<T>
void write(std::byte* mem, std::size_t offset, const T& value)
{
    std::memcpy(mem + offset, &value, sizeof(T));
}

class TestMem : public QObject {
    Q_OBJECT
  private:


  public:
    TestMem() {
    }

  private slots:

    void testInstance() {
        //struct VmProperties vmProperties;
        //Test1Vm             vm1(&vmProperties);
    }

    void testRawCompleteUse() {

        MemoryHandlerProperties properties;
        properties.m_memsize = 2_kb;

        MemoryRaw raw(properties);
        std::byte *mem = raw.reset();

        raw.writePointer<uint32_t>(20) = 0x11223344;
        raw.writePointer<uint32_t>(24, 0x55667788);
        QVERIFY(read<int8_t>(mem, 20) == 0x44);
        QVERIFY(read<int8_t>(mem, 27) == 0x55);

        std::byte b1 = raw.readPointer<std::byte>(20);
        QVERIFY(b1 == 0x44);

        uint32_t uint1 = raw.readPointer<uint32_t>(20);
        QVERIFY(uint1 == 0x11223344);

        raw.writePointer<std::byte>(21) = b1;
        QVERIFY(read<int8_t>(mem, 21) == 0x44);

        std::byte b2 = raw[23];
        QVERIFY(b2 == 0x11);

        raw[23] = raw[24];
        QVERIFY(mem[23] == 0x88);
        QVERIFY(raw.readPointer<uint32_t>(20) == 0x88224444);
    }


    void testProtectedCompleteUse() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 32, AccessPermission::READ_WRITE});
        properties.m_layout.push_back({32, 32, AccessPermission::READ});
        properties.m_layout.push_back({64, 32, AccessPermission::WRITE});

        MemoryProtected pro(properties);
        std::byte *mem = pro.reset();
        bool exceptionRaised = false;
        [[maybe_unused]] int i = 0;
        [[maybe_unused]] char c = 0;

        pro.writePointer<uint32_t>(20) = 0x11223344;
        pro.writePointer<uint32_t>(24, 0x55667788);
        QVERIFY(read<uint32_t>(mem, 20) == 0x11223344);
        QVERIFY(read<uint32_t>(mem, 24) == 0x55667788);

        std::byte b1 = pro.readPointer<std::byte>(20);
        QVERIFY(b1 == 0x44);

        uint32_t int1 = pro.readPointer<uint32_t>(20);
        QVERIFY(int1 == 0x11223344);

        pro.writePointer<std::byte>(21) = b1;
        QVERIFY(read<uint32_t>(mem, 20) == 0x11224444);

        std::byte b2 = pro[23];
        QVERIFY(b2 == 0x11);

        pro[23] = pro[24];
        QVERIFY(mem[23] == 0x88);
        QVERIFY(read<uint32_t>(mem, 20) == 0x88224444);

        mem[36] = std::byte(0x77);

        try {
            b2 = pro[36];

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == false);
        QVERIFY(b2 == std::byte(0x77));

        try {
            pro[36] = std::byte(0x44);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == true);
        QVERIFY(mem[36] == std::byte(0x77));
        exceptionRaised = false;

        try {
            c = pro.readPointer<char>(62);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == false);

        try {
            i = pro.readPointer<int>(62);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == true);
        exceptionRaised = false;

        try {
            i = pro.readPointer<uint16_t>(62);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == false);
        exceptionRaised = false;

        try {
            pro.writePointer<uint16_t>(64);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == false);
        exceptionRaised = false;

        try {
            pro.writePointer<uint16_t>(63);

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(exceptionRaised == true);
        exceptionRaised = false;
    }



    void testReadWrite8() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});

        std::byte           *mem = nullptr;
        MemoryProtected    pro(properties);
        uint8_t            v1 = 0x11;
        uint8_t            v2 = 0;
        uint8_t            v3 = 0;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            write<uint8_t>(mem, 10, v1);
            pro.writePointer<uint8_t>(20) = v1;
            QVERIFY(read<int8_t>(mem, 20) == 0x11);
            QVERIFY(read<int8_t>(mem, 21) == 0x0);
            QVERIFY(read<int8_t>(mem, 22) == 0x0);
            QVERIFY(read<int8_t>(mem, 23) == 0x0);

            write<uint8_t>(mem, 24, v1);
            pro.writePointer<uint8_t>(28) = v1;
            QVERIFY(read<int8_t>(mem, 24) == 0x11);
            QVERIFY(pro[28] == std::byte(0x11));

            v2 = read<uint8_t>(mem, 24);
            v3 = pro.readPointer<uint8_t>(28);
            QVERIFY(v2 == v3);

            write<uint8_t>(mem, 32, 12);
            pro.writePointer<uint8_t>(33) = 12;

            v1  = read<int8_t>(mem, 32);
            v2  = pro.readPointer<uint8_t>(33);
            QVERIFY(v1 == v2);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }


    void testReadWrite16() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});

        std::byte           *mem = nullptr;
        MemoryProtected    pro(properties);
        const uint16_t     v1 = 0x1122;
        uint16_t           v2 = 0;
        uint16_t           v3 = 0;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            write<uint8_t>(mem, 10, static_cast<uint8_t>(v1));
            pro.writePointer<uint8_t>(20) = static_cast<uint8_t>(v1);

            QVERIFY(mem[20] == 0x22);
            QVERIFY(mem[21] == 0x0);
            QVERIFY(mem[22] == 0x0);
            QVERIFY(mem[23] == 0x0);

            write<uint16_t>(mem, 24, v1);
            pro.writePointer<uint16_t>(28) = v1;

            QVERIFY(mem[24] == 0x22);
            QVERIFY(mem[25] == 0x11);
            QVERIFY(pro[28] == 0x22);
            QVERIFY(pro[29] == 0x11);

            write<uint16_t>(mem, 24, v1);
            pro.writePointer<uint16_t>(28) = v1;

            v2 = read<uint8_t>(mem, 24);
            v3 = pro.readPointer<uint8_t>(28);
            QVERIFY(v2 == v3);

            v2 = read<uint16_t>(mem, 24);
            v3 = pro.readPointer<uint16_t>(28);
            QVERIFY(v2 == v3);

            write<uint8_t>(mem, 32, 0xEB);
            pro.writePointer<uint8_t>(33) = 0xEB;

            v2 = read<uint8_t>(mem, 32);
            v3 = read<uint8_t>(mem, 33);
            QVERIFY(v2 == v3);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testOutOfRange8() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 32, AccessPermission::READ_WRITE});
        properties.m_layout.push_back({32, 32, AccessPermission::NONE});
        std::byte           *mem;

        MemoryProtected    pro(properties);
        const uint8_t      v1 = 0x11;

        bool exceptionRaised = false;
        mem = pro.reset(std::byte{45});

        try {
            write<uint8_t>(mem, 10, v1);
            pro.writePointer<uint8_t>(20) = read<uint8_t>(mem, 10);

            QVERIFY(mem[20] == v1);

            write<uint8_t>(mem, 10, v1);
            pro.writePointer<uint8_t>(42) = read<uint8_t>(mem, 10);
        }
        catch (std::exception &) {
            exceptionRaised = true;
        }
        catch(...) {
            exceptionRaised = true;
        }

        QVERIFY(mem[42] == 45);
        QVERIFY(exceptionRaised == true);
    }

    void testReading() {

        MemoryHandlerProperties properties;
        [[maybe_unused]] std::byte b;
        bool exceptionRaised = false;
        properties.m_layout.push_back({0, 32, AccessPermission::READ_WRITE});
        properties.m_layout.push_back({32, 32, AccessPermission::WRITE});

        MemoryProtected    pro(properties);
        pro.reset(std::byte{45});

        try {
            b = pro[0];
            b = pro[12];
            b = pro[31];
        }
        catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);

        pro[32] = std::byte{3};

        try {
            b = pro[32];
        }
        catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(b);
        QVERIFY(exceptionRaised == true);

    }


    void testOutOfRange16() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 32, AccessPermission::READ_WRITE});
        std::byte           *mem;

        MemoryProtected    pro(properties);
        const uint16_t     v1 = 0x1122;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            write<uint16_t>(mem, 10, v1);
            pro.writePointer<uint16_t>(20) = read<uint16_t>(mem, 10);

            QVERIFY(mem[20] == 0x22);
            QVERIFY(mem[21] == 0x11);

            write<uint16_t>(mem, 10, v1);
            pro.writePointer<uint16_t>(31) = read<uint16_t>(mem, 10);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(mem[31] == 0);
        QVERIFY(exceptionRaised == true);
    }

    void testOutOfRange32() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 32, AccessPermission::NONE});
        properties.m_layout.push_back({32, 32, AccessPermission::READ_WRITE});
        std::byte           *mem;
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;
        uint32_t           v2 = 0;
        uint32_t           v3 = 0;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            write<uint32_t>(mem, 1, v1);
            pro.writePointer<uint32_t>(44) = read<uint32_t>(mem, 1);

            QVERIFY(mem[44] == 0x44);
            QVERIFY(mem[45] == 0x33);
            QVERIFY(mem[46] == 0x22);
            QVERIFY(mem[47] == 0x11);

            v2 = read<uint16_t>(mem, 1);
            v3 = pro.readPointer<uint16_t>(44);

            QVERIFY(v2 == v3);

            pro.writePointer<uint32_t>(30) = pro.readPointer<uint32_t>(44);

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

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 4, AccessPermission::NONE});
        properties.m_layout.push_back({4, 4, AccessPermission::READ_WRITE});
        std::byte           *mem;

        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;
        const uint32_t     v2 = 0x55667788;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            pro.writePointer<uint32_t>(4) = v1;

            QVERIFY(mem[4] == 0x44);
            QVERIFY(mem[5] == 0x33);
            QVERIFY(mem[6] == 0x22);
            QVERIFY(mem[7] == 0x11);

            pro.writePointer<uint32_t>(2) = v2;

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
            pro.writePointer<uint32_t>(5) = v1;

        } catch (std::exception &) {
            exceptionRaised = true;
        }
        QVERIFY(mem[5] == 0x33);
        QVERIFY(mem[6] == 0x22);
        QVERIFY(mem[7] == 0x11);
        QVERIFY(exceptionRaised == true);
    }

    void testMinus() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::NONE});
        properties.m_layout.push_back({64, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.reset();

        try {
            pro.writePointer<uint32_t>(-3) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testMinus2() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::NONE});
        properties.m_layout.push_back({64, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.reset();

        try {
            pro.writePointer<uint32_t>(-4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testMinus3() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.reset();

        try {
            pro.writePointer<uint32_t>(4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }

    void testMinus4() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.reset();

        try {
            pro.writePointer<uint32_t>(-4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == true);
    }

    void testPlus4() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        pro.addAccessRangeImpl({0, 64, AccessPermission::READ_WRITE});
        pro.reset();

        try {
            pro.writePointer<uint32_t>(4) = v1;
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }


    void testReadSucced() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 32, AccessPermission::NONE});
        properties.m_layout.push_back({32, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        uint32_t     v1 = 0;

        bool exceptionRaised = false;
        std::byte *mem = pro.reset();

        *reinterpret_cast<uint32_t*>(mem+32)=0x12345678;

        try {
            v1 = pro.readPointer<uint32_t>(32);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == false);
        QVERIFY(v1 == 0x12345678);
    }

    void testReadSucced2() {

        MemoryHandlerProperties properties;

        uint32_t     v1 = 0;
        bool exceptionRaised = false;
        properties.m_layout.push_back({0, 32, AccessPermission::NONE});
        properties.m_layout.push_back({32, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        std::byte *mem = pro.reset();

        write<uint32_t>(mem, 32, 0x12345678);

        try {
            v1 = pro.readPointer<uint32_t>(32);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == false);
        QVERIFY(v1 == 0x12345678);
    }

    void testReadFailed() {

        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        uint32_t     v1 = 0;

        bool exceptionRaised = false;
        pro.reset();

        try {
            v1 = pro.readPointer<uint32_t>(-1);
        } catch (std::exception &) {
            exceptionRaised = true;
        }

        (void)v1;

        QVERIFY(exceptionRaised == true);
    }

    void testMinusPlus() {

        std::byte           *mem;
        MemoryHandlerProperties properties;
        properties.m_layout.push_back({0, 64, AccessPermission::NONE});
        properties.m_layout.push_back({64, 64, AccessPermission::READ_WRITE});
        MemoryProtected    pro(properties);
        const uint32_t     v1 = 0x11223344;

        bool exceptionRaised = false;
        mem = pro.reset();

        try {
            pro.writePointer<uint32_t>(109) = v1;

            QVERIFY(mem[109] == 0x44);
            QVERIFY(mem[110] == 0x33);
            QVERIFY(mem[111] == 0x22);
            QVERIFY(mem[112] == 0x11);

        } catch (std::exception &) {
            exceptionRaised = true;
        }

        QVERIFY(exceptionRaised == false);
    }
#if 0
#endif
    //    void testPlusValue16() {

      //        uint8_t            mem[64] = {0};
      //
      //        MemoryProtected    pro(properties);
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
      //        MemoryProtected    pro(properties);
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
      //        MemoryProtected    pro(properties);
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
//QTEST_MAIN(armv4vm::TestMem)
//#include "testmem.moc"
