#pragma once

#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace armv4vm {


template<typename Derived>
class MemoryInterface {
  public:
    inline uint8_t read(uint32_t addr) const {
        return static_cast<const Derived*>(this)->readImpl(addr);
    }

    inline void write(uint32_t addr, uint8_t value) {
        static_cast<Derived*>(this)->writeImpl(addr, value);
    }
};

using Range = std::pair<uint32_t, uint32_t>;

template <typename T> class IsInAuthorizedRange {
  public:
    const uint8_t *m_origin;
    const uint32_t m_offset;
    IsInAuthorizedRange(const uint8_t *origin, const uint32_t offset) : m_origin(origin), m_offset(offset) {}
    bool operator()(const Range &space) const {

        return ((m_origin + space.first) <= (m_origin + m_offset)) &&
               ((m_origin + sizeof(T) + m_offset /*- 1*/) <= (m_origin + space.first + space.second));
    }
};

class ByteRef {

  public:
    inline static uint8_t                  *m_origin;
    inline static const std::vector<Range> *m_authorized;

  private:
    int32_t m_offset;

  public:
    ByteRef(const uint32_t offset) : m_offset(offset) {}
    ByteRef(const ByteRef &other) : m_offset(other.m_offset) {}

    ByteRef &operator=(const ByteRef &other) {

        m_offset = other.m_offset;
        return *this;
    }

    uint8_t operator*() { return *(m_origin + m_offset); }

    friend ByteRef operator+(const ByteRef &left, const uint32_t right);
    friend ByteRef operator-(const ByteRef &left, const uint32_t right);
    friend ByteRef operator+(const ByteRef &left, const uint8_t right);

    template <typename T> friend T  readPointer(const ByteRef &b);
    template <typename T> friend T &writePointer(ByteRef b);
};

inline ByteRef operator+(const ByteRef &left, const uint32_t right) { return ByteRef(left.m_offset + right); }
inline ByteRef operator-(const ByteRef &left, const uint32_t right) { return ByteRef(left.m_offset - right); }
inline ByteRef operator+(const ByteRef &left, const uint8_t right) { return ByteRef(left.m_offset + right); }

template <typename T> inline T  readPointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T &writePointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }

template <typename T> inline T readPointer(const ByteRef &b) {

    if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

        throw std::runtime_error("segmentation fault");
    }

    return *reinterpret_cast<T *>(b.m_origin + b.m_offset);

}

template <typename T> inline T &writePointer(ByteRef b) {

    if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

        throw std::runtime_error("segmentation fault");
    }

    return *reinterpret_cast<T *>(b.m_origin + b.m_offset);
}

class RawMemory : public MemoryInterface<RawMemory> {
  public:
    uint8_t* data = nullptr;

    inline uint8_t* readImpl(uint32_t addr) const { return data + addr; }
    inline void writeImpl(uint32_t addr, uint8_t v) { data[addr] = v; }
};


class MemoryProtected : public MemoryInterface<MemoryProtected> {

  public:
    MemoryProtected() : m_size(0) {}

    uint8_t *init(const uint32_t size, const std::vector<Range> &authorized) {

        m_mem.resize(size, 0);
        std::fill(m_mem.begin(), m_mem.end(), 0);
        m_size       = size;
        m_authorized = authorized;

        ByteRef::m_origin     = m_mem.data();
        ByteRef::m_authorized = &authorized;

        return m_mem.data();
    }

    inline uint8_t operator[](const uint32_t offset) const { return *(m_mem.data() + offset); }
    operator uint8_t *() { return m_mem.data(); }

    friend inline ByteRef operator+(MemoryProtected &mem, const uint32_t offset);
    friend inline ByteRef operator-(MemoryProtected &mem, const uint32_t offset);

    uint8_t *getMem() { return m_mem.data(); }

    inline uint8_t readImpl(uint32_t addr) const {

        // if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

        //     throw std::runtime_error("segmentation fault");
        // }

        // return *reinterpret_cast<T *>(b.m_origin + b.m_offset);
    }

    inline void writeImpl(uint32_t addr, uint8_t v) {
        //if (addr >= size) throw std::out_of_range("Write OOB");
        //data[addr] = v;
    }

  private:
    std::vector<uint8_t> m_mem;
    uint32_t             m_size;
    std::vector<Range>   m_authorized;
};

inline ByteRef operator+(MemoryProtected &mem, const uint32_t offset) {

    (void)mem;
    return ByteRef(1 * offset);

    //ByteRef b;

}

inline ByteRef operator-(MemoryProtected &mem, const uint32_t offset) {

    //static_assert(1, "mlm");
    // todo : A corriger, mais on met un négatif dans un non signé, du coup
    // ByteRef::m_offset recçoit une grande valeur positive
    (void)mem;
    return ByteRef(-1 * offset);
}



} // namespace armv4vm
