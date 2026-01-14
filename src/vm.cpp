#include "vm.hpp"

namespace armv4vm {

template class Vfpv2<MemoryRaw>;
template class Vfpv2<MemoryProtected>;
template class Vm<MemoryRaw, Vfpv2Unprotected>;
template class Vm<MemoryProtected, Vfpv2Protected>;

} // namespace armv4vm
