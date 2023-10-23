// #include "memoryhandler.h"
namespace armv4vm {

// uint8_t operator&(const ByteRef &left, const ByteRef right) { return *left & *right; }
// uint8_t operator|(const ByteRef &left, const ByteRef right) { return *left | *right; }

// template <> uint8_t ByteRef::operator=<uint8_t>(const uint8_t value) {

//    if (std::any_of(m_authorized->begin(), m_authorized->end(), IsInAuthorizedRange<uint8_t>(m_origin, m_offset))) {

//        *(reinterpret_cast<uint8_t *>(m_origin + m_offset)) = value;

//    } else {
//        throw std::runtime_error("segmentation fault");
//    }

//    return value;
//}

// explicit template
// uint8_t ByteRef8::operator=(const uint8_t value);
// template uint16_t ByteRef8::operator=<uint16_t>(const uint16_t value);
// template uint32_t ByteRef8::operator=<uint32_t>(const uint32_t value);
// template int32_t  ByteRef8::operator=<int32_t>(const int32_t value);
// template uint64_t ByteRef::operator=<uint64_t>(const uint64_t value);

} // namespace armv4vm

// template <typename T> void ByteRef::operator=(const T value) {

//    if (std::any_of(m_authorized->begin(), m_authorized->end(), [&](const Range &space) {
//            return ((m_origin + space.first) <= (m_origin + m_offset)) &&
//                   ((m_origin + sizeof(T) + m_offset - 1) <= (m_origin + space.first + space.second));
//        })) {

//        *(reinterpret_cast<T *>(m_origin + m_offset)) = value;

//    } else {
//        throw std::runtime_error("segmentation fault");
//    }
//}
