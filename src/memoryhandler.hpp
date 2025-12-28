//    Copyright (c) 2020-26, thierry vic
//
//    This file is part of armv4vm.
//
//    armv4vm is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    armv4vm is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with armv4vm.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "armv4vm_p.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <cstring>

namespace armv4vm {

enum class AccessPermission {

    NONE    = 0b0000,
    READ    = 0b0001,
    WRITE   = 0b0010,
    READ_WRITE = READ | WRITE,
};

inline constexpr AccessPermission operator | (const AccessPermission a, const AccessPermission b) {

    return static_cast<AccessPermission>(static_cast<int>(a) | static_cast<int>(b));
}

inline constexpr bool operator&(AccessPermission a, AccessPermission b) {

    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

template <typename T>
class MemoryRef {
  public:
    MemoryRef(std::byte* base, uint32_t address)
        : m_base(base), m_address(address) {}

           // Lecture implicite
    operator T() const {
        T value;
        std::memcpy(&value, m_base + m_address, sizeof(T));
        return value;
    }

           // Affectation
    MemoryRef& operator=(const T& value) {
        std::memcpy(m_base + m_address, &value, sizeof(T));
        return *this;
    }

  private:
    std::byte* m_base;
    uint32_t   m_address;
};

class AccessRange {
  public:
    uint32_t         start;
    size_t           size;
    AccessPermission permission;
};

// Depuis que tout est headers et template, je pense que le CRTP suivant
// n'est plus vraiment justifié. On pourrait peut-être revenir sur
// du polymorphsime classique qui serait effacé par le compilateur. A voir.
template <typename Derived> class MemoryInterface {
  protected:
    MemoryInterface()          = default;
    virtual ~MemoryInterface() = default;

  public:
    using byte = std::byte;

#if 1
    template<typename T>
    inline MemoryRef<T> writePointer(const uint32_t addr) {
        return static_cast<Derived *>(this)->template writePointerImpl<T>(addr);
    }

    template<typename T>
    inline T readPointer(const uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointerImpl<T>(addr);
    }
#else
    inline uint32_t readPointer32<uint32_t>(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointerImpl<uint32_t>(addr);
    }

    inline uint16_t readPointer16(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointerImpl<uint16_t>(addr);
    }

    inline uint8_t readPointer8(uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointerImpl<uint8_t>(addr);
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
#endif

    inline byte *getAdressZero() { return static_cast<Derived *>(this)->getAddressZeroImpl(); }
    inline byte *allocate(const size_t size) { return static_cast<Derived *>(this)->allocate(size); }
    inline uint8_t operator[](const size_t index) { return static_cast<Derived *>(this)->operator[](index); }
    inline void addAccessRange(const AccessRange &accessRange) {
        static_cast<Derived *>(this)->addAccessRangeImpl(accessRange);
    }
};

class MemoryRaw : public MemoryInterface<MemoryRaw> {
  public:
    using byte = std::byte;

    MemoryRaw()  = default;
    ~MemoryRaw() = default;

           // -------- Allocation --------

    byte* allocate(std::size_t size) {
        m_ram  = std::make_unique<byte[]>(size);
        m_size = static_cast<uint32_t>(size);
        return m_ram.get();
    }

    byte* getAddressZeroImpl() {
        return m_ram.get();
    }

    const byte* getAddressZeroImpl() const {
        return m_ram.get();
    }

    std::size_t size() const {
        return m_size;
    }

           // -------- Lecture --------

    template <typename T>
    T readPointerImpl(uint32_t addr) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryRaw::readPointerImpl requires trivially copyable T");

        T value;
        std::memcpy(&value, m_ram.get() + addr, sizeof(T));
        return value;
    }

           // -------- Écriture --------

    // template <typename T>
    // void writePointerImpl(uint32_t addr, const T& value) {
    //     static_assert(std::is_trivially_copyable_v<T>,
    //                   "MemoryRaw::writePointerImpl requires trivially copyable T");

    //     std::memcpy(m_ram.get() + addr, &value, sizeof(T));
    // }

    // template <typename T>
    // T& writePointerImpl(const uint32_t addr) {
    //     static_assert(std::is_trivially_copyable_v<T>,
    //                   "MemoryRaw::writePointerImpl requires trivially copyable T");

    //     //std::memcpy(m_ram.get() + addr, &value, sizeof(T));
    //     return m_ram.get();
    // }
    template <typename T>
    MemoryRef<T> writePointerImpl(const uint32_t address) {

        return MemoryRef<T>(m_ram.get(), address);
    }

           // -------- Accès octet --------

    byte operator[](std::size_t index) const {
        return m_ram[index];
    }

    void setByte(std::size_t index, byte value) {
        m_ram[index] = value;
    }

           // -------- Permissions / hooks --------

    void addAccessRangeImpl(const AccessRange& accessRange) {
        (void)accessRange;
    }

  private:
    std::unique_ptr<byte[]> m_ram;
    uint32_t                m_size = 0;
};



class MemoryProtected : public MemoryInterface<MemoryProtected> {
  public:
    using byte = std::byte;

    MemoryProtected() = default;
    ~MemoryProtected() = default;

           // -------- Allocation --------

    byte* allocate(std::size_t size) {
        m_ram = std::make_unique<std::vector<byte>>(size);
        return m_ram->data();
    }

    byte* getAddressZeroImpl() {
        return m_ram ? m_ram->data() : nullptr;
    }

    const byte* getAddressZeroImpl() const {
        return m_ram ? m_ram->data() : nullptr;
    }

    std::size_t size() const {
        return m_ram ? m_ram->size() : 0;
    }

           // -------- Lecture --------

    template <typename T>
    T readPointerImpl(uint32_t address) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryProtected::readPointerImpl requires trivially copyable T");

        isAccessible(address, sizeof(T), AccessPermission::READ);

        T value;
        std::memcpy(&value, m_ram->data() + address, sizeof(T));
        return value;
    }

           // -------- Écriture --------

    // template <typename T>
    // void writePointerImpl(uint32_t address, const T& value) {
    //     static_assert(std::is_trivially_copyable_v<T>,
    //                   "MemoryProtected::writePointerImpl requires trivially copyable T");

    //     isAccessible(address, sizeof(T), AccessPermission::WRITE);

    //     std::memcpy(m_ram->data() + address, &value, sizeof(T));
    // }

    // template <typename T>
    // T& writePointerImpl(uint32_t address/*, const T& value*/) {
    //     static_assert(std::is_trivially_copyable_v<T>,
    //                   "MemoryProtected::writePointerImpl requires trivially copyable T");

    //     isAccessible(address, sizeof(T), AccessPermission::WRITE);

    //     //std::memcpy(m_ram->data() + address, &value, sizeof(T));
    //     return m_ram->data();
    // }

    template <typename T>
    MemoryRef<T> writePointerImpl(const uint32_t address) {
        static_assert(std::is_trivially_copyable_v<T>);
        isAccessible(address, sizeof(T), AccessPermission::WRITE);
        return MemoryRef<T>(m_ram.get()->data(), address);
    }

           // -------- Accès octet --------

    byte operator[](uint32_t offset) const {
        return (*m_ram)[offset];
    }

    void setByte(uint32_t offset, byte value) {
        (*m_ram)[offset] = value;
    }

           // -------- Permissions --------

    void addAccessRangeImpl(const AccessRange& accessRange) {
        m_accessRanges.push_back(accessRange);
    }

  private:
    // -------- Vérification d'accès --------

    void isAccessible(uint32_t address,
                      std::size_t dataSize,
                      const AccessPermission& permission) const
    {
        auto test = [&](const AccessRange& range) {
            return  (range.permission & permission) &&
                   (address >= range.start) &&
                   ((address + dataSize) <= (range.start + range.size));
        };

        const bool allowed = std::any_of(m_accessRanges.begin(), m_accessRanges.end(), test);

        if (!allowed) {
            throw std::runtime_error("segmentation fault");
        }
    }

  private:
    std::unique_ptr<std::vector<byte>> m_ram;
    std::vector<AccessRange>           m_accessRanges;
};


} // namespace armv4vm
