#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

namespace armv4vm {

enum class AccessPermission {

    NONE    = 0b0000,
    READ    = 0b0001,
    WRITE   = 0b0010,
    READ_WRITE = READ | WRITE,
};

class AccessRange {
  public:
    uint32_t         start;
    size_t           size;
    AccessPermission permission;
};

template <typename Derived> class MemoryInterface {
  protected:
    MemoryInterface()          = default;
    virtual ~MemoryInterface() = default;

  public:
    inline uint32_t readPointer32(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointer<uint32_t>(addr);
    }

    inline uint16_t readPointer16(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointer<uint16_t>(addr);
    }

    inline uint8_t readPointer8(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointer<uint8_t>(addr);
    }

    inline uint32_t &writePointer32(uint32_t addr) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint32_t>(addr);
    }

    inline uint16_t &writePointer16(uint32_t addr) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint16_t>(addr);
    }

    inline uint8_t &writePointer8(uint32_t addr) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint8_t>(addr);
    }

    inline void writePointer32(uint32_t addr, const uint32_t value) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint32_t>(addr, value);
    }

    inline void writePointer16(uint32_t addr, const uint16_t value) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint16_t>(addr, value);
    }

    inline void writePointer8(uint32_t addr, const uint8_t value) {
        return static_cast<Derived *>(this)->template writePointerImpl<uint8_t>(addr, value);
    }

    inline uint8_t *getAdressZero() { return static_cast<Derived *>(this)->getAdressZeroImpl(); }

    inline uint8_t *allocate(const size_t size) { return static_cast<Derived *>(this)->allocate(size); }

    inline uint8_t operator[](const size_t index) { return static_cast<Derived *>(this)->operator[](index); }

    inline void addAccessRange(const AccessRange &accessRange) {
        static_cast<Derived *>(this)->addAccessRangeImpl(accessRange);
    }
};

class MemoryRaw : public MemoryInterface<MemoryRaw> {

  public:
    MemoryRaw()  = default;
    ~MemoryRaw() = default;

    template <typename T> inline T readPointer(uint32_t addr) const {
        return *reinterpret_cast<T *>(m_ram.get() + addr);
    }

    template <typename T> inline T &writePointerImpl(uint32_t addr) {
        return *reinterpret_cast<T *>(m_ram.get() + addr);
    }

    template <typename T> inline void writePointerImpl(uint32_t addr, const T value) {
        *reinterpret_cast<T *>(m_ram.get() + addr) = value;
    }

    inline uint8_t *getAdressZeroImpl() { return m_ram ? m_ram.get() : nullptr; }

    inline uint8_t *allocate(const size_t size) {

        m_ram  = std::make_unique<uint8_t[]>(size);
        m_size = size;
        return m_ram.get();
    }

    inline uint8_t operator[](const size_t index) { return *reinterpret_cast<uint8_t *>(m_ram.get() + index); }

    void addAccessRangeImpl(const AccessRange &accessRange) { (void)accessRange; }

  private:
    std::unique_ptr<uint8_t[]> m_ram;
    uint32_t                   m_size;
};

template <typename T> inline T  readPointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }
template <typename T> inline T &writePointer(uint8_t *mem) { return *reinterpret_cast<T *>(mem); }

class MemoryProtected : public MemoryInterface<MemoryProtected> {

  private:
    bool isAccessible(const uint32_t address, const AccessPermission &permission) const {

        if (!std::any_of(m_accessRanges.begin(), m_accessRanges.end(), [&](AccessRange range) {

                return (range.permission == permission && (address >= range.start) && (address < (range.start + range.size)));
            }))
        {
            throw std::runtime_error("segmentation fault");
        }

        return false;
    }

  public:
    MemoryProtected() = default;

    inline uint8_t operator[](const uint32_t offset) const { return *(m_ram.get()->data() + offset); }
    operator uint8_t *() { return m_ram.get()->data(); }

    template <typename T>
    inline T readPointer(uint32_t address) const {

        isAccessible(address, AccessPermission::READ);
        return *reinterpret_cast<T*>(m_ram.get()->data() + address);
    }

    template <typename T>
    inline T &writePointerImpl(uint32_t address) {

        isAccessible(address, AccessPermission::WRITE);
        return *reinterpret_cast<T*>(m_ram.get()->data() + address);
    }

    template <typename T>
    inline void writePointerImpl(uint32_t address, const T value) {

        isAccessible(address, AccessPermission::WRITE);
        *reinterpret_cast<T*>(m_ram.get()->data() + address) = value;
    }

    inline uint8_t *allocate(const size_t size) {

        m_ram = std::make_unique<std::vector<uint8_t>>(size);
        return m_ram.get()->data();
    }

    inline uint8_t *getAdressZeroImpl() { return m_ram.get()->data(); }

    void addAccessRangeImpl(const AccessRange &accessRange) { m_accessRanges.push_back(accessRange); }

  private:
    std::unique_ptr<std::vector<uint8_t>> m_ram;
    std::vector<AccessRange> m_accessRanges;
};

} // namespace armv4vm
