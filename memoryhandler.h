#pragma once

#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace armv4vm {

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

  private:
    uint8_t                  *m_origin;
    uint32_t                  m_offset;
    const std::vector<Range> *m_authorized;

  public:
    ByteRef(uint8_t *mem, const uint32_t offset, const std::vector<Range> &authorized)
        : m_origin(mem), m_offset(offset), m_authorized(&authorized) {}

    ByteRef(const ByteRef &other)
        : m_origin(other.m_origin), m_offset(other.m_offset), m_authorized(other.m_authorized) {}

    ByteRef &operator=(const ByteRef &other) {

        m_origin     = other.m_origin;
        m_offset     = other.m_offset;
        m_authorized = other.m_authorized;

        return *this;
    }

    uint8_t operator*() { return *(m_origin + m_offset); }
    operator uint8_t() { return *(m_origin + m_offset); }

    friend ByteRef                  operator+(const ByteRef &left, const uint32_t right);
    friend ByteRef                  operator-(const ByteRef &left, const uint32_t right);
    template <typename T> friend T  readPointer(const ByteRef &b);
    template <typename T> friend T &writePointer(ByteRef b);
};

inline ByteRef operator+(const ByteRef &left, const uint32_t right) {

    return ByteRef(left.m_origin, left.m_offset + right, *left.m_authorized);
}

inline ByteRef operator-(const ByteRef &left, const uint32_t right) {

    return ByteRef(left.m_origin, left.m_offset - right, *left.m_authorized);
}

template <typename T> inline T readPointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T readPointer(const ByteRef &b) { return *reinterpret_cast<T *>(b.m_origin + b.m_offset); }
template <typename T> inline T &writePointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T &writePointer(ByteRef b) {

    if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

        throw std::runtime_error("segmentation fault");
    }

    return *reinterpret_cast<T *>(b.m_origin + b.m_offset);
}

class MemoryProtected {

  public:
    MemoryProtected() : m_mem(nullptr) {}

    void init(uint8_t *mem, const uint32_t sizee, std::vector<Range> &authorized) {

        m_mem        = mem;
        m_size       = sizee;
        m_authorized = authorized;
    }

    inline uint8_t operator[](const uint32_t offset) const { return *(m_mem + offset); }
    operator uint8_t *() { return m_mem; }

    friend inline ByteRef operator+(const MemoryProtected &mem, const uint32_t offset);

  private:
    uint8_t           *m_mem;
    uint32_t           m_size;
    std::vector<Range> m_authorized;
};

inline ByteRef operator+(const MemoryProtected &mem, const uint32_t offset) {

    return ByteRef(mem.m_mem, offset, mem.m_authorized);
}

} // namespace armv4vm
