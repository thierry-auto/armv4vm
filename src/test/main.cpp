#include "armv4vm.hpp"

#ifdef MY_LIBRARY_HEADER_ONLY
template class armv4vm::Vfpv2<armv4vm::MemoryRaw>;
template class armv4vm::Vfpv2<armv4vm::MemoryProtected>;
template class armv4vm::VmImplementation<armv4vm::MemoryRaw, armv4vm::Vfpv2<armv4vm::MemoryRaw>>;
template class armv4vm::VmImplementation<armv4vm::MemoryProtected, armv4vm::Vfpv2<armv4vm::MemoryProtected>>;
#endif

#include "testmem.hpp"
#include "testalu.hpp"
#include "testvfp.hpp"

int main(int argc, char** argv)
{
    int status = 0;

    {
        armv4vm::TestMem tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    // Flat
    {

        // {
        //     armv4vm::TestAluInstructionFlat tc;
        //     status |= QTest::qExec(&tc, argc, argv);
        // }

        // {
        //     armv4vm::TestAluProgramFlat tc;
        //     status |= QTest::qExec(&tc, argc, argv);
        // }

        // {
        //     armv4vm::TestVfp tc;
        //     status |= QTest::qExec(&tc, argc, argv);
        // }
    }

    // Protected
    {
    //     {
    //         armv4vm::TestAluInstructionProtected tc;
    //         status |= QTest::qExec(&tc, argc, argv);
    //     }

    //     {
    //         armv4vm::TestAluProgramProtected tc;
    //         status |= QTest::qExec(&tc, argc, argv);
    //     }

    //     {
    //         armv4vm::TestVfp tc;
    //         status |= QTest::qExec(&tc, argc, argv);
    //     }
    }

    return status;
}

//#include "main.moc"
