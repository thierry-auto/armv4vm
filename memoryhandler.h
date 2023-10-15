#pragma once

#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace armv4vm {

using Space = std::pair<uint32_t, uint32_t>;

template <typename T> class IsInAuthorizedRange {
  public:
    const uint8_t *m_origin;
    const uint32_t m_offset;
    IsInAuthorizedRange(const uint8_t *origin, const uint32_t offset) : m_origin(origin), m_offset(offset) {}
    bool operator()(const Space &space) const {

        return ((m_origin + space.first) <= (m_origin + m_offset)) &&
               ((m_origin + sizeof(T) + m_offset - 1) <= (m_origin + space.first + space.second));
    }
};

class MemoryProtected {

  public:
    class ByteRef {
      public:
        ByteRef(uint8_t *mem, uint32_t offset, std::vector<Space> &authorized)
            : m_origin(mem), m_offset(offset), m_authorized(&authorized) {}

        template <typename T> void operator=(const T value);
        uint8_t operator*() { return *(m_origin + m_offset); }

      private:
        uint8_t            *m_origin;
        uint32_t            m_offset;
        std::vector<Space> *m_authorized;
    };

  public:
    MemoryProtected() : m_mem(nullptr) {}

    void init(uint8_t *mem, const uint32_t size, std::vector<Space> authorized) {

        m_mem        = mem;
        m_size       = size;
        m_authorized = authorized;
    }

    ByteRef operator[](const uint32_t offset) { return ByteRef(m_mem, offset, m_authorized); }
    ByteRef operator+(const uint32_t offset) { return ByteRef(m_mem, offset, m_authorized); }

    explicit operator uint8_t *() { return m_mem; }
    operator uint32_t *() { return reinterpret_cast<uint32_t *>(m_mem); }

  private:
    uint8_t           *m_mem;
    uint32_t           m_size;
    std::vector<Space> m_authorized;
};

} // namespace armv4vm
