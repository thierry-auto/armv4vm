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
#include "vmproperties.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <cstring>
#include <ranges>

namespace armv4vm {

class TestMem;
class TestAlu;
class TestVfp;

inline constexpr AccessPermission operator | (const AccessPermission a, const AccessPermission b) {
    return static_cast<AccessPermission>(static_cast<int>(a) | static_cast<int>(b));
}

inline constexpr bool operator&(AccessPermission a, AccessPermission b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

class MemoryProtected;

template <typename T>
class MemoryRef {
  public:
    MemoryRef(std::byte* base, std::size_t address)
        : m_base(base), m_address(address) {}

    operator T() const {        
        static_assert(std::is_trivially_copyable_v<T>);
        T value;
        std::memcpy(&value, m_base + m_address, sizeof(T));
        return value;
    }

    MemoryRef& operator=(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        std::memcpy(m_base + m_address, &value, sizeof(T));
        return *this;
    }

    template <typename U>
    friend bool operator == (const MemoryRef<std::byte> &left, const U right);
    template <typename U>
    friend bool operator == (const U right, const MemoryRef<std::byte> &left);

  protected:
    std::byte*  m_base;
    std::size_t m_address;
};

template <typename T>
class MemoryRefProtectedBase : public MemoryRef<T> {
  public:
    MemoryRefProtectedBase(MemoryProtected *memoryPorected, std::byte* base, std::size_t address)
        : MemoryRef<T>(base, address), m_memoryProtected(memoryPorected) {}

    // Lecture
    operator T() const;

    // Écriture
    MemoryRefProtectedBase& operator=(T value);

    template <typename U>
    friend bool operator == (const MemoryRef<std::byte> &left, const U right);
    template <typename U>
    friend bool operator == (const U right, const MemoryRef<std::byte> &left);

  private:
    MemoryProtected* m_memoryProtected;
};

template<typename T>
using MemoryProtectedRef = MemoryRefProtectedBase<T>;



// class MemoryInterfaceBase {
//   public:

// };

// Depuis que tout est headers et template, je pense que le CRTP suivant
// n'est plus vraiment justifié. On pourrait peut-être revenir sur
// du polymorphsime classique qui serait probablement effacé par le compilateur. A voir.
template <typename Derived> class MemoryInterface /*: public MemoryInterfaceBase*/ {
  protected:
    //MemoryInterface(struct VmProperties * vmProperties = nullptr) : m_vmProperties(*vmProperties) {};
    MemoryInterface() = default;
    virtual ~MemoryInterface() = default;

  public:
    using byte = std::byte;

    template<typename T>
    inline MemoryRef<T> writePointer(const uint32_t addr) {
        return static_cast<Derived *>(this)->template writePointerImpl<T>(addr);
    }

    template<typename T>
    inline MemoryRef<T> writePointer(const uint32_t addr, const T& value) {
        return static_cast<Derived *>(this)->template writePointerImpl<T>(addr, value);
    }

    template<typename T>
    inline T readPointer(const uint32_t addr) const {
        return static_cast<const Derived *>(this)->template readPointerImpl<T>(addr);
    }

    inline byte *getAdressZero() { return static_cast<Derived *>(this)->getAddressZeroImpl(); }
    inline byte *allocate(const size_t size) { return static_cast<Derived *>(this)->allocate(size); }
    inline uint8_t operator[](const size_t index) { return static_cast<Derived *>(this)->operator[](index); }
    inline void addAccessRange(const AccessRange &accessRange) {
        static_cast<Derived *>(this)->addAccessRangeImpl(accessRange);
    }

    // struct VmProperties  m_vmProperties;
};

class MemoryRaw : public MemoryInterface<MemoryRaw> {

  public:
    friend TestMem;
    friend TestAlu;
    friend TestVfp;

  public:
    using byte = std::byte;

    //MemoryRaw(struct VmProperties * vmProperties = nullptr) : MemoryInterface(vmProperties) {}
    MemoryRaw() = default;
    ~MemoryRaw() = default;

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

    template <typename T>
    T readPointerImpl(uint32_t addr) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryRaw::readPointerImpl requires trivially copyable T");

        T value;
        std::memcpy(&value, m_ram.get() + addr, sizeof(T));
        return value;
    }

    template <typename T>
    MemoryRef<T> writePointerImpl(const uint32_t address) {

        return MemoryRef<T>(m_ram.get(), address);
    }

    template <typename T>
    MemoryRef<T> writePointerImpl(const uint32_t address, const T& value) {
        static_assert(std::is_trivially_copyable_v<T>, "MemoryRaw::writePointerImpl requires trivially copyable T");
        std::memcpy(m_ram.get() + address, &value, sizeof(T));
        return MemoryRef<T>(m_ram.get(), address);
    }

    byte operator[](std::size_t index) const {
        return m_ram[index];
    }

    void setByte(std::size_t index, byte value) {
        m_ram[index] = value;
    }

    void addAccessRangeImpl(const AccessRange& accessRange) {
        (void)accessRange;
    }

  private:
    std::unique_ptr<byte[]> m_ram;
    uint32_t                m_size = 0;
};

class MemoryProtected : public MemoryInterface<MemoryProtected> {

  public:
    friend TestMem;
    friend TestAlu;
    friend TestVfp;

  public:
    using byte = std::byte;

    MemoryProtected() = default;
    ~MemoryProtected() = default;

    byte* allocate(std::size_t size, const std::byte fillingValue = std::byte{0}) {
        m_ram = std::make_unique<std::vector<byte>>(size);
        std::ranges::fill(*m_ram, fillingValue);
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

    template <typename T>
    T readPointerImpl(uint32_t address) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryProtected::readPointerImpl requires trivially copyable T");

        isAccessible(address, sizeof(T), AccessPermission::READ);

        T value;
        std::memcpy(&value, m_ram->data() + address, sizeof(T));
        return value;
    }

    template <typename T>
    MemoryRef<T> writePointerImpl(const uint32_t address) {
        static_assert(std::is_trivially_copyable_v<T>);
        isAccessible(address, sizeof(T), AccessPermission::WRITE);
        return MemoryRef<T>(m_ram.get()->data(), address);
    }

    MemoryProtectedRef<std::byte> operator[](std::size_t index) {
        return MemoryProtectedRef<std::byte>(this, m_ram.get()->data(), index);
    }

    void setByte(uint32_t offset, byte value) {
        (*m_ram)[offset] = value;
    }

    void addAccessRangeImpl(const AccessRange& accessRange) {
        m_accessRanges.push_back(accessRange);
    }

  private:
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

  public:
    // Limiter le friend à la spécialisation std::byte ?
    template<typename T> friend class MemoryRefProtectedBase;

  private:
    std::unique_ptr<std::vector<byte>> m_ram;
    std::vector<AccessRange>           m_accessRanges;
};

template<typename T>
inline MemoryRefProtectedBase<T>::operator T() const {
    m_memoryProtected->isAccessible(MemoryRef<T>::m_address, sizeof(T), AccessPermission::READ);
    return MemoryRefProtectedBase<T>::m_base[MemoryRef<T>::m_address];
}

template<typename T>
inline MemoryRefProtectedBase<T>& MemoryRefProtectedBase<T>::operator=(T value) {
    m_memoryProtected->isAccessible(MemoryRef<T>::m_address, sizeof(T), AccessPermission::WRITE);
    MemoryRef<T>::m_base[MemoryRef<T>::m_address] = value;
    return *this;
}

template <typename T>
inline T readPointer(const std::byte* mem) {
    static_assert(std::is_trivially_copyable_v<T>);
    T value;
    std::memcpy(&value, mem, sizeof(T));
    return value;
}

template <typename T>
inline MemoryRef<T> writePointer(std::byte* mem) {
    static_assert(std::is_trivially_copyable_v<T>);
    return MemoryRef<T>(mem, 0);
}

inline bool operator == (const std::byte &left, const int &right) {
    return std::to_integer<int>(left) == right;
}

template <typename U>
bool operator == (const MemoryRef<std::byte> &left, const U right) {
    return std::to_integer<int>(*(left.m_base + left.m_address)) == right;
}

template <typename U>
bool operator == (const U right, const MemoryRef<std::byte> &left) {
    return std::to_integer<int>(*(left.m_base + left.m_address)) == right;
}

} // namespace armv4vm
