#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "armv4vm_p.hpp"

namespace armv4vm {

class MemoryProtected;

struct AluProperties {

};

struct MemoryProperties {

    enum Type {
        UNDEFINED, // peut-être à dégager..
        RAW,
        PROTECTED,
    };

    Type                        m_type;
    std::size_t m_memorySizeBytes;
    std::vector<armv4vm::MemoryLayout> m_layout;

    MemoryProperties() : m_type(Type::UNDEFINED), m_memorySizeBytes(0) { m_layout.clear(); }
    MemoryProperties(const MemoryProperties &other) {

        m_type = other.m_type;
        m_memorySizeBytes = other.m_memorySizeBytes;
        m_layout = other.m_layout;
    }

    MemoryProperties operator=(const MemoryProperties &other) {

        m_type = other.m_type;
        m_memorySizeBytes = other.m_memorySizeBytes;
        m_layout = other.m_layout;
        return *this;
    }
};

struct CoproProperties {

};

struct VmProperties {

    VmProperties() {
        m_bin     = "";
        m_debug   = false;
    }
    ~VmProperties() = default;

    VmProperties(const VmProperties &other) {

        m_bin = other.m_bin;
        m_debug = other.m_debug;
        m_memoryProperties = other.m_memoryProperties;
    }

    VmProperties operator=(const VmProperties &other) {

        m_bin      = other.m_bin;
        m_debug    = other.m_debug;
        m_memoryProperties = other.m_memoryProperties;

        return *this;
    }

    bool m_debug;
    std::string m_bin;
    AluProperties m_aluProperties;
    MemoryProperties m_memoryProperties;
    CoproProperties m_coproProperties;

    void clear() {

        m_bin     = "";
        m_debug   = false;
    }
};

} // namespace armv4vm
