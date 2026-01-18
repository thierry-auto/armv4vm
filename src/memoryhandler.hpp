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
#include "properties.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <cstring>
#include <ranges>
#include <numeric>

namespace armv4vm {

class TestMem;
template<typename T>
class TestAluInstruction;
class TestVfp;

inline constexpr AccessPermission operator | (const AccessPermission a, const AccessPermission b) {
    return static_cast<AccessPermission>(static_cast<int>(a) | static_cast<int>(b));
}

inline constexpr bool operator&(AccessPermission a, AccessPermission b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

class MemoryProtected;

template <typename T>
class MemoryRefUnsafe {
  public:
    MemoryRefUnsafe(std::byte* base, std::size_t address/*, MemoryProtected* memoryProtected*//*, AccessPermission permission = AccessPermission::READ_WRITE*/)
        : m_base(base), m_address(address)/*, m_memoryProtected(memoryProtected)*/ /*, m_permission(permission)*/ {}

    operator T() const {        
        static_assert(std::is_trivially_copyable_v<T>);
        T value;
        std::memcpy(&value, m_base + m_address, sizeof(T));
        return value;
    }

    MemoryRefUnsafe<T>& operator=(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        std::memcpy(m_base + m_address, &value, sizeof(T));
        return *this;
    }

    template <typename U>
    friend bool operator == (const MemoryRefUnsafe<T> &left, const U right);
    template <typename U>
    friend bool operator == (const U right, const MemoryRefUnsafe<T> &left);

    MemoryRefUnsafe<T> & operator = (const MemoryRefUnsafe<T> &other) {

        std::memcpy(m_base + m_address, other.m_base + other.m_address, sizeof(T));
        return *this;
    }

  protected:
    std::byte*  m_base;
    std::size_t m_address;
    // AccessPermission m_permission;
    //MemoryProtected* m_memoryProtected;
};

template <typename T>
class MemoryRefSafe {
  public:
    MemoryRefSafe(std::byte* base, std::size_t address, MemoryProtected* memoryProtected/*, AccessPermission permission = AccessPermission::READ_WRITE*/)
        : m_base(base), m_address(address), m_memoryProtected(memoryProtected) /*, m_permission(permission)*/ {}

    operator T() const {
        static_assert(std::is_trivially_copyable_v<T>);
        T value;
        std::memcpy(&value, m_base + m_address, sizeof(T));
        return value;
    }

    MemoryRefSafe& operator=(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        std::memcpy(m_base + m_address, &value, sizeof(T));
        return *this;
    }

    MemoryRefSafe<T> & operator = (const MemoryRefSafe<T> &other) {

        // Ilfaudra vérifier les accés
        std::memcpy(m_base + m_address, other.m_base + other.m_address, sizeof(T));
        return *this;
    }

    template <typename U>
    friend bool operator == (const MemoryRefSafe<T> &left, const U right);
    template <typename U>
    friend bool operator == (const U right, const MemoryRefSafe<T> &left);

  protected:
    std::byte*  m_base;
    std::size_t m_address;
    // AccessPermission m_permission;
    MemoryProtected* m_memoryProtected;
};

// template <typename T>
// class MemoryRefProtectedBase : public MemoryRef<T> {
//   public:
//     MemoryRefProtectedBase(MemoryProtected *memoryPorected, std::byte* base, std::size_t address)
//         : MemoryRef<T>(base, address), m_memoryProtected(memoryPorected) {}

//     // nb:peut-être que de passer memoryPorected, on pourrait seulement passer
//     // la permission. La permission est unique par MemoryRefProtectedBase..
//     // A réfléchir..

//     // Lecture
//     operator T() const;

//     // Écriture
//     MemoryRefProtectedBase& operator=(T value);

//     template <typename U>
//     friend bool operator == (const MemoryRef<std::byte> &left, const U right);
//     template <typename U>
//     friend bool operator == (const U right, const MemoryRef<std::byte> &left);

//   private:
//     MemoryProtected* m_memoryProtected;
// };

// template<typename T>
// using MemoryProtectedRef = MemoryRefProtectedBase<T>;



// class MemoryInterfaceBase {
//   public:

// };

// Depuis que tout est headers et template, je pense que le CRTP suivant
// n'est plus vraiment justifié. On pourrait peut-être revenir sur
// du polymorphsime classique qui serait probablement effacé par le compilateur. A voir.
// template <typename Derived>
// class MemoryInterface {
//   protected:
//     MemoryInterface(struct MemoryHandlerProperties & properties) : m_properties(properties) {};
//     MemoryInterface() = default;
//     virtual ~MemoryInterface() = default;

//   public:
//     using byte = std::byte;

//     template<typename T>
//     inline MemoryRef<T> writePointer(const uint32_t addr) {
//         return static_cast<Derived *>(this)->template writePointerImpl<T>(addr);
//     }

//     template<typename T>
//     inline MemoryRef<T> writePointer(const uint32_t addr, const T& value) {
//         return static_cast<Derived *>(this)->template writePointerImpl<T>(addr, value);
//     }

//     template<typename T>
//     inline T readPointer(const uint32_t addr) const {
//         return static_cast<const Derived *>(this)->template readPointerImpl<T>(addr);
//     }

//     inline byte *getAdressZero() { return static_cast<Derived *>(this)->getAddressZeroImpl(); }
//     //inline byte *allocate(const size_t size) { return static_cast<Derived *>(this)->allocateImpl(size); }
//     //inline MemoryRef<std::byte> operator[](const size_t index) { return static_cast<Derived *>(this)->getByteImpl(index); }
//     inline void addAccessRange(const MemoryLayout &accessRange) {
//         static_cast<Derived *>(this)->addAccessRangeImpl(accessRange);
//     }
//     inline byte *reset(const std::byte fillingValue = std::byte{0}) {
//         return static_cast<Derived *>(this)->resetImpl(fillingValue);
//     }

//     struct MemoryHandlerProperties m_properties;
// };


class MemoryRaw /*: public MemoryInterface<MemoryRaw>*/ {

  public:
    friend TestMem;
    friend TestAluInstruction<MemoryRaw>;
    friend TestVfp;

  public:
    using byte = std::byte;

    MemoryRaw(struct MemoryHandlerProperties & properties) /*: MemoryInterface(properties)*/ {
        m_ram  = std::make_unique<byte[]>(m_properties.m_memsize);
    }
    ~MemoryRaw() = default;

    byte* reset(const std::byte fillingValue = std::byte{0}) {
        //m_size = static_cast<uint32_t>(m_properties.m_memsize);
        std::memset(m_ram.get(), static_cast<char>(fillingValue), m_properties.m_memsize);
        return m_ram.get();
    }

    byte* getAddressZero() {
        return m_ram.get();
    }

    const byte* getAddressZero() const {
        return m_ram.get();
    }

    std::size_t size() const {
        return m_size;
    }

    template <typename T>
    T readPointer(uint32_t addr) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryRaw::readPointerImpl requires trivially copyable T");

        T value;
        std::memcpy(&value, m_ram.get() + addr, sizeof(T));
        return value;
    }

    template <typename T>
    MemoryRefUnsafe<T> writePointer(const uint32_t address) {

        return MemoryRefUnsafe<T>(m_ram.get(), address);
    }

    template <typename T>
    MemoryRefUnsafe<T> writePointer(const uint32_t address, const T& value) {

        static_assert(std::is_trivially_copyable_v<T>, "MemoryRaw::writePointerImpl requires trivially copyable T");
        std::memcpy(m_ram.get() + address, &value, sizeof(T));
        return MemoryRefUnsafe<T>(m_ram.get(), address);
    }

    MemoryRefUnsafe<std::byte> operator[](std::size_t index) const {
        return MemoryRefUnsafe<std::byte>(m_ram.get(), index);
    }

    // MemoryRef<std::byte> getByteImpl(const std::size_t index) {

    //     return MemoryRef<std::byte>(m_ram.get(), index);
    // }

    void setByte(std::size_t index, byte value) {
        m_ram[index] = value;
    }

    void addAccessRangeImpl(const MemoryLayout& accessRange) {
        (void)accessRange;
    }

  private:
    std::unique_ptr<byte[]> m_ram;
    uint32_t                m_size = 0;
    struct MemoryHandlerProperties m_properties;
};

class MemoryProtected /*: public MemoryInterface<MemoryProtected>*/ {

  public:
    friend TestMem;
    friend TestAluInstruction<MemoryProtected>;
    friend TestVfp;

  public:
    using byte = std::byte;

    MemoryProtected(struct MemoryHandlerProperties & properties) /*: MemoryInterface(properties)*/ {

        m_memoryLayout = properties.m_layout;

        const size_t totalAllocation = std::accumulate(
            m_memoryLayout.begin(),
            m_memoryLayout.end(),
            0,
            [](const int &cumul, const MemoryLayout &layout){ return cumul + layout.size; });

        m_ram = std::make_unique<std::vector<byte>>(totalAllocation);
        std::ranges::fill(*m_ram, std::byte{0});
    }
    ~MemoryProtected() = default;

    byte *reset(const std::byte fillingValue = std::byte{0}) {

        std::ranges::fill(*m_ram, fillingValue);
        return m_ram->data();
    }

    byte* getAddressZero() {
        return m_ram ? m_ram->data() : nullptr;
    }

    const byte* getAddressZero() const {
        return m_ram ? m_ram->data() : nullptr;
    }

    std::size_t size() const {
        return m_ram ? m_ram->size() : 0;
    }

    template <typename T>
    T readPointer(uint32_t address) const {
        static_assert(std::is_trivially_copyable_v<T>,
                      "MemoryProtected::readPointerImpl requires trivially copyable T");

        isAccessible(address, sizeof(T), AccessPermission::READ);

        T value;
        std::memcpy(&value, m_ram->data() + address, sizeof(T));
        return value;
    }

    template <typename T>
    MemoryRefSafe<T> writePointer(const uint32_t address, const T& value) {
        static_assert(std::is_trivially_copyable_v<T>, "MemoryProtected::writePointerImpl requires trivially copyable T");
        isAccessible(address, sizeof(T), AccessPermission::WRITE);
        std::memcpy(m_ram.get()->data() + address, &value, sizeof(T));
        return MemoryRefSafe<T>(m_ram.get()->data(), address, this);
    }

    template <typename T>
    MemoryRefSafe<T> writePointer(const uint32_t address) {
        static_assert(std::is_trivially_copyable_v<T>);
        isAccessible(address, sizeof(T), AccessPermission::WRITE);
        return MemoryRefSafe<T>(m_ram.get()->data(), address, this);
    }

    MemoryRefSafe<std::byte> operator[](std::size_t index) {
//        isAccessible(index, sizeof(std::byte), AccessPermission::WRITE);
//        return MemoryRef<std::byte>(m_ram.get()->data(), index);
        //isAccessible(index, sizeof(std::byte), AccessPermission::WRITE);
        return MemoryRefSafe<std::byte>(m_ram.get()->data(), index, this);

    }

    // MemoryRef<std::byte> getByteImpl(const std::size_t index) {

    //     isAccessible(index, sizeof(std::byte), AccessPermission::WRITE);
    //     return MemoryRef<std::byte>(m_ram.get()->data(), index);
    // }

    void setByte(uint32_t offset, byte value) {
        (*m_ram)[offset] = value;
    }

    void addAccessRangeImpl(const MemoryLayout& accessRange) {
        m_memoryLayout.push_back(accessRange);
    }

  private:
    void isAccessible(uint32_t address,
                      std::size_t dataSize,
                      const AccessPermission& permission) const
    {
        auto test = [&](const MemoryLayout& range) {
            return  (range.permission & permission) &&
                   (address >= range.start) &&
                   ((address + dataSize) <= (range.start + range.size));
        };

        const bool allowed = std::any_of(m_memoryLayout.begin(), m_memoryLayout.end(), test);

        if (!allowed) {
            throw std::runtime_error("segmentation fault");
        }
    }

  public:
    // Limiter le friend à la spécialisation std::byte ?
    template<typename T> friend class MemoryRefProtectedBase;

  private:
    std::unique_ptr<std::vector<byte>> m_ram;
    std::vector<MemoryLayout>           m_memoryLayout;
    struct MemoryHandlerProperties m_properties;
};

// Defini ici car m_memoryProtected est enfin connu
// template<typename T>
// inline MemoryRefProtectedBase<T>::operator T() const {
//     m_memoryProtected->isAccessible(MemoryRef<T>::m_address, sizeof(T), AccessPermission::READ);
//     return MemoryRefProtectedBase<T>::m_base[MemoryRef<T>::m_address];
// }

// // Defini ici car m_memoryProtected est enfin connu
// template<typename T>
// inline MemoryRefProtectedBase<T>& MemoryRefProtectedBase<T>::operator=(T value) {
//     m_memoryProtected->isAccessible(MemoryRef<T>::m_address, sizeof(T), AccessPermission::WRITE);
//     MemoryRef<T>::m_base[MemoryRef<T>::m_address] = value;
//     return *this;
// }

template <typename T>
inline T readPointer(const std::byte* mem) {
    static_assert(std::is_trivially_copyable_v<T>);
    T value;
    std::memcpy(&value, mem, sizeof(T));
    return value;
}

// template <typename T>
// inline MemoryRef<T> writePointer(std::byte* mem) {
//     static_assert(std::is_trivially_copyable_v<T>);
//     return MemoryRef<T>(mem, 0);
// }

inline bool operator == (const std::byte &left, const int &right) {
    return std::to_integer<int>(left) == right;
}

// template <typename U>
// bool operator == (const MemoryRef<std::byte> &left, const U right) {
//     return std::to_integer<int>(*(left.m_base + left.m_address)) == right;
// }

// template <typename U>
// bool operator == (const U right, const MemoryRef<std::byte> &left) {
//     return std::to_integer<int>(*(left.m_base + left.m_address)) == right;
// }

} // namespace armv4vm
