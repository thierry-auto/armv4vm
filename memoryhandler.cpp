#include "memoryhandler.h"
namespace armv4vm {

template <typename T> void MemoryProtected::ByteRef::operator=(const T value) {

    if (std::any_of(m_authorized->begin(), m_authorized->end(), IsInAuthorizedRange<T>(m_origin, m_offset))) {

        *(reinterpret_cast<T *>(m_origin + m_offset)) = value;

    } else {
        throw std::runtime_error("segmentation fault");
    }
}

// explicit template
template void MemoryProtected::ByteRef::operator=<uint8_t>(const uint8_t value);
template void MemoryProtected::ByteRef::operator=<uint16_t>(const uint16_t value);
template void MemoryProtected::ByteRef::operator=<uint32_t>(const uint32_t value);
template void MemoryProtected::ByteRef::operator=<uint64_t>(const uint64_t value);

} // namespace armv4vm

// template <typename T> void MemoryProtected::ByteRef::operator=(const T value) {

//    if (std::any_of(m_authorized->begin(), m_authorized->end(), [&](const Space &space) {
//            return ((m_origin + space.first) <= (m_origin + m_offset)) &&
//                   ((m_origin + sizeof(T) + m_offset - 1) <= (m_origin + space.first + space.second));
//        })) {

//        *(reinterpret_cast<T *>(m_origin + m_offset)) = value;

//    } else {
//        throw std::runtime_error("segmentation fault");
//    }
//}
