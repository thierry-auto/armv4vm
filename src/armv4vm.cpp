#include "armv4vm.hpp"

namespace armv4vm {

// instanciations explicites
template class VirtualMachine<MemoryRaw, Vfpv2>;
template class VirtualMachine<MemoryProtected, Vfpv2>;


} // namespace armv4vm
