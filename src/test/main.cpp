#include "armv4vm.hpp"

#ifdef MY_LIBRARY_HEADER_ONLY
template class Vfpv2<MemoryRaw>;
template class Vfpv2<MemoryProtected>;
template class Vm<MemoryRaw, Vfpv2Unprotected>;
template class Vm<MemoryProtected, Vfpv2Protected>;
s
#endif

#include "testmem.hpp"
#include "testalu.hpp"
#include "testvfp.hpp"

int main(int argc, char** argv)
{
    int status = 0;

    // {
    //     armv4vm::TestMem tc;
    //     status |= QTest::qExec(&tc, argc, argv);
    // }

    {
        armv4vm::TestAlu tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    // {
    //     armv4vm::TestVfp tc;
    //     status |= QTest::qExec(&tc, argc, argv);
    // }

    return status;
}

//#include "main.moc"
