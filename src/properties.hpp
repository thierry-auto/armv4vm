#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "armv4vm_p.hpp"

namespace armv4vm {

class MemoryProtected;

struct AluProperties {

};

struct MemoryHandlerProperties {

    enum Type {
        UNDEFINED,
        FLAT,
        PROTECTED,
    };

    Type                        m_type;
    uint32_t m_memsize;
    std::vector<armv4vm::MemoryLayout> m_layout;

    MemoryHandlerProperties() : m_type(Type::UNDEFINED), m_memsize(0) { m_layout.clear(); }
    MemoryHandlerProperties(const MemoryHandlerProperties &other) {

        m_type = other.m_type;
        m_memsize = other.m_memsize;
        m_layout = other.m_layout;
    }

    MemoryHandlerProperties operator=(const MemoryHandlerProperties &other) {

        m_type = other.m_type;
        m_memsize = other.m_memsize;
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
        m_memoryHandlerProperties = other.m_memoryHandlerProperties;
    }

    VmProperties operator=(const VmProperties &other) {

        m_bin      = other.m_bin;
        m_debug    = other.m_debug;
        m_memoryHandlerProperties = other.m_memoryHandlerProperties;

        return *this;
    }

    bool m_debug;
    std::string m_bin;
    AluProperties m_aluProperties;
    MemoryHandlerProperties m_memoryHandlerProperties;
    CoproProperties m_coproProperties;

    void clear() {

        m_bin     = "";
        m_debug   = false;
    }
};

} // namespace armv4vm
