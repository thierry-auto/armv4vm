#pragma once

#include <algorithm>
//#include <csetjmp>
#include <cstdint>
//#include <stdexcept>
#include <vector>
#include <memory>
#include <array>

namespace armv4vm {

enum class AccessPermission {

    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
};

class AccessRange {
  public:
    uint32_t start;
    size_t size;
    AccessPermission permission;
};

template<typename Derived>
class MemoryInterface {
  protected:
    MemoryInterface() = default;
    virtual ~MemoryInterface() = default;

  public:

    inline uint32_t readPointer32(uint32_t addr) const {
        return static_cast<const Derived*>(this)->template readPointer<uint32_t>(addr);
    }

    inline uint16_t readPointer16(uint32_t addr) const {
        return static_cast<const Derived*>(this)->template readPointer<uint16_t>(addr);
    }

    inline uint8_t readPointer8(uint32_t addr) const {
        return static_cast<const Derived*>(this)->template readPointer<uint8_t>(addr);
    }

    inline uint32_t& writePointer32(uint32_t addr) {
        return static_cast<Derived*>(this)->template writePointerImpl<uint32_t>(addr);
    }

    inline uint16_t& writePointer16(uint32_t addr) {
        return static_cast<Derived*>(this)->template writePointerImpl<uint16_t>(addr);
    }

    inline uint8_t& writePointer8(uint32_t addr) {
        return static_cast<Derived*>(this)->template writePointerImpl<uint8_t>(addr);
    }

    inline uint8_t* getAdressZero() {
        return static_cast<Derived*>(this)->getAdressZeroImpl();
    }

    inline uint8_t* allocate(const size_t size) {
        return static_cast<Derived*>(this)->allocate(size);
    }

    inline void addAccessRange(const AccessRange &accessRange) {
        static_cast<Derived*>(this)->addAccessRangeImpl(accessRange);
    }
};

//using Range = std::pair<uint32_t, uint32_t>;

// inline ByteRef operator+(const ByteRef &left, const uint32_t right) { return ByteRef(left.m_offset + right); }
// inline ByteRef operator-(const ByteRef &left, const uint32_t right) { return ByteRef(left.m_offset - right); }
// inline ByteRef operator+(const ByteRef &left, const uint8_t right) { return ByteRef(left.m_offset + right); }


class MemoryRaw : public MemoryInterface<MemoryRaw> {
  public:
    MemoryRaw() = default;
    ~MemoryRaw() = default;
  public:

    std::unique_ptr<uint8_t[]> m_ram;
    uint32_t m_size;

    template<typename T>
    inline T readPointer(uint32_t addr) const { return *reinterpret_cast<T*>(m_ram.get() + addr); }

    template<typename T>
    inline T& writePointerImpl(uint32_t addr) { return *reinterpret_cast<T *>(m_ram.get() + addr); }

    inline uint8_t* getAdressZeroImpl() { return m_ram ? m_ram.get() : nullptr; }

    inline uint8_t* allocate(const size_t size) {

        m_ram = std::make_unique<uint8_t[]>(size);
        m_size = size;
        return m_ram.get();
    }

    void addAccessRangeImpl(const AccessRange &accessRange) { (void) accessRange; }
};


template <typename T> inline T  readPointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T &writePointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }




class MemoryProtected : public MemoryInterface<MemoryProtected> {

  public:

    template <typename T>
    class IsInAuthorizedRange {
      public:
        const uint8_t *m_origin;
        const uint32_t m_offset;
        IsInAuthorizedRange(const uint8_t *origin, const uint32_t offset) : m_origin(origin), m_offset(offset) {}
        bool operator()(const AccessRange &space) const {

            // return ((m_origin + space.first) <= (m_origin + m_offset)) &&
            //        ((m_origin + sizeof(T) + m_offset) <= (m_origin + space.first + space.second));

            return false;
        }
    };

    class ByteRef {

      public:
        inline static uint8_t                  *m_origin;
        inline static const std::vector<AccessRange> *m_authorized;

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

  public:
    MemoryProtected() : m_size(0) {}

    // uint8_t *init(const uint32_t size, const std::vector<Range> &authorized) {

    //     m_mem.resize(size, 0);
    //     std::fill(m_mem.begin(), m_mem.end(), 0);
    //     m_size       = size;
    //     m_authorized = authorized;

    //     ByteRef::m_origin     = m_mem.data();
    //     ByteRef::m_authorized = &authorized;

    //     return m_mem.data();
    // }

    inline uint8_t operator[](const uint32_t offset) const { return *(m_ram.get()->data() + offset); }
    operator uint8_t *() { return m_ram.get()->data(); }

    friend inline ByteRef operator+(MemoryProtected &mem, const uint32_t offset);
    friend inline ByteRef operator-(MemoryProtected &mem, const uint32_t offset);

    template<typename T>
    inline T readPointer(uint32_t addr) const { return *reinterpret_cast<T*>(m_ram.get() + addr); }

    template<typename T>
    inline T& writePointerImpl(uint32_t addr) { return *reinterpret_cast<T *>(m_ram.get() + addr); }

    inline uint8_t* allocate(const size_t size) {

        m_ram = std::make_unique<std::vector<uint8_t>>(size);
        m_size = size;
        return m_ram.get()->data();
    }

    inline uint8_t* getAdressZeroImpl() { return m_ram.get()->data(); }

    void addAccessRangeImpl(const AccessRange &accessRange) { m_accessRanges.push_back(accessRange); }

  private:

    std::unique_ptr<std::vector<uint8_t>> m_ram;
    uint32_t             m_size;
    std::vector<AccessRange> m_accessRanges;
};

// template <typename T> inline T readPointer(const ByteRef &b) {

//     if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

//         throw std::runtime_error("segmentation fault");
//     }

//     return *reinterpret_cast<T *>(b.m_origin + b.m_offset);

// }

// template <typename T> inline T &writePointer(ByteRef b) {

//     if (!std::any_of(b.m_authorized->begin(), b.m_authorized->end(), IsInAuthorizedRange<T>(b.m_origin, b.m_offset))) {

//         throw std::runtime_error("segmentation fault");
//     }

//     return *reinterpret_cast<T *>(b.m_origin + b.m_offset);
// }

// inline ByteRef operator+(MemoryProtected &mem, const uint32_t offset) {

//     (void)mem;
//     return ByteRef(1 * offset);

//     //ByteRef b;

// }

// inline ByteRef operator-(MemoryProtected &mem, const uint32_t offset) {

//     //static_assert(1, "mlm");
//     // todo : A corriger, mais on met un négatif dans un non signé, du coup
//     // ByteRef::m_offset recçoit une grande valeur positive
//     (void)mem;
//     return ByteRef(-1 * offset);
// }



} // namespace armv4vm
